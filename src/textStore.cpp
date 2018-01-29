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
#include <algorithm>
#include <iostream>

/// Trokam
#include "textStore.h"

Trokam::TextStore::TextStore()
{
    sorted= false;
}

void Trokam::TextStore::insert(const std::string &text)
{
    sorted= false;

    /**
     * Compare the text in the argument to each one of the texts
     * already stored. If a match is found then its occurrence is 
     * increased in one.
     **/
    for(std::vector<Trokam::TextOcc>::iterator it= textCollection.begin(); it!=textCollection.end(); ++it)
    {
        if(text==it->text)
        {
            it->occurrence++;
            return;
        }
    }
    
    /**
     * Reaching here means that the text was not found in
     * the collection. A new one is inserted.
     **/
    Trokam::TextOcc to;
    to.text= text;
    to.occurrence= 1;
    textCollection.push_back(to);
}

void Trokam::TextStore::show(const int &value)
{
    if(sorted == false)
    {
        sort();
        sorted= true;
    }
    
    int count= 0;
    for(std::vector<Trokam::TextOcc>::iterator it= textCollection.begin(); it!=textCollection.end(); ++it)
    {
        std::cout << "occ: [" << it->occurrence << "]\tseq: [" << it->text << "]\n";
        count++;
        if(count > value)
        {
            return;
        }
    }
}

void Trokam::TextStore::sort()
{   
    std::sort(textCollection.begin(),
              textCollection.end(),
              [](Trokam::TextOcc a, Trokam::TextOcc b)
                {
                    return (a.occurrence > b.occurrence);   
                }
             );
}
