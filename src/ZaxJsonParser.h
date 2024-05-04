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

#ifndef _ZAXJSONPARSER_H_
#define _ZAXJSONPARSER_H_

#ifndef PRId64
#define PRId64 "ld"
#define PRIu64 "lu"
#endif

#define JSON_OBJ_TRAIL_BYTE -97

class ZaxJsonTopTokenizer
{
    struct cstring_comparator
    {
        bool inline operator()(const char* a_lhs, const char* a_rhs) const
        {
            while (*a_lhs == *a_rhs++)
                if (*a_lhs++ == 0)
                    return false;
            return (*(const unsigned char *)a_lhs - * (const unsigned char *)(a_rhs - 1)) < 0;
        }
    };
    char* m_own_buffer = nullptr;

public:
    std::map<const char*, const char*, cstring_comparator> m_values;
    std::vector<const char*> m_list_values;
    explicit ZaxJsonTopTokenizer(const char* a_json, bool a_in_situ = true, bool* a_success = nullptr);
    virtual ~ZaxJsonTopTokenizer();
};

class ZaxJsonParser
{
    static uint32_t initial_alloc_size_;
    static uint32_t maximum_alloc_size_;
    static uint32_t nr_indent_;
    static bool warnings_enabled_;

    template<typename T, size_t N>
    static inline int print_val(char* a_json, const char* a_json_buffer_end, const T (&a_dst)[N], int a_deep)
    {
        return append(a_json, a_json_buffer_end, "", a_dst, a_deep);
    }

