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

#include <new>
#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/transitions/NoTransition.hpp>
#include <gui/template_screen/TemplateView.hpp>
#include <gui/template_screen/TemplatePresenter.hpp>
#include <gui/common/FrontendHeap.hpp>

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : touchgfx::MVPApplication(),
      transitionCallback(),
      frontendHeap(heap),
      model(m)
{
}

void FrontendApplication::gotoTemplateScreen()
{
    transitionCallback = touchgfx::Callback< FrontendApplication >(this, &FrontendApplication::gotoTemplateScreenImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplication::gotoTemplateScreenImpl()
{
    touchgfx::makeTransition< TemplateView, TemplatePresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}
