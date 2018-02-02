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

#ifndef TROKAM_INITIALISER_H
#define TROKAM_INITIALISER_H

/// C++
#include <string>

/// Trokam
#include "options.h"

/**
 * \brief
 *
 **/
namespace Trokam
{
    class Initialiser
    {
        public:

            Initialiser(const Trokam::Options &value);
            void fill();

        private:

            Trokam::Options settings;
    };
}

#endif  /// TROKAM_INITIALISER_H
