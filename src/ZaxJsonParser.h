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

struct ZaxStringWrap
{
    const char* m_str;
    ZaxStringWrap(const char* a_str);
    bool operator < (const ZaxStringWrap &a_rhs) const;
};

class ZaxJsonFlatParser
{
    char* m_own_buffer;
public:
    std::map<ZaxStringWrap, const char*> m_values;
    std::vector<const char*> m_list_values;
    ZaxJsonFlatParser(const char* a_json, bool a_in_situ = true, bool* a_success = 0);
    virtual ~ZaxJsonFlatParser();
};

class ZaxJsonParser
{
    static unsigned int initial_alloc_size_;
    static unsigned int maximum_alloc_size_;

    template <typename vtype>
    static inline int print_val(char* a_json, const char* a_json_buffer_end, const vtype& a_val)
    {
        int result = 0;
        unsigned int alloc_size = initial_alloc_size();
        char* json = new char[alloc_size];
        while (!a_val.to_json(json, json + alloc_size - 1))
        {
            delete[] json;
            alloc_size *= 2;
            if (alloc_size > maximum_alloc_size())
                return false;
            json = new char[alloc_size];
        }
        result = snprintf(a_json, a_json_buffer_end - a_json, "%s", json);
        delete[] json;
        return result;
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const int a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%d", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const unsigned int a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%u", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const bool a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%s", a_val ? "true" : "false");
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const char a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%c", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const float a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "%f", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const char* a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\"", a_val);
    }

    static inline int print_val(char* a_json, const char* a_json_buffer_end, const std::string& a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\"", a_val.c_str());
    }

    template <template <typename, typename... > class ct,  class vt>
    static inline int print_val(char* a_json, const char* a_json_buffer_end, const ct<vt>& a_vals)
    {
        return ZaxJsonParser::append(a_json, a_json_buffer_end, "", a_vals);
    }

    template <typename mt>
    static inline int print_val(char* a_json, const char* a_json_buffer_end, const std::map<std::string, mt>& a_vals)
    {
        return ZaxJsonParser::append(a_json, a_json_buffer_end, "", a_vals);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const char* a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":\"%s\"", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const std::string& a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":\"%s\"", a_key, a_val.c_str());
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const int a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%d", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const unsigned int a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%d", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const bool a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%s", a_key, a_val ? "true" : "false");
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const char a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%c", a_key, a_val);
    }

    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const float a_val)
    {
        return snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%f", a_key, a_val);
    }

    template <typename vtype>
    static inline int print_key_and_val(char* a_json, const char* a_json_buffer_end, const char* a_key, const vtype& a_val)
    {
        int result = 0;
        unsigned int alloc_size = initial_alloc_size();
        char* json = new char[alloc_size];
        while (!a_val.to_json(json, json + alloc_size - 1))
        {
            delete[] json;
            alloc_size *= 2;
            if (alloc_size > maximum_alloc_size())
                return false;
            json = new char[alloc_size];
        }
        result = snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%s", a_key, json);
        delete[] json;
        return result;
    }

    static inline void get_val(std::string& a_dst, const char* a_json, std::string* a_error_output)
    {
        a_dst = a_json ? a_json : "";
    }

    static inline void get_val(char* a_dst, const char* a_json, std::string* a_error_output)
    {
        a_json ? strcpy(a_dst, a_json) : strcpy(a_dst, "");
    }

    static inline void get_val(int& a_dst, const char* a_json, std::string* a_error_output)
    {
        a_dst = a_json ? atoi(a_json) : 0;
    }

    static inline void get_val(unsigned int& a_dst, const char* a_json, std::string* a_error_output)
    {
        a_dst = a_json ? atoi(a_json) : 0;
    }

    static inline void get_val(bool& a_dst, const char* a_json, std::string* a_error_output)
    {
        if (a_json)
        {
            switch (*((int*)a_json))
            {
            case 1702195828: /** true */
                a_dst = true;
                break;
            case 1936482662: /** false */
                a_dst = false;
                break;
            default:
                if (a_error_output)
                    (*a_error_output) += std::string("ERROR: '") + a_json + "' is not a boolean\n";
            }
        }
        else
            a_dst = false;
    }

    static inline void get_val(char& a_dst, const char* a_json, std::string* a_error_output)
    {
        a_dst = a_json ? atoi(a_json) : 0;
    }

    static inline void get_val(float& a_dst, const char* a_json, std::string* a_error_output)
    {
        a_dst = a_json ? atof(a_json) : 0.0;
    }

    template <template <typename, typename... > class ct,  class vt>
    static inline void get_val(ct<vt>& a_dst, const char* a_json, std::string* a_error_output)
    {
        a_json ? ZaxJsonParser::parse(a_dst, " unnamed list", a_json, a_error_output) : a_dst.clear();
    }

    template <typename mt>
    static inline void get_val(std::map<std::string, mt>& a_dst, const char* a_json, std::string* a_error_output)
    {
        a_json ? ZaxJsonParser::parse(a_dst, " unnamed map", a_json, a_error_output) : a_dst.clear();
    }

    template <typename vtype>
    static inline void get_val(vtype& a_dst, const char* a_json, std::string* a_error_output)
    {
        a_dst.from_json(a_json);
    }

    static inline void json_begin(int& a_result, char* a_json, const char* a_json_buffer_end, const char* a_key, char a_brace)
    {
        if (a_json < a_json_buffer_end)
        {
            if (a_key && a_key[0])
                a_result = snprintf(a_json, a_json_buffer_end - a_json, "\"%s\":%c", a_key, a_brace);
            else
            {
                *a_json = a_brace;
                a_json[1] = 0;
                ++a_result;
            }
        }
    }

    static inline void json_end(int& a_result, char* a_json, const char* a_json_buffer_end, char a_brace)
    {
        if (a_result++ && (a_json_buffer_end - a_json > 1))
            *a_json = a_brace;
        else
            a_result = 0;
    }

