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

#ifndef TROKAM_TEXT_PROCESSING_H
#define TROKAM_TEXT_PROCESSING_H

/// C++
#include <string>

/// Boost
#include <boost/scoped_ptr.hpp>

/// Trokam
#include "textStore.h"

namespace Trokam
{
    class TextProcessing
    {
        public:

            /**
             * Extract the sequences of text.
             **/
            static void extractSequences(std::string &content,
                                         boost::scoped_ptr<Trokam::TextStore> &store);

            /**
             * Extract the sequences of text.
             **/
            static void extractUrls(std::string &links,
                                    boost::scoped_ptr<Trokam::TextStore> &urlBag);

            /**
             * Split the URL into protocol, domain and path.
             **/
            static bool splitUrl(const std::string &url,
                                       std::string &protocol,
                                       std::string &domain,
                                       std::string &path);


            static std::string leftPadding(const std::string &text,
                                           const int &total_length,
                                           const std::string &fill= " ");

            static std::string rightPadding(const std::string &text,
                                            const int &total_length,
                                            const std::string &fill= " ");
    };
}

#endif /// TROKAM_TEXT_PROCESSING_H
