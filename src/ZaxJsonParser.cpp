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

#include <climits>
#include <cstring>
#include <string>
#include <cstdio>
#include <map>
#include <vector>
#include "ZaxJsonParser.h"

static inline const char* zax_get_close_token(const char* a_str_to_find, char a_open_token, char a_close_token)
{
    int equilibrium = 0;
    while (*a_str_to_find++)
    {
        if (*a_str_to_find == a_close_token)
        {
            if (*(a_str_to_find - 1) != '\\')
            {
                if (equilibrium == 0)
                    return a_str_to_find;
                else
                    --equilibrium;
            }
        }
        else if (*a_str_to_find == a_open_token)
            if (*(a_str_to_find - 1) != '\\')
                ++equilibrium;
    }
    return nullptr;
}

static inline const char* zax_get_value_end(const char* a_str_to_find, bool a_whitespace_means_end = false)
{
    const char* l_end;
    while (*a_str_to_find)
    {
        switch (*a_str_to_find++)
        {
        case '\\':
            ++a_str_to_find;
            break;
        case '\t':
        case '\n':
        case '\r':
        case ' ':
            if (!a_whitespace_means_end)
                break;
        case ',':
        case '}':
        case ']':
        case '"':
            return --a_str_to_find;
        case '{':
            return (l_end = zax_get_close_token(a_str_to_find - 1, '{', '}')) ? (++l_end) : l_end;
        case '[':
            return (l_end = zax_get_close_token(a_str_to_find - 1, '[', ']')) ? (++l_end) : l_end;
        }
    }
    return nullptr;
}

static inline bool zax_is_value(const char* a_str_to_find)
{
    while (*a_str_to_find)
    {
        switch (*a_str_to_find++)
        {
        case '\t':
        case '\n':
        case '\r':
        case ' ':
        case 0:
            break;
        default:
            return true;
        }
    }
    return false;
}

static inline const char* zax_get_value_start(const char* a_str_to_find)
{
    bool colon_found = false;
    while (*a_str_to_find)
    {
        switch (*a_str_to_find++)
        {
        case ':':
            colon_found = true;
        case '\t':
        case '\n':
        case '\r':
        case ' ':
            break;
        case '\\':
            ++a_str_to_find;
            break;
        case '}':
        case ']':
            return nullptr;
        case '"':
            if (colon_found)
                return a_str_to_find;
            else
                return nullptr;
        default:
            if (colon_found)
                return --a_str_to_find;
            else
                return nullptr;
        }
    }
    return nullptr;
}

enum EJsonState
{
    EJson_Begin = 0,
    EJson_Object,
    EJson_Array
};

ZaxJsonTopTokenizer::ZaxJsonTopTokenizer(const char* a_json, bool a_in_situ, bool* a_success)
{
    bool result = true;
    if (a_json)
    {
        if (!a_in_situ)
        {
            m_own_buffer = new char[strlen(a_json) + 1];
            strcpy(m_own_buffer, a_json);
            a_json = m_own_buffer;
        }
        EJsonState state = EJson_Begin;
        char* close_token = (char*)-1;
        while (a_json && (a_json < close_token) && result)
        {
            switch (*a_json)
            {
            case '\t':
            case '\n':
            case '\r':
            case ' ':
                break;
            case '{':
                if (state == EJson_Begin)
                {
                    state = EJson_Object;
                    if ((close_token = (char*)zax_get_close_token(a_json, '{', '}')) == nullptr)
                        result = false;
                }
                else
                    goto def_label;
                break;
            case '[':
                if (state == EJson_Begin)
                {
                    state = EJson_Array;
                    if ((close_token = (char*)zax_get_close_token(a_json, '[', ']')) == nullptr)
                        result = false;
                }
                else
                    goto def_label;
                break;
            case '"':
            {
                if (char* closing_brace = (char*)zax_get_close_token(a_json, '"', '"'))
                {
                    *closing_brace = 0;
                    if (state == EJson_Array)
                    {
                        if (zax_is_value(++a_json))
                            m_list_values.push_back(a_json);
                        a_json = closing_brace;
                    }
                    else if (state == EJson_Object)
                    {
                        if (char* value = (char*)zax_get_value_start(closing_brace + 1))
                        {
                            if (char* value_end = (char*)zax_get_value_end(value))
                            {
                                *value_end = 0;
                                if (*((int*)(value)) == 1819047278) /** null */
                                    m_values.insert(std::make_pair((++a_json), (char*)nullptr));
                                else
                                    m_values.insert(std::make_pair(++a_json, value));
                                a_json = value_end;
                            }
                            else
                                result = false;
                        }
                        else
                            result = false;
                    }
                    else
                        result = false;
                }
                else
                    result = false;
            }
            break;
            case ',':
                break;
            default:
def_label:
                if (state == EJson_Array)
                {
                    if (char* value_end = (char*)zax_get_value_end(a_json, true))
                    {
                        *value_end = 0;
                        if (zax_is_value(a_json))
                            m_list_values.push_back(a_json);
                        a_json = value_end;
                    }
                    else
                        result = false;
                }
                else
                    result = false;
                break;
            }
            ++a_json;
        }
    }
    else
        result = false;
    if (a_success)
        *a_success = result;
}

ZaxJsonTopTokenizer::~ZaxJsonTopTokenizer()
{
    delete[] m_own_buffer;
}

unsigned int ZaxJsonParser::initial_alloc_size_ = 10000000;
unsigned int ZaxJsonParser::maximum_alloc_size_ = 100000000;
unsigned int ZaxJsonParser::nr_indent_ = 0;
bool ZaxJsonParser::warnings_enabled_ = true;

unsigned int ZaxJsonParser::initial_alloc_size()
{
    return initial_alloc_size_;
}

unsigned int ZaxJsonParser::maximum_alloc_size()
{
    return maximum_alloc_size_;
}

unsigned int ZaxJsonParser::get_nr_indent()
{
    return nr_indent_;
}

void ZaxJsonParser::set_initial_alloc_size(unsigned int a_size)
{
    initial_alloc_size_ = a_size;
}

void ZaxJsonParser::set_maximum_alloc_size(unsigned int a_size)
{
    maximum_alloc_size_ = a_size;
}

void ZaxJsonParser::set_indent(unsigned int a_size)
{
    nr_indent_ = a_size;
}

bool ZaxJsonParser::warnings_enabled()
{
    return warnings_enabled_;
}

void ZaxJsonParser::set_warnings_enabled(bool val)
{
    warnings_enabled_ = val;
}
