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

#ifndef TROKAM_FILE_OPS_H
#define TROKAM_FILE_OPS_H

/// C++
#include <string>

/**
 * File Operations.
 **/
namespace Trokam
{
    class FileOps
    {
        public:
            /**
             * Return the content of the file as a string.
             **/
            static std::string read(const std::string &filename);

            /**
             * Put all the file in content.
             **/
            static void read(const std::string &filename,
                                   std::string &content);

            /**
             * Return if the file is empty.
             **/
            static bool isEmpty(const std::string &filename);

            /**
             * Write the content in file.
             **/
            static void save(const std::string &filename,
                             const std::string &content);

            /**
             * Delete a file.
             **/
            static void rmFile(const std::string &filename);

            /**
             * Delete a directory.
             **/
            static void rmDir(const std::string &dirname);
            
            /**
             * Creates a directory.
             **/
            static void mkDir(const std::string &dirname);
    };
}

#endif  /// TROKAM_FILE_OPS_H
