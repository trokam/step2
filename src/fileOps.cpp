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
#include <fstream>
#include <string>

/// Magic
#include <magic.h>

/// Trokam
#include "fileOps.h"

std::string Trokam::FileOps::read(const std::string &filename)
{
    std::string content;
    std::ifstream inputFile(filename.c_str(), std::ios::in | std::ios::binary);
    if(inputFile)
    {
        inputFile.seekg(0, std::ios::end);
        content.resize(inputFile.tellg());
        inputFile.seekg(0, std::ios::beg);
        inputFile.read(&content[0], content.size());
        inputFile.close();
    }
    return content;
}

void Trokam::FileOps::read(const std::string &filename,
                                 std::string &content)
{
    std::ifstream inputFile(filename.c_str(), std::ios::in | std::ios::binary);
    if(inputFile)
    {
        inputFile.seekg(0, std::ios::end);
        content.resize(inputFile.tellg());
        inputFile.seekg(0, std::ios::beg);
        inputFile.read(&content[0], content.size());
        inputFile.close();
    }
}

bool Trokam::FileOps::isEmpty(const std::string &filename)
{
    std::ifstream inputFile(filename.c_str());
    bool state= (inputFile.peek() == std::ifstream::traits_type::eof());
    return state;
}

void Trokam::FileOps::save(const std::string &filename,
                           const std::string &content)
{
    std::ofstream out(filename.c_str());
    out << content;
    out.close();
}

void Trokam::FileOps::rmFile(const std::string &filename)
{
    std::string command= "rm -f " + filename;
    const int status= system(command.c_str());
    if(status != 0)
    {
        std::cerr << "failure on deleting file: " << filename << "\n";
    }
}

void Trokam::FileOps::rmDir(const std::string &dirname)
{
    std::string command= "rm -rf " +  dirname;
    const int status= system(command.c_str());
    if(status != 0)
    {
        std::cerr << "failure on deleting directory: " << dirname << "\n";
    }
}

void Trokam::FileOps::mkDir(const std::string &dirname)
{
    std::string command= "mkdir -p " +  dirname;
    const int status= system(command.c_str());
    if(status != 0)
    {
        std::cerr << "failure on creating directory: " << dirname << "\n";
    }
}

std::string Trokam::FileOps::type(const std::string &file)
{
    magic_t mg= magic_open(MAGIC_CONTINUE|MAGIC_ERROR|MAGIC_MIME);
    magic_load(mg, NULL);
    std::string type(magic_file(mg, file.c_str()));
    magic_close(mg);

    return type;
}
