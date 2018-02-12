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

#ifndef TROKAM_COMMON_H
#define TROKAM_COMMON_H

/**
 * Configuration files.
 **/

const std::string CONFIG_FILE= "/usr/local/etc/trokam.cfg";

/**
 * String literals.
 **/

const std::string HTTP= "http://";
const std::string HTTPS= "https://";
const std::string SLASH= "/";
const std::string OUT_OF_TIME= "2000-01-01";
const std::string EMPTY= "";
const std::string HTML= "text/html";

/**
 * Errors.
 **/

const std::string ERROR_NO_PAGES_TO_PROCESS=     "error: there are no pages to process.";
const std::string ERROR_DOWNLOAD_FAIL=           "error: download fail";
const std::string ERROR_TYPE_NOT_SUPPORTED=      "error: file type not supported.";
const std::string ERROR_EXTRACTING_CONTENT_FAIL= "error: extracting content fail";
const std::string ERROR_EXTRACTING_LINKS_FAIL=   "error: extracting links fail";


/**
 * State of page processing.
 **/

const int UNKNOWN =                 -1;
const int SUCCESS =                  0;
const int NO_PAGES_TO_PROCESS =      1;
const int DOWNLOAD_FAIL =            2;
const int TYPE_NOT_SUPPORTED =       3;
const int EXTRACTING_CONTENT_FAIL =  4;
const int EXTRACTING_LINKS_FAIL =    5;


#endif /// TROKAM_COMMON_H
