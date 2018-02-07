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
#include "infoStore.h"
#include "textProcessing.h"
#include "textStore.h"
#include "web.h"

Trokam::Cruncher::Cruncher(const Trokam::Options &value): settings(value)
{}

void Trokam::Cruncher::run()
{
    Trokam::InfoStore pi(settings);
    int pages= 0;

    while(pages < settings.pagesLimit())
    {
        std::string content, links;

        /**
         * Get an URL from the database.
         **/
        int index, level;
        std::string url;
        pi.getUrlForProcessing(index, url, level);

        std::cout << "\nprocessing index: " << index
                               << " url: '" << url
                               << "' level: " << level
                               << "'" << std::endl;

        if((index != -1) || (url != ""))
        {
            /**
             * Fetch the URL content.
             **/
            Trokam::Web w(settings);
            w.fetch(url, content, links);

            /**
             * Creates an empty bag to keep the sequences.
             * Then, extract text sequences from content and their
             * number of occurrences from page content
             * and put into the bag.
             **/
            boost::scoped_ptr<Trokam::TextStore> seqBag(new Trokam::TextStore);
            Trokam::TextProcessing::extractSequences(content, seqBag);

            /**
             * Show the most frequent sequences in the file.
             **/
            std::cout << "content length: " << content.length() << "\n";
            std::cout << "extracted: " << seqBag->size() << " sequences\n";
            std::cout << "complexity: " << float(seqBag->size())/float(content.length()) << "\n";

            /**
             * Insert the page's sequences, urls and content in the infoStore.
             **/
            pi.insertPage(index, seqBag, links, content, level);

            pages++;
        }
        else
        {
            exit(1);
        }
    }
}
