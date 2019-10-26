/*Copyright (C) 2008-2009  Timothy B. Terriberry (tterribe@xiph.org)
  You can redistribute this library and/or modify it under the terms of the
   GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2.1 of the License, or (at your option) any later
   version.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qrcode.h"
#include "qrdec.h"
#include "util.h"
#include "image.h"
#include "error.h"
#include "img_scanner.h"

static int text_is_ascii(const unsigned char *_text,int _len){
  int i;
  for(i=0;i<_len;i++)if(_text[i]>=0x80)return 0;
  return 1;
}

static int text_is_latin1(const unsigned char *_text,int _len){
  int i;
  for(i=0;i<_len;i++){
    /*The following line fails to compile correctly with gcc 3.4.4 on ARM with
       any optimizations enabled.*/
    if(_text[i]>=0x80&&_text[i]<0xA0)return 0;
  }
  return 1;
}

#if 0
static void enc_list_mtf(iconv_t _enc_list[3],iconv_t _enc){
  int i;
  for(i=0;i<3;i++)if(_enc_list[i]==_enc){
    int j;
    for(j=i;j-->0;)_enc_list[j+1]=_enc_list[j];
    _enc_list[0]=_enc;
    break;
  }
}
#endif

int qr_code_data_list_extract_text(const qr_code_data_list *_qrlist,
                                   zbar_image_scanner_t *iscn,
                                   zbar_image_t *img)
{
  const qr_code_data  *qrdata;
  int                  nqrdata;
  unsigned char       *mark;
  int                  ntext;
  int                  i;
  zbar_symbol_t *syms, **sym;

  qrdata=_qrlist->qrdata;
  nqrdata=_qrlist->nqrdata;
  mark=(unsigned char *)calloc(nqrdata,sizeof(*mark));
  ntext=0;
  for(i=0;i<nqrdata;i++)if(!mark[i]){
    const qr_code_data       *qrdataj;
    const qr_code_data_entry *entry;
    int                       sa[16];
    int                       sa_size;
    char                     *sa_text;
    size_t                    sa_ntext;
    size_t                    sa_ctext;
    int                       fnc1;
    int                       eci;
    int                       err;
    int                       j;
    int                       k;
    
    sa[0]=i;
    sa_size=1;

    /*Step 1: Detect FNC1 markers and estimate the required buffer size.*/
    for(j=0;j<sa_size;j++)if(sa[j]>=0){
      qrdataj=qrdata+sa[j];
      for(k=0;k<qrdataj->nentries;k++){
        entry=qrdataj->entries+k;
        switch(entry->mode){
          /*FNC1 applies to the entire code and ignores subsequent markers.*/
          case QR_MODE_BYTE:
          break;
          default:
          continue;  //We only support direct byte mode      
          break;
        }
      }
    }

    sa_ctext=entry->payload.data.len;
    fnc1=0;

    /*Step 2: Convert the entries.*/
    sa_text=(char *)malloc((sa_ctext+1)*sizeof(*sa_text));
    memcpy(sa_text,entry->payload.data.buf,sa_ctext);
    sa_text[sa_ctext] = '\0';

    sa_ntext=0;
    eci=-1;

    err=0;
    syms = NULL;
    sym = &syms;
    for(j = 0; j < sa_size && !err; j++, sym = &(*sym)->next) {
      *sym = _zbar_image_scanner_alloc_sym(iscn, ZBAR_QRCODE, 0);
      (*sym)->datalen = sa_ntext;
      if(sa[j]<0){
        /* generic placeholder for unfinished results */
        (*sym)->type = ZBAR_PARTIAL;

        /*Skip all contiguous missing segments.*/
        for(j++;j<sa_size&&sa[j]<0;j++);
        /*If there aren't any more, stop.*/
        if(j>=sa_size)break;

        /* mark break in data */
        sa_text[sa_ntext++]='\0';
        (*sym)->datalen = sa_ntext;

        /* advance to next symbol */
        sym = &(*sym)->next;
        *sym = _zbar_image_scanner_alloc_sym(iscn, ZBAR_QRCODE, 0);
      }

      qrdataj=qrdata+sa[j];
      /* expose bounding box */
      sym_add_point(*sym, qrdataj->bbox[0][0], qrdataj->bbox[0][1]);
      sym_add_point(*sym, qrdataj->bbox[2][0], qrdataj->bbox[2][1]);
      sym_add_point(*sym, qrdataj->bbox[3][0], qrdataj->bbox[3][1]);
      sym_add_point(*sym, qrdataj->bbox[1][0], qrdataj->bbox[1][1]);
    }

    if(!err){
      zbar_symbol_t *sa_sym;

      if(sa_size == 1)
          sa_sym = syms;
      else {
          int xmin = img->width, xmax = -2;
          int ymin = img->height, ymax = -2;
          int next;
          /* create "virtual" container symbol for composite result */
          sa_sym = _zbar_image_scanner_alloc_sym(iscn, ZBAR_QRCODE, 0);
          sa_sym->syms = _zbar_symbol_set_create();
          sa_sym->syms->head = syms;

          /* cheap out w/axis aligned bbox for now */
          

          /* fixup data references */
          for(; syms; syms = syms->next) {
              _zbar_symbol_refcnt(syms, 1);
              if(syms->type == ZBAR_PARTIAL)
                  sa_sym->type = ZBAR_PARTIAL;
              else
                  for(j = 0; j < syms->npts; j++) {
                      int u = syms->pts[j].x;
                      if(xmin >= u) xmin = u - 1;
                      if(xmax <= u) xmax = u + 1;
                      u = syms->pts[j].y;
                      if(ymin >= u) ymin = u - 1;
                      if(ymax <= u) ymax = u + 1;
                  }
              syms->data = sa_text + syms->datalen;
              next = (syms->next) ? syms->next->datalen : sa_ntext;
              assert(next > syms->datalen);
              syms->datalen = next - syms->datalen - 1;
          }
          if(xmax >= -1) {
              sym_add_point(sa_sym, xmin, ymin);
              sym_add_point(sa_sym, xmin, ymax);
              sym_add_point(sa_sym, xmax, ymax);
              sym_add_point(sa_sym, xmax, ymin);
          }
      }
      sa_sym->data = sa_text;
      sa_sym->data_alloc = sa_ctext+1;
      sa_sym->datalen = sa_ctext;

      _zbar_image_scanner_add_sym(iscn, sa_sym);
    }
    else {
        _zbar_image_scanner_recycle_syms(iscn, syms);
        free(sa_text);
    }
  }

  free(mark);
  return ntext;
}
