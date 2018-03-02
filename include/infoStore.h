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
#include "reporting.h"
#include "textProcessing.h"
#include "pageProcessing.h"
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

            void getUrlForProcessing(Trokam::PageInfo &page,
                                     int &level);

            bool insertPage(const std::string &url,
                            const int &level);

            void insertPage(const int &index,
                            const int &level,
                            const Trokam::PageInfo &info);

            void setPageState(const int &index,
                              const int &error);

        private:

            Trokam::Options settings;
            Trokam::Postgresql db;
            Trokam::Reporting msg;

            int getDomainIndex(const std::string &domain);

            void insertSequences(const int &index,
                                 const Trokam::TextStore &bag);

            void insertSequenceOccurrence(const Trokam::TextStore &bag);

            void updateSequenceOccurrence(const Trokam::TextStore &bag);

            void insertTraits(const int &index,
                              const Trokam::PageInfo &info);

            void insertUrls(const std::string &links,
                            const int &level);

            void insertUrls(const Trokam::DifferentStrings &links,
                            const int &level);

            void insertUrls(const Trokam::PageInfo &page,
                            const int &level);

            void deleteSeqOccOfPage(const int &pageIndex);

            std::string generateSentenceOccInPage(const int &pageIndex,
                                                  const Trokam::TextOcc &to);

            std::string generateSentenceInsertSeqOcc(const Trokam::TextOcc &to);

            std::string generateSentenceUpdateOcc(const Trokam::TextOcc &to);

            void setCrunched(const int &pageIndex,
                             const Trokam::PageInfo &info);

            void savePageContent(const int &pageIndex,
                                 const std::string &value);

            void getDirFile(const int &pageIndex,
                                  std::string &directory,
                                  std::string &file);
    };
}

#endif  /// TROKAM_INFOSTORE_H
