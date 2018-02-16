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
#include <sstream>

/// Boost
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

/// Trokam
#include "common.h"
#include "textProcessing.h"

void Trokam::TextProcessing::extractSequences(std::string &content,
                                              Trokam::TextStore &bag)
{
    std::cout << "extracting sequences ..." << std::endl;

    /**
     * All the text is processed in lower case.
     **/
    boost::algorithm::to_lower(content);

    /**
     * Remove all single quotes.
     **/
    boost::replace_all(content, "'s", "");
    boost::replace_all(content, "'", "");

    int wordCount= 0;
    int textLength= 0;

    boost::tokenizer<> tok(content);
    for(boost::tokenizer<>::iterator it= tok.begin(); it!=tok.end(); ++it)
    {
        for(int maxLen= 1; maxLen<=SEQUENCE_SIZE; maxLen++)
        {
            int len=0;
            std::string sequence;

            boost::tokenizer<>::iterator sec;
            for(sec= it; ((sec!=tok.end()) && (len<maxLen)); ++sec)
            {
                std::string token= *sec;
                sequence+= token + " ";
                len++;
            }

            boost::algorithm::trim_if(sequence, boost::algorithm::is_any_of(" \n\r"));
            bag.insert(sequence);

            /**
             * If the iterator reaches the end of the content,
             * then jump out of this loop otherwise appear
             * false repeated sequences.
             **/
            if(sec==tok.end())
            {
                break;
            }
        }

        wordCount++;
        textLength+= it->length();

        if ((wordCount >= WORDS_LIMIT) || (textLength >= TEXT_LIMIT))
        {
            break;
        }
    }

    std::cout << "wordCount: " << wordCount << " textLength: " << textLength << std::endl;

    /**
     * Set the relevance of each sequence.
     **/
    bag.setRelevance(content.length());
}

bool Trokam::TextProcessing::splitUrl(const std::string &url,
                                            std::string &protocol,
                                            std::string &domain,
                                            std::string &path)
{
    /**
     * Get the protocol.
     * For instance 'http://'
     **/
    std::size_t twoSlash= url.find("//");
    if (twoSlash == std::string::npos)
    {
        return false;
    }
    else
    {
        protocol= url.substr(0, twoSlash+2);
    }

    /**
     * Get the domain and the path.
     * For instance, domain is 'www.trokam.com'
     * and path is '/some/interesting/page.html'
     **/
    std::size_t firstSlash= url.find('/', twoSlash+3);
    if (firstSlash != std::string::npos)
    {
        domain= url.substr(twoSlash+2, firstSlash-(twoSlash+2));
        path= url.substr(firstSlash+1, url.length()-firstSlash-1);
    }
    else
    {
        std::size_t end= url.length()-1;
        domain= url.substr(twoSlash+2, end-(twoSlash+2)+1);
        path= "";
    }

    return true;
}

std::string Trokam::TextProcessing::leftPadding(const std::string &text,
                                                const int &totalLength,
                                                const std::string &fill)
{
    std::string piece;
    int diff= totalLength-text.length();

    for (int i=0; i<diff; i++)
    {
        piece+= fill;
    }

    return (piece + text);
}


std::string Trokam::TextProcessing::rightPadding(const std::string &text,
                                                 const int &totalLength,
                                                 const std::string &fill)
{
    std::string piece;
    int diff= totalLength-text.length();

    for (int i=0; i<diff; i++)
    {
        piece+= fill;
    }

    return (text + piece);
}


void Trokam::TextProcessing::extractTitle(const std::string &content,
                                          std::string &title)
{
    std::size_t ini= content.find("<title>");
    std::size_t end= content.find("</title>");
    if((ini != std::string::npos) && (end != std::string::npos))
    {
        title= content.substr(ini+7, end-ini-7);
    }

    /**
     * Clean the title from unwanted characters.
     **/
    boost::algorithm::to_lower(title);
    boost::algorithm::trim_if(title, boost::algorithm::is_any_of(" \n\r\t"));

    /**
     * Remove all single quotes from title.
     **/
    boost::replace_all(title, "'s", "");
    boost::replace_all(title, "'", "");

    /**
     * Remove unreadable strings.
     **/
    boost::replace_all(title, "&nbsp;", "");
    boost::replace_all(title, "&ndash;", "");
}
