# zax-parser
Zax is a very basic JSON parser implemented in C++11 with probably the simplest possible interface, providing a kind of a struct to JSON and JSON to struct conversion.

### Example1:

#### Code:

```cpp

struct some_class
{
    int x = 9;
    std::string title = "some title";
};

some_class some_obj;
zax_convert_from_json("{\"x\":17, \"title\":\"new title\"}", some_obj, JSON_PROPERTY(x), JSON_PROPERTY(title));

```

### Example2:

#### Code:

```cpp

struct some_class
{
    int x = 9;
    std::string title = "some title";

    ZAX_JSON_SERIALIZABLE_BASIC(JSON_PROPERTY(x), JSON_PROPERTY(title))
};

some_class some_obj;
some_obj.zax_from_json("{\"x\":17, \"title\":\"new title\"}");

```

### Example3:

#### Code:

```cpp

struct some_class
{
    int x = 9;
    std::string title = "some title";

    ZAX_JSON_SERIALIZABLE_BASIC(JSON_PROPERTY(x), JSON_PROPERTY(title))
};

some_class some_obj;
std::string some_json;
some_obj.zax_to_json(some_json);
std::cout << some_json << std::endl;

```
#### Result:

```cpp

{"x":9, "title":"some title"}

```


### Example4:

#### Code:

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
#### Result:

```cpp

{"x":17, "title":"new title", "scores":[[1, 1, 2], [3], [5, 8], [13, 21]]}

```

[Please check the documentation](https://tamask1s.github.io/zax-parser/index.html)

version 1.0


