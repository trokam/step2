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

#ifndef TROKAM_PAGE_PROCESSING_H
#define TROKAM_PAGE_PROCESSING_H

/// C++
#include <string>

/// Trokam
#include "options.h"
#include "pageInfo.h"
#include "textProcessing.h"

/**
 * \brief
 *
 **/
namespace Trokam
{
    class PageProcessing
    {
        public:

            /**
             * Extract information of web page.
             **/
            static void extractPageInfo(Trokam::PageInfo &info);

        private:

            static void computeComplexity(Trokam::PageInfo &info);
    };
}
#endif  /// TROKAM_PAGE_PROCESSING_H
