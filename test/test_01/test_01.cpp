#include <iostream>
#include <tuple>
#include <map>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <queue>
#include <list>
#include <forward_list>
#include <stack>
#include <fstream>
#include <string>

/** lib includes */
#include "ZaxJsonParser.h"

using namespace std;

struct struct1
{
    int x = 7;
    bool b = true;
    //string name = "元野球部マネージャー❤︎…最高の夏をありがとう…❤︎";
    string name = "some str";
    list<int> weights = {1, 2, 3};
};

#define struct1_json_properties JSON_PROPERTY(x), JSON_PROPERTY(b), JSON_PROPERTY(name), JSON_PROPERTY(weights)
//#define struct1_json_properties JSON_PROPERTY(x), JSON_PROPERTY(name, "with_letters")

void write_file(const char* fileName, const char* pData, int len)
{
    ofstream binFile (fileName, ios::out | ios::binary);
    if (binFile.is_open())
    {
        binFile.write(pData, len);
        binFile.close();
    }
}

void read_file(const char* fileName, std::vector<char>& buffer)
{
    std::ifstream infile(fileName);
    if (infile.is_open())
    {
        //get length of file
        infile.seekg(0, infile.end);
        size_t length = infile.tellg();
        infile.seekg(0, infile.beg);

        buffer.resize(length);
        //read file
        if (length)
            infile.read(&buffer[0], length);
    }
}

void json_example1()
{
    struct1 some_obj;

    char json_string[1000];
    cout << "sizeof(json_string)" << sizeof(json_string) << endl;
    zax_convert_to_json(json_string, sizeof(json_string), some_obj, struct1_json_properties);
    cout << json_string << endl;
    write_file("file2", json_string, strlen(json_string));
}

void json_example2()
{
    struct1 some_obj;

    char json_string[] = R"({"x":null, "name":null, "b":null, "weights":null})";
    //char json_string[] = R"({"x":11, "int2":12, "name":"eleven", "string2":"twelve"})";
    //zax_convert_from_json(json_string, some_obj, (JSON_PROPERTY(x), JSON_PROPERTY(b), JSON_PROPERTY(name), JSON_PROPERTY(weights)));
    zax_convert_from_json(json_string, some_obj, struct1_json_properties);

    char json_string2[10000];
    zax_convert_to_json(json_string2, sizeof(json_string2), some_obj, struct1_json_properties);
    cout << json_string2 << endl;

    char* tmp = new char[10];
    strcpy(tmp, "true");
    cout << *((int*)tmp) << endl;
    strcpy(tmp, "false");
    cout << *((int*)tmp) << endl;
    strcpy(tmp, "null");
    cout << *((int*)tmp) << endl;
}

void json_example3()
{
    struct1 some_obj;

    string json_string = R"({"x":11, "int2":7, "name":"eleven", "string2":"seven"})";
    zax_convert_from_json(json_string.c_str(), some_obj, struct1_json_properties);

    char json_string2[10000];
    zax_convert_to_json(json_string2, 10000, some_obj, struct1_json_properties);
    cout << json_string2 << endl;
}

#define struct2_json_properties\
    JSON_PROPERTY(int1),\
    JSON_PROPERTY(int2),\
    JSON_PROPERTY(string1),\
    JSON_PROPERTY(string2)

struct struct2
{
    int int1;
    int int2;
    string string1;
    string string2;

    struct2()
        :int1(10)
    {}

    ZAX_JSON_SERIALIZABLE_WDC(struct2, struct2_json_properties)
    //ZAX_JSON_SERIALIZABLE_WDC(struct2, JSON_PROPERTY(int1), JSON_PROPERTY(int2), JSON_PROPERTY(string1), JSON_PROPERTY(string2))
};

void json_example4()
{
    string json_string = R"({"int1":11, "int2":7, "string1":"eleven", "string2":"seven"})";
    struct2 some_obj = json_string;
    string json_string2 = some_obj;
    cout << json_string2 << endl;
    cout << some_obj << endl;
}

#define CMxTableData_JOSONProps \
    JSON_PROPERTY(title), \
    JSON_PROPERTY(options, "criteria"), \
    JSON_PROPERTY(aspects), \
    JSON_PROPERTY(scores), \
    JSON_PROPERTY(x), \
    JSON_PROPERTY(weights)

