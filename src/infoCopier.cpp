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
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

/// Boost
#include <boost/algorithm/string.hpp>

/// Trokam
#include "common.h"
#include "fileOps.h"
#include "exception.h"
#include "infoCopier.h"
#include "reporting.h"
#include "pageInfo.h"
#include "pageProcessing.h"
#include "textProcessing.h"
#include "textStore.h"


/// ======================== P U B L I C ===============================


Trokam::InfoCopier::InfoCopier(const Trokam::Options &value): settings(value),
                                                              dbOrigin(value, DB_TEXT_SEARCH),
                                                              dbDepot(value, DB_DEPOT),
                                                              msg(value)
{}

void Trokam::InfoCopier::copySequences()
{
    std::cout << "copying sequences ...\n";

    const int BUNCH_SIZE= 10000;

    int maxOnDepot= getMaxOnDepot();
    int seqTotal= getSequencesTotal(maxOnDepot);
    std::cout << "total of sequences to copy: " << seqTotal << "\n";

    std::div_t d= div(seqTotal, BUNCH_SIZE);

    int cycles= d.quot;
    if (d.rem > 0)
    {
        cycles++;
    }

    for(int i=0; i<cycles; i++)
    {
        std::string sentence;

        sentence  = "SELECT index, value, count, modified ";
        sentence += "FROM sequence ";
        sentence += "WHERE modified>" + std::to_string(maxOnDepot) + " ";
        sentence += "ORDER BY index ASC ";
        sentence += "LIMIT " + std::to_string(BUNCH_SIZE) + " ";
        sentence += "OFFSET " + std::to_string(i * BUNCH_SIZE) + " ";

        std::cout << "sentence: " << sentence << "\n";

        pqxx::result answer;
        dbOrigin.execSql(sentence, answer);

        std::vector<std::string> sqlBundle;

        for(pqxx::result::const_iterator row= answer.begin(); row != answer.end(); ++row)
        {
            const std::string index= row[0].as(std::string());

            std::string sentenceExist;
            sentenceExist=  "SELECT * ";
            sentenceExist+= "FROM words ";
            sentenceExist+= "WHERE index=" + index;

            pqxx::result answerExist;
            dbDepot.execSql(sentenceExist, answerExist);

            if(answerExist.begin() == answerExist.end())
            {
                const std::string value=   row[1].as(std::string());
                const std::string count=   row[2].as(std::string());
                const std::string updated= row[3].as(std::string());

                std::string sentenceInsert;
                sentenceInsert=  "INSERT INTO words(index, value, count, tokens, updated) ";
                sentenceInsert+= "VALUES(" + index + ", ";
                sentenceInsert+= "'" + value + "', ";
                sentenceInsert+= count + ", ";
                sentenceInsert+= "to_tsvector('" + value + "'), ";
                sentenceInsert+= updated + ") ";

                sqlBundle.push_back(sentenceInsert);
            }
            else
            {
                const std::string count=   row[2].as(std::string());
                const std::string updated= row[3].as(std::string());

                std::string sentenceUpdate;
                sentenceUpdate=  "UPDATE words ";
                sentenceUpdate+= "SET count=" + count + ", ";
                sentenceUpdate+= "updated=" + updated + " ";
                sentenceUpdate+= "WHERE index=" + index;

                sqlBundle.push_back(sentenceUpdate);
            }
        }

        try
        {
            dbDepot.execSql(sqlBundle);
        }
        catch(const std::exception &e)
        {
            std::cerr << "error: " << e.what() << "\n";
        }
    }

    std::cout << "...done\n";
}

