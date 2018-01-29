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
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

/// Trokam
#include "fileOps.h"
#include "textProcessing.h"

Trokam::TextProcessing::TextProcessing(const std::string &filename)
{
    content= Trokam::FileOps::read(filename);
 
    /**
     * All text is converted to lower case.
     **/
    boost::algorithm::to_lower(content);
}

void Trokam::TextProcessing::sequences(boost::scoped_ptr<Trokam::TextStore> &store)
{
    boost::tokenizer<> tok(content);
    for(boost::tokenizer<>::iterator it= tok.begin(); it!=tok.end(); it++)
    {
        int limit= 10;
        
        for(int maxLen= 1; maxLen<=limit; maxLen++)
        {
            int len=0;
            std::string sequence;

            boost::tokenizer<>::iterator sec;
            for(sec= it; ((sec!=tok.end()) && (len<maxLen)); sec++)
            {
                std::string token= *sec;
                sequence+= token + " ";
                len++;
            }
            
            boost::algorithm::trim_if(sequence, boost::algorithm::is_any_of(" \n\r"));
            store->insert(sequence);

            /**
             * If the iterator reaches the end of the content,
             * then jump out of this loop otherwise appear
             * false repeated sequences.
             **/
            if(sec==tok.end())
            {
                break;
            }
        }
    }
}

