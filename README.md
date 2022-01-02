# zax-json-parser
Zax is a very basic JSON parser implemented in C++11 with probably the simplest possible interface, providing a kind of a struct to JSON and JSON to struct conversion.

```cpp

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include "ZaxJsonParser.h"

struct some_class
{
    int x = 9;
    std::string name = "some name";
    ZAX_JSON_SERIALIZABLE(some_class, JSON_PROPERTY(x), JSON_PROPERTY(name))
};

int main()
{
    some_class some_obj = R"({"x": 7, "name": "new name"})";
    std::cout << some_obj;
    return 0;
}

```
#### Result:

```cpp

{"x":7, "name":"new name"}

```
# examples:
#### Example1:

```cpp

struct some_class
{
    int x = 9;
    std::string name = "some name";
};

some_class some_obj;
zax_convert_from_json(R"({"x": 7, "name": "new name"})", some_obj, JSON_PROPERTY(x), JSON_PROPERTY(name));

```

#### Example2:

```cpp

struct some_class
{
    int x = 9;
    std::string title = "some title";

    ZAX_JSON_SERIALIZABLE_BASIC(JSON_PROPERTY(x), JSON_PROPERTY(title))
};

some_class some_obj;
some_obj.zax_from_json("{\"x\":7, \"title\":\"new title\"}");

```

#### Example3:

##### Code:

```cpp

struct some_class
{
    int x = 9;
    std::string title = "some title";

    ZAX_JSON_SERIALIZABLE_BASIC(JSON_PROPERTY(x), JSON_PROPERTY(title))
};

some_class some_obj;
std::string some_json = some_obj.zax_to_json();
std::cout << some_json << std::endl;

```
##### Result:

```cpp

{"x":9, "title":"some title"}

```


#### Example4:

##### Code:

```cpp

#define some_json_properties JSON_PROPERTY(x), JSON_PROPERTY(title), JSON_PROPERTY(scores)

struct some_class
{
    int x = 9;
    std::string title = "some title";
    std::vector<std::vector<int>> scores = {{1, 2, 3}, {4, 5, 6}};

    ZAX_JSON_SERIALIZABLE(some_class, some_json_properties)
};

some_class some_obj = R"({"x":17, "title":"new title", "scores":[[1, 1, 2], [3], [5, 8], [13, 21]]})";
std::string some_json = some_obj;
std::cout << some_json << std::endl;

```
##### Result:

```cpp

{"x":17, "title":"new title", "scores":[[1, 1, 2], [3], [5, 8], [13, 21]]}

```

#### Example5:

##### Code:

```cpp

struct some_class
{
    int x = 9;
    int scores[2][3] = {{1, 2, 3}, {4, 5, 6}};

    ZAX_JSON_SERIALIZABLE(some_class, JSON_PROPERTY(x), JSON_PROPERTY(scores))
};

some_class some_obj = R"({"x":17, "scores":[[11, 12, 13], [14, 15, 16]]})";
cout << some_obj << endl;

```
##### Result:

```cpp

{"x":17, "scores":[[11, 12, 13], [14, 15, 16]]}

```


#### Example6:

##### Code:

```cpp

struct some_class3
{
    map<string, string> assets = {{"Darth Vader", "Death Star"}, {"Lyra Belacqua", "Alethiometer"}};
    ZAX_JSON_SERIALIZABLE(some_class3, JSON_PROPERTY(assets))
};

cout << some_class3() << endl;

```
##### Result:

```cpp

{"assets":{"Darth Vader":"Death Star", "Lyra Belacqua":"Alethiometer"}}

```


#### Example7:

##### Code:

```cpp

struct classA
{
    int x = 9;
    int scores[2][3] = {{1, 2, 3}, {4, 5, 6}};
    ZAX_JSON_SERIALIZABLE(classA, JSON_PROPERTY(x), JSON_PROPERTY(scores))
};

struct classB
{
    int x = 7;
    std::vector<std::vector<int>> scores = {{1, 1}, {2}, {3, 5, 8}};
    ZAX_JSON_SERIALIZABLE(classB, JSON_PROPERTY(x), JSON_PROPERTY(scores))
};

classA objA;
classB objB = objA;
cout << objB << endl;

```
##### Result:

```cpp

{"x":9, "scores":[[1, 2, 3], [4, 5, 6]]}

```

#### Example8:

##### Code:

```cpp
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

classF objF;
classE objE = objF;
cout << objE << endl;

```
##### Result:

```cpp

{"some_info":{"x":4, "y":5.600000, "name":"new name"}}

```

[Please check the documentation](https://tamask1s.github.io/zax-parser/index.html)

version 1.3