public:
    static unsigned int initial_alloc_size();
    static unsigned int maximum_alloc_size();
    static void set_initial_alloc_size(unsigned int a_size);
    static void set_maximum_alloc_size(unsigned int a_size);

    static inline void cat_comma_space(char*& a_json, int& a_result)
    {
        *a_json = ',';
        *++a_json = ' ';
        *++a_json = 0;
        a_result += 2;
    }

    static inline int append(char* a_json, const char* a_json_buffer_end, const char* a_key, const std::string& a_value)
    {
        return print_key_and_val(a_json, a_json_buffer_end, a_key, a_value);
    }

    template <typename vt>
    static inline int append(char* a_json, const char* a_json_buffer_end, const char* a_key, const vt& a_value)
    {
        return print_key_and_val(a_json, a_json_buffer_end, a_key, a_value);
    }

    template <typename mt>
    static inline int append(char* a_json, const char* a_json_buffer_end, const char* a_key, const std::map<std::string, mt>& a_values)
    {
        int _result = 0;
        json_begin(_result, a_json, a_json_buffer_end, a_key, '{');
        if (_result > 0)
        {
            a_json += _result;
            for (typename std::map<std::string, mt>::const_iterator r = a_values.begin(); r != a_values.end(); ++r)
            {
                if (r != a_values.begin())
                    cat_comma_space(a_json, _result);
                int written = print_key_and_val(a_json, a_json_buffer_end, r->first.c_str(), r->second);
                if (written <= 0)
                {
                    _result = 0;
                    break;
                }
                a_json += written;
                _result += written;
            }
            json_end(_result, a_json, a_json_buffer_end, '}');
        }
        return _result;
    }

    template <template <typename, typename... > class ct,  class vt>
    static inline int append(char* a_json, const char* a_json_buffer_end, const char* a_key, const ct<vt>& a_values)
    {
        int _result = 0;
        json_begin(_result, a_json, a_json_buffer_end, a_key, '[');
        if (_result > 0)
        {
            a_json += _result;
            for (typename ct<vt>::const_iterator r = a_values.begin(); r != a_values.end(); ++r)
            {
                if (r != a_values.begin())
                    cat_comma_space(a_json, _result);
                int written = print_val(a_json, a_json_buffer_end, *r);
                if (written <= 0)
                {
                    _result = 0;
                    break;
                }
                a_json += written;
                _result += written;
            }
            json_end(_result, a_json, a_json_buffer_end, ']');
        }
        return _result;
    }

    template <typename type>
    static inline void parse(type& a_dst, const char* a_property, const char* a_json, std::string* a_error_output)
    {
        get_val(a_dst, a_json, a_error_output);
    }

    static inline void parse(std::string& a_dst, const char* a_property, const char* a_json, std::string* a_error_output)
    {
        get_val(a_dst, a_json, a_error_output);
    }

    template <template <typename, typename... > class ct,  class vt>
    static inline void parse(ct<vt>& a_vect, const char* a_property, const char* a_json, std::string* a_error_output)
    {
        if (a_json)
        {
            bool success = false;
            ZaxJsonFlatParser vector_data(a_json, true, &success);
            if (!success)
                (*a_error_output) += std::string("ERROR: error parsing a vector in JSON: '") + a_json + "'\n";
            a_vect.resize(vector_data.m_list_values.size());
            auto r = a_vect.begin();
            for (unsigned int i = 0; i < vector_data.m_list_values.size(); ++i)
                get_val(*r++, vector_data.m_list_values[i], a_error_output);
        }
        else
            a_vect.clear();
    }

    template <typename mt>
    static inline void parse(std::map<std::string, mt>& a_vect, const char* a_property, const char* a_json, std::string* a_error_output)
    {
        if (a_json)
        {
            bool success = false;
            ZaxJsonFlatParser vector_data(a_json, true, &success);
            if (!success)
                (*a_error_output) += std::string("ERROR: error parsing a map in JSON: '") + a_json + "'\n";
            for (std::map<ZaxStringWrap, const char*>::iterator ite = vector_data.m_values.begin(); ite != vector_data.m_values.end(); ++ite)
            {
                if (!ite->second)
                    a_vect.erase(ite->first.m_str);
                else
                {
                    mt tmp;
                    get_val(tmp, ite->second, a_error_output);
                    a_vect[ite->first.m_str] = tmp;
                }
            }
        }
        else
            a_vect.clear();
    }
};

