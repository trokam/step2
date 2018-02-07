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

            void getUrlForProcessing(int &index,
                                     std::string &url,
                                     int &level);

            void insertPage(const std::string &url,
                            const int &level);

            void insertPage(const int &index,
                            const boost::scoped_ptr<Trokam::TextStore> &seqBag,
                            const std::string &links,
                            const std::string &content,
                            const int &level);

        private:

            Trokam::Options settings;
            Trokam::Postgresql db;

            int insertOneSequence(const std::string &value);

            void insertSevSequences(const int &index,
                                    const boost::scoped_ptr<Trokam::TextStore> &bag);

            void insertUrls(const std::string &links,
                            const int &level);

            void deleteSeqOccOfPage(const int &pageIndex);

            void insertSeqOccInPage(const int &pageIndex,
                                    const int &seqIndex,
                                    const int &occurrence);

            void setCrunched(const int &pageIndex);

            void savePageContent(const int &pageIndex,
                                 const std::string &value);

            void getDirFile(const int &pageIndex,
                                  std::string &directory,
                                  std::string &file);
    };
}

#endif  /// TROKAM_INFOSTORE_H
