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

/// Trokam
#include "common.h"
#include "pageInfo.h"
#include "reporting.h"
#include "textProcessing.h"
#include "textStore.h"

void Trokam::PageProcessing::extractPageInfo(Trokam::PageInfo &info)
{
    Trokam::TextProcessing::extractTitle(info.raw, info.title);
    const int textLength= Trokam::TextProcessing::extractSequences(info.content, info.sequences);
    Trokam::TextProcessing::extractURLs(info.links, info.urlBag);
    computeComplexity(info);

    if (info.urlForwarded != VOID)
    {
        info.sequences.setRelevance(textLength, info.title, info.urlForwarded);
    }
    else
    {
        info.sequences.setRelevance(textLength, info.title, info.urlOrigin);
    }

    info.sequences.show();
}


void Trokam::PageProcessing::computeComplexity(Trokam::PageInfo &info)
{
    info.size= info.content.length();

    if(info.size != 0)
    {
        info.complexity= float(info.sequences.size())/float(info.size);
    }
    else
    {
        info.complexity= 0;
    }
}

