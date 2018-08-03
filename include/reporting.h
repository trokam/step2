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

#ifndef TROKAM_REPORTING_H
#define TROKAM_REPORTING_H

/// C++
#include <string>

/// Trokam
#include "options.h"
#include "pageInfo.h"
#include "pageProcessing.h"

namespace Trokam
{
    class Reporting
    {
        public:

            Reporting(const Trokam::Options &value);

            void processingNow(const int &page,
                               const int &index,
                               const std::string &url,
                               const int &level);

            void processingOutcome(Trokam::PageInfo &info);

            static void showGeneralError(const int &error);

        private:

            Trokam::Options settings;
    };
}
#endif  /// TROKAM_REPORTING_H
