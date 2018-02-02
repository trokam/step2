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

/// Trokam
#include "common.h"
#include "infoStore.h"
#include "textProcessing.h"

Trokam::InfoStore::InfoStore(const Trokam::Options &value): db(value)
{}

void Trokam::InfoStore::getUrlForProcessing(const int &level,
                                            int &index,
                                            std::string &url)
{
    std::string selectSentence;

    /*
    selectSentence=  "SELECT index ";
    selectSentence+= "FROM page ";
    selectSentence+= "WHERE level=" + std::to_string(level) + " ";
    selectSentence+= "AND processing=false ";
    selectSentence+= "ORDER BY crunched ASC ";
    selectSentence+= "LIMIT 1";
    */

    selectSentence=  "SELECT index ";
    selectSentence+= "FROM page ";
    selectSentence+= "WHERE processing=false ";
    selectSentence+= "ORDER BY crunched ASC, level ASC ";
    selectSentence+= "LIMIT 1";

    std::string updateSentence;
    updateSentence=  "UPDATE page ";
    updateSentence+= "SET processing=true ";
    updateSentence+= "WHERE index=(" + selectSentence + ") ";
    updateSentence+= "RETURNING index, protocol, domain, path ";

    // std::cout << "sql sentence: " << updateSentence << "\n";

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
    }
    else
    {
        index= -1;
        url= "";
    }

    // std::cout << "index: " << index << " url: '" << url << "'" << std::endl;
}

int Trokam::InfoStore::getUrlIndex(const std::string &url)
{
    int index= -1;

    std::string protocol, domain, path;
    Trokam::TextProcessing::splitUrl(url, protocol, domain, path);

    std::string sentence;
    sentence=  "SELECT index ";
    sentence+= "FROM page ";
    sentence+= "WHERE protocol='" + protocol + "' ";
    sentence+= "AND domain='" + domain + "' ";
    sentence+= "AND path='" + path + "' ";
    // std::cout << "sql sentence: " << sentence << "\n";

    boost::scoped_ptr<pqxx::result> answer;
    db.execSql(sentence, answer);

    pqxx::result::const_iterator col= answer->begin();
    if(col != answer->end())
    {
        index= col[0].as(int());
        std::cout << "index: " << index << "\n";
    }

    return index;
}

void Trokam::InfoStore::insertUrl(const std::string &url,
                                  const int &level)
{
    /**
     * Get the index of the URL,
     * perhaps it is already in the database.
     **/
    const int urlIndex= getUrlIndex(url);

    /**
     * Insert it only if it does not exist.
     **/
    if(urlIndex == -1)
    {
        std::string protocol, domain, path;
        if(Trokam::TextProcessing::splitUrl(url, protocol, domain, path))
        {
            if((protocol==HTTP) || (protocol==HTTPS))
            {
                const std::string lev = std::to_string(level);

                std::string sentence;
                sentence=  "INSERT INTO page ";
                sentence+= "VALUES (";
                sentence+= "DEFAULT, ";             ///  index       |  integer
                sentence+= "'" + protocol + "', ";  ///  protocol    |  varchar(10)
                sentence+= "'" + domain + "', ";    ///  domain      |
                sentence+= "'" + path + "', ";      ///  path        |
                sentence+= lev + ", ";              ///  level       |  integer
                sentence+= "false, ";               ///  processing  |  boolean
                sentence+= "'2000-01-01'";          ///  crunched    |  date
                sentence+= ") ";

                // std::cout << "sql sentence: " << sentence << "\n";
                db.execSql(sentence);
            }
        }
    }
    else
    {
        std::cout << "URL already in database\n";
    }
}

void Trokam::InfoStore::insertPage(const int &index,
                                   const boost::scoped_ptr<Trokam::TextStore> &seqBag,
                                   const boost::scoped_ptr<Trokam::TextStore> &urlBag,
                                   const std::string &content)
{
    insertSequences(seqBag);
    insertSequences(urlBag);
    insertPageContent(content);
    setCrunched(index);
}

void Trokam::InfoStore::insertSequences(const boost::scoped_ptr<Trokam::TextStore> &bag)
{
    std::cout << "inserting sequences ...\n";
}

void Trokam::InfoStore::insertUrls(const boost::scoped_ptr<Trokam::TextStore> &bag)
{
    std::cout << "inserting urls ...\n";
}

void Trokam::InfoStore::insertPageContent(const std::string &value)
{
    std::cout << "inserting content ...\n";
}

void Trokam::InfoStore::setCrunched(const int &index)
{
    std::cout << "seting crunched ...\n";

    std::string updateSentence;
    updateSentence=  "UPDATE page ";
    updateSentence+= "SET processing=false, crunched=(SELECT current_date) ";
    updateSentence+= "WHERE index=(" + std::to_string(index) + ") ";

    std::cout << "sql sentence: " << updateSentence << "\n";

    db.execSql(updateSentence);
}
