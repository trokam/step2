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
#include <iostream>
#include <sstream>
#include <cctype>
#include <clocale>
#include <cstdlib>

/// Boost
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

/// Trokam
#include "common.h"
#include "differentStrings.h"
#include "textProcessing.h"
#include "fileOps.h"

int Trokam::TextProcessing::extractSequences(std::string &content,
                                             Trokam::TextStore &bag)
{
    std::cout << "extracting sequences ..." << std::endl;

    /**
     * Convert text to lower case and remove
     * unwanted characters.
     **/
    boost::algorithm::to_lower(content);
    removeUnwantedChars(content);

    int wordCount= 0;
    int textLength= 0;

    typedef boost::char_separator<char> char_sep;
    typedef boost::tokenizer<char_sep> tokenizer;

    boost::char_separator<char> sep(" \n");
    tokenizer tok(content, sep);

    for(tokenizer::iterator it= tok.begin(); it!=tok.end(); ++it)
    {
        for(int maxLen= 1; maxLen<=SEQUENCE_SIZE; maxLen++)
        {
            int len=0;
            std::string sequence;

            tokenizer::iterator sec;
            for(sec= it; ((sec!=tok.end()) && (len<maxLen)); ++sec)
            {
                std::string token= *sec;
                sequence+= token + " ";
                len++;
            }

            boost::algorithm::trim_if(sequence, boost::algorithm::is_any_of(" "));

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
    return textLength;
}

void Trokam::TextProcessing::extractURLs(const std::string &text,
                                              Trokam::DifferentStrings &bag)
{
    std::cout << "extracting URLs ..." << std::endl;

    std::istringstream input(text);
    for(std::string line; std::getline(input, line);)
    {
        /**
         * Trimming unwanted characters.
         **/
        boost::algorithm::trim_if(line, boost::algorithm::is_any_of(" \n\r\""));

        /**
         * Must be within a minimum and maximun length.
         **/
        if((line.length() < 8) || (500 < line.length()))
        {
            continue;
        }

        /**
         * URL that include a single quote is rejected.
         **/
        std::size_t loc= line.find('\'');
        if(loc != std::string::npos)
        {
            continue;
        }

        /**
         * URL that include a single a '#' is rejected.
         **/
        loc= line.find('#');
        if(loc != std::string::npos)
        {
            continue;
        }

        /**
         * Reject type not yet supported.
         **/
        const int len= line.length();
        std::string last4= line.substr(len-4, len-1);
        boost::algorithm::to_lower(last4);
        if((last4 == ".pdf") ||
           (last4 == ".xml") ||
           (last4 == ".rss") ||
           (last4 == ".zip") ||
           (last4 == ".jpg"))
        {
            continue;
        }

        /**
         * Verify that it starts with http or https.
         **/
        const std::string first7= line.substr(0, 7);
        const std::string first8= line.substr(0, 8);

        /**
         * Only 'http' and 'https' protocols accepted.
         **/
        if((first7==HTTP) || (first8==HTTPS))
        {
            bag.insert(line);
        }
    }

    std::cout << "... done" << std::endl;
}

int Trokam::TextProcessing::relevance(const std::string &block,
                                      const std::string &piece)
{
    if((block.length() == 0) ||
       (piece.length() == 0))
    {
        return 1;
    }

    int value= 1;
    std::string lcBlock= block;
    boost::algorithm::to_lower(lcBlock);

    float lenSequence= 0.0;
    Trokam::DifferentStrings stringBag;
    boost::tokenizer<> tok(piece);
    for(boost::tokenizer<>::iterator it= tok.begin(); it!=tok.end(); ++it)
    {
        const std::string token= *it;
        lenSequence+= token.length();
        stringBag.insert(token);
    }

    float lenIncluded= 0.0;
    for(int i= 0; i<stringBag.size(); i++)
    {
        const std::string token= stringBag.get(i);
        if(lcBlock.find(token) != std::string::npos)
        {
            lenIncluded+= token.length();
        }
    }

    const float lenBlock= lcBlock.length();

    if((int(lenSequence) == 0) ||
       (int(lenIncluded) == 0))
    {
        return 1;
    }

    value = int((100 * lenIncluded * lenIncluded) / (lenBlock * lenSequence));

    return value;
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

    //int diff= totalLength-text.size();

    // int len= Trokam::TextProcessing::length(text);
    // int diff= totalLength-len;

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

    if ( title.length() > 300 )
    {
        title = title.substr(0, 300);
    }

    /**
     * Convert to lower case, trim and clean the title
     * from unwanted characters.
     **/
    boost::algorithm::trim_if(title, boost::algorithm::is_any_of(" \n\r\t"));
    boost::algorithm::to_lower(title);
    removeUnwantedChars(title);
    removeUglyChars(title);
}

void Trokam::TextProcessing::removeUnwantedChars(std::string &text)
{
    /**
     * Remove all single quotes.
     **/
    boost::replace_all(text, "'s", "");
    boost::replace_all(text, "'", "");

    boost::replace_all(text, " + ", " ");
    boost::replace_all(text, " +", " ");
    boost::replace_all(text, "—", " ");
    boost::replace_all(text, "–", " ");
    boost::replace_all(text, "_", " ");
    boost::replace_all(text, ",", " ");
    boost::replace_all(text, ".", " ");
    boost::replace_all(text, ":", " ");
    boost::replace_all(text, ";", " ");
    boost::replace_all(text, "/", " ");
    boost::replace_all(text, "*", " ");
    boost::replace_all(text, "#", " ");
    boost::replace_all(text, "|", " ");
    boost::replace_all(text, "»", " ");
    boost::replace_all(text, "«", " ");
    boost::replace_all(text, "<", " ");
    boost::replace_all(text, ">", " ");
    boost::replace_all(text, "[", " ");
    boost::replace_all(text, "]", " ");
    boost::replace_all(text, "(", " ");
    boost::replace_all(text, ")", " ");
    boost::replace_all(text, "{", " ");
    boost::replace_all(text, "}", " ");
    boost::replace_all(text, "&", " ");
    boost::replace_all(text, "^", " ");
    boost::replace_all(text, "¿", " ");
    boost::replace_all(text, "?", " ");
    boost::replace_all(text, "¡", " ");
    boost::replace_all(text, "!", " ");

    boost::replace_all(text, "=", " ");
    boost::replace_all(text, "-", " ");
    boost::replace_all(text, "\t", " ");
    boost::replace_all(text, "\"", " ");

    boost::replace_all(text, "，", " ");
    boost::replace_all(text, "；", " ");
    boost::replace_all(text, "：", " ");
    boost::replace_all(text, "（", " ");
    boost::replace_all(text, "）", " ");
    boost::replace_all(text, "・", " ");
    boost::replace_all(text, "？", " ");

    boost::replace_all(text, "•", " ");
    boost::replace_all(text, "·", " ");
    boost::replace_all(text, "”", " ");
    boost::replace_all(text, "„", " ");

    boost::replace_all(text, "『", " ");
    boost::replace_all(text, "』", " ");
    boost::replace_all(text, "「", " ");
    boost::replace_all(text, "」", " ");
    boost::replace_all(text, "。", " ");
    boost::replace_all(text, "、", " ");

    boost::replace_all(text, "►", " ");
    boost::replace_all(text, "↑", " ");
}

void Trokam::TextProcessing::removeUglyChars(std::string &text)
{
    /**
     * Remove unreadable strings.
     **/
    boost::replace_all(text, "&nbsp;", "");
    boost::replace_all(text, "&ndash;", "");

    /**
     * Remove strange characters.
     **/
    boost::replace_all(text, "\n", " ");
    boost::replace_all(text, "\r", " ");
    boost::replace_all(text, "\t", " ");

    /**
     * Remove excessive white space.
     **/
    boost::replace_all(text, "          ", " ");
    boost::replace_all(text, "     ", " ");
    boost::replace_all(text, "   ", " ");
    boost::replace_all(text, "  ", " ");
}

std::size_t Trokam::TextProcessing::length(const std::string &s)
{
    std::size_t result = 0;
    const char* ptr = s.data();
    const char* end = ptr + s.size();
    while (ptr < end)
    {
        int next= std::mbstowcs(NULL, s.c_str(), s.size());
        if (next == -1)
        {
            throw std::runtime_error("strlen_mb(): conversion error");
        }
        ptr += next;
        ++result;
    }
    return result;
}

std::string Trokam::TextProcessing::extractLocation(const std::string &file)
{
    const std::string TAG= "Location: ";

    std::string location = VOID;
    std::string content;

    Trokam::FileOps::read(file, content);

    std::istringstream input(content);

    /**
     * There might be several lines begining with 'Location'.
     * It must get the value of the last one.
     **/

    for(std::string line; std::getline(input, line);)
    {
        const std::string first10 = line.substr(0, 10);
        if (first10 == TAG)
        {
            std::cout << "line: '" << line << "'\n";
            const int loc = line.find(" ", 12);
            const std::string text= line.substr(10, loc-10);

            const std::string first7= text.substr(0, 7);
            const std::string first8= text.substr(0, 8);

            if((first7==HTTP) || (first8==HTTPS))
            {
                location = text;
            }
        }
    }

    return location;
}

std::string Trokam::TextProcessing::generateLikeClause(const std::string &text)
{
    std::string clause;

    typedef boost::char_separator<char> char_sep;
    typedef boost::tokenizer<char_sep> tokenizer;
    boost::char_separator<char> sep(" ");
    tokenizer tok(text, sep);

    for(tokenizer::iterator it= tok.begin(); it!=tok.end(); ++it)
    {
        const std::string token= *it;
        if(token.length() >= 2)
        {
            clause+= "AND value LIKE '%" + token + "%' ";
        }
    }

    return clause;
}
