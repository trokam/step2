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
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

/// Boost
#include <boost/algorithm/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

/// Trokam
#include "common.h"
#include "data.h"
#include "fileOps.h"
#include "exception.h"
#include "infoStore.h"
#include "reporting.h"
#include "pageInfo.h"
#include "pageProcessing.h"
#include "textProcessing.h"
#include "textStore.h"


/// ======================== P U B L I C ===============================

Trokam::InfoStore::InfoStore(const Trokam::Options &value): settings(value),
                                                            db(value, DB_TEXT_SEARCH),
                                                            msg(value)
{}

bool Trokam::InfoStore::getUrlForProcessing(std::vector<Trokam::PageInfo> &pages,
                                                                      int &level,
                                                                const int &periodBeginning)
{
    int totalPages= 20;

    if ((settings.pagesLimit() != 0) && (settings.pagesLimit() < 20))
    {
        totalPages= settings.pagesLimit();
    }

    std::string selectSentence;
    selectSentence=  "SELECT index ";
    selectSentence+= "FROM page ";
    selectSentence+= "WHERE processing=false ";
    selectSentence+= "AND crunched<" + std::to_string(periodBeginning) + " ";
    selectSentence+= "AND level=" + std::to_string(level) + " ";
    selectSentence+= "ORDER BY random() ";
    selectSentence+= "LIMIT " + std::to_string(totalPages);

    std::cout << "query: '" << selectSentence << "'\n";

    pqxx::result answerIndex;
    db.execSql(selectSentence, answerIndex);

    pqxx::result::iterator it= answerIndex.begin();
    if(it == answerIndex.end())
    {
        return false;
    }
    else
    {
        for(pqxx::result::const_iterator rowIndex= answerIndex.begin(); rowIndex != answerIndex.end(); ++rowIndex)
        {
            const int index= rowIndex[0].as(int());

            std::string updateSentence;
            updateSentence=  "UPDATE page ";
            updateSentence+= "SET processing=true ";
            updateSentence+= "WHERE page.index=" + std::to_string(index) + " ";
            db.execSql(updateSentence);

            selectSentence=  "SELECT protocol.value, domain.value, page.path, page.level, domain.index, domain.type, page.type ";
            selectSentence+= "FROM protocol, page, domain ";
            selectSentence+= "WHERE page.index=" + std::to_string(index) + " ";
            selectSentence+= "AND page.index_domain=domain.index ";
            selectSentence+= "AND page.index_protocol=protocol.index ";

            pqxx::result answerFields;
            db.execSql(selectSentence, answerFields);

            pqxx::result::iterator row= answerFields.begin();
            if(row != answerFields.end())
            {
                Trokam::PageInfo page;

                page.index= index;
                const std::string protocol= row[0].as(std::string());
                const std::string domain=   row[1].as(std::string());
                const std::string path=     row[2].as(std::string());
                page.urlOrigin = protocol + domain + SLASH + path;
                level=                      row[3].as(int());
                page.domainIndex=           row[4].as(int());
                page.domainType=            row[5].as(int());
                page.pageType =             row[6].as(int());

                pages.push_back(page);
            }
            else
            {
                throw Trokam::Exception(NO_PAGES_TO_PROCESS);
            }
        }
    }

    return true;
}

bool Trokam::InfoStore::insertPage(const std::string &url,
                                   const int &level,
                                   const int &type)
{
    std::cout << "inserting page: '" << url << "' ...\n";

    try
    {
        std::string protocol, domain, path;
        if(Trokam::TextProcessing::splitUrl(url, protocol, domain, path))
        {
            if((protocol==HTTP) || (protocol==HTTPS))
            {
                int domainIndex= getDomainIndex(domain);
                int protocolIndex= getProtocolIndex(protocol);
                int seconds= std::time(nullptr);

                std::string sentence;
                sentence=  "INSERT INTO page(index_protocol, \
                                             index_domain, \
                                             path, \
                                             level, \
                                             processing, \
                                             crunched, \
                                             state, \
                                             type, \
                                             inserted) ";
                sentence+= "VALUES (";
                sentence+= std::to_string(protocolIndex) + ", ";
                sentence+= std::to_string(domainIndex) + ", ";
                sentence+= "'" + path + "', ";
                sentence+= std::to_string(level) + ", ";
                sentence+= "false, ";
                sentence+= "0, ";
                sentence+= std::to_string(UNKNOWN) + ", ";
                sentence+= std::to_string(type) + ", ";
                sentence+= std::to_string(seconds);
                sentence+= ") ";
                db.execSql(sentence);
            }
            else
            {
                return false;
            }
        }
    }
    catch(const std::exception &e)
    {
        std::cout << e.what() << "\n";
        return false;
    }
    return true;
}

