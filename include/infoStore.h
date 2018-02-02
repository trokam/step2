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

#ifndef TROKAM_INFOSTORE_H
#define TROKAM_INFOSTORE_H

/// C++
#include <string>

/// Boost
#include <boost/scoped_ptr.hpp>

/// Trokam
#include "textStore.h"
#include "options.h"
#include "postgresql.h"

/**
 * \brief
 *
 **/
namespace Trokam
{
    class InfoStore
    {
        public:

            InfoStore(const Trokam::Options &value);

            void getUrlForProcessing(const int &level, int &index, std::string &url);

            int getUrlIndex(const std::string &url);

            void insertUrl(const std::string &url,
                           const int &level);

            // void insertSequences(boost::scoped_ptr<Trokam::TextStore> &data);

            void insertPage(const int &index,
                            const boost::scoped_ptr<Trokam::TextStore> &seqBag,
                            const boost::scoped_ptr<Trokam::TextStore> &urlBag,
                            const std::string &content);

        private:

            Trokam::Options settings;
            Trokam::Postgresql db;

            void insertSequences(const boost::scoped_ptr<Trokam::TextStore> &bag);
            void insertUrls(const boost::scoped_ptr<Trokam::TextStore> &bag);
            void insertPageContent(const std::string &value);
            void setCrunched(const int &index);
    };
}

#endif  /// TROKAM_INFOSTORE_H
