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
        /**
         * File location.
         **/
        // std::string filename= "/tmp/someText.txt";

        std::string content, links;

        /**
         * Get an URL from the database.
         **/
        int index;
        std::string url;
        pi.getUrlForProcessing(settings.level(), index, url);

        std::cout << "\nindex: " << index << " url: '" << url << "'" << std::endl;

        if((index != -1) && (url != ""))
        {
            /**
             * Fetch the URL content.
             **/
            Trokam::Web w(settings);
            w.fetch(url, content, links);

            /**
             * Creates bags to keep the sequences and links.
             **/
            boost::scoped_ptr<Trokam::TextStore> seqBag(new Trokam::TextStore);
            boost::scoped_ptr<Trokam::TextStore> urlBag(new Trokam::TextStore);

            /**
             * Extract text sequences from file and put into the store.
             **/
            // Trokam::TextProcessing tp(filename);
            // tp.sequences(store);

            /**
             * Extract text sequences and their number of occurrences
             * from page content and put into the bag.
             **/
            Trokam::TextProcessing::extractSequences(content, seqBag);

            /**
             * Extract urls from the links and put into the bag.
             **/
            Trokam::TextProcessing::extractUrls(links, urlBag);

            /**
             * Show the most frequent sequences in the file.
             **/
            // store->show(100);
            std::cout << "content length is " << content.length() << "\n";
            std::cout << "extracted " << seqBag->size() << " sequences\n";
            std::cout << "extracted " << urlBag->size() << " urls\n";

            /**
             * Insert the sequences, urls and content in the database.
             **/
            //pi.insertSequences(store);
            pi.insertPage(index, seqBag, urlBag, content);

            pages++;
        }
        else
        {
            exit(1);
        }
    }
}
