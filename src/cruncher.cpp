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

/// Boost
#include <boost/scoped_ptr.hpp>

/// Trokam
#include "cruncher.h"
#include "common.h"
#include "pageProcessing.h"
#include "textStore.h"
#include "web.h"

Trokam::Cruncher::Cruncher(const Trokam::Options &value): settings(value),
                                                          storage(settings),
                                                          msg(settings)
{
    index= -1;
}

void Trokam::Cruncher::run()
{
    int pages= 0;

    while(pages < settings.pagesLimit())
    {
        try
        {
            /**
             * Page counter is incremented at the begining because
             * the attempt to process a page is taken into the count.
             **/
            pages++;

            /**
             * Clean variables for every processing page.
             **/
            std::string url;
            int level;
            Trokam::PageInfo info;
            info.size= 0;
            info.complexity= 0.0;

            /**
             * Get an URL from the database and report.
             **/
            storage.getUrlForProcessing(index, url, level);
            msg.processingNow(pages, index, url, level);


            info.url= url;

            /**
             * Fetch the URL content.
             **/
            Trokam::Web w(settings);
            w.fetch(url, info);

            /**
             * Extract page information.
             **/
            Trokam::PageProcessing::extractPageInfo(info);

            /**
             * Report on the page just processed and
             * insert its information in the store.
             **/
            msg.processingOutcome(info);
            storage.insertPage(index, level, info);
        }
        catch(const int &e)
        {
            Trokam::Reporting::showGeneralError(e);
            action(index, e);
        }
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
