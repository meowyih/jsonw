# JsonW - C++ Wide Character Json Tool

Author   | Email
---------|------------------
Yih Horng|yhorng75@gmail.com

# About JsonW

*JsonW* is a *_C++11_* single header Json tool that underlying using wchar_t/std::wstring to store text value. It is part of the [octillion-cubes](https://github.com/meowyih/octillion-cubes) project. 

I know there are already plenty of C++ Json libraries out there. The reason I reinvented the wheel is just for fun.

# About Json 

General introduction is here: https://www.json.org/

Here are somethings that *JsonW* user needs to know.

1. Json text (JsonTextW) is an utf8 encoding text contains a _value_ (JsonValueW).

[RFC7159](https://www.rfc-editor.org/info/rfc7159)
> JSON-text = ws value ws

2. A _value_ (JsonValueW) is either a number, a string, a boolean, a null, an _object_ (JsonObjectW) or an _array_ (JsonArrayW).
3. An _object_ (JsonObjectW) is a set of name/_value_ (JsonValueW) pair. 
4. An _array_ (JsonArrayW) is a collection of _value_ (JsonValueW).

# About character set

_wchar_t_ is 2 bytes in Windows and 4 bytes in linux. Since *JsonW* stores text data in _wchar_t_ underlying, *JsonW* supports all characters defined in USC2 in Windows and USC4 in Linux.

# Installation

Make sure the compiler supports C++11 and include find the header file _jsonw.hpp_.

# Usage

_All the sample code in this section is also available in test.cpp._

## Read json from utf8 data

``` c++
using namespace octillion;

    // literal 'u8' is not necessary if compiler uses utf8 as default 
    char jsondata[] = u8"{\"name\":\"meowyih\",\"age\":123}";

    // initialize JsonTextW object using jsondata
    JsonW json(jsondata);

    // json in utf8
    std::cout << json << std::endl;
    
```

## Read json from utf8 file

``` c++
using namespace octillion;

    // 'sample.json' must utf-8 format without BOM
    std::string jsonfile = "sample.json";

    // initialize JsonTextW object using ifstream
    std::ifstream fin(jsonfile);

    if (!fin.good())
    {
        std::wcout << L"bad ifstream" << std::endl;
        return;
    }

    // initialize JsonTextW object using fin
    JsonW json(fin);

    // json in utf8
    std::cout << json << std::endl;
    
```

## Create json programatically

Assume I want to create a JsonTextW in the structure like this.

``` json
 {
     "txt1": "some text1",
     "num1": 123,
     "array": 
     [
         true,
         {
             "txt2": "some text2",
             "num2": 456
         }
     ]
 }
 
```

Here is how to do it.

``` c++
using namespace octillion;

    // create two JsonW object
    JsonW jobject1, jobject2;
    
    jobject1[u8"txt1"] = "some text1";
    jobject1[u8"num1"] = 123;

    jobject2[u8"null"]; // default JsonW value is NULL
                        // do no use 'json = NULL' this macro
                        // '#define NULL 0' is standard in compiler

    jobject2[u8"num2"] = 0.456;

    // create the array
    JsonW jarray;
    jarray[0] = true;
    jarray[1] = jobject2;

    // put array in jobject1
    jobject1[u8"array"] = jarray;

    // display the json text in utf8
    std::cout << jobject1 << std::endl;

```

## Work with value

_JsonW_ represents a _value_ defined in json standard. This example shows how to handle the data in it based on the different type of value.

``` c++
using namespace octillion;

    // jvalue is a null as default
    JsonW jvalue;

    // jvalue is a number
    jvalue = (short)123;
    jvalue = 123;
    jvalue = (long)123;
    jvalue = (long long)123;

    jvalue = (float)4.56;
    jvalue = 4.56;
    jvalue = (long double)4.56;

    // jvalue is a string
    jvalue = "utf8 c-string";
    jvalue = L"ucs c-string";
    jvalue = std::string("utf8 string");
    jvalue = std::wstring(L"utf8 string");

    // jvalue is a boolean
    jvalue = true;

    // jvalue is an json object, which is also another JsonW object
    JsonW jobject(u8"{\"name\":\"meowyih\",\"age\":123}");
    jvalue = jobject;

    // jvalue is a json array, which is also another JsonW object
    JsonW jarray(u8"[1,2,3,4,5]");
    jvalue = jarray;

    // check jvalue's type and retrieve the data
    switch (jvalue.type())
    {
    // error type
    case JsonW::BAD: 
        std::cout << "jvalue is an BAD type" << std::endl;
        break;
    // jvalue is an json object, check "working with object" for detail
    case JsonW::OBJECT: 
        std::cout << "jvalue is an json object" << std::endl;
        break;
    // jvalue is an json array, check "working with array" for detail
    case JsonW::ARRAY:
        std::cout << "jvalue is an json array" << std::endl;
        break;
    // jvalue is an integer number
    case JsonW::INTEGER:
        std::cout << "jvalue is an integer " << jvalue.integer() << std::endl;
        break;
    // jvalue is an floating point number
    case JsonW::FLOAT:
        std::cout << "jvalue is an integer " << jvalue.frac() << std::endl;
        break;
    // jvalue is a string, JsonW provides both utf8 and ucs string format
    case JsonW::STRING:
        std::cout << "jvalue is an string, utf8:" << jvalue.str() << std::endl;
        std::wcout << L"jvalue is an string, ucs:" << jvalue.wstr() << std::endl;
        break;
    // jvalue is a boolean
    case JsonW::BOOLEAN:
        std::cout << "jvalue is a boolean " << jvalue.boolean() << std::endl;
        break;
    case JsonW::NULLVALUE:
        std::cout << "jvalue is null" << std::endl;
    }

    // get json format string
    std::string json_utf8 = jvalue.text();
    std::wstring json_ucs = jvalue.wtext();

    // overloading ostream << operator
    std::cout << "json format:" << jvalue << std::endl;
    
```

## Work with object

An json object contains multiple key-value pairs. Here is an example shows how to get all keys in jobject, as well as retrieving the value via the key. 

``` c++
using namespace octillion;

    // create a test json text using ascii (utf8) string
    octillion::JsonW jobject(u8"{\"last\":\"Lee\",\"first\":\"Peter\"}");

    // check if json is valid
    if (jobject.valid() == false)
    {
        std::cout << "error: invalid json" << std::endl;
        return;
    }

    // check if json has object
    if (jobject.type() != octillion::JsonW::OBJECT)
    {
        std::cout << "error: json is not json object" << std::endl;
        return;
    }

    // list all keys in object in std::wstring type and std::string type
    std::vector<std::string> keys;

    jobject.keys(keys);

    // display name/value pair in ucs (wchar_t), normally programmer should 
    // check the value type before handling it, but for demo purpose, we 
    // 'magically' knows it is string type since it was created by our own.
    for (size_t i = 0; i < keys.size(); i++)
    {
        std::string key = keys.at(i);
        std::string value = jobject[key].str();
        std::cout << "key-" << i << ":" << key << " value:" << value << std::endl;
    }
    
```

## Work with array

An _array_ contains multiple values. Here is an example shows how to access all _values_ inside an jarray.

``` c++
    
    // create a test json text using ascii (utf8) string
    octillion::JsonW jarray(u8"[12,13,-42,20]");

    std::cout << "array size is " << jarray.size() << std::endl;

    // list each value (again, for demo purpose, we 'magically' know all value in are integer)
    for (size_t i = 0; i < jarray.size(); i++)
    {
        std::cout << "item-" << i << ":" << jarray[i].integer() << std::endl;
    }
    
```    

# Avoiding deep copying

Consider the code below.

``` c++
using namespace octillion;

    JsonW json, jobject, jarray;
    
    jarray[1] = 10; // JsonW automatically assign NULL to jarray[0]
    jobject["data"] = "data"; 

    // deep copy jarray into jobject
    jobject["array"] = jarray;

    // deep copy jobject (as well as jarray) into json
    json["object"] = jobject;

    std::cout << "json:" << json << std::endl;

```
It is not a problem when data size is small, however, if JsonW contains huge data, says 100MB. Deep copy might be a problem the memory usage is a concern.

If caller has such concern, do not use assignment operaotr (i.e. '='). Use JsonW's member function find()/key()/set() for json object and size()/at()/add() for json array manipulation.

``` c++
using namespace octillion;

    JsonW *p_json, *p_object, *p_jarray;

    p_jarray = new JsonW();

    // null is a standard json string, see README.md for detail
    p_jarray->add(new JsonW("null"));

    // 10 is a standard json string, see README.md for detail
    p_jarray->add(new JsonW("10"));

    p_object = new JsonW();

    // "data" is a standard json string, see README.md for detail
    p_object->set("data", new JsonW("\"data\"")); 

    // assign p_jarray into p_object, it is not deep copy.
    // p_object just copy the address of p_jarray
    p_object->set("array", p_jarray);

    p_json = new JsonW();

    // assign p_object into p_json, it is not deep copy.
    // p_json just copy the address of p_object
    p_json->set("object", p_object);

    std::cout << "p_json:" << p_json->text() << std::endl;

    // when delete the p_json, all the JsonW objects in it
    // would be deleted, includes p_jobject and p_jarray.
    delete p_json;

```

# Memory leak detection

In _jsonw.hpp_, line#19, there is a macro named OCTILLION_JSONW_ENABLE_MEMORY_LEAK_DETECTION. If you enable it, JsonW will keep track of all the new/delete function call for JsonW class. Then show the memory leak report by calling octillion::JsonW::memory_leak_detect_result() static function. There is a mutex lock during the detection. Do not enable this macro unless you really want to check the memory leak.

# Known issues and TODO

1. *JsonW* does NOT support the big number. The Json contains number that greater than LLONG_MAX/DBL_MAX  or less than LLONG_MIN/DBL_MIN  is treated as invalid during creation.
2. *JsonW* does NOT handle the memory overflow when reading or creating super massive Json object. If you try to feed several terabytes data in it, the behavior is undefined.
3. *JsonW* does NOT support _pretty format_ output yet. When calling _text()_ or _wtext()_, the retrurns data is a single-line text in utf8 or usc encoding.
