/***********************************************************************
 *                            T R O K A M
 *                       Internet Search Engine
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
    level= 0;
    count= 0;

    int PERIOD_SIZE = 0;

    if (settings.cruncherType() == PAGE_CRUNCHER)
    {
        PERIOD_SIZE = 60 * 60 * 12;
        std::cout << "this is a *** PAGE *** cruncher.\n";
    }
    else if (settings.cruncherType() == DOMAIN_CRUNCHER)
    {
        PERIOD_SIZE = 60 * 60 * 24 * 30;
        std::cout << "this is a *** DOMAIN *** cruncher.\n";
    }
    else
    {
        std::cerr << "error: this machine is not meant to be used for crunching.\n";
        exit(1);
    }

    int seconds= std::time(nullptr);
    std::div_t d= div(seconds, PERIOD_SIZE);
    periodBeginning = d.quot * PERIOD_SIZE;

    std::cout << "seconds since 1970: " << seconds << "\n";
    std::cout << "period beginning:   " << periodBeginning << "\n";
}

int Trokam::Cruncher::run()
{
    Trokam::Control control(settings);

    storage.setAllNotProcessing();

    if(settings.pagesLimit() != 0)
    {
        while(count < settings.pagesLimit())
        {
            process();
        }
        return 0;
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
        return 1;
    }
}

void Trokam::Cruncher::process()
{
    Trokam::Control control(settings);

    try
    {
        /**
         * Get an URL from the database and report.
         **/
        std::vector<Trokam::PageInfo> pages;

        if (storage.getUrlForProcessing(pages, level, periodBeginning))
        {
            for(size_t i=0; i<pages.size(); i++)
            {
                try
                {
                    Trokam::PageInfo info= pages[i];

                    /**
                     * The attempt to process a page is taken into the count.
                     **/
                    count++;

                    msg.processingNow(count, info.index, info.urlOrigin, level);

                    /**
                     * Fetch the URL content.
                     **/
                    Trokam::Web w(settings);
                    w.fetch(info.urlOrigin, info);

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

                    std::cout << "done\n\n" << std::endl;
                }
                catch(const Trokam::Exception &e)
                {
                    Trokam::Reporting::showGeneralError(e.getError());
                    action(e.getIndex(), e.getError());
                }
            }

            if (control.run() == false)
            {
                std::cout << "commanded to exit ..." << std::endl;
                exit(1);
            }
        }
        else
        {
            std::cout << "no pages avaliable for crunching at level: " << level << "\n";
            level++;
            if (level > 5)
            {
                exit(0);
            }
        }
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
