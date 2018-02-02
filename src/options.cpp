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

/// C
#include <stdlib.h>
#include <unistd.h>

/// C++
#include <string>
#include <iostream>

/// Boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>

/// Trokam
#include "fileOps.h"
#include "options.h"

Trokam::Options::Options()
{
    optPagesLimit= 1;
    optLevel= 0;
    createWorkingDirectory();
}

Trokam::Options::Options(int argc, const char* argv[])
{
    optPagesLimit= 1;
    optLevel= 0;
    createWorkingDirectory();

    /**
     * Program Options.
     **/
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help",       "Produce help message")
        ("pages-limit", boost::program_options::value<int>(),         "Number of pages to process.")
        ("level",       boost::program_options::value<int>(),         "Level in the page table.")
        ("seeds-file",  boost::program_options::value<std::string>(), "File with first URLs.");
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    /**
     * Option 'help'.
     **/
    if(vm.count("help"))
    {
        std::cout << desc << std::endl;
        exit(0);
    }

    /**
     * Action for 'pages-limit'.
     **/
    if(vm.count("pages-limit"))
    {
		optPagesLimit= vm["pages-limit"].as<int>();
    }

    /**
     * Action for 'pages-limit'.
     **/
    if(vm.count("level"))
    {
		optLevel= vm["level"].as<int>();
    }

    /**
     * Action for 'pages-limit'.
     **/
    if(vm.count("seeds-file"))
    {
		optSeedsFile= vm["seeds-file"].as<std::string>();
    }
}

void Trokam::Options::readSettings(const std::string &filename)
{
    try
    {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(filename, pt);
        optDbHost= pt.get<std::string>("database.host");
        optDbName= pt.get<std::string>("database.name");
        optDbUser= pt.get<std::string>("database.user");
        optDbPass= pt.get<std::string>("database.pass");
    }
    catch(const std::exception& e)
    {
        std::cerr << "settings file not found.\n";
        std::cerr << e.what() << std::endl;
    }
}


Trokam::Options::Options(const Trokam::Options &opt)
{
    optDbHost= opt.dbHost();
    optDbName= opt.dbName();
    optDbUser= opt.dbUser();
    optDbPass= opt.dbPass();
    optWorkingDir= opt.workingDir();
    optSeedsFile= opt.seedsFile();
    optPagesLimit = opt.pagesLimit();
    optLevel = opt.level();
}

/**
 * Copy constructor.
 **/
Trokam::Options& Trokam::Options::operator= (const Trokam::Options &opt)
{
    /**
     * Avoid self-assignment.
     */
    if(this == &opt)
    {
        return *this;
    }

    /**
     * Copy data.
     */
    optDbHost= opt.dbHost();
    optDbName= opt.dbName();
    optDbUser= opt.dbUser();
    optDbPass= opt.dbPass();
    optWorkingDir= opt.workingDir();
    optSeedsFile= opt.seedsFile();
    optPagesLimit = opt.pagesLimit();
    optLevel = opt.level();

    /**
     * Return 'this' object.
     */
    return *this;
}

std::string Trokam::Options::dbHost() const
{
    return optDbHost;
}

std::string Trokam::Options::dbName() const
{
    return optDbName;
}

std::string Trokam::Options::dbUser() const
{
    return optDbUser;
}

std::string Trokam::Options::dbPass() const
{
    return optDbPass;
}

std::string Trokam::Options::workingDir() const
{
    return optWorkingDir;
}

std::string Trokam::Options::seedsFile() const
{
    return optSeedsFile;
}

int Trokam::Options::pagesLimit() const
{
    return optPagesLimit;
}

int Trokam::Options::level() const
{
    return optLevel;
}

void Trokam::Options::createWorkingDirectory()
{
    int pid= getpid();
    optWorkingDir= "/tmp/trokam/cruncher_" + std::to_string(pid);
    Trokam::FileOps::mkDir(optWorkingDir);
}

void Trokam::Options::deleteWorkingDirectory()
{
    std::cout << "------ deleting: '" << optWorkingDir << "'\n";
    Trokam::FileOps::rmDir(optWorkingDir);
}