void Trokam::InfoStore::insertPage(const int &index,
                                   const int &level,
                                         Trokam::PageInfo &info)
{
    int ini0= std::time(nullptr);
    insertSequences(index, info.sequences);
    int end0= std::time(nullptr);

    int ini1= std::time(nullptr);
    insertSeqInPage(index, info.sequences);
    int end1= std::time(nullptr);

    int ini2= std::time(nullptr);
    insertTraits(index, info);
    int end2= std::time(nullptr);

    int ini3= std::time(nullptr);
    insertUrls(info, level+1);
    int end3= std::time(nullptr);

    int ini4= std::time(nullptr);
    savePageContent(index, info.content);
    int end4= std::time(nullptr);

    int ini5= std::time(nullptr);
    setCrunched(index, info);
    int end5= std::time(nullptr);

    std::cout << "ini0: " << ini0 << " end5: " << end5 << "\n";

    const std::string duration0= std::to_string(end0-ini0);
    const std::string duration1= std::to_string(end1-ini1);
    const std::string duration2= std::to_string(end2-ini2);
    const std::string duration3= std::to_string(end3-ini3);
    const std::string duration4= std::to_string(end4-ini4);
    const std::string duration5= std::to_string(end5-ini5);
    const std::string total=     std::to_string(end5-ini0);

    std::cout << "+----------------------------+\n";
    std::cout << "| insertSequences         " << Trokam::TextProcessing::leftPadding(duration0, 2, " ") + " |\n";
    std::cout << "| insertSeqInPage         " << Trokam::TextProcessing::leftPadding(duration1, 2, " ") + " |\n";
    std::cout << "| insertTraits            " << Trokam::TextProcessing::leftPadding(duration2, 2, " ") + " |\n";
    std::cout << "| insertUrls              " << Trokam::TextProcessing::leftPadding(duration3, 2, " ") + " |\n";
    std::cout << "| savePageContent         " << Trokam::TextProcessing::leftPadding(duration4, 2, " ") + " |\n";
    std::cout << "| setCrunched             " << Trokam::TextProcessing::leftPadding(duration5, 2, " ") + " |\n";
    std::cout << "+----------------------------+\n";
    std::cout << "| total                   " << Trokam::TextProcessing::leftPadding(total, 2, " ") + " |\n";
    std::cout << "+----------------------------+\n";

    /***
    insertSequences(index, info.sequences);
    insertSeqInPage(index, info.sequences);
    insertTraits(index, info);
    insertUrls(info, level+1);
    savePageContent(index, info.content);
    setCrunched(index, info);
    ***/
}

void Trokam::InfoStore::insertSeqInPage(const int &pageIndex,
                                              Trokam::TextStore &bag)
{
    std::cout << "inserting in page_seq ...\n";

    /**
     * Before inserting indexes of the sequences of a given page,
     * delete all sequences associated to the page.
     **/
    std::string sentenceDelete;
    sentenceDelete=  "DELETE FROM page_seq ";
    sentenceDelete+= "WHERE index_page=" + std::to_string(pageIndex);
    db.execSql(sentenceDelete);

    int count= 0;
    for(int i=0; i<bag.size(); i++)
    {
        if (count < SEQUENCE_LIMIT)
        {
            try
            {
                std::string sentenceInsert;
                sentenceInsert=  "INSERT INTO page_seq(index_page, \
                                                       index_sequence, \
                                                       seq_count, \
                                                       relevance_in_body, \
                                                       relevance_in_url, \
                                                       relevance_in_title, \
                                                       relevance_total) ";
                sentenceInsert+= "VALUES(" + std::to_string(pageIndex) + ", ";
                sentenceInsert+= std::to_string(bag[i].index) + ", ";
                sentenceInsert+= std::to_string(bag[i].occurrence) + ", ";
                sentenceInsert+= std::to_string(bag[i].relevanceInBody) + ", ";
                sentenceInsert+= std::to_string(bag[i].relevanceInUrl) + ", ";
                sentenceInsert+= std::to_string(bag[i].relevanceInTitle) + ", ";
                sentenceInsert+= std::to_string(bag[i].relevanceTotal) + ") ";
                db.execSql(sentenceInsert);

                count++;
            }
            catch(const std::exception &e)
            {
                std::cout << "error: " << e.what() << "\n";
            }
        }
    }
}

