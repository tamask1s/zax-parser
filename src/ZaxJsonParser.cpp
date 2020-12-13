/***************************************************************************
* Copyright 2020 Tamas Levente Kis                                         *
*                                                                          *
* Licensed under the Apache License, Version 2.0 (the "License");          *
* you may not use this file except in compliance with the License.         *
* You may obtain a copy of the License at                                  *
*                                                                          *
*     http://www.apache.org/licenses/LICENSE-2.0                           *
*                                                                          *
* Unless required by applicable law or agreed to in writing, software      *
* distributed under the License is distributed on an "AS IS" BASIS,        *
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
* See the License for the specific language governing permissions and      *
* limitations under the License.                                           *
***************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include "ZaxJsonParser.h"

inline const char* nextTokenOpen(const char* a_str_to_find, char* a_close_token_pair)
{
    while (*a_str_to_find)
    {
        switch (*a_str_to_find++)
        {
        case '"':
            *a_close_token_pair = '"';
            return --a_str_to_find;
        case '\\':
            ++a_str_to_find;
            break;
        case '{':
            *a_close_token_pair = '}';
            return --a_str_to_find;
        case '[':
            *a_close_token_pair = ']';
            return --a_str_to_find;
        }
    }
    return 0;
}

inline const char* next_number_end(const char* a_str_to_find)
{
    while (*a_str_to_find)
    {
        switch (*a_str_to_find++)
        {
        case ',':
            return --a_str_to_find;
        case '\\':
            ++a_str_to_find;
            break;
        case '}':
            return --a_str_to_find;
        case ']':
            return --a_str_to_find;
        }
    }
    return 0;
}

inline const char* next_number_start(const char* a_str_to_find)
{
    while (*a_str_to_find)
    {
        switch (*a_str_to_find++)
        {
        case ':':
            return a_str_to_find;
        case '\\':
            ++a_str_to_find;
            break;
        case ',':
            return a_str_to_find;
        }
    }
    return 0;
}

inline const char* nextTokenClose(const char* a_str_to_find, char a_close_token, char a_open_token)
{
    int equilibrium = 0;
    while (*a_str_to_find)
    {
        if (*a_str_to_find == a_close_token)
        {
            if (*(a_str_to_find - 1) != '\\')
            {
                if (!equilibrium)
                    return a_str_to_find;
                else
                    --equilibrium;
            }
        }
        else if (*a_str_to_find == a_open_token)
            ++equilibrium;
        ++a_str_to_find;
    }
    return 0;
}

inline char* nextQuotMark(const char* a_colon_value_start)
{
    if (char* colon_value_stop = strchr((char*)a_colon_value_start + 1, '"'))
    {
        while (*(colon_value_stop - 1) == '\\')
            colon_value_stop = strchr(colon_value_stop + 1, '"');
        return colon_value_stop;
    }
    return 0;
}

inline char* nextSqBraceOpen(const char* a_colon_value_start)
{
    if (char* colon_value_stop = strchr((char*)a_colon_value_start, '['))
    {
        if (colon_value_stop > a_colon_value_start)
            while (*(colon_value_stop - 1) == '\\')
                colon_value_stop = strchr(colon_value_stop + 1, '[');
        return colon_value_stop;
    }
    return 0;
}

inline int strcmp_(const char *a_s1,  const char *a_s2)
{
    while (*a_s1 == *a_s2++)
        if (*a_s1++ == 0)
            return 0;
    return (*(const unsigned char *)a_s1 - *(const unsigned char *)(a_s2 - 1));
}

ZaxStringWrap::ZaxStringWrap(const char* a_str)
    :m_str(a_str)
{}

bool ZaxStringWrap::operator < (const ZaxStringWrap &a_rhs) const
{
    return strcmp_(m_str, a_rhs.m_str) < 0;
}

ZaxJsonFlatParser::ZaxJsonFlatParser(const char* a_json, bool a_in_situ, bool* a_success)
    :m_own_buffer(0)
{
    int success = true;
    if (!a_json)
        success = false;
    else
    {
        char* l_json = (char*)a_json;
        if (!a_in_situ)
        {
            m_own_buffer = new char[strlen(a_json) + 1];
            strcpy(m_own_buffer, a_json);
            l_json = m_own_buffer;
        }

        char* colon_key_start = 0;
        if (l_json[0])
            colon_key_start = nextQuotMark(l_json);
        bool is_list = false;

        if (char* next_sq_brace_open = nextSqBraceOpen(l_json))
            if (!colon_key_start || next_sq_brace_open < colon_key_start)
            {
                is_list = true;
                if (const char* next_sq_brace_stop = nextTokenClose(next_sq_brace_open + 1, ']', '['))
                {
                    m_list_values.clear();
                    char close_token = ' ';
                    const char* colon_value_start = nextTokenOpen(next_sq_brace_open + 1, &close_token);
                    if (!colon_value_start || (colon_value_start >= next_sq_brace_stop)) // case of empty list or numbers
                    {
                        const char* start = next_sq_brace_open + 1;
                        for (const char* i = next_sq_brace_open; i <= next_sq_brace_stop; ++ i)
                        {
                            if (*i == ',' || *i == ']' ) // case of numbers
                            {
                                *((char*)i) = 0;
                                while (*start == ' ')
                                    ++start;
                                if (*start)
                                    m_list_values.push_back(start);
                                start = i + 1;
                            }
                        }
                    }
                    else while (colon_value_start && colon_value_start < next_sq_brace_stop)
                    {
                        if (const char* colon_value_stop = nextTokenClose(colon_value_start + 1, close_token, colon_value_start[0]))
                        {
                            if (close_token == '}' || close_token == ']')
                            {
                                --colon_value_start;
                                ++colon_value_stop;
                            }
                            const char* origColonValueStop = colon_value_stop;
                            *((char*)colon_value_stop) = 0;

                            while (*(--colon_value_stop) == ' ')
                                *((char*)colon_value_stop) = 0;
                            m_list_values.push_back(colon_value_start + 1);
                            colon_value_start = nextTokenOpen(origColonValueStop + 1, &close_token);
                        }
                        else
                        {
                            success = false;
                            break;
                        }
                    }
                }
                else
                {
                    success = false;
                }
            }

        if (!is_list)
            while (colon_key_start)
            {
                if (char* colon_key_stop = nextQuotMark(colon_key_start))
                {
                    *colon_key_stop = 0;
                    char close_token;
                    if (const char* colon_value_start = nextTokenOpen(colon_key_stop + 1, &close_token))
                    {
                        bool number = false;
                        if (const char* next_number_delimiter = next_number_end(colon_key_stop + 1))
                        {
                            if (next_number_delimiter <= colon_value_start)
                            {
                                if (const char* _next_number_start = next_number_start(colon_key_stop + 1))
                                {
                                    *((char*)next_number_delimiter) = 0;
                                    m_values.insert(std::make_pair<ZaxStringWrap, const char*>(ZaxStringWrap(colon_key_start + 1), _next_number_start + 0));
                                    number = true;
                                    l_json = (char*)next_number_delimiter;
                                }
                            }
                        }
                        if (!number)
                        {
                            if (const char* colon_value_stop = nextTokenClose(colon_value_start + 1, close_token, colon_value_start[0]))
                            {
                                l_json = (char*)colon_value_stop + 1;
                                if (close_token == '}' || close_token == ']')
                                {
                                    --colon_value_start;
                                    ++colon_value_stop;
                                }
                                *((char*)colon_value_stop) = 0;

                                while (*(--colon_key_stop) == ' ')
                                    *colon_key_stop = 0;
                                while (*(--colon_value_stop) == ' ')
                                    *((char*)colon_value_stop) = 0;
                                m_values.insert(std::make_pair<ZaxStringWrap, const char*>(ZaxStringWrap(colon_key_start + 1), colon_value_start + 1));
                            }
                            else
                            {
                                success = false;
                                break;
                            }
                        }
                    }
                    else
                    {
                        if (const char* next_number_delimiter = next_number_end(colon_key_stop + 1))
                        {
                            if (const char* _next_number_start = next_number_start(colon_key_stop + 1))
                            {
                                *((char*)next_number_delimiter) = 0;
                                m_values.insert(std::make_pair<ZaxStringWrap, const char*>(ZaxStringWrap(colon_key_start + 1), _next_number_start + 0));
                                l_json = (char*)next_number_delimiter;
                            }
                        }
                        else
                        {
                            success = false;
                            break;
                        }
                    }
                }
                colon_key_start = nextQuotMark(l_json);
            }
    }
    if (a_success)
        *a_success = success;
}

ZaxJsonFlatParser::~ZaxJsonFlatParser()
{
    if (m_own_buffer)
        delete[] m_own_buffer;
}

unsigned int ZaxJsonParser::initial_alloc_size_ = 10000;
unsigned int ZaxJsonParser::maximum_alloc_size_ = 100000000;

unsigned int ZaxJsonParser::initial_alloc_size()
{
    return initial_alloc_size_;
}

unsigned int ZaxJsonParser::maximum_alloc_size()
{
    return maximum_alloc_size_;
}

void ZaxJsonParser::set_initial_alloc_size(unsigned int a_size)
{
    initial_alloc_size_ = a_size;
}

void ZaxJsonParser::set_maximum_alloc_size(unsigned int a_size)
{
    maximum_alloc_size_ = a_size;
}