template <typename mt>
std::pair<std::string, mt*> json_property(std::string a_name, mt& a_obj)
{
    return std::make_pair(a_name, &a_obj);
}

template<std::size_t I = 0, typename... vt>
inline typename std::enable_if<I == sizeof...(vt), void>::type
zax_to_json(char* a_json, const char* a_json_buffer_end, int& a_result, std::tuple<vt...> a_tuple, bool a_insert_object_trails = true)
{}

template<std::size_t I = 0, typename... vt>
inline typename std::enable_if < I < sizeof...(vt), void>::type
zax_to_json(char* a_json, const char* a_json_buffer_end, int& a_result, std::tuple<vt...> a_tuple, bool a_insert_object_trails = true)
{
    if (!I)
    {
        if (a_json_buffer_end - a_json > 1)
        {
            if (a_insert_object_trails)
            {
                strcpy(a_json, "{");
                ++a_result;
                ++a_json;
            }
            else
                a_json[0] = 0;
        }
        else
            a_result = 0;
    }
    int l_result = ZaxJsonParser::append(a_json, a_json_buffer_end, std::get<I>(a_tuple).first.c_str(), *std::get<I>(a_tuple).second);
    if (!l_result)
        return;
    a_json += l_result;
    a_result += l_result;
    if (I < sizeof...(vt) - 1)
        ZaxJsonParser::cat_comma_space(a_json, a_result);
    zax_to_json < I + 1, vt... > (a_json, a_json_buffer_end, a_result, a_tuple, a_insert_object_trails);
    if (!a_result)
        return;
    if ((I == sizeof...(vt) - 1) && a_insert_object_trails)
    {
        if ((a_json_buffer_end - a_json) > 1)
        {
            strcpy(a_json, "}");
            ++a_result;
        }
        else
            a_result = 0;
    }
}

#define JSON_PROPERTY_(member) json_property(#member, object_to_convert.member)
#define JSON_PROPERTY_NAME(member, name_in_json) json_property(name_in_json, object_to_convert.member)
#define JSON_PROPERTY_CHOOSER(no_arg,member,name_in_json,FUNC, ...) FUNC
#define JSON_PROPERTY(...) JSON_PROPERTY_CHOOSER(no_arg,##__VA_ARGS__, JSON_PROPERTY_NAME(__VA_ARGS__), JSON_PROPERTY_(__VA_ARGS__))

#define zax_convert_to_json(arg_json, a_json_buffer_length, arg_obj, arg_json_properties, ...) ({\
    int _result = 0;\
    decltype(arg_obj)& object_to_convert = arg_obj; \
    const char* _json_buffer_end = arg_json + a_json_buffer_length; \
    zax_to_json(arg_json, _json_buffer_end, _result, std::make_tuple(arg_json_properties), ##__VA_ARGS__);\
    _result;\
})

template<std::size_t I = 0, typename... vt>
inline typename std::enable_if<I == sizeof...(vt), void>::type
zax_from_json(const char* a_json, std::tuple<vt...> a_tuple, ZaxJsonFlatParser* parsed_json, std::string* a_error_output = 0)
{}