void Trokam::InfoCopier::copyFindings()
{
    std::cout << "copying findings ...\n";

    const int BUNCH_SIZE= 10000;

    const int maxCrunched= getMaxFindingCrunchedOnDepot();
    const int findTotal= getFindingsTotal(maxCrunched);
    std::cout << "total of findings to copy: " << findTotal << "\n";

    std::div_t d= div(findTotal, BUNCH_SIZE);

    int cycles= d.quot;
    if (d.rem > 0)
    {
        cycles++;
    }

    for(int i=0; i<cycles; i++)
    {
        std::string sentence;

        sentence  = "SELECT page_seq.index_sequence, \
                            page_seq.index_page, \
                            protocol.value, \
                            domain.value, \
                            page.path, \
                            trait.title, \
                            page_seq.relevance_in_body, \
                            page_seq.relevance_in_url, \
                            page_seq.relevance_in_title, \
                            page_seq.relevance_total, \
                            sequence.value, \
                            page.crunched ";
        sentence += "FROM page_seq, page, protocol, domain, trait, sequence ";
        sentence += "WHERE page_seq.index_page=page.index ";
        sentence += "AND page.index_protocol=protocol.index ";
        sentence += "AND page.index_domain=domain.index ";
        sentence += "AND trait.index_page=page.index ";
        sentence += "AND page_seq.index_sequence=sequence.index ";
        sentence += "AND page.state=0 ";
        sentence += "AND crunched>" + std::to_string(maxCrunched) + " ";
        sentence += "ORDER BY page.index ASC ";
        sentence += "LIMIT " + std::to_string(BUNCH_SIZE) + " ";
        sentence += "OFFSET " + std::to_string(i * BUNCH_SIZE) + " ";

        std::cout << "\ncopying from: " << i * BUNCH_SIZE << " to: " <<  (i+1) * BUNCH_SIZE << "\n";

        pqxx::result answer;
        dbOrigin.execSql(sentence, answer);

        int count= 0;
        for(pqxx::result::const_iterator row= answer.begin(); row != answer.end(); ++row)
        {
            const std::string indexWords=       row[0].as(std::string());
            const std::string indexPage=        row[1].as(std::string());
            const std::string protocol=         row[2].as(std::string());
            const std::string domain=           row[3].as(std::string());
            const std::string path=             row[4].as(std::string());
            const std::string title=            row[5].as(std::string());
            const std::string relevanceInBody=  row[6].as(std::string());
            const std::string relevanceInUrl=   row[7].as(std::string());
            const std::string relevanceInTitle= row[8].as(std::string());
            const std::string relevanceTotal=   row[9].as(std::string());
            const std::string sequence=         row[10].as(std::string());
            const std::string crunched=         row[11].as(std::string());

            const std::string url= protocol + domain + "/" + path;

            std::string snippet;
            getFileSnippet(sequence, indexPage, snippet);

            std::string sentenceExist;
            sentenceExist=  "SELECT * ";
            sentenceExist+= "FROM findings ";
            sentenceExist+= "WHERE index_words=" + indexWords + " ";
            sentenceExist+= "AND index_page=" + indexPage + " ";

            pqxx::result answerExist;
            dbDepot.execSql(sentenceExist, answerExist);

            if(answerExist.begin() == answerExist.end())
            {

                std::cout << "i";

                std::string sentenceInsert;
                sentenceInsert=  "INSERT INTO findings(index_words, \
                                                       index_page, \
                                                       url, \
                                                       title, \
                                                       relevance_in_body, \
                                                       relevance_in_url, \
                                                       relevance_in_title, \
                                                       relevance_total, \
                                                       snippet, \
                                                       crunched)";
                sentenceInsert+= "VALUES(" + indexWords + ", ";
                sentenceInsert+= indexPage + ", ";
                sentenceInsert+= "'" + url + "', ";
                sentenceInsert+= "'" + title + "', ";
                sentenceInsert+= relevanceInBody + ", ";
                sentenceInsert+= relevanceInUrl + ", ";
                sentenceInsert+= relevanceInTitle + ", ";
                sentenceInsert+= relevanceTotal + ", ";
                sentenceInsert+= "'" + snippet + "', ";
                sentenceInsert+= crunched + ")";

                try
                {
                    dbDepot.execSql(sentenceInsert);
                }
                catch(const std::exception &e)
                {
                    std::cerr << "error: " << e.what() << "\n";
                    std::cerr << "snippet: '" << snippet << "'\n";
                }
            }
            else
            {
                std::cout << "u";

                std::string sentenceUpdate;
                sentenceUpdate=  "UPDATE findings ";
                sentenceUpdate+= "SET title='" + title + "', ";
                sentenceUpdate+= "relevance_in_body=" + relevanceInBody + ", ";
                sentenceUpdate+= "relevance_in_url=" + relevanceInUrl + ", ";
                sentenceUpdate+= "relevance_in_title=" + relevanceInTitle + ", ";
                sentenceUpdate+= "relevance_total=" + relevanceTotal + ", ";
                sentenceUpdate+= "snippet='" + snippet + "', ";
                sentenceUpdate+= "crunched=" + crunched + " ";
                sentenceUpdate+= "WHERE index_words=" + indexWords + " ";
                sentenceUpdate+= "AND index_page=" + indexPage + " ";

                try
                {
                    dbDepot.execSql(sentenceUpdate);
                }
                catch(const std::exception &e)
                {
                    std::cerr << "error: " << e.what() << "\n";
                    std::cerr << "snippet: '" << snippet << "'\n";
                }
            }

            count++;
            if ((count % 1000) == 0)
            {
                std::cout.flush();
            }
       }
    }

    std::cout << "...done\n" << std::endl;
}