void Trokam::InfoStore::getSequences(const std::string &partial,
                                           Trokam::Bundle<Trokam::Sequence> &results)
{}

void Trokam::InfoStore::getPages(const std::vector<int> seqIndex,
                                 Trokam::Bundle<Trokam::Page> &results)
{
    std::string sentence;
    sentence += "SELECT page.index, page.protocol, domain.value, page.path, page_seq.relevance_total ";
    sentence += "FROM page, page_seq, sequence, domain ";
    sentence += "WHERE page.index=page_seq.index_page ";
    sentence += "AND sequence.index=page_seq.index_sequence ";
    sentence += "AND page.index_domain=domain.index ";
    sentence += "AND ((sequence.index=1070732)) ";
    sentence += "ORDER BY relevance_total DESC ";
    sentence += "LIMIT 10 ";
}

void Trokam::InfoStore::fillProtocol()
{
    std::string sentenceInsert;

    try
    {
        sentenceInsert=  "INSERT INTO protocol(index, value) ";
        sentenceInsert+= "VALUES(" + std::to_string(INDEX_HTTP) + ", '" + HTTP + "');";
        db.execSql(sentenceInsert);
    }
    catch(...)
    {}

    try
    {
        sentenceInsert=  "INSERT INTO protocol(index, value) ";
        sentenceInsert+= "VALUES(" + std::to_string(INDEX_HTTPS) + ", '" + HTTPS + "');";
        db.execSql(sentenceInsert);
    }
    catch(...)
    {}
}

bool Trokam::InfoStore::insertDomainToIndex(const std::string &text)
{
    return insertDomain(text, DOMAIN_INDEX);
}

bool Trokam::InfoStore::insertSeedPage(const std::string &text)
{
    return insertPage(text, 0, PAGE_REJECT);
}

bool Trokam::InfoStore::insertPageToIndex(const std::string &text)
{
    return insertPage(text, 0, PAGE_INDEX);
}

/// ======================= P R I V A T E ==============================


int Trokam::InfoStore::getDomainIndex(const std::string &domain)
{
    int index = -1;

    std::string sentence;
    sentence=  "SELECT index ";
    sentence+= "FROM domain ";
    sentence+= "WHERE value='" + domain + "' ";

    pqxx::result answer;
    db.execSql(sentence, answer);

    pqxx::result::const_iterator row = answer.begin();
    if (row != answer.end())
    {
        index= row[0].as(int());
    }
    else
    {
        index = insertDomain(domain, DOMAIN_REJECT);
    }
    return index;
}

int Trokam::InfoStore::insertDomain(const std::string &text,
                                    const int &type)
{
    int index = -1;

    std::string sentence;
    sentence+= "INSERT INTO domain(value, type) ";
    sentence+= "VALUES('" + text + "', ";
    sentence+= std::to_string(type) + ") ";
    sentence+= "RETURNING index ";

    pqxx::result answer;
    db.execSql(sentence, answer);

    pqxx::result::const_iterator row = answer.begin();
    if(row != answer.end())
    {
        index= row[0].as(int());
    }

    return index;
}

int Trokam::InfoStore::getProtocolIndex(const std::string &protocol)
{
    if (protocol == HTTP )
    {
        return INDEX_HTTP;
    }

    if (protocol == HTTPS )
    {
        return INDEX_HTTPS;
    }

    return -1;
}

