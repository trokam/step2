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
#include <exception>

/// Trokam
#include "common.h"
#include "fileOps.h"
#include "options.h"
#include "web.h"

Trokam::Web::Web(const Trokam::Options &value): settings(value)
{}

void Trokam::Web::fetch(const std::string &url,
                              Trokam::PageInfo &info)
{
    std::string command;
    int status;

    /**
     * Setting up the files.
     **/
    const std::string rawFile= settings.workingDir() + "/raw";
    const std::string contentFile= settings.workingDir() + "/content";
    const std::string linksFile= settings.workingDir() + "/links";

    /**
     * Deleting them if they exists.
     **/
    Trokam::FileOps::rmFile(rawFile);
    Trokam::FileOps::rmFile(contentFile);
    Trokam::FileOps::rmFile(linksFile);

    /**
     * Download the web page and put it in a file.
     **/
    command= "wget -q --timeout=60 -O " + rawFile + " -k \"" + url + "\"";
    status= system(command.c_str());
    if(status!=0)
    {
        throw DOWNLOAD_FAIL;
    }

    std::string type= Trokam::FileOps::type(rawFile);
    info.type= type.substr(0,9);

    if(info.type == HTML)
    {
        Trokam::FileOps::read(rawFile, info.raw);

        /**
         * Extract the user-readable content.
         **/
        command= "lynx -dump -force_html -nolist " + rawFile + " >  " + contentFile;
        status= system(command.c_str());
        if(status==0)
        {
            Trokam::FileOps::read(contentFile, info.content);
        }
        else
        {
            throw EXTRACTING_CONTENT_FAIL;
        }

        /**
         * Extract the links.
         **/
        command= "lynx -dump -force_html -listonly -nonumbers " + rawFile + " > " + linksFile;
        status= system(command.c_str());
        if(status==0)
        {
            Trokam::FileOps::read(linksFile, info.links);
        }
        else
        {
            throw EXTRACTING_LINKS_FAIL;
        }
    }
    else
    {
        throw TYPE_NOT_SUPPORTED;
    }
}