    template <typename vtype>
    static inline int print_val(char* a_json, const char* a_json_buffer_end, const vtype& a_val, int a_deep)
    {
        char* json = print_to_json_alloc(a_val, a_deep);
        int result = snprintf(a_json, a_json_buffer_end - a_json, "%s", json ? json : "");
        delete[] json;
        return result;
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const int32_t a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%d", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const uint32_t a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%u", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const bool a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%s", a_val ? "true" : "false");
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const int8_t a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%c", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const float a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%f", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const double a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%0.8f", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const uint16_t a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%d", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const uint8_t a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%hhu", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const long long int a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%" "lld", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const long int a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%" "ld", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const long long unsigned int a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%" "llu", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const long unsigned int a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%" "lu", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const char* a_val, int a_deep)
    {
        if (a_val[0] != JSON_OBJ_TRAIL_BYTE)
            return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\"", a_val);
        else
            return snprintf(a_json, a_json_buffer_end - a_json, "%s", ++a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const std::string& a_val, int a_deep)
    {
        if (a_val.c_str()[0] != JSON_OBJ_TRAIL_BYTE)
            return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\"", a_val.c_str());
        else
            return snprintf(a_json, a_json_buffer_end - a_json, "%s", a_val.c_str() + 1);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const int16_t a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%d", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const long double a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%Le", a_val);
    }

    template <template <typename, typename... > class ct,  class vt>
    static inline int print_val(char* a_json, const char* a_json_buffer_end, const ct<vt>& a_vals, int a_deep)
    {
        return ZaxJsonParser::append(a_json, a_json_buffer_end, "", a_vals, a_deep);
    }

    template <template <typename, typename, typename... > class ct, class mt, class cot>
    static inline int print_val(char* a_json, const char* a_json_buffer_end, const ct<std::string, mt, cot>& a_vals, int a_deep)
    {
        return ZaxJsonParser::append(a_json, a_json_buffer_end, "", a_vals, a_deep);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const char* a_val, int a_deep)
    {
        if (a_val[0] != JSON_OBJ_TRAIL_BYTE)
            return snprintf(a_json, a_json_buffer_end - a_json, R"("%s":"%s")", a_key, a_val);
        else
            return snprintf(a_json, a_json_buffer_end - a_json, R"("%s":%s)", a_key, ++a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const std::string& a_val, int a_deep)
    {
        if (a_val.c_str()[0] != JSON_OBJ_TRAIL_BYTE)
            return snprintf(a_json, a_json_buffer_end - a_json, R"("%s":"%s")", a_key, a_val.c_str());
        else
            return snprintf(a_json, a_json_buffer_end - a_json, R"("%s":%s)", a_key, a_val.c_str() + 1);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const int32_t a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%d", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const uint32_t a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%u", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const bool a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%s", a_key, a_val ? "true" : "false");
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const int8_t a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%c", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const float a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%f", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const double a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%0.8f", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const uint16_t a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%d", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const uint8_t a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%hhu", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const long long int a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%" "lld", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const long int a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%" "ld", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const long long unsigned int a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%" "llu", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const long unsigned int a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%" "lu", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const int16_t a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%d", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const long double a_val, int a_deep)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%Le", a_key, a_val);
    }

    template <typename vtype>
    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const vtype& a_val, int a_deep)
    {
        char* json = print_to_json_alloc(a_val, a_deep);
        int result = snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%s", a_key, json ? json : "");
        delete[] json;
        return result;
    }

    static inline void get_val(std::string& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        a_dst = a_json ? a_json : "";
    }

    template<size_t N>
    static inline void get_val(char (&a_dst)[N], const char* a_json, std::vector<std::string>* a_error_output)
    {
        a_json ? strncpy(a_dst, a_json, N) : strncpy(a_dst, "", N);
    }

    template<typename T, size_t N>
    static inline void get_val(T (&a_dst)[N], const char* a_json, std::vector<std::string>* a_error_output)
    {
        parse(a_dst, a_json, a_error_output);
    }

    static inline void get_val(int32_t& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char *endptr;
        errno = 0;
        int32_t tmp = 0;
        if (a_json)
            tmp = strtol(a_json, &endptr, 10);

        if (a_error_output)
        {
            if (errno == 0 && endptr != a_json)
                a_dst = tmp;
            else
                a_error_output->push_back(std::string("ERROR: error parsing int32_t in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    static inline void get_val(uint32_t& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char* endptr;
        errno = 0;
        uint32_t tmp = 0;
        if (a_json)
            tmp = strtoul(a_json, &endptr, 10);

        if (a_error_output)
        {
            if (errno == 0 && a_json[0] != '-' && endptr != a_json)
                a_dst = tmp;
            else
                a_error_output->push_back(std::string("ERROR: error parsing uint32_t in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    static inline void get_val(bool& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        if (a_json)
        {
            switch (*((int*)a_json))
            {
            case 1702195828: /** true */
                a_dst = true;
                break;
            case 1936482662: /** fals(e) */
                if (a_json[4] == 'e')
                {
                    a_dst = false;
                    break;
                }
            default:
                if (a_error_output)
                    a_error_output->push_back(std::string("ERROR: '") + a_json + "' is not a boolean\n");
            }
        }
        else
            a_dst = false;
    }

    static inline void get_val(int8_t& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char *endptr;
        errno = 0;
        int32_t tmp = 0;
        if (a_json)
            tmp = strtol(a_json, &endptr, 10);

        if (a_error_output)
        {
            if (errno == 0  && endptr != a_json) {
                if (tmp <= INT8_MAX && tmp >= INT8_MIN)
                    a_dst = tmp;
                else
                    a_error_output->push_back(std::string("ERROR: error parsing int8_t in JSON, out of range: '") + a_json + "'\n");
            }
            else
                a_error_output->push_back(std::string("ERROR: error parsing int8_t in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    static inline void get_val(float& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char *endptr;
        errno = 0;
        float tmp = 0.0;
        if (a_json)
            tmp = strtof(a_json, &endptr);

        if (a_error_output)
        {
            if (errno == 0 && endptr != a_json)
                a_dst = tmp;
            else
                a_error_output->push_back(std::string("ERROR: error parsing float in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    static inline void get_val(double& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char *endptr;
        errno = 0;
        double tmp = 0.0;
        if (a_json)
            tmp = strtod(a_json, &endptr);

        if (a_error_output)
        {
            if (errno == 0 && endptr != a_json)
                a_dst = tmp;
            else
                a_error_output->push_back(std::string("ERROR: error parsing double in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    static inline void get_val(uint16_t& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char* endptr;
        errno = 0;
        int32_t tmp = 0;
        if (a_json)
            tmp = strtol(a_json, &endptr, 10);

        if (a_error_output)
        {
            if (errno == 0 && endptr != a_json) {
                if (tmp <= UINT16_MAX && tmp >= 0)
                    a_dst = tmp;
                else
                    a_error_output->push_back(std::string("ERROR: error parsing uint16_t in JSON, out of range: '") + a_json + "'\n");
            }
            else
                a_error_output->push_back(std::string("ERROR: error parsing uint16_t in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    static inline void get_val(uint8_t& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char* endptr;
        errno = 0;
        int32_t tmp = 0;
        if (a_json)
            tmp = strtol(a_json, &endptr, 10);

        if (a_error_output)
        {
            if (errno == 0 && endptr != a_json) {
                if (tmp <= UINT8_MAX && tmp >= 0)
                    a_dst = tmp;
                else
                    a_error_output->push_back(std::string("ERROR: error parsing uint8_t in JSON, out of range: '") + a_json + "'\n");
            }
            else
                a_error_output->push_back(std::string("ERROR: error parsing uint8_t in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    static inline void get_val(long long int& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char *endptr;
        errno = 0;
        long long int tmp = 0;
        if (a_json)
            tmp = strtoll(a_json, &endptr, 10);

        if (a_error_output)
        {
            if (errno == 0 && endptr != a_json)
                a_dst = tmp;
            else
                a_error_output->push_back(std::string("ERROR: error parsing long long int in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    static inline void get_val(long int& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char *endptr;
        errno = 0;
        long long int tmp = 0;
        if (a_json)
            tmp = strtoll(a_json, &endptr, 10);

        if (a_error_output)
        {
            if (errno == 0 && endptr != a_json)
                a_dst = tmp;
            else
                a_error_output->push_back(std::string("ERROR: error parsing long int in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    static inline void get_val(long long unsigned int& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char *endptr;
        errno = 0;
        long unsigned int tmp = 0;
        if (a_json)
            tmp = strtoull(a_json, &endptr, 10);

        if (a_error_output)
        {
            if (errno == 0 && a_json[0] != '-' && endptr != a_json)
                a_dst = tmp;
            else
                a_error_output->push_back(std::string("ERROR: error parsing long long unsigned int in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    static inline void get_val(long unsigned int& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char *endptr;
        errno = 0;
        long unsigned int tmp = 0;
        if (a_json)
            tmp = strtoull(a_json, &endptr, 10);

        if (a_error_output)
        {
            if (errno == 0 && a_json[0] != '-' && endptr != a_json)
                a_dst = tmp;
            else
                a_error_output->push_back(std::string("ERROR: error parsing long unsigned int in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    static inline void get_val(int16_t& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char *endptr;
        errno = 0;
        int32_t tmp = 0;
        if (a_json)
            tmp = strtol(a_json, &endptr, 10);

        if (a_error_output)
        {
            if (errno == 0 && endptr != a_json)
            {
                if (tmp <= INT16_MAX && tmp >= INT16_MIN)
                    a_dst = tmp;
                else
                    a_error_output->push_back(std::string("ERROR: error parsing int16_t in JSON, out of range: '") + a_json + "'\n");
            }
            else
                 a_error_output->push_back(std::string("ERROR: error parsing int16_t in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    static inline void get_val(long double& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        char *endptr;
        errno = 0;
        long double tmp = 0.0;
        if (a_json)
            tmp = strtold(a_json, &endptr);

        if (a_error_output)
        {
            if (errno == 0 && endptr != a_json)
                a_dst = tmp;
            else
                a_error_output->push_back(std::string("ERROR: error parsing long double in JSON: '") + strerror(errno) + ": " + a_json + "'\n");
        }
        else
            a_dst = tmp;
    }

    template <template <typename, typename... > class ct,  class vt>
    static inline void get_val(ct<vt>& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        a_json ? ZaxJsonParser::parse(a_dst, a_json, a_error_output) : a_dst.clear();
    }

    template <template <typename, typename, typename... > class ct, class mt, class cot>
    static inline void get_val(ct<std::string, mt, cot>& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        a_json ? ZaxJsonParser::parse(a_dst, a_json, a_error_output) : a_dst.clear();
    }

    template <typename vtype>
    static inline void get_val(vtype& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        a_dst.zax_from_json(a_json, a_error_output);
    }

    template <typename vtype>
    static inline void get_val(vtype*& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        a_dst->zax_from_json(a_json, a_error_output);
    }

    static inline int json_begin(char* a_json, const char* a_json_buffer_end, const char* a_key, const char* a_brace)
    {
        int result = 0;
        if (a_json < a_json_buffer_end)
        {
            if (a_key && a_key[0] && a_key[0] != '^')
                result = snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%s", a_key, a_brace);
            else
                cat_char_noinc(a_json, result, a_brace[0]);
        }
        return result;
    }

    static inline void json_end(int& a_result, char* a_json, const char* a_json_buffer_end, const char* a_brace)
    {
        if (snprintf(a_json + strlen(a_json), a_json_buffer_end - a_json, "%s", a_brace) == 1)
            ++a_result;
    }

    template <typename vtype>
    static inline char* print_to_json_alloc(const vtype& a_val, int a_deep)
    {
        uint32_t alloc_size = initial_alloc_size();
        char* json = new char[alloc_size];
        while (!a_val.zax_to_json(json, alloc_size - 1, a_deep))
            if (reallocate_json(json, alloc_size))
                break;
        return json;
    }

public:
    static uint32_t initial_alloc_size();
    static uint32_t maximum_alloc_size();
    static uint32_t get_nr_indent();
    static void set_initial_alloc_size(uint32_t a_size);
    static void set_maximum_alloc_size(uint32_t a_size);
    static void set_indent(uint32_t a_size);
    static bool warnings_enabled();
    static void set_warnings_enabled(bool val);

    static inline void indent(char*& a_json, int& a_result, int a_deep)
    {
        if (uint32_t nr_indent = ZaxJsonParser::get_nr_indent())
        {
            *a_json++ = '\n';
            ++a_result;
            for (uint32_t i = 0; i < nr_indent * a_deep; ++i)
            {
                *a_json++ = ' ';
                ++a_result;
            }
            a_json[0] = 0;
        }
    }

    static inline void cat_comma_space(char*& a_json, int& a_result, int a_deep)
    {
        *a_json = ',';
        if (nr_indent_)
        {
            *++a_json = '\n';
            for (uint32_t i = 0; i < nr_indent_ * a_deep; ++i)
            {
                *++a_json = ' ';
                ++a_result;
            }
        }
        else
            *++a_json = ' ';
        *++a_json = 0;
        a_result += 2;
    }

    static inline void cat_char_noinc(char* a_json, int& a_result, char a_char)
    {
        *a_json = a_char;
        a_json[1] = 0;
        ++a_result;
    }

    static inline bool reallocate_json(char*& a_json, uint32_t& a_alloc_size)
    {
        delete[] a_json;
        a_alloc_size *= 2;
        if (a_alloc_size > maximum_alloc_size())
            return (a_json = nullptr);
        return (a_json = new char[a_alloc_size]);
    }

    static inline int append(char* a_json, const char* a_json_buffer_end, const char* a_key, const std::string& a_value, int a_deep)
    {
        return print_key_and_val(a_json, a_json_buffer_end, a_key, a_value, a_deep);
    }

    template <typename vt>
    static inline int append(char* a_json, const char* a_json_buffer_end, const char* a_key, const vt& a_value, int a_deep)
    {
        return print_key_and_val(a_json, a_json_buffer_end, a_key, a_value, a_deep);
    }

    template<size_t N>
    static inline int append(char* a_json, const char* a_json_buffer_end, const char* a_key, const char (&a_values)[N], int a_deep)
    {
        return print_key_and_val(a_json, a_json_buffer_end, a_key, a_values, a_deep);
    }

    template<typename T, size_t N>
    static inline int append(char* a_json, const char* a_json_buffer_end, const char* a_key, const T (&a_values)[N], int a_deep)
    {
        int _result = json_begin(a_json, a_json_buffer_end, a_key, "[");
        if (_result > 0)
        {
            a_json += _result;
            for (uint32_t i = 0; i < N; ++i)
            {
                if (i)
                    cat_comma_space(a_json, _result, a_deep);
                int written = print_val(a_json, a_json_buffer_end, a_values[i], a_deep);
                if (written <= 0)
                {
                    _result = 0;
                    break;
                }
                a_json += written;
                _result += written;
            }
            json_end(_result, a_json, a_json_buffer_end, "]");
        }
        return _result;
    }

    template <template <typename, typename... > class ct, class vt>
    static inline int append(char* a_json, const char* a_json_buffer_end, const char* a_key, const ct<vt>& a_values, int a_deep)
    {
        int _result = json_begin(a_json, a_json_buffer_end, a_key, "[");
        if (_result > 0)
        {
            a_json += _result;
            for (typename ct<vt>::const_iterator r = a_values.begin(); r != a_values.end(); ++r)
            {
                if (r != a_values.begin())
                    cat_comma_space(a_json, _result, a_deep);
                int written = print_val(a_json, a_json_buffer_end, *r, a_deep);
                if (written <= 0)
                {
                    _result = 0;
                    break;
                }
                a_json += written;
                _result += written;
            }
            json_end(_result, a_json, a_json_buffer_end, "]");
        }
        return _result;
    }

    template <template <typename, typename... > class ct,  class vt>
    static inline int append(char* a_json, const char* a_json_buffer_end, const char* a_key, const ct<vt*>& a_values, int a_deep)
    {
        int _result = json_begin(a_json, a_json_buffer_end, a_key, "[");
        if (_result > 0)
        {
            a_json += _result;
            for (typename ct<vt*>::const_iterator r = a_values.begin(); r != a_values.end(); ++r)
            {
                if (r != a_values.begin())
                    cat_comma_space(a_json, _result, a_deep);
                int written = print_val(a_json, a_json_buffer_end, **r, a_deep);
                if (written <= 0)
                {
                    _result = 0;
                    break;
                }
                a_json += written;
                _result += written;
            }
            json_end(_result, a_json, a_json_buffer_end, "]");
        }
        return _result;
    }

    template <template <typename, typename, typename... > class ct, class mt, class cot>
    static inline int append(char* a_json, const char* a_json_buffer_end, const char* a_key, const ct<std::string, mt, cot>& a_values, int a_deep)
    {
        int _result = json_begin(a_json, a_json_buffer_end, a_key, "{");
        if (_result > 0)
        {
            a_json += _result;
            indent(a_json, _result, ++a_deep);
            for (typename ct<std::string, mt, cot>::const_iterator r = a_values.begin(); r != a_values.end(); ++r)
            {
                if (r != a_values.begin())
                    cat_comma_space(a_json, _result, a_deep);
                int written = print_key_and_val(a_json, a_json_buffer_end, r->first.c_str(), r->second, a_deep);
                if (written <= 0)
                {
                    _result = 0;
                    break;
                }
                a_json += written;
                _result += written;
            }
            indent(a_json, _result, --a_deep);
            json_end(_result, a_json, a_json_buffer_end, "}");
        }
        return _result;
    }

    template <typename type>
    static inline void parse(type& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        get_val(a_dst, a_json, a_error_output);
    }

    static inline void parse(std::string& a_dst, const char* a_json, std::vector<std::string>* a_error_output)
    {
        get_val(a_dst, a_json, a_error_output);
    }

    template<size_t N>
    static inline void parse(char (&a_vect)[N], const char* a_json, std::vector<std::string>* a_error_output)
    {
        get_val(a_vect, a_json, a_error_output);
    }

    template<typename T, size_t N>
    static inline void parse(T (&a_vect)[N], const char* a_json, std::vector<std::string>* a_error_output)
    {
        if (a_json)
        {
            bool success = false;
            ZaxJsonTopTokenizer vector_data(a_json, false, &success);
            if (!success && a_error_output)
                a_error_output->push_back(std::string("ERROR: error parsing a vector in JSON: '") + a_json + "'\n");
            uint32_t l_size = N > vector_data.m_list_values.size() ? vector_data.m_list_values.size() : N;
            for (uint32_t i = 0; i < l_size; ++i)
                get_val(a_vect[i], vector_data.m_list_values[i], a_error_output);
        }
        else
            for (uint32_t i = 0; i < N; ++i)
                get_val(a_vect[i], 0, a_error_output);
    }

    template <template <typename, typename... > class ct,  class vt>
    static inline void parse(ct<vt>& a_vect, const char* a_json, std::vector<std::string>* a_error_output)
    {
        if (a_json)
        {
            bool success = false;
            ZaxJsonTopTokenizer vector_data(a_json, false, &success);
            if (!success && a_error_output)
                a_error_output->push_back(std::string("ERROR: error parsing a vector in JSON: '") + a_json + "'\n");
            a_vect.resize(vector_data.m_list_values.size());
            auto r = a_vect.begin();
            for (auto & m_list_value : vector_data.m_list_values)
                get_val(*r++, m_list_value, a_error_output);
        }
        else
            a_vect.clear();
    }

    template <template <typename, typename, typename... > class ct, class mt, class cot>
    static inline void parse(ct<std::string, mt, cot>& a_map, const char* a_json, std::vector<std::string>* a_error_output)
    {
        a_map.clear();
        if (a_json)
        {
            bool success = false;
            ZaxJsonTopTokenizer vector_data(a_json, false, &success);
            if (!success && a_error_output)
                a_error_output->push_back(std::string("ERROR: error parsing a map in JSON: '") + a_json + "'\n");
            for (auto & m_value : vector_data.m_values)
            {
                if (!m_value.second)
                    a_map.erase(m_value.first);
                else
                {
                    mt tmp;
                    get_val(tmp, m_value.second, a_error_output);
                    a_map.insert(std::make_pair<std::string, mt>(m_value.first, mt(tmp)));
                }
            }
        }
    }
};

template <typename mt>
std::pair<const char*, mt*> json_property(const char* a_name, mt& a_obj)
{
    return std::make_pair(a_name, &a_obj);
}

template<std::size_t I = 0, typename... vt>
inline typename std::enable_if<I == sizeof...(vt), void>::type
zax_to_json_(char* a_json, const char* a_json_buffer_end, int& a_result, std::tuple<vt...> a_tuple, bool a_insert_object_trails, int a_deep)
{}

template<std::size_t I = 0, typename... vt>
inline typename std::enable_if < I < sizeof...(vt), void>::type
zax_to_json_(char* a_json, const char* a_json_buffer_end, int& a_result, std::tuple<vt...> a_tuple, bool a_insert_object_trails, int a_deep)
{
    if (!I)
    {
        if (std::get<I>(a_tuple).first[0] == '^')
            a_insert_object_trails = false;
        if ((a_result = a_json_buffer_end - a_json > 1))
        {
            ++a_deep;
            if (a_insert_object_trails)
            {
                ZaxJsonParser::cat_char_noinc(a_json++, a_result, '{');
                ZaxJsonParser::indent(a_json, a_result, a_deep);
            }
            else
                a_json[0] = 0;
        }
    }

    int l_result = ZaxJsonParser::append(a_json, a_json_buffer_end, std::get<I>(a_tuple).first, *std::get<I>(a_tuple).second, a_deep);
    if (!l_result)
        return;
    a_json += l_result;
    a_result += l_result;
    if (I < sizeof...(vt) - 1)
        ZaxJsonParser::cat_comma_space(a_json, a_result, a_deep);

    zax_to_json_ < I + 1, vt... > (a_json, a_json_buffer_end, a_result, a_tuple, a_insert_object_trails, a_deep);
    if (!a_result)
        return;
    if ((I == sizeof...(vt) - 1) && a_insert_object_trails)
    {
        --a_deep;
        if ((a_json_buffer_end - a_json) > 1)
        {
            ZaxJsonParser::indent(a_json, a_result, a_deep);
            ZaxJsonParser::cat_char_noinc(a_json, a_result, '}');
        }
        else
            a_result = 0;
    }
}

#define JSON_PROPERTY_(member) json_property(#member, object_to_convert.member)
#define JSON_PROPERTY_NAME(member, name_in_json) json_property(name_in_json, object_to_convert.member)
#define JSON_PROPERTY_CHOOSER(no_arg,member,name_in_json,FUNC, ...) FUNC
#define JSON_PROPERTY(...) JSON_PROPERTY_CHOOSER(no_arg,##__VA_ARGS__, JSON_PROPERTY_NAME(__VA_ARGS__), JSON_PROPERTY_(__VA_ARGS__))

#define zax_convert_to_json_trails(arg_json, a_json_buffer_length, arg_obj, insert_trails, a_deep, ...) ({\
    int _result = 0;\
    decltype(arg_obj)& object_to_convert = arg_obj; \
    const char* _json_buffer_end = arg_json + a_json_buffer_length; \
    zax_to_json_(arg_json, _json_buffer_end, _result, std::make_tuple(__VA_ARGS__), insert_trails, a_deep);\
    _result;\
})
#define zax_convert_to_json_deep(arg_json, a_json_buffer_length, arg_obj, a_deep, ...) zax_convert_to_json_trails(arg_json, a_json_buffer_length, arg_obj, true, a_deep, __VA_ARGS__)
#define zax_convert_to_json(arg_json, a_json_buffer_length, arg_obj, ...) zax_convert_to_json_deep(arg_json, a_json_buffer_length, arg_obj, 0, __VA_ARGS__)

template<std::size_t I = 0, typename... vt>
inline typename std::enable_if<I == sizeof...(vt), void>::type
zax_from_json_(const char* a_json, std::tuple<vt...> a_tuple, ZaxJsonTopTokenizer* parsed_json, std::vector<std::string>* a_error_output = nullptr)
{}

template<std::size_t I = 0, typename... vt>
inline typename std::enable_if < I < sizeof...(vt), void>::type
zax_from_json_(const char* a_json, std::tuple<vt...> a_tuple, ZaxJsonTopTokenizer* parsed_json, std::vector<std::string>* a_error_output = nullptr)
{
    if (!parsed_json)
    {
        if (std::get<I>(a_tuple).first[0] == '^')
            ZaxJsonParser::parse(*std::get<I>(a_tuple).second, a_json, a_error_output);
        else
        {
            bool success = false;
            parsed_json = new ZaxJsonTopTokenizer(a_json, false, &success);
            if (!success && a_error_output)
                a_error_output->push_back(std::string("ERROR: error parsing JSON: '") + a_json + "'\n");
        }
    }
    if (parsed_json && I < sizeof...(vt))
    {
        auto it = parsed_json->m_values.find(std::get<I>(a_tuple).first);
        if (it != parsed_json->m_values.end())
            ZaxJsonParser::parse(*std::get<I>(a_tuple).second, parsed_json->m_values[std::get<I>(a_tuple).first], a_error_output);
        else if (a_error_output && ZaxJsonParser::warnings_enabled())
            a_error_output->push_back(std::string("WARNING: JSON property is missing: '") + std::get<I>(a_tuple).first + "'\n");
    }
    zax_from_json_ < I + 1, vt... > (a_json, a_tuple, parsed_json, a_error_output);
    if (I == sizeof...(vt) - 1)
        delete parsed_json;
}

template<std::size_t I = 0, typename... vt>
inline typename std::enable_if<I == sizeof...(vt), void>::type
zax_from_json_(char* a_json, std::tuple<vt...> a_tuple, ZaxJsonTopTokenizer* parsed_json, std::vector<std::string>* a_error_output = nullptr)
{}

template<std::size_t I = 0, typename... vt>
inline typename std::enable_if < I < sizeof...(vt), void>::type
zax_from_json_(char* a_json, std::tuple<vt...> a_tuple, ZaxJsonTopTokenizer* parsed_json, std::vector<std::string>* a_error_output = nullptr)
{
    if (!parsed_json)
    {
        bool success = false;
        parsed_json = new ZaxJsonTopTokenizer(a_json, true, &success);
        if (!success && a_error_output)
            a_error_output->push_back(std::string("ERROR: error parsing JSON: '") + a_json + "'\n");
    }
    if (I < sizeof...(vt))
    {
        auto it = parsed_json->m_values.find(std::get<I>(a_tuple).first);
        if (it != parsed_json->m_values.end())
            ZaxJsonParser::parse(*std::get<I>(a_tuple).second, parsed_json->m_values[std::get<I>(a_tuple).first], a_error_output);
        else if (a_error_output && ZaxJsonParser::warnings_enabled())
            a_error_output->push_back(std::string("WARNING: JSON property is missing: '") + std::get<I>(a_tuple).first + "'\n");
    }
    zax_from_json_ < I + 1, vt... > (a_json, a_tuple, parsed_json, a_error_output);
    if (I == sizeof...(vt) - 1)
        delete parsed_json;
}

#define zax_convert_from_json(arg_json, arg_obj, ...) decltype(arg_obj)& object_to_convert = arg_obj; zax_from_json_(arg_json, std::make_tuple(__VA_ARGS__), 0);
#define zax_convert_from_json_err(arg_json, arg_obj, err_stream, ...) decltype(arg_obj)& object_to_convert = arg_obj; zax_from_json_(arg_json, std::make_tuple(__VA_ARGS__), 0, err_stream);

#define ZAX_JSON_SERIALIZABLE_BASIC(...)\
    virtual void zax_from_json(const char* a_json, std::vector<std::string>* a_err_stream = 0) {\
        zax_convert_from_json_err(a_json, *this, a_err_stream, ##__VA_ARGS__);\
    }\
    virtual int zax_to_json(char* a_json, int a_alloc_size, int a_deep = 0) const {\
        return zax_convert_to_json_deep(a_json, a_alloc_size, *this, a_deep, ##__VA_ARGS__);\
    }\
    virtual std::string zax_to_json(int a_deep = 0, bool as_obj = false) const {\
        uint32_t alloc_size = ZaxJsonParser::initial_alloc_size();\
        char* json = new char[alloc_size];\
        if (as_obj) {*json = JSON_OBJ_TRAIL_BYTE; ++json;}\
        while (!zax_convert_to_json_deep(json, alloc_size - 1, *this, a_deep, ##__VA_ARGS__))\
            if (!ZaxJsonParser::reallocate_json(json, alloc_size))\
                break;\
        if (as_obj) --json;\
        std::string a_json = json ? json : "";\
        delete[] json;\
        return a_json;\
    }

#define ZAX_JSON_SERIALIZABLE_WDC(class_name, ...)\
    ZAX_JSON_SERIALIZABLE_BASIC(__VA_ARGS__)\
    class_name(const char* a_json) {\
        *this = a_json;\
    }\
    class_name(const std::string& a_json) {\
        *this = a_json;\
    }\
    virtual void operator = (const char* a_json) {\
        zax_convert_from_json(a_json, *this, ##__VA_ARGS__);\
    }\
    virtual void operator = (const std::string& a_json) {\
        zax_convert_from_json(a_json.c_str(), *this, ##__VA_ARGS__);\
    }\
    template <typename T> operator T() const {\
        return zax_to_json();\
    }\
    friend std::ostream& operator<<(std::ostream& os, const class_name& a_obj) {\
        std::string s = a_obj;\
        return os << s;\
    }\

#define ZAX_JSON_SERIALIZABLE(class_name, ...)\
    class_name() {}\
    ZAX_JSON_SERIALIZABLE_WDC(class_name, ##__VA_ARGS__)\

#endif /// _ZAXJSONPARSER_H_
