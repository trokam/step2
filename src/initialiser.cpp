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

/// C++
#include <fstream>
#include <iomanip>
#include <iostream>

#include <vector>
#include <string>

/// Boost
#include <boost/algorithm/string.hpp>

/// Trokam
#include "fileOps.h"
#include "initialiser.h"
#include "data.h"
#include "bundle.h"

Trokam::Initialiser::Initialiser(const Trokam::Options &value): settings(value),
                                                                info(value)
{}

void Trokam::Initialiser::createSchema()
{}

void Trokam::Initialiser::fillDomainToIndex()
{
    const std::string filename= settings.domainToIndexFile();

    std::cout << "file with domains to index: '" << filename << "'\n";

    if(filename == "")
    {
        std::cerr << "error: no file with domains to index provided" << std::endl;
        return;
    }

    int count= 0;
    std::ifstream source(filename.c_str());
    for(std::string line; std::getline(source, line);)
    {
        /**
         * Trimming of unwanted characters.
         **/
        boost::algorithm::trim_if(line, boost::algorithm::is_any_of(" \n\r\t\""));

        if ((line[0] != '#') && (line != ""))
        {
            /**
             ** Inserting a page, it is just the URL without content.
             **/
            if(info.insertDomainToIndex(line))
            {
                std::cout << "inserted domain to index '" <<  line << "'\n";
                count++;
            }
        }
    }
}

void Trokam::Initialiser::fillSeedPages()
{
    const std::string filename= settings.seedsFile();

    std::cout << "file with seed pages: '" << filename << "'\n";

    if(filename == "")
    {
        std::cerr << "error: no file with seed pages provided" << std::endl;
        return;
    }

    int count= 0;
    std::ifstream source(filename.c_str());
    for(std::string line; std::getline(source, line);)
    {
        /**
         * Trimming of unwanted characters.
         **/
        boost::algorithm::trim_if(line, boost::algorithm::is_any_of(" \n\r\t\""));

        if ((line[0] != '#') && (line != ""))
        {
            /**
             ** Inserting a page, it is just the URL without content.
             **/
            if(info.insertSeedPage(line))
            {
                std::cout << "inserted seed page: '" <<  line << "'\n";
                count++;
            }
        }
    }
}

void Trokam::Initialiser::fillPagesToIndex()
{
    const std::string filename= settings.pagesToIndexFile();

    std::cout << "file with pages to index: '" << filename << "'\n";

    if(filename == "")
    {
        std::cerr << "error: no file with pages to index provided" << std::endl;
        return;
    }

    int count= 0;
    std::ifstream source(filename.c_str());
    for(std::string line; std::getline(source, line);)
    {
        /**
         * Trimming of unwanted characters.
         **/
        boost::algorithm::trim_if(line, boost::algorithm::is_any_of(" \n\r\t\""));

        if ((line[0] != '#') && (line != ""))
        {
            /**
             ** Inserting a page, it is just the URL without content.
             **/
            if(info.insertPageToIndex(line))
            {
                std::cout << "inserted page to index: '" <<  line << "'\n";
                count++;
            }
        }
    }
}

void Trokam::Initialiser::fillProtocol()
{
    info.fillProtocol();
}
