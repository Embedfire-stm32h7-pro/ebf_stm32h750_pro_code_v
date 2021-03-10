/**
  ******************************************************************************
  * This file is part of the TouchGFX 4.12.3 distribution.
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include <platform/hal/simulator/sdl2/HALSDL2.hpp>
#include <touchgfx/hal/NoDMA.hpp>
#include <common/TouchGFXInit.hpp>
#include <platform/driver/touch/SDL2TouchController.hpp>
#if !defined(USE_BPP) || USE_BPP==16
#include <platform/driver/lcd/LCD16bpp.hpp>
#elif USE_BPP==32
#include <platform/driver/lcd/LCD32bpp.hpp>
#elif USE_BPP==24
#include <platform/driver/lcd/LCD24bpp.hpp>
#elif USE_BPP==4
#include <platform/driver/lcd/LCD4bpp.hpp>
#elif USE_BPP==2
#include <platform/driver/lcd/LCD2bpp.hpp>
#elif USE_BPP==1
#include <platform/driver/lcd/LCD1bpp.hpp>
#elif defined(LCD8BPP_RGBA2222)
#include <platform/driver/lcd/LCD8bpp_RGBA2222.hpp>
#elif defined(LCD8BPP_BGRA2222)
#include <platform/driver/lcd/LCD8bpp_BGRA2222.hpp>
#elif defined(LCD8BPP_ARGB2222)
#include <platform/driver/lcd/LCD8bpp_ARGB2222.hpp>
#elif defined(LCD8BPP_ABGR2222)
#include <platform/driver/lcd/LCD8bpp_ABGR2222.hpp>
#else
#error Unknown USE_BPP
#endif
#include <stdlib.h>

//#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
//#define CANVAS_BUFFER_SIZE (3600)

#ifdef __linux__
int main(int argc, char** argv)
{
#else
#include <shellapi.h>
#ifdef _UNICODE
#error Cannot run in unicode mode
#endif
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc;
    char** argv = touchgfx::HALSDL2::getArgv(&argc);
#endif
    touchgfx::NoDMA dma; //For windows/linux, DMA transfers are simulated
#if !defined(USE_BPP) || USE_BPP==16
    touchgfx::LCD16bpp lcd;
#elif USE_BPP==32
    touchgfx::LCD32bpp lcd;
#elif USE_BPP==24
    touchgfx::LCD24bpp lcd;
#elif USE_BPP==4
    touchgfx::LCD4bpp lcd;
#elif USE_BPP==2
    touchgfx::LCD2bpp lcd;
#elif USE_BPP==1
    touchgfx::LCD1bpp lcd;
#elif defined(LCD8BPP_RGBA2222)
    touchgfx::LCD8bpp_RGBA2222 lcd;
#elif defined(LCD8BPP_BGRA2222)
    touchgfx::LCD8bpp_BGRA2222 lcd;
#elif defined(LCD8BPP_ARGB2222)
    touchgfx::LCD8bpp_ARGB2222 lcd;
#elif defined(LCD8BPP_ABGR2222)
    touchgfx::LCD8bpp_ABGR2222 lcd;
#else
#error Unknown USE_BPP
#endif
    touchgfx::SDL2TouchController tc;

    // Create hardware layer. Use a display size of 480x272.
    touchgfx::HAL& hal = touchgfx::touchgfx_generic_init<touchgfx::HALSDL2>(dma, lcd, tc, 480, 272, 0, 0);

    // Simulate hardware running at 60Hz generating a vsync every 16.6667 ms
    static_cast<touchgfx::HALSDL2&>(hal).setVsyncInterval(16.6667f);

    // Initialize SDL
    bool sdl_init_result = static_cast<touchgfx::HALSDL2&>(hal).sdl_init(argc, argv);
    assert(sdl_init_result && "Error during SDL initialization");

    // Ensure there is a console window to print to using printf() or
    // std::cout, and read from using e.g. fgets or std::cin.
    // Alternatively, instead of using printf(), always use
    // touchgfx_printf() which will ensure there is a console to write
    // to.
    //touchgfx_enable_stdio();

    // Setup the CanvasWidgetRenderer. ONLY needed if you use CanvasWidgets
    // in your application. The CANVAS_BUFFER_SIZE can be adjusted to match
    // your needs in performance vs. RAM usage. Read more on this in the
    // TouchGFX Manual.
    //static uint8_t canvasBuffer[CANVAS_BUFFER_SIZE];
    //touchgfx::CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);

    touchgfx::HAL::getInstance()->taskEntry(); //Never returns

    return EXIT_SUCCESS;
}
