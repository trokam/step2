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
#include "reporting.h"
#include "pageProcessing.h"
#include "textProcessing.h"
#include "textStore.h"

Trokam::InfoStore::InfoStore(const Trokam::Options &value): settings(value), db(value), msg(value)
{}

void Trokam::InfoStore::getUrlForProcessing(int &index,
                                            std::string &url,
                                            int &level)
{
    std::string selectSentence;
    selectSentence=  "SELECT index ";
    selectSentence+= "FROM page ";
    selectSentence+= "WHERE processing=false ";
    selectSentence+= "ORDER BY crunched ASC, level ASC ";
    selectSentence+= "LIMIT 1 ";
    selectSentence+= "FOR UPDATE ";

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
        throw NO_PAGES_TO_PROCESS;
    }
}

bool Trokam::InfoStore::insertPage(const std::string &url,
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
                sentence+= "'" + OUT_OF_TIME + "', ";
                sentence+=  std::to_string(UNKNOWN) + " ";
                sentence+= ") ";
                db.execSql(sentence);
            }
            else
            {
                return false;
            }
        }
    }
    catch(const pqxx::sql_error &e)
    {
        /**
         * Only if the error is different to 'duplicate key value
         * violates unique constraint page_unique' is shown.
         * Duplicate URLs are fairly common and they occurrence
         * are not a failure.
         **/
        const std::string error(e.what());
        if(error.find(PAGE_UNIQUE) ==  std::string::npos)
        {
            msg.showSqlError(e);
        }
        return false;
    }
    return true;
}

void Trokam::InfoStore::insertPage(const int &index,
                                   const int &level,
                                   const Trokam::PageInfo &info)
{
    insertSequences(index, info.sequences);
    insertSequenceOccurrence(info.sequences);
    updateSequenceOccurrence(info.sequences);
    insertTraits(index, info);
    insertUrls(info.links, level+1);
    savePageContent(index, info.content);
    setCrunched(index, info);
}

void Trokam::InfoStore::insertSequences(const int &pageIndex,
                                        const Trokam::TextStore &bag)
{
    std::cout << "inserting sequences relevance by page ...\n";

    try
    {
        std::vector<std::string> bundle;
        for(int i=0; i<bag.size(); i++)
        {
            const TextOcc to= bag.get(i);
            if(to.relevanceTotal >= RELEVANCE_THRESHOLD)
            {
                const std::string sentence= generateSentenceOccInPage(pageIndex, to);
                bundle.push_back(sentence);
            }
        }
        db.execSql(bundle);
        std::cout << "inserted: " << bundle.size() << " sequences.\n";
    }
    catch(const pqxx::sql_error &e)
    {
        msg.showSqlError(e);
    }
}

void Trokam::InfoStore::insertSequenceOccurrence(const Trokam::TextStore &bag)
{
    std::cout << "inserting sequence occurrence ...\n";

    try
    {
        std::vector<std::string> bundle;
        for(int i=0; i<bag.size(); i++)
        {
            const TextOcc to= bag.get(i);
            if(to.relevanceTotal >= RELEVANCE_THRESHOLD)
            {
                const std::string sentence= generateSentenceInsertSeqOcc(to);
                bundle.push_back(sentence);
            }
        }
        db.execSql(bundle);
        std::cout << "inserting: " << bundle.size() << " sequences and occurrences.\n";
    }
    catch(const pqxx::sql_error &e)
    {
        msg.showSqlError(e);
    }
}

void Trokam::InfoStore::updateSequenceOccurrence(const Trokam::TextStore &bag)
{
    std::cout << "updating sequence occurrence ...\n";

    try
    {
        std::vector<std::string> bundle;
        for(int i=0; i<bag.size(); i++)
        {
            const TextOcc to= bag.get(i);
            if(to.relevanceTotal >= RELEVANCE_THRESHOLD)
            {
                const std::string sentence= generateSentenceUpdateOcc(to);
                bundle.push_back(sentence);
            }
        }
        db.execSql(bundle);
        std::cout << "updated: " << bundle.size() << " sequences.\n";
    }
    catch(const pqxx::sql_error &e)
    {
        msg.showSqlError(e);
    }
}

