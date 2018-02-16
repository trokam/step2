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
#include <Wt/WApplication.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WEnvironment.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WCssTheme.h>
#include <Wt/WLink.h>

/// Trokam
#include "common.h"
#include "appGenerator.h"
#include "topWindow.h"

Trokam::AppGenerator::AppGenerator(Trokam::Options &opt): commonResources(new Trokam::SharedResources(opt))
{}

std::unique_ptr<Wt::WApplication> Trokam::AppGenerator::createApplication(const Wt::WEnvironment& env)
{
    /**
     * Instantiate the application. object.
     **/
    auto app = Wt::cpp14::make_unique<Wt::WApplication>(env);

    Wt::log("info") << "appRoot: '" << app->appRoot() << "'";  

    /**
     * Verifying approot directory.
     **/
    if (app->appRoot().empty())
    {
        Wt::log("error") << WARNING_APPROOT_EMPTY;
    }

    /**
     * Using Bootstrap CSS version 3.
     **/
    auto bootstrapTheme = std::make_shared<Wt::WBootstrapTheme>();
    bootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
    bootstrapTheme->setResponsive(true);
    app->setTheme(bootstrapTheme);
    app->useStyleSheet(Wt::WLink("http://project.trokam.com/docroot/style/bootstrap.css"));
    app->useStyleSheet(Wt::WLink("http://project.trokam.com/docroot/style/moz-transitions.css"));
    app->useStyleSheet(Wt::WLink("http://project.trokam.com/docroot/style/wt.css"));
    app->useStyleSheet(Wt::WLink("http://project.trokam.com/docroot/style/bootstrap-theme.min.css"));

    /**
     * Load the default bootstrap3 theme from the web.
     **/
    app->useStyleSheet(Wt::WLink(BOOTSTRAP_MIN_CCS));
    app->useStyleSheet(Wt::WLink(BOOTSTRAP_THEME_MIN_CCS));

    /**
     * Load text bundles.
     **/
    app->messageResourceBundle().use(app->appRoot() + "trokam_project");

    /**
     * Add the only one widget in the application layout.
     **/
    auto layout = app->root()->setLayout(Wt::cpp14::make_unique<Wt::WHBoxLayout>());
    layout->setPreferredImplementation(Wt::LayoutImplementation::JavaScript);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(Wt::cpp14::make_unique<Trokam::TopWindow>(commonResources, app.get()));

    /**
     * Set web site title.
     **/
    app->setTitle("Trokam Project");

    /**
     * Additional stylesheets.
     **/
    app->useStyleSheet(Wt::WLink("http://project.trokam.com/docroot/style/trokam.css"));
    app->useStyleSheet(Wt::WLink("http://project.trokam.com/docroot/style/bootstrap.css"));
    app->useStyleSheet(Wt::WLink("http://project.trokam.com/docroot/style/bootstrap.min.css"));
    app->useStyleSheet(Wt::WLink("http://project.trokam.com/docroot/style/bootstrap-theme.css"));
    app->useStyleSheet(Wt::WLink("http://project.trokam.com/docroot/style/bootstrap-theme.min.css"));

    return app;
}

