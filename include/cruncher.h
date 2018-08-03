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

#ifndef TROKAM_CRUNCHER_H
#define TROKAM_CRUNCHER_H

/// C++
#include <string>

/// Trokam
#include "infoStore.h"
#include "options.h"
#include "reporting.h"

/**
 * Cruncher extract information of web pages.
 **/
namespace Trokam
{
    class Cruncher
    {
        public:

            Cruncher(const Trokam::Options &value);

            int run();

        private:

            int level;
            int count;
            int periodBeginning;
            Trokam::Options settings;
            Trokam::InfoStore storage;
            Trokam::Reporting msg;

            void process();

            void action(const int &index,
                        const int &error);
    };
}
#endif  /// TROKAM_CRUNCHER_H
