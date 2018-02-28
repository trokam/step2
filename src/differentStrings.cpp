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
#include <cmath>

/// Trokam
#include "textStore.h"
#include "differentStrings.h"

Trokam::DifferentStrings::DifferentStrings()
{}

void Trokam::DifferentStrings::insert(const std::string &text)
{
    /**
     * Compare the text in the argument to each one of the texts
     * already stored. If a match is found then it is not included.
     **/
    for(std::vector<std::string>::iterator it= textCollection.begin(); it!=textCollection.end(); ++it)
    {
        const std::string value= *it;
        if(text==value)
        {
            return;
        }
    }

    /**
     * Reaching here means that the text was not found in
     * the collection. A new one is inserted.
     **/
    textCollection.push_back(text);
}

void Trokam::DifferentStrings::show(const int &value)
{
    int count= 0;
    for(std::vector<std::string>::iterator it= textCollection.begin(); it!=textCollection.end(); ++it)
    {
        std::cout << "text: [" << *it << "]\n";
        count++;

        if(count > value)
        {
            return;
        }
    }
}

int Trokam::DifferentStrings::size() const
{
    return textCollection.size();
}

std::string Trokam::DifferentStrings::get(const int &id) const
{
    return textCollection[id];
}
