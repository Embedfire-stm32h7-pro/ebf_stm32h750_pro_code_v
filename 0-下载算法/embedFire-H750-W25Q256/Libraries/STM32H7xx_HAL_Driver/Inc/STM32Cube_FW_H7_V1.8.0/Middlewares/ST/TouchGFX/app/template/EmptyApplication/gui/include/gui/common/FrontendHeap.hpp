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

#ifndef FRONTENDHEAP_HPP
#define FRONTENDHEAP_HPP

#include <common/Meta.hpp>
#include <common/Partition.hpp>
#include <mvp/MVPHeap.hpp>
#include <touchgfx/transitions/NoTransition.hpp>
#include <gui/common/FrontendApplication.hpp>
#include <gui/model/Model.hpp>
#include <gui/template_screen/TemplateView.hpp>
#include <gui/template_screen/TemplatePresenter.hpp>

/**
 * This class provides the memory that shall be used for memory allocations
 * in the frontend. A single instance of the FrontendHeap is allocated once (in heap
 * memory), and all other frontend objects such as views, presenters and data model are
 * allocated within the scope of this FrontendHeap. As such, the RAM usage of the entire
 * user interface is sizeof(FrontendHeap).
 *
 * @note The FrontendHeap reserves memory for the most memory-consuming presenter and
 * view only. The largest of these classes are determined at compile-time using template
 * magic. As such, it is important to add all presenters, views and transitions to the
 * type lists in this class.
 *
 */
class FrontendHeap : public touchgfx::MVPHeap
{
public:
    /**
     * A list of all view types. Must end with meta::Nil.
     * @note All view types used in the application MUST be added to this list!
     */
    typedef touchgfx::meta::TypeList< TemplateView,
            touchgfx::meta::Nil
            > ViewTypes;

    /**
     * Determine (compile time) the View type of largest size.
     */
    typedef touchgfx::meta::select_type_maxsize< ViewTypes >::type MaxViewType;

    /**
     * A list of all presenter types. Must end with meta::Nil.
     * @note All presenter types used in the application MUST be added to this list!
     */
    typedef touchgfx::meta::TypeList< TemplatePresenter,
            touchgfx::meta::Nil
            > PresenterTypes;

    /**
     * Determine (compile time) the Presenter type of largest size.
     */
    typedef touchgfx::meta::select_type_maxsize< PresenterTypes >::type MaxPresenterType;

    /**
     * A list of all transition types. Must end with meta::Nil.
     * @note All transition types used in the application MUST be added to this list!
     */
    typedef touchgfx::meta::TypeList< touchgfx::NoTransition,
            touchgfx::meta::Nil
            > TransitionTypes;

    /**
     * Determine (compile time) the Transition type of largest size.
     */
    typedef touchgfx::meta::select_type_maxsize< TransitionTypes >::type MaxTransitionType;

    static FrontendHeap& getInstance()
    {
        static FrontendHeap instance;
        return instance;
    }

    touchgfx::Partition< PresenterTypes, 1 > presenters;
    touchgfx::Partition< ViewTypes, 1 > views;
    touchgfx::Partition< TransitionTypes, 1 > transitions;
    FrontendApplication app;
    Model model;
private:
    FrontendHeap()
        : MVPHeap(presenters, views, transitions, app),
          presenters(),
          views(),
          transitions(),
          app(model, *this)
    {
        // Goto start screen
        app.gotoTemplateScreen();
    }
};

#endif
