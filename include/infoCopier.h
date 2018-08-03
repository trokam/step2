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

#ifndef TROKAM_INFOCOPIER_H
#define TROKAM_INFOCOPIER_H

/// C++
#include <string>

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
    class InfoCopier
    {
        public:

            InfoCopier(const Trokam::Options &value);

            void copySequences();
            void copyFindings();

        private:

            Trokam::Options settings;
            Trokam::Postgresql dbOrigin;
            Trokam::Postgresql dbDepot;
            Trokam::Reporting msg;

            int getSequencesTotal(const int &maxInserted);

            int getFindingsTotal(const int &maxCrunched);

            void getFileSnippet(const std::string &terms,
                                const std::string &pageIndex,
                                      std::string &snippet);

            void getDirFile(const std::string &index,
                                  std::string &directory,
                                  std::string &file);

            int getMaxOnDepot();

            int getMaxFindingCrunchedOnDepot();
    };
}

#endif  /// TROKAM_INFOCOPIER_H
