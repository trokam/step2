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

/// C++
#include <iostream>

/// Trokam
#include "common.h"
#include "reporting.h"

Trokam::Reporting::Reporting(const Trokam::Options &value): settings(value)
{}

void Trokam::Reporting::processingNow(const int &page,
                                      const int &index,
                                      const std::string &url,
                                      const int &level)
{
    std::cout << "\nprocessing page: " << page
              << " index: " << index
              << " url: '" << url
              << "' level: " << level
              << "'" << std::endl;
}

void Trokam::Reporting::processingOutcome(Trokam::PageInfo &info)
{
    std::cout << "page report\n"
              << "\tcontent length: " << info.content.length() << "\n"
              << "\tsequences: " << info.sequences.size() << "\n"
              << "\turls: " << info.urlBag.size() << "\n"
              << "\tcomplexity: " << info.complexity << "\n"
              << "\ttitle: '" << info.title << "'\n";
}

void Trokam::Reporting::showGeneralError(const int &error)
{
    if(error == NO_PAGES_TO_PROCESS)
    {
        std::cerr << ERROR_NO_PAGES_TO_PROCESS << std::endl;
    }
    else if(error == DOWNLOAD_FAIL)
    {
        std::cerr << ERROR_DOWNLOAD_FAIL << std::endl;
    }
    else if(error == TYPE_NOT_SUPPORTED)
    {
        std::cerr << ERROR_TYPE_NOT_SUPPORTED << std::endl;
    }
    else if(error == EXTRACTING_CONTENT_FAIL)
    {
        std::cerr << ERROR_EXTRACTING_CONTENT_FAIL << std::endl;
    }
    else if(error == EXTRACTING_LINKS_FAIL)
    {
        std::cerr << ERROR_EXTRACTING_LINKS_FAIL << std::endl;
    }
    else if(error == COULD_NOT_CONNECT_TO_DATABASE)
    {
        std::cerr << ERROR_COULD_NOT_CONNECT_TO_DATABASE << std::endl;
    }
    else
    {
        std::cerr << "error: " << error << "\n";
    }
}

void Trokam::Reporting::showSqlError(const pqxx::sql_error &error)
{
    std::cerr << "SQL error: " << error.what() << "\n";
    std::cerr << "Query was: " << error.query() << std::endl;
}
