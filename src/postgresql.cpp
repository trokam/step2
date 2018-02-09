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
#include <cassert>
#include <iostream>

/// Trokam
#include "postgresql.h"

/**
 *
 **/
Trokam::Postgresql::Postgresql(const Trokam::Options &value)
{
    std::string connParameters;

    if(value.dbHost() != "")
    {
        connParameters+= "host=" + value.dbHost() + " ";
    }

    if(value.dbName() != "")
    {
        connParameters+= "dbname=" + value.dbName() + " ";
    }

    if(value.dbUser() != "")
    {
        connParameters+= "user=" + value.dbUser() + " ";
    }

    if(value.dbPass() != "")
    {
        connParameters+= "password=" + value.dbPass() + " ";
    }

    std::cout << "connectin parameters: " << connParameters << "\n";

    dbConnection= new pqxx::connection(connParameters);

    std::cout << "connected to database \n";
    std::cout << "backend version: " << dbConnection->server_version() << "\n";
    std::cout << "protocol version: " << dbConnection->protocol_version() << std::endl;
}

/**
 *
 **/
Trokam::Postgresql::~Postgresql()
{
    delete dbConnection;
}

/**
 *
 **/
void Trokam::Postgresql::execSql(const std::string &sentence)
{
    try
    {
        /**
         * Begin a transaction acting on our current connection.
         **/
        pqxx::work T(*dbConnection, "execute_no_answer");

        /**
         * Perform a query on the database, there are no results.
         **/
        T.exec(sentence);

        /**
         * Tell the transaction that it has been successful.
         * This is not really necessary if no modifications are made
         * to commit.
         **/
        T.commit();
    }
    catch(const pqxx::sql_error &e)
    {
        /**
         * The sql_error exception class gives us
         * some extra information.
         **/
        // std::cerr << "SQL error: " << e.what() << "\n";
        // std::cerr << "Query was: " << e.query() << std::endl;
        throw(e);
    }
    catch(const std::exception &e)
    {
        /**
         * All exceptions thrown by libpqxx are
         * derived from std::exception.
         **/
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch(...)
    {
        /**
         * This is unexpected.
         **/
        std::cerr << "Unhandled exception\n";
    }
}


/**
 *
 **/
void Trokam::Postgresql::execSql(const std::string &sentence,
                                 boost::scoped_ptr<pqxx::result> &answer)
{
    try
    {
        /**
         * Begin a transaction acting on our current connection.
         **/
        pqxx::work T(*dbConnection, "execute");

        /**
         * Perform a query on the database, storing result in answer.
         **/
        answer.reset(new pqxx::result(T.exec(sentence)));

        /**
         * Tell the transaction that it has been successful.
         * This is not really necessary if no modifications are made
         * to commit.
         **/
        T.commit();
    }
    catch(const pqxx::sql_error &e)
    {
        /**
         * The sql_error exception class gives us
         * some extra information.
         **/
        // std::cerr << "SQL error: " << e.what() << "\n";
        // std::cerr << "Query was: " << e.query() << std::endl;
        throw e;
    }
    catch(const std::exception &e)
    {
        /**
         * All exceptions thrown by libpqxx are
         * derived from std::exception.
         **/
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch(...)
    {
        /**
         * This is unexpected.
         **/
        std::cerr << "Unhandled exception\n";
    }
}

/**
 *
 **/
// void Trokam::Postgresql::execSevSql(const std::vector<std::string> &bundle)
void Trokam::Postgresql::execSevSql(std::vector<std::string> &bundle)
{
    try
    {
        /**
         * Begin a transaction acting on our current connection.
         **/
        pqxx::work T(*dbConnection, "execute_several_no_answer");

        /**
         *
         **/
        for(std::vector<std::string>::iterator it= bundle.begin(); it!=bundle.end(); ++it)
        {
            const std::string sentence= *it;
            T.exec(sentence);
        }

        /**
         *
         **/
        T.commit();
    }
    catch(const pqxx::sql_error &e)
    {
        /**
         * The sql_error exception class gives us
         * some extra information.
         **/
        std::cerr << "SQL error: " << e.what() << "\n";
        std::cerr << "Query was: " << e.query() << std::endl;
        throw(e);
    }
    catch(const std::exception &e)
    {
        /**
         * All exceptions thrown by libpqxx are
         * derived from std::exception.
         **/
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch(...)
    {
        /**
         * This is unexpected.
         **/
        std::cerr << "Unhandled exception\n";
    }
}

void Trokam::Postgresql::extract(const boost::scoped_ptr<pqxx::result> &answer, int &value)
{
    const pqxx::result::const_iterator col= answer->begin();
    if(col != answer->end())
    {
        value= col[0].as(int());
    }
}
