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
#include <iostream>
#include <exception>

/// Trokam
#include "common.h"
#include "exception.h"
#include "fileOps.h"
#include "options.h"
#include "textProcessing.h"
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
    const std::string logFile= settings.workingDir() + "/log";

    /**
     * Deleting them if they exists.
     **/
    Trokam::FileOps::rmFile(rawFile);
    Trokam::FileOps::rmFile(contentFile);
    Trokam::FileOps::rmFile(linksFile);
    Trokam::FileOps::rmFile(logFile);

    /**
     * Download the web page and put it in a file.
     **/
    command= "wget -o " + logFile + " --header=\"User-Agent: Trokambot/1.0 (+http://trokam.com/bot.html)\" --timeout=20 --tries=1 --local-encoding=UTF8 -O " + rawFile + " -k \"" + url + "\"";

    std::cout << "command: " << command << "\n";

    status= system(command.c_str());
    if(status!=0)
    {
        throw Trokam::Exception(DOWNLOAD_FAIL, info);
    }

    const std::string urlForwarded = Trokam::TextProcessing::extractLocation(logFile);
    std::cout << "urlForwarded: '" << urlForwarded << "'\n";
    info.urlForwarded = urlForwarded;

    std::string type= Trokam::FileOps::type(rawFile);
    info.type= type.substr(0,8);
    std::cout << "file type: " << type << "\n";

    if (type.find("gzip") != std::string::npos)
    {
        std::string command;

        command= "mv " + rawFile + " " + rawFile + ".gz";
        std::cout << "command: " << command << "\n";
        status= system(command.c_str());

        command= "gunzip " + rawFile + ".gz";
        std::cout << "command: " << command << "\n";
        status= system(command.c_str());

        std::string type= Trokam::FileOps::type(rawFile);
        info.type= type.substr(0,8);
        std::cout << "file type: " << type << "\n";
    }

    if ((info.type == HTML) || (info.type == XML))
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
            throw Trokam::Exception(EXTRACTING_CONTENT_FAIL, info);
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
            throw Trokam::Exception(EXTRACTING_LINKS_FAIL, info);
        }
    }
    else
    {
        throw Trokam::Exception(TYPE_NOT_SUPPORTED, info);
    }
}
