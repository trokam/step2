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

/// Boost
#include <boost/scoped_ptr.hpp>

/// Trokam
#include "cruncher.h"
#include "textProcessing.h"
#include "textStore.h"

void Trokam::Cruncher::run()
{
    /**
     * File location.
     **/
    std::string filename= "/tmp/someText.txt";

    /**
     * Creates a store to keep and sort the sequences.
     **/
    boost::scoped_ptr<Trokam::TextStore> store(new Trokam::TextStore);
   
    /**
     * Extract text sequences from file and put into the store. 
     **/
    Trokam::TextProcessing tp(filename);
    tp.sequences(store);

    /**
     * Show the most frequent sequences in the file. 
     **/
    store->show(100);
}