void Trokam::InfoStore::insertTraits(const int &index,
                                     const Trokam::PageInfo &info)
{
    std::cout << "inserting page traits ...\n";

    try
    {
        std::string sentence;
        sentence=  "DELETE FROM trait ";
        sentence+= "WHERE index_page=" + std::to_string(index);
        db.execSql(sentence);

        sentence=  "INSERT INTO trait ";
        sentence+= "VALUES (";
        sentence+= std::to_string(index) + ", ";
        sentence+= "'" + info.title + "', ";
        sentence+= std::to_string(info.content.length()) + ", ";
        sentence+= std::to_string(info.complexity) + ") ";
        db.execSql(sentence);
    }
    catch(const pqxx::sql_error &e)
    {
        msg.showSqlError(e);
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

void Trokam::InfoStore::setCrunched(const int &index,
                                    const Trokam::PageInfo &info)
{
    std::cout << "setting crunched ...\n";

    std::string sentence;
    sentence=  "UPDATE page ";
    sentence+= "SET processing=false, ";
    sentence+= "crunched=(SELECT current_date), ";
    sentence+= "state=" + std::to_string(SUCCESS) + " ";
    sentence+= "WHERE index=(" + std::to_string(index) + ") ";
    db.execSql(sentence);
}

void Trokam::InfoStore::setPageState(const int &index,
                                     const int &error)
{
    std::cout << "setting page state ...\n";

    std::string sentence;
    sentence=  "UPDATE page ";
    sentence+= "SET processing=false, ";
    sentence+= "crunched=(SELECT current_date), ";
    sentence+= "state=" + std::to_string(error) + " ";
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

std::string Trokam::InfoStore::generateSentenceOccInPage(const int &pageIndex,
                                                         const Trokam::TextOcc &to)
{
    std::string sentence;
    sentence=  "WITH s AS ( ";
    sentence+= "SELECT index ";
    sentence+= "FROM sequence ";
    sentence+= "WHERE value = '" + to.text + "' ";
    sentence+= "), i as ( ";
    sentence+= "INSERT INTO sequence(\"value\") ";
    sentence+= "SELECT '" + to.text + "' ";
    sentence+= "WHERE NOT EXISTS (SELECT 1 FROM s) ";
    sentence+= "RETURNING index ";
    sentence+= "), ";
    sentence+= "m AS (SELECT index FROM i UNION ALL ";
    sentence+= "SELECT index FROM s) ";
    sentence+= "INSERT INTO page_seq VALUES (" + std::to_string(pageIndex) + ", ";
    sentence+= "(SELECT index FROM m), ";
    sentence+= "1, ";
    sentence+= std::to_string(to.occurrence) + ", ";
    sentence+= std::to_string(to.relevanceInBody) + ", ";
    sentence+= std::to_string(to.relevanceInUrl) + ", ";
    sentence+= std::to_string(to.relevanceInTitle) + ", ";
    sentence+= std::to_string(to.relevanceTotal) + ") ";
    return sentence;
}

std::string Trokam::InfoStore::generateSentenceInsertSeqOcc(const Trokam::TextOcc &to)
{
    std::string sentence;
    sentence=  "WITH s AS (SELECT index FROM sequence WHERE value= '" +  to.text + "') ";
    sentence+= "INSERT INTO seq_occ(\"index_sequence\", \"index_period\", \"count\") ";
    sentence+= "VALUES ((SELECT index FROM s), 1, 0) ";
    sentence+= "ON CONFLICT ON CONSTRAINT unique_seq_occ DO NOTHING";
    return sentence;
}

std::string Trokam::InfoStore::generateSentenceUpdateOcc(const Trokam::TextOcc &to)
{
    std::string sentence;
    sentence=  "UPDATE seq_occ ";
    sentence+= "SET count = count + " + std::to_string(to.occurrence) + " ";
    sentence+= "FROM sequence, period ";
    sentence+= "WHERE seq_occ.index_sequence=sequence.index ";
    sentence+= "AND seq_occ.index_period=period.index ";
    sentence+= "AND period.index=1 ";
    sentence+= "AND sequence.value='" + to.text + "' ";
    return sentence;
}
