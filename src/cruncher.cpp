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

/// C++
#include <iostream>
#include <chrono>
#include <thread>

/// Boost
#include <boost/scoped_ptr.hpp>

/// Trokam
#include "cruncher.h"
#include "control.h"
#include "common.h"
#include "exception.h"
#include "pageInfo.h"
#include "pageProcessing.h"
#include "textStore.h"
#include "web.h"

Trokam::Cruncher::Cruncher(const Trokam::Options &value): settings(value),
                                                          storage(settings),
                                                          msg(settings)
{
    count= 0;
}

void Trokam::Cruncher::run()
{
    Trokam::Control control(settings);

    if(settings.pagesLimit() != 0)
    {
        while(count < settings.pagesLimit())
        {
            process();
        }
    }
    else
    {
        while(control.run())
        {
            if(control.active())
            {
                process();
            }
            else
            {
                std::cout << "pause ..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }
        std::cout << "commanded to exit ..." << std::endl;
    }
}

void Trokam::Cruncher::process()
{
    try
    {
        /**
         * Page counter is incremented at the begining because
         * the attempt to process a page is taken into the count.
         **/
        count++;

        /**
         * Clean variables for every processing page.
         **/
        int level;
        Trokam::PageInfo info;

        /**
         * Get an URL from the database and report.
         **/
        storage.getUrlForProcessing(info, level);
        msg.processingNow(count, info.index, info.url, level);

        /**
         * Fetch the URL content.
         **/
        Trokam::Web w(settings);
        w.fetch(info.url, info);

        /**
         * Extract page information.
         **/
        Trokam::PageProcessing::extractPageInfo(info);

        /**
         * Report on the page just processed and
         * insert its information in the store.
         **/
        msg.processingOutcome(info);
        storage.insertPage(info.index, level, info);
    }
    catch(const Trokam::Exception &e)
    {
        Trokam::Reporting::showGeneralError(e.getError());
        action(e.getIndex(), e.getError());
    }
}

void Trokam::Cruncher::action(const int &index,
                              const int &error)
{
    if(error == NO_PAGES_TO_PROCESS)
    {
        /**
         * If there are not URLs to process then
         * there is nothing to do. Probably, the
         * database must be initilised.
         **/
        exit(1);
    }
    else
    {
        /**
         * Saving the error code in the database
         * to know what happened processing this page.
         **/
        storage.setPageState(index, error);
    }
}
