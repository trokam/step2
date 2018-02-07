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
#include <string>
#include <iostream>

/// Boost
#include <boost/algorithm/string.hpp>

/// Trokam
#include "common.h"
#include "fileOps.h"
#include "infoStore.h"
#include "textProcessing.h"
#include "textStore.h"

Trokam::InfoStore::InfoStore(const Trokam::Options &value): settings(value), db(value)
{}

void Trokam::InfoStore::getUrlForProcessing(int &index,
                                            std::string &url,
                                            int &level)
{
    std::cout << "get for processing ...\n";

    std::string selectSentence;
    selectSentence=  "SELECT index ";
    selectSentence+= "FROM page ";
    selectSentence+= "WHERE processing=false ";
    selectSentence+= "ORDER BY crunched ASC, level ASC ";
    selectSentence+= "LIMIT 1";

    std::string updateSentence;
    updateSentence=  "UPDATE page ";
    updateSentence+= "SET processing=true ";
    updateSentence+= "WHERE index=(" + selectSentence + ") ";
    updateSentence+= "RETURNING index, protocol, domain, path, level ";

    boost::scoped_ptr<pqxx::result> answer;
    db.execSql(updateSentence, answer);

    pqxx::result::const_iterator col= answer->begin();
    if(col != answer->end())
    {
        index= col[0].as(int());
        const std::string protocol= col[1].as(std::string());
        const std::string domain= col[2].as(std::string());
        const std::string path= col[3].as(std::string());
        url= protocol + domain + SLASH + path;
        level= col[4].as(int());
    }
    else
    {
        index= -1;
        url= "";
        level= -1;
    }
}

void Trokam::InfoStore::insertPage(const std::string &url,
                                   const int &level)
{
    try
    {
        std::string protocol, domain, path;
        if(Trokam::TextProcessing::splitUrl(url, protocol, domain, path))
        {
            if((protocol==HTTP) || (protocol==HTTPS))
            {
                std::string sentence;
                sentence=  "INSERT INTO page ";
                sentence+= "VALUES (";
                sentence+= "DEFAULT, ";
                sentence+= "'" + protocol + "', ";
                sentence+= "'" + domain + "', ";
                sentence+= "'" + path + "', ";
                sentence+= std::to_string(level) + ", ";
                sentence+= "false, ";
                sentence+= "'2000-01-01'";
                sentence+= ") ";
                db.execSql(sentence);
            }
        }
    }
    catch(const pqxx::sql_error &e)
    {
        /**
         * If the URL already exists, then a 'duplicate key' error
         * is thrown and is catched here. Nothing else to do.
         **/
    }
}

void Trokam::InfoStore::insertPage(const int &index,
                                   const boost::scoped_ptr<Trokam::TextStore> &seqBag,
                                   const std::string &links,
                                   const std::string &content,
                                   const int &level)
{
    insertSevSequences(index, seqBag);
    insertUrls(links, level+1);
    savePageContent(index, content);
    setCrunched(index);
}

void Trokam::InfoStore::insertSevSequences(const int &pageIndex,
                                           const boost::scoped_ptr<Trokam::TextStore> &bag)
{
    std::cout << "inserting sequences and their occurrences ...\n";

    deleteSeqOccOfPage(pageIndex);

    for(int i=0; i<bag->size(); i++)
    {
        const TextOcc to= bag->get(i);
        const int seqIndex= insertOneSequence(to.text);
        insertSeqOccInPage(pageIndex, seqIndex, to.occurrence);
    }
}

void Trokam::InfoStore::insertUrls(const std::string &links,
                                   const int &level)
{
    std::cout << "inserting urls ...\n";

    std::istringstream input(links);
    for(std::string line; std::getline(input, line);)
    {
        /**
         * Trimming unwanted characters.
         **/
        boost::algorithm::trim_if(line, boost::algorithm::is_any_of(" \n\r\""));

        /**
         * Verify that it starts with http or https.
         **/
        const std::string first7= line.substr(0, 7);
        const std::string first8= line.substr(0, 8);

        if((first7==HTTP) || (first8==HTTPS))
        {
            insertPage(line, level);
        }
    }
}

void Trokam::InfoStore::savePageContent(const int &index, const std::string &value)
{
    std::cout << "saving content ...\n";

    std::string directory, file;
    getDirFile(index, directory, file);

    Trokam::FileOps::mkDir(directory);
    Trokam::FileOps::rmFile(file);
    Trokam::FileOps::save(file, value);
}

void Trokam::InfoStore::setCrunched(const int &index)
{
    std::cout << "setting crunched ...\n";

    std::string sentence;
    sentence=  "UPDATE page ";
    sentence+= "SET processing=false, crunched=(SELECT current_date) ";
    sentence+= "WHERE index=(" + std::to_string(index) + ") ";
    db.execSql(sentence);
}

void Trokam::InfoStore::getDirFile(const int &index,
                                   std::string &directory,
                                   std::string &file)
{
    const std::string locator= Trokam::TextProcessing::leftPadding(std::to_string(index), 12, "0");
    const std::string dir1= locator.substr(0, 4);
    const std::string dir2= locator.substr(4, 4);
    const std::string filename= locator.substr(8, 4);

    directory= settings.contentDir() + SLASH + dir1 + SLASH + dir2;
    file= directory + SLASH + filename;
}

void Trokam::InfoStore::deleteSeqOccOfPage(const int &pageIndex)
{
    std::string sentence;
    sentence=  "DELETE FROM sequence ";
    sentence+= "WHERE index=" + std::to_string(pageIndex);
    db.execSql(sentence);
}

int Trokam::InfoStore::insertOneSequence(const std::string &value)
{
    int index= -1;

    try
    {
        std::string sentence;
        sentence=  "INSERT INTO sequence ";
        sentence+= "VALUES (";
        sentence+= "DEFAULT, ";
        sentence+= "'" + value + "' ";
        sentence+= ") ";
        sentence+= "RETURNING index ";

        boost::scoped_ptr<pqxx::result> answer;
        db.execSql(sentence, answer);
        Trokam::Postgresql::extract(answer, index);
    }
    catch(const pqxx::sql_error &e)
    {
        /**
         * If the sequence already exists, then a 'duplicate key' error
         * is thrown and is catched here.
         **/
        std::string sentence;
        sentence=  "SELECT index ";
        sentence+= "FROM sequence ";
        sentence+= "WHERE value='" + value + "' ";

        boost::scoped_ptr<pqxx::result> answer;
        db.execSql(sentence, answer);
        Trokam::Postgresql::extract(answer, index);
    }

    return index;
}

void Trokam::InfoStore::insertSeqOccInPage(const int &pageIndex,
                                           const int &seqIndex,
                                           const int &occurrence)
{
    try
    {
        std::string sentence;
        sentence=  "INSERT INTO page_seq ";
        sentence+= "VALUES (";
        sentence+= std::to_string(pageIndex) + ", ";
        sentence+= std::to_string(seqIndex) + ", ";
        sentence+= std::to_string(occurrence) + ") ";
        db.execSql(sentence);
    }
    catch(const pqxx::sql_error &e)
    {
        /**
         * If the pair (pageIndex, seqIndex) already exists,
         * then a 'duplicate key' error is thrown and is
         * catched here. There is something wrong!
         **/
        std::cerr << "SQL error: " << e.what() << "\n";
        std::cerr << "Query was: " << e.query() << std::endl;
    }
}
