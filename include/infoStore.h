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

#ifndef TROKAM_INFOSTORE_H
#define TROKAM_INFOSTORE_H

/// C++
#include <string>
#include <vector>

/// Trokam
#include "bundle.h"
#include "data.h"
#include "reporting.h"
#include "textProcessing.h"
#include "pageProcessing.h"
#include "textStore.h"
#include "options.h"
#include "postgresql.h"

namespace Trokam
{
    class InfoStore
    {
        public:

            InfoStore(const Trokam::Options &value);

            bool getUrlForProcessing(std::vector<Trokam::PageInfo> &pages,
                                     int &level,
                                     const int &periodBeginning);

            bool insertPage(const std::string &url,
                            const int &level,
                            const int &type);

            void insertPage(const int &index,
                            const int &level,
                                  Trokam::PageInfo &info);

            void setPageState(const int &index,
                              const int &error);

            void getSequences(const std::string &partial,
                                    Trokam::Bundle<Trokam::Sequence> &results);

            void getPages(const std::vector<int> seqIndex,
                          Trokam::Bundle<Trokam::Page> &results);

            void getFileSnippet(const std::string &terms,
                                const int &pageIndex,
                                      std::string &snippet);

            void fillProtocol();

            bool insertDomainToIndex(const std::string &text);

            bool insertSeedPage(const std::string &text);

            bool insertPageToIndex(const std::string &text);

            void setAllNotProcessing();

        private:

            Trokam::Options settings;
            Trokam::Postgresql db;
            Trokam::Reporting msg;

            int getDomainIndex(const std::string &domain);

            int insertDomain(const std::string &text,
                             const int &type);

            int getProtocolIndex(const std::string &protocol);

            void insertSequences(const int &index,
                                       Trokam::TextStore &bag);

            void insertSeqInPage(const int &pageIndex,
                                       Trokam::TextStore &bag);

            void insertTraits(const int &index,
                              const Trokam::PageInfo &info);

            void insertUrls(const Trokam::DifferentStrings &links,
                            const int &level);

            void insertUrls(const Trokam::PageInfo &page,
                            const int &level);

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
