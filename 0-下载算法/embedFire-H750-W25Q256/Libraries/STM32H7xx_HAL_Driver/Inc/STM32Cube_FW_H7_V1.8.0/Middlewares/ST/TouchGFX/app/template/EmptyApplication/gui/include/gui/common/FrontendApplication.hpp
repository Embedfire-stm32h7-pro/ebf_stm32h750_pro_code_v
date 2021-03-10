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

#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include <mvp/View.hpp>
#include <mvp/MVPApplication.hpp>
#include <gui/model/Model.hpp>

class FrontendHeap;

/**
 * The FrontendApplication is the concrete implementation of an MVPApplication
 * for a specific application. Its primary job is to provide functions for
 * switching between screens (presenter/view pairs). By convention these functions
 * are usually called gotoXXScreen().
 *
 * Note that the base class Application is a singleton. Switching screen can be done
 * from anywhere (usually presenters) by e.g. the following:
 * static_cast<FrontendApplication*>(Application::getInstance())->gotoXXScreen();
 */
class FrontendApplication : public touchgfx::MVPApplication
{
public:
    FrontendApplication(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplication() { }

    /**
     * Request a transition to the "Template" screen. The actual switching will
     * be done at next tick (by gotoTemplateScreenImpl).
     *
     * @note The reason for not switching immediately is that the new presenter and view
     * reuse the same memory as the current ones, so the switch cannot occur instantaneously
     * since that would cause memory corruption at the point of calling this function.
     */
    void gotoTemplateScreen();

    /**
     * Called automatically every frame. Will call tick on the model and then delegate
     * the tick event to the framework for further processing.
     */
    virtual void handleTickEvent()
    {
        model.tick();
        touchgfx::MVPApplication::handleTickEvent();
    }
private:
    touchgfx::Callback<FrontendApplication> transitionCallback;
    FrontendHeap& frontendHeap;
    Model& model;
    void gotoTemplateScreenImpl();
};

#endif /* FRONTENDAPPLICATION_HPP */