struct CMxTableData
{
    int x;
    string title;
    forward_list<string> options;
    deque<string> aspects;
    std::deque<std::deque<int>> scores;
    deque<int> weights;

    ZAX_JSON_SERIALIZABLE_BASIC(JSON_PROPERTY(title), JSON_PROPERTY(options, "criteria"), JSON_PROPERTY(aspects), JSON_PROPERTY(scores), JSON_PROPERTY(x), JSON_PROPERTY(weights))
};

#define CClassInside_JOSONProps\
    JSON_PROPERTY(x),\
    JSON_PROPERTY(y)

struct CClassInside
{
    int x = 11;
    int y = 7;

    ZAX_JSON_SERIALIZABLE_BASIC(JSON_PROPERTY(x), JSON_PROPERTY(y))
};

#define CClass_JOSONProps\
    JSON_PROPERTY(title),\
    JSON_PROPERTY(options),\
    JSON_PROPERTY(aspects, "criteria"),\
    JSON_PROPERTY(scores),\
    JSON_PROPERTY(x),\
    JSON_PROPERTY(inside),\
    JSON_PROPERTY(weights)

struct CClass
{
    CClassInside inside;
    int x = 6;
    string title = "some title";
    deque<string> options = {"option1", "option2", "option3"};
    deque<string> aspects = {"criteria1", "criteria2"};
    std::deque<std::deque<int>> scores = {{1, 2, 3}, {7, 8, 9}};
    deque<int> weights = {1, 2, 3};

    ZAX_JSON_SERIALIZABLE(CClass, CClass_JOSONProps)
};

void json_example5()
{
    CClass some_obj = R"({"title": "some other title", "x": 17, "inside":{"x":9, "y":11}})";
    string json_string = some_obj;
    cout << json_string << endl;
    struct2 some_obj2 = some_obj;
}

void test_parser()
{
    char json_string_copy[400000] = "gfd";
    //strcpy(json_string_copy, R"({ "ix_":"99", "title":"test bible title", "options":["option 1", "option 2", "option 3"], "aspects":["aspect 1", "aspect 2", "aspect 3", "aspect 4"], "scores":[[1, 2, 3], [1, 2, 3], [1, 2, 3], [1, 2, 3]], "weights":[1, 2, 3, 4], "inside":{"x":9, "y":11}})");
    //strcpy(json_string_copy, R"({ "ix_":99, "title":"test bible title", "options":["option 1", "option 2", "option 3"], "aspects":["aspect 1", "aspect 2", "aspect 3", "aspect 4"], "scores":[[1, 2, 3], [1, 2, 3], [1, 2, 3], [1, 2, 3]], "weights":[1, 2, 3, 4], "inside":{"x":"9", "y":"11"}})");
    strcpy(json_string_copy, R"({ "ix_":99, "title":"test bible title", "options":["option 1", "option 2", "option 3"], "aspects":["aspect 1", "aspect 2", "aspect 3", "aspect 4"], "scores":[[1, 2, 3], [1, 2, 3], [1, 2, 3], [1, 2, 3]], "weights":[]})");
    //strcpy(json_string_copy, R"({"ix_":99, "title":"bible title tile telling lie""})";
    //strcpy(json_string_copy, R"({"title":"test bible title", "options":["option 1", "option 2", "option 3"], "aspects":["aspect 1", "aspect 2", "aspect 3", "aspect 4"], "scores":[[[1, 2, 3], [1, 2, 3], [1, 2, 3]], [[1, 2, 3], [1, 2, 3], [1, 9, 3]], [[1, 8], [1, 2, 3], [7, 2, 3]], [[1, 2, 3], [1, 2, 3], [1, 2, 3]]], "ix_":99, "inside":{"x":7, "y":9}, "weights":[]} )";
    CClass some_obj = "{}";
    cout << "some_obj.inside.x: " << some_obj.inside.x << endl;
    some_obj = json_string_copy;
    cout << "some_obj.inside.x2 : " << some_obj << endl;
    string json_std_string = some_obj;
    cout << json_std_string << endl << endl;

    some_obj = R"({ "ix_":99, "title":"test bible title", "options":["option 1", "option 2", "option 3"], "aspects":["aspect 1", "aspect 2", "aspect 3", "aspect 4"], "scores":[[1, 2, 3], [1, 2, 3], [1, 2, 3], [1, 2, 3]], "weights":[1, 2, 3, 4], "inside":{"x":"9", "y":"11"}})";
    cout << some_obj << endl << endl;
}