/// ======================= P R I V A T E ==============================


int Trokam::InfoCopier::getSequencesTotal(const int &maxInserted)
{
    std::string sentence;
    sentence=  "SELECT COUNT(*) ";
    sentence+= "FROM sequence ";
    sentence+= "WHERE modified>" + std::to_string(maxInserted);

    pqxx::result answer;
    dbOrigin.execSql(sentence, answer);

    int total= 0;
    pqxx::result::iterator row= answer.begin();
    if(row != answer.end())
    {
        total= row[0].as(int());
    }
    return total;
}

int Trokam::InfoCopier::getFindingsTotal(const int &maxCrunched)
{
    /**
     * SELECT COUNT(*)
     * FROM page_seq, page, protocol, domain, trait
     * WHERE page_seq.index_page=page.index
     * AND page.index_protocol=protocol.index
     * AND page.index_domain=domain.index
     * AND trait.index_page=page.index;
     **/

    std::string sentence;
    sentence=  "SELECT COUNT(*) ";
    sentence+= "FROM page_seq, page, protocol, domain, trait ";
    sentence+= "WHERE page_seq.index_page=page.index ";
    sentence+= "AND page.index_protocol=protocol.index ";
    sentence+= "AND page.index_domain=domain.index ";
    sentence+= "AND trait.index_page=page.index ";
    sentence+= "AND page.state=0 ";
    sentence+= "AND crunched>" + std::to_string(maxCrunched);

    pqxx::result answer;
    dbOrigin.execSql(sentence, answer);

    int total= 0;
    pqxx::result::iterator row= answer.begin();
    if(row != answer.end())
    {
        total= row[0].as(int());
    }
    return total;
}

void Trokam::InfoCopier::getFileSnippet(const std::string &terms,
                                        const std::string &pageIndex,
                                              std::string &snippet)
{
    std::string directory;
    std::string file;
    getDirFile(pageIndex, directory, file);

    std::ifstream inputFile(file.c_str(), std::ios::in | std::ios::binary);

    std::string p0, p1, p2;

    bool first= true;
    std::string line, header;
    while (std::getline(inputFile, line))
    {
        std::string part = p0 + " " + p1 + " " + p2 + " " + line;

        if ((p0 != "") && (p1 != "") && (p2 != "") && (line != "") && (first==true))
        {
            header= part;
            first= false;
        }

        if(part.find(terms) != std::string::npos)
        {
            snippet+= part + " ";
        }

        if (snippet.length() >= 400)
        {
            break;
        }

        p0 = p1;
        p1 = p2;
        p2 = line;
    }

    if (snippet == "")
    {
        snippet = header;
    }

    const std::string strongText= "<strong>" + terms + "</strong>";
    boost::replace_all(snippet, terms, strongText);

    boost::replace_all(snippet, "___________", "");
    boost::replace_all(snippet, "_____", "");
    boost::replace_all(snippet, "__", "");
}

void Trokam::InfoCopier::getDirFile(const std::string &index,
                                    std::string &directory,
                                    std::string &file)
{
    const std::string locator= Trokam::TextProcessing::leftPadding(index, 12, "0");
    const std::string dir1= locator.substr(0, 4);
    const std::string dir2= locator.substr(4, 4);
    const std::string filename= locator.substr(8, 4);

    directory= settings.contentDir() + SLASH + dir1 + SLASH + dir2;
    file= directory + SLASH + filename;
}

int Trokam::InfoCopier::getMaxOnDepot()
{
    std::string sentence;
    sentence=  "SELECT MAX(updated) ";
    sentence+= "FROM words ";

    pqxx::result answer;
    dbDepot.execSql(sentence, answer);

    int max= 0;
    pqxx::result::iterator row= answer.begin();
    if(row != answer.end())
    {
        max= row[0].as(int());
    }
    return max;
}

int Trokam::InfoCopier::getMaxFindingCrunchedOnDepot()
{
    std::string sentence;
    sentence=  "SELECT MAX(crunched) ";
    sentence+= "FROM findings ";

    pqxx::result answer;
    dbDepot.execSql(sentence, answer);

    int max= 0;
    pqxx::result::iterator row= answer.begin();
    if(row != answer.end())
    {
        max= row[0].as(int());
    }
    return max;
}
