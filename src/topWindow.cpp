/***********************************************************************
 *                            T R O K A M
 *                         Fair Search Engine
 *
 * Copyright (C) 2018, Nicolas Slusarenko
 *                     nicolas.slusarenko@trokam.com
 *
 * This file is part of Trokam.
 *
 * Trokam is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Trokam is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Trokam. If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

/// Wt
#include <Wt/WFlags.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WMenu.h>
#include <Wt/WNavigationBar.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>

/// Trokam
#include "common.h"
#include "sharedResources.h"
#include "topWindow.h"

Trokam::TopWindow::TopWindow(boost::shared_ptr<Trokam::SharedResources> &sr,
                             Wt::WApplication* app): Wt::WContainerWidget(),
                                                     application(app)
{
    resources= sr;
    setOverflow(Wt::Overflow::Auto);

    auto navigation = std::make_unique<Wt::WNavigationBar>();
    navigation.get()->addStyleClass("navbar-inverse");
    navigation.get()->setTitle("Trokam", WEB_SITE_ADDR);
    navigation.get()->setResponsive(true);

    auto contentsStack = std::make_unique<Wt::WStackedWidget>();
    Wt::WAnimation animation(Wt::AnimationEffect::Fade,
                             Wt::TimingFunction::Linear,
                             200);
    contentsStack.get()->setTransitionAnimation(animation, true);

    /**
     * Setup the top-level menu
     **/
    auto menu = std::make_unique<Wt::WMenu>(contentsStack.get());
    menu->setInternalPathEnabled("/");

    /**
     * Add each one of the menu items in the menu.
     **/
    navigation.get()->addMenu(std::move(menu));

    /**
     * Add it all inside a layout.
     **/
    auto layout = this->setLayout(std::make_unique<Wt::WVBoxLayout>());
    layout->setPreferredImplementation(Wt::LayoutImplementation::JavaScript);
    layout->addWidget(std::move(navigation), 0);
    layout->addWidget(std::move(contentsStack), 1);
    layout->setContentsMargins(0, 0, 0, 0);
}

Trokam::TopWindow::~TopWindow()
{}

void Trokam::TopWindow::addMenuItem(Wt::WMenu *menu,
                                    const char *itemKey,
		                            const char *contentsKey,
                                    const char *path)
{
    auto container = std::make_unique<Wt::WContainerWidget>();

    container->setOverflow(Wt::Overflow::Auto);
    container->addWidget(std::make_unique<Wt::WText>(Wt::WString::tr(contentsKey) +
                                                     Wt::WString::tr("footlinks")));

    auto wrap = std::make_unique<Wt::WMenuItem>(Wt::WString::tr(itemKey),
                                                std::move(container));
    auto item = menu->addItem(std::move(wrap));
    item->setPathComponent(path);

}