void Trokam::InfoStore::insertSequences(const int &pageIndex,
                                              Trokam::TextStore &bag)
{
    std::cout << "inserting sequences ...\n";

    int count= 0;

    std::cout << "bag.size: " << bag.size() << "\n";

    for(int i=0; i<bag.size(); i++)
    {
        const TextOcc to= bag.get(i);
        if (count < SEQUENCE_LIMIT)
        {
            try
            {
                std::string sentence;
                sentence=  "SELECT index ";
                sentence+= "FROM sequence ";
                sentence+= "WHERE value='" + to.text + "' ";

                pqxx::result answer;
                db.execSql(sentence, answer);

                pqxx::result::iterator row = answer.begin();
                if (row != answer.end())
                {
                    int index= row[0].as(int());
                    bag[i].index = index;

                    int seconds= std::time(nullptr);

                    /**
                     * Update the count of the sequence.
                     **/
                    std::string sentenceUpdate;
                    sentenceUpdate=  "UPDATE sequence ";
                    sentenceUpdate+= "SET count=count+" + std::to_string(to.occurrence) + ", ";
                    sentenceUpdate+= "modified=" + std::to_string(seconds) + " ";
                    sentenceUpdate+= "WHERE index=" + std::to_string(index);
                    db.execSql(sentenceUpdate);
                }
                else
                {
                    try
                    {
                        int seconds= std::time(nullptr);

                        /**
                         * Insert the new sequence and get its index.
                         **/
                        std::string sentenceInsert;
                        sentenceInsert=  "INSERT INTO sequence(value, count, modified) ";
                        sentenceInsert+= "VALUES('" + to.text + "', ";
                        sentenceInsert+= std::to_string(to.occurrence) + ", ";
                        sentenceInsert+= std::to_string(seconds) + ") ";
                        sentenceInsert+= "RETURNING index ";
                        pqxx::result answer;
                        db.execSql(sentenceInsert, answer);

                        pqxx::result::iterator row = answer.begin();
                        if(row != answer.end())
                        {
                            int index= row[0].as(int());

                            /**
                             * Keep the index.
                             **/
                            bag[i].index = index;
                        }
                    }
                    catch(const pqxx::sql_error &e)
                    {
                        std::cout << "error: " << e.what() << "\n";
                    }
                }
            }
            catch(const pqxx::sql_error &e)
            {
                std::cout << "error: " << e.what() << "\n";
            }

            count++;
        }
    }
    std::cout << "sequences inserted: " << count << "\n";
}

void Trokam::InfoStore::insertTraits(const int &index,
                                     const Trokam::PageInfo &info)
{
    std::cout << "inserting page traits ...\n";

    try
    {
        std::string selectSentence;
        selectSentence=  "SELECT index_page ";
        selectSentence+= "FROM trait ";
        selectSentence+= "WHERE index_page=" + std::to_string(index);

        pqxx::result answerIndex;
        db.execSql(selectSentence, answerIndex);

        pqxx::result::iterator it= answerIndex.begin();
        if(it == answerIndex.end())
        {
            /**
             * Insert a new row.
             **/
            std::string sentence;
            sentence=  "INSERT INTO trait ";
            sentence+= "VALUES (";
            sentence+= std::to_string(index) + ", ";
            sentence+= "'" + info.title + "', ";
            sentence+= std::to_string(info.content.length()) + ", ";
            sentence+= std::to_string(info.complexity) + ", ";
            sentence+= "'" + info.urlForwarded + "') ";
            db.execSql(sentence);
        }
        else
        {
            /**
             * Update the row.
             **/
            std::string sentence;
            sentence=  "UPDATE trait ";
            sentence+= "SET(title, content_size, complexity, forwarded)=(";
            sentence+= "'" + info.title + "', ";
            sentence+= std::to_string(info.content.length()) + ", ";
            sentence+= std::to_string(info.complexity) + ", ";
            sentence+= "'" + info.urlForwarded + "') ";
            sentence+= "WHERE index_page=" + std::to_string(index);
            db.execSql(sentence);
        }
    }
    catch(const pqxx::sql_error &e)
    {
        std::cerr << "error: " << e.what() << "\n";
    }
}

void Trokam::InfoStore::insertUrls(const Trokam::DifferentStrings &links,
                                   const int &level)
{
    std::cout << "inserting urls ...";

    int count= 0;
    for(int i= 0; ((i<links.size()) && (i<DOMAIN_URL_LIMIT)); i++)
    {
        insertPage(links.get(i), level, PAGE_REJECT);
        count++;
    }

    std::cout << "inserted: " << count << " out of " << links.size() << "\n";
}