void test_parser2()
{
    char json_string_copy[400000];
    strcpy(json_string_copy, R"({"x":99, "criteria":["aspect x"], "title":"test title", "options":["option 1", "option 2", "option 3"], "nonexisting_field":"", "scores":[[1, 1, 2], [3, 5, 8], [13, 21], [34, 55]], "weights":[4, 3, 2, 1], "inside":{"x":"11", "y":"9"}})");
    cout << "Json string to be assigned to object:" << endl << json_string_copy << endl << endl;
    CClass some_obj = "{}";
    cout << "Values before assignment: " << endl << some_obj << endl << endl;
//    cout << "initial values of some_obj.inside.x: " << some_obj.inside.x << endl;
    some_obj = json_string_copy;
//    cout << "values of some_obj.inside.x after assignment: " << some_obj.inside.x << endl;
    string json_std_string = some_obj;
    cout << "Json string computed after assignment: " << endl << json_std_string << endl << endl;
}

void json_example_01()
{
    ZaxJsonTopTokenizer json_doc(R"({"title":"some title","x":17})", false);

    cout << json_doc.m_values["title"] << endl;
    cout << json_doc.m_values["x"] << endl;
    const char* tmp = json_doc.m_values["nosuchkey"];
    cout << (long long) tmp << endl;

//    bool success = true;
//    ZaxJsonTopTokenizer lDocument(aActionParam, aActionParamChangeAllowed, &success);
//    if (success)
//        fpl_batonUse(atoll(lDocument.m_values["smartBaton"]));
}

void json_example_02()
{
    ZaxJsonTopTokenizer json_doc("[3, 7, 11]", false);
    //ZaxJsonTopTokenizer json_doc("["3", "7", "11"]", false);

    cout << json_doc.m_list_values[0] << endl;
    cout << json_doc.m_list_values[1] << endl;
    cout << json_doc.m_list_values[2] << endl;
}

#define struct3_json_props \
    JSON_PROPERTY(x), \
    JSON_PROPERTY(options)
    //JSON_PROPERTY(scores)

struct struct3
{
    int x = 9;
    deque<string> options;
    //std::deque<std::deque<int>> scores = {{1, 2, 3}, {7, 8, 9}};
    ZAX_JSON_SERIALIZABLE(struct3, struct3_json_props)
};

void json_example_03()
{
    struct3 obj1 = R"({ "x":99, "title":"test bible title", "options":[1, 2, 3]})";
    string json_string2 = obj1;
    cout << json_string2 << endl;
}

void json_example_04()
{
    struct3 obj1 = R"({ "x":99, "title":"test bible title", "scores":[[1, 2, 3], [1, 2, 3], [1, 2, 3], [1, 2, 3]]})";
    cout << obj1 << endl;
}


#define some_json_properties JSON_PROPERTY(x), JSON_PROPERTY(title), JSON_PROPERTY(scores)

struct some_class
{
    int x = 9;
    char title[200];
    std::vector<std::vector<int>> scores = {{1, 2, 3}, {4, 5, 6}};

    ZAX_JSON_SERIALIZABLE(some_class, some_json_properties)
};

void some_example()
{
    some_class some_obj = R"({"x":17, "title":"test title", "scores":[[1, 1, 2], [3, 5, 8], [13, 21], [34, 55]]})";
    string some_json = some_obj;

    cout << some_json << endl;
}

#define some_json_properties2 JSON_PROPERTY(x), JSON_PROPERTY(dmap), JSON_PROPERTY(title), JSON_PROPERTY(scores)

struct some_class2
{
    int x = 9;
    string title = "some title";
    map<string, float> dmap = {{"AAA", 5.5}, {"BBB", 7.7}};
    std::deque<std::deque<int>> scores = {{1, 2, 3}, {4, 5, 6}};

    ZAX_JSON_SERIALIZABLE(some_class2, some_json_properties2)
};

void json_example12()
{
    some_class2 some_obj = R"({"x":17, "title":"test", "scores":[[1, 1, 2], [3], [5, 8], [13, 21]], "dmap":{"BBB": 9.9}})";

    cout << some_obj << endl;
}

#define some_json_properties3 JSON_PROPERTY(assets)

