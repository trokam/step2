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
#include "fileOps.h"
#include "options.h"
#include "web.h"

Trokam::Web::Web(const Trokam::Options &value): settings(value)
{}

void Trokam::Web::fetch(const std::string &url,
                              std::string &content,
                              std::string &links)
{
    std::string command;

    /**
     *
     **/
    std::cout << "getting the plain text ..." << std::endl;

    std::string downloadPlain= settings.workingDir() + "/plain.txt";
    Trokam::FileOps::rmFile(downloadPlain);
    command= "lynx -dump -force_html -nolist " + url + " >  " + downloadPlain;
    system(command.c_str());
    Trokam::FileOps::read(downloadPlain, content);

    /**
     *
     **/
    std::cout << "getting the links ..." << std::endl;

    std::string downloadLinks= settings.workingDir() + "/links.txt";
    Trokam::FileOps::rmFile(downloadLinks);
    command= "lynx -dump -force_html -listonly -nonumbers " + url + " > " + downloadLinks;
    system(command.c_str());
    Trokam::FileOps::read(downloadLinks, links);
}