template<std::size_t I = 0, typename... vt>
inline typename std::enable_if < I < sizeof...(vt), void>::type
zax_from_json(const char* a_json, std::tuple<vt...> a_tuple, ZaxJsonFlatParser* parsed_json, std::string* a_error_output = 0)
{
    if (!parsed_json)
    {
        bool success = false;
        parsed_json = new ZaxJsonFlatParser(a_json, false, &success);
        if (!success)
            (*a_error_output) += std::string("ERROR: error parsing JSON: '") + a_json + "'\n";
    }
    if (I < sizeof...(vt))
    {
        std::map<ZaxStringWrap, const char*>::iterator it = parsed_json->m_values.find(std::get<I>(a_tuple).first.c_str());
        if (it != parsed_json->m_values.end())
            ZaxJsonParser::parse(*std::get<I>(a_tuple).second, std::get<I>(a_tuple).first.c_str(), parsed_json->m_values[std::get<I>(a_tuple).first.c_str()], a_error_output);
        else if (a_error_output)
            (*a_error_output) += std::string("WARNING: JSON property is missing: '") + std::get<I>(a_tuple).first.c_str() + "'\n";
    }
    zax_from_json < I + 1, vt... > (a_json, a_tuple, parsed_json, a_error_output);
    if (I == sizeof...(vt) - 1)
        delete parsed_json;
}

template<std::size_t I = 0, typename... vt>
inline typename std::enable_if<I == sizeof...(vt), void>::type
zax_from_json(char* a_json, std::tuple<vt...> a_tuple, ZaxJsonFlatParser* parsed_json, std::string* a_error_output = 0)
{}

template<std::size_t I = 0, typename... vt>
inline typename std::enable_if < I < sizeof...(vt), void>::type
zax_from_json(char* a_json, std::tuple<vt...> a_tuple, ZaxJsonFlatParser* parsed_json, std::string* a_error_output = 0)
{
    if (!parsed_json)
    {
        bool success = false;
        parsed_json = new ZaxJsonFlatParser(a_json, true, &success);
        if (!success)
            (*a_error_output) += std::string("ERROR: error parsing JSON: '") + a_json + "'\n";
    }
    if (I < sizeof...(vt))
    {
        std::map<ZaxStringWrap, const char*>::iterator it = parsed_json->m_values.find(std::get<I>(a_tuple).first.c_str());
        if (it != parsed_json->m_values.end())
            ZaxJsonParser::parse(*std::get<I>(a_tuple).second, std::get<I>(a_tuple).first.c_str(), parsed_json->m_values[std::get<I>(a_tuple).first.c_str()], a_error_output);
        else if (a_error_output)
            (*a_error_output) += std::string("WARNING: JSON property is missing: '") + std::get<I>(a_tuple).first.c_str() + "'\n";
    }
    zax_from_json < I + 1, vt... > (a_json, a_tuple, parsed_json, a_error_output);
    if (I == sizeof...(vt) - 1)
        delete parsed_json;
}

#define zax_convert_from_json(arg_json, arg_obj, arg_json_properties, ...) decltype(arg_obj)& object_to_convert = arg_obj; zax_from_json(arg_json, std::make_tuple(arg_json_properties), 0, ##__VA_ARGS__);

#define ZAX_JSON_SERIALIZABLE_WDC(class_name, ...)\
    virtual void from_json(const char* a_json)\
    {\
        zax_convert_from_json(a_json, *this, ##__VA_ARGS__);\
    }\
    virtual int to_json(char* a_json, const char* a_json_buffer_end) const\
    {\
        return zax_convert_to_json(a_json, (a_json_buffer_end - a_json), *this, ##__VA_ARGS__);\
    }\
    class_name(const char* a_json)\
    {\
        *this = a_json;\
    }\
    class_name(const string& a_json)\
    {\
        *this = a_json;\
    }\
    void operator = (const char* a_json)\
    {\
        zax_convert_from_json(a_json, *this, ##__VA_ARGS__);\
    }\
    void operator = (const string& a_json)\
    {\
        zax_convert_from_json(a_json.c_str(), *this, ##__VA_ARGS__);\
    }\
    template <typename T> operator T() const\
    {\
        unsigned int alloc_size = ZaxJsonParser::initial_alloc_size();\
        char* json = new char[alloc_size];\
        while (!zax_convert_to_json(json, alloc_size - 1, *this, ##__VA_ARGS__))\
        {\
            delete[] json;\
            alloc_size *= 2;\
            json = new char[alloc_size];\
            if (alloc_size > ZaxJsonParser::maximum_alloc_size())\
                break;\
        }\
        string result(json);\
        delete[] json;\
        return result;\
    }\
    friend std::ostream& operator<<(std::ostream& os, const class_name& a_obj)\
    {\
        string s = a_obj;\
        return os << s;\
    }\

#define ZAX_JSON_SERIALIZABLE(class_name, ...)\
    class_name() {}\
    ZAX_JSON_SERIALIZABLE_WDC(class_name, ##__VA_ARGS__)\

#endif /// _ZAXJSONPARSER_H_