struct some_class3
{
    map<string, string> assets = {{"Darth Vader", "Death Star"}, {"Lyra Belacqua", "Alethiometer"}};

    ZAX_JSON_SERIALIZABLE(some_class3, some_json_properties3)
};

void some_example3()
{
    some_class3 some_obj3 = R"({"assets":{"Marquez": "Semantic dementia", "Lyra Belacqua":null}})";

    cout << some_obj3 << endl;

    string jstr = some_obj3.zax_to_json();
    //cout << jstr << endl;
}

struct some2_class
{
    int x = 9;
    int scores[2][3] = {{1, 2, 3}, {4, 5, 6}};
    //int scores[2][3][2] = {{{1, 2}, {3, 4}, {5, 6}}, {{7, 8}, {9, 10}, {11, 12}}};
    //std::vector<std::vector<std::vector<int>>> scores = {{{1, 2}, {3, 4}, {5, 6}}, {{7, 8}, {9, 10}, {11, 12}}};

    ZAX_JSON_SERIALIZABLE(some2_class, JSON_PROPERTY(x), JSON_PROPERTY(scores))
};

void some2_example4()
{
    some2_class some2_obj = R"({"x":17, "scores":[[11, 12, 13], [14, 15, 16]]})";
    cout << some2_obj << endl;
}

struct classA
{
    int x = 9;
    int scores[2][3] = {{1, 2, 3}, {4, 5, 6}};
    ZAX_JSON_SERIALIZABLE(classA, JSON_PROPERTY(x), JSON_PROPERTY(scores))
};

struct classB
{
    int x = 7;
    int h = 9;
    std::vector<std::vector<int>> scores = {{1, 1, 2}, {3, 5, 8}};
    ZAX_JSON_SERIALIZABLE(classB, JSON_PROPERTY(x), JSON_PROPERTY(scores))
};

void some2_example5()
{
    classA objA;
    classB objB = objA;
    cout << objB << endl;

    objB = (const string&)objA;
}

struct classM
{
    int x = 7;
    multimap<string, float> dmap = {{"AAA", 5.5}, {"BBB", 7.7}, {"BBB", 7.8}};
    ZAX_JSON_SERIALIZABLE(classM, JSON_PROPERTY(x), JSON_PROPERTY(dmap))
};

void some2_example6()
{
    classM objA = R"({"x":7, "dmap":{"AAA":5.500000, "BBB":7.600000, "BBB":7.900000}})";
    cout << objA << endl;
}

struct classD
{
    int x = 1;
    float y = 2.3;
    string name = "some name";
    ZAX_JSON_SERIALIZABLE(classD, JSON_PROPERTY(x), JSON_PROPERTY(y), JSON_PROPERTY(name))
};

struct classE
{
    classD some_info;
    ZAX_JSON_SERIALIZABLE(classE, JSON_PROPERTY(some_info))
};

struct classF
{
    map<string, string> some_info = {{"x", "4"}, {"y", "5.6"}, {"name", "new name"}};
    ZAX_JSON_SERIALIZABLE(classF, JSON_PROPERTY(some_info))
};

void some2_example7()
{
    classF objF;
    classE objE = objF;
    cout << "objE: " << objE << endl;
    cout << "objF: " << objF << endl;
    cout << some_class3() << endl;
}

struct class1A
{
    int x = 9;
    int scores[2][3] = {{1, 2, 3}, {4, 5, 6}};
    ZAX_JSON_SERIALIZABLE(class1A, JSON_PROPERTY(x), JSON_PROPERTY(scores))
};

struct class1B
{
    int x = 7;
    std::vector<std::vector<int>> scores = {{1, 1}, {2}, {3, 5, 8}};
    ZAX_JSON_SERIALIZABLE(class1B, JSON_PROPERTY(x), JSON_PROPERTY(scores))
};

void some2_example8()
{
    class1A objA;
    class1B objB = objA;
    cout << objB << endl;
}

int main()
{
    json_example1();
    json_example2();
    json_example3();
    json_example4();
    json_example5();
    test_parser();
    test_parser2();
    json_example_01();
    json_example_02();
    json_example_03();
    json_example_04();
    some_example();
    json_example12();
    some_example3();
    some2_example4();
    some2_example5();
    some2_example6();
    some2_example7();
    some2_example8();

    return 0;
}