void Trokam::InfoStore::insertUrls(const Trokam::PageInfo &page,
                                   const int &level)
{
    std::cout << "inserting urls ...\n";

    int count= 0;
    for(int i= 0; ((i<page.urlBag.size()) && (i<DOMAIN_URL_LIMIT)); i++)
    {
        try
        {
            std::string protocol, domain, path;
            if(Trokam::TextProcessing::splitUrl(page.urlBag.get(i), protocol, domain, path))
            {
                int domainIndex= getDomainIndex(domain);
                int protocolIndex= getProtocolIndex(protocol);
                int seconds= std::time(nullptr);

                bool simpleUrl = true;
                if (path.find("?lang") !=  std::string::npos)
                {
                    simpleUrl = false;
                }

                if (path.find("action=edit") !=  std::string::npos)
                {
                    simpleUrl = false;
                }

                const bool domainIndexing = (page.domainType == DOMAIN_INDEX) &&
                                            (page.domainIndex == domainIndex) &&
                                            (count < DOMAIN_URL_LIMIT) &&
                                            simpleUrl;

                const bool pageIndexing = (page.pageType == PAGE_INDEX) &&
                                          (count < PAGE_URL_LIMIT) &&
                                          simpleUrl;

                if (domainIndexing || pageIndexing)
                {
                    std::string sentence;
                    sentence=  "INSERT INTO page(index_protocol, \
                                                 index_domain, \
                                                 path, \
                                                 level, \
                                                 processing, \
                                                 crunched, \
                                                 state, \
                                                 type, \
                                                 inserted) ";
                    sentence+= "VALUES (";
                    sentence+= std::to_string(protocolIndex) + ", ";
                    sentence+= std::to_string(domainIndex) + ", ";
                    sentence+= "'" + path + "', ";
                    sentence+= std::to_string(level) + ", ";
                    sentence+= "false, ";
                    sentence+= std::to_string(0) + ", ";
                    sentence+= std::to_string(UNKNOWN) + ", ";
                    sentence+= std::to_string(PAGE_REJECT) + ", ";
                    sentence+= std::to_string(seconds);
                    sentence+= ") ";
                    sentence+= "ON CONFLICT ON CONSTRAINT page_unique DO NOTHING;";
                    db.execSql(sentence);

                    count++;
                }
                else
                {
                    /// std::cout << "not alloweded to insert URLs from domain: " << domain << "\n";
                }
            }
        }
        catch(const std::exception &e)
        {
            std::cerr << "error: " << e.what() << "\n";
        }
    }

    std::cout << "inserted: " << count << " out of " << page.urlBag.size() << "\n";
}


void Trokam::InfoStore::savePageContent(const int &index, const std::string &value)
{
    std::cout << "saving content, page index: " << index << "\n";

    std::string directory, file;
    getDirFile(index, directory, file);

    Trokam::FileOps::mkDir(directory);
    Trokam::FileOps::rmFile(file);
    Trokam::FileOps::save(file, value);
}

void Trokam::InfoStore::setCrunched(const int &index,
                                    const Trokam::PageInfo &info)
{
    std::cout << "setting crunched, page index: " << index << "\n";

    int seconds= std::time(nullptr);

    std::string sentence;
    sentence=  "UPDATE page ";
    sentence+= "SET processing=false, ";
    sentence+= "crunched=" + std::to_string(seconds) + ", ";
    sentence+= "state=" + std::to_string(SUCCESS) + " ";
    sentence+= "WHERE index=(" + std::to_string(index) + ") ";
    db.execSql(sentence);
}

void Trokam::InfoStore::setPageState(const int &index,
                                     const int &error)
{
    std::cout << "setting page state, page index: " << index << "\n";

    int seconds= std::time(nullptr);

    std::string sentence;
    sentence=  "UPDATE page ";
    sentence+= "SET processing=false, ";
    sentence+= "crunched=" + std::to_string(seconds) + ", ";
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

void Trokam::InfoStore::getFileSnippet(const std::string &terms,
                                       const int &pageIndex,
                                             std::string &snippet)
{
    std::string directory;
    std::string file;
    getDirFile(pageIndex, directory, file);

    std::cout << "filename: " << file << "\n";

    std::string content;
    Trokam::FileOps::read(file, content);

    std::size_t loc= content.find(terms);
    if(loc != std::string::npos)
    {
        std::cout << "terms found at: " << loc << "\n";

        std::size_t ini= 0;
        std::size_t count= 400;

        if(loc > 200)
        {
            ini = loc - 200;
        }

        snippet = content.substr(ini, count);
        const std::string strongText= "<strong>" + terms + "</strong>";
        boost::replace_all(snippet, terms, strongText);
    }
    else
    {
        std::cout << "terms not found\n";
        snippet = content.substr(0, 400);
    }

    boost::replace_all(snippet, "___________", "");
    boost::replace_all(snippet, "_____", "");
    boost::replace_all(snippet, "__", "");
}

void Trokam::InfoStore::setAllNotProcessing()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    try
    {
        std::string sentence;
        sentence=  "UPDATE page ";
        sentence+= "SET processing=false ";
        db.execSql(sentence);
    }
    catch(const pqxx::sql_error &e)
    {
        std::cerr << "error: " << e.what() << "\n";
    }
}
