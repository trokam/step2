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
#include <fstream>
#include <iomanip>
#include <iostream>

/// Boost
#include <boost/algorithm/string.hpp>

/// Trokam
#include "fileOps.h"
#include "initialiser.h"
#include "infoStore.h"

Trokam::Initialiser::Initialiser(const Trokam::Options &value): settings(value)
{}

void Trokam::Initialiser::fill()
{
    Trokam::InfoStore info(settings);
    const std::string filename= settings.seedsFile();

    std::cout << "seeds file: '" << filename << "'\n";

    if(filename == "")
    {
        std::cerr << "no seeds file provided" << std::endl;
        return;
    }

    int count= 0;
    std::ifstream source(filename.c_str());
    for(std::string line; std::getline(source, line);)
    {
        /**
         * Trimming of unwanted characters.
         **/
        boost::algorithm::trim_if(line, boost::algorithm::is_any_of(" \n\r\""));

        /**
         *
         **/
        // Verify that it starts with http or https

        /**
         ** Inserting an individual URL.
         **/
        try
        {
            info.insertUrl(line, 0);
            std::cout << "inserted URL '" <<  line << "'\n";
            count++;
        }
        catch(const std::exception& e)
        {

        }
    }
}
