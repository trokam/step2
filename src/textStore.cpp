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
#include "textProcessing.h"

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
    to.relevanceInBody= 1;
    to.relevanceInUrl= 1;
    to.relevanceInTitle= 1;
    textCollection.push_back(to);
}

void Trokam::TextStore::show(const int &value)
{
    if(sorted == false)
    {
        sortRelevanceTotal();
        sorted= true;
    }

    int count= 0;
    for(std::vector<Trokam::TextOcc>::iterator it= textCollection.begin(); it!=textCollection.end(); ++it)
    {
        std::string text= "[" + it->text + "]";
        text= Trokam::TextProcessing::rightPadding(text, 50);

        std::cout << "seq: " << text
                  << "\tbody: [" << it->relevanceInBody
                  << "]\turl: [" << it->relevanceInUrl
                  << "]\ttitle: [" << it->relevanceInTitle
                  << "]\ttotal: [" << it->relevanceTotal << "]\n";
        count++;

        if(count > value)
        {
            return;
        }
    }
}

int Trokam::TextStore::size() const
{
    return textCollection.size();
}

Trokam::TextOcc Trokam::TextStore::get(const int &id) const
{
    return textCollection[id];
}

void Trokam::TextStore::sortRelevanceBody()
{
    std::sort(textCollection.begin(),
              textCollection.end(),
              [](Trokam::TextOcc a, Trokam::TextOcc b)
                {
                    return (a.relevanceInBody > b.relevanceInBody);
                }
             );
}

void Trokam::TextStore::sortRelevanceTotal()
{
    std::sort(textCollection.begin(),
              textCollection.end(),
              [](Trokam::TextOcc a, Trokam::TextOcc b)
                {
                    return (a.relevanceTotal > b.relevanceTotal);
                }
             );
}

void Trokam::TextStore::setRelevance(const int &total,
                                     const std::string &title,
                                     const std::string &url)
{
    float length= float(total);
    for(std::vector<Trokam::TextOcc>::iterator it= textCollection.begin(); it!=textCollection.end(); ++it)
    {
        float value= float(1000 * it->occurrence * it->text.length())/length;
        it->relevanceInBody= int(value);
        it->relevanceInTitle= Trokam::TextProcessing::relevance(title, it->text);
        it->relevanceInUrl= Trokam::TextProcessing::relevance(url, it->text);
        it->relevanceTotal= it->relevanceInBody * it->relevanceInTitle * it->relevanceInUrl;
    }
}
