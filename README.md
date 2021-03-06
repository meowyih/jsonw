# JsonW - C++ Wide Character Json Tool

Author   | Email
---------|------------------
Yih Horng|yhorng75@gmail.com

# About JsonW

*JsonW* is a *_C++11_* single header Json tool that underlying using wchar_t/std::wstring to store text value. 

I know there are already plenty of C++ Json libraries out there. But reinventing the wheel is really fun. :boom:

# About Json 

General introduction is here: https://www.json.org/

Here are somethings that *JsonW* user needs to know.

1. Json text is an utf8 encoding text contains a _value_.

[RFC7159](https://www.rfc-editor.org/info/rfc7159)
> JSON-text = ws value ws

2. A _value_ is either a number, a string, a boolean, a null, an _object_  or an _array_.
3. An _object_ is a set of _name/_value_ pair. 
4. An _array_ is a collection of _value_.

# About character set

_wchar_t_ is 2 bytes in Windows and 4 bytes in linux. Since *JsonW* stores text data in _wchar_t_ underlying, *JsonW* supports all characters defined in USC2 in Windows and USC4 in Linux.

# Installation

Make sure the compiler supports C++11 and include the header file _jsonw.hpp_.

# Usage

All the sample codes in this section area available in _test.cpp_.

## Read json from utf8 data

``` c++

    // literal 'u8' is not necessary if compiler uses utf8 as default 
    char jsondata[] = u8"{\"name\":\"meowyih\",\"age\":123}";

    // initialize JsonTextW object using jsondata
    JsonW json(jsondata);

    // json in utf8
    std::cout << json << std::endl;
    
```

## Read json from utf8 file

``` c++

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
             "null": null,
             "num2": 0.456
         }
     ]
 }
 
```

Here is how to do it.

``` c++

    // create two JsonW object
    JsonW jobject1, jobject2;
    
    jobject1[u8"txt1"] = u8"some text1";
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
    jvalue = u8"utf8 c-string";
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
    std::cout << u8"json format:" << jvalue << std::endl;
    
```

## Work with object

An json object contains multiple key-value pairs. Here is an example shows how to get all keys in jobject, as well as retrieving the value via the key. 

``` c++

    // create a test json text using ascii (utf8) string
    JsonW jobject(u8"{\"last\":\"Lee\",\"first\":\"Peter\"}");

    // check if json is valid
    if (jobject.valid() == false)
    {
        std::cout << "error: invalid json" << std::endl;
        return;
    }

    // check if json has object
    if (jobject.type() != JsonW::OBJECT)
    {
        std::cout << "error: json is not json object" << std::endl;
        return;
    }
    
    std::cout << "json object contains " << jobject.size() 
        << " name-pair value(s)" << std::endl;

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
    
    // earse some value and see the result
    jobject.erase(u8"last");
    std::cout << jobject << std::endl;
    
```

## Work with array

An _array_ contains multiple values. Here is an example shows how to access all _values_ inside an jarray.

``` c++
    
    // create a test json text using ascii (utf8) string
    JsonW jarray(u8"[12,13,-42,20]");

    std::cout << "array size is " << jarray.size() << std::endl;

    // list each value (again, for demo purpose, we 'magically' know all value in are integer)
    for (size_t i = 0; i < jarray.size(); i++)
    {
        std::cout << "item-" << i << ":" << jarray[i].integer() << std::endl;
    }
    
    // erase some value and see the result
    jarray.erase(0); // be careful, after erase, the size of jarray became 3
    jarray.erase(1); // be careful, after erase, the size of jarray became 2
    std::cout << jarray << std::endl;
    
```    

# Avoid deep copy

Consider the code below.

``` c++

    JsonW json, jobject, jarray;
    
    jarray[1] = 10; // JsonW automatically assign NULL to jarray[0]
    jobject[u8"data"] = u8"data"; 

    // deep copy jarray into jobject
    jobject[u8"array"] = jarray;

    // deep copy jobject (as well as jarray) into json
    json[u8"object"] = jobject;

    std::cout << "json:" << json << std::endl;

```
It is not a problem when data size is small, however, if JsonW contains huge data, says 100MB. Deep copy might be a problem if the memory usage is a concern.

If caller has such concern, do not use assignment operaotr (i.e. '='). Use size()/get()/add()/keys() for json object and size()/get()/add() for json array.
``` c++

    std::shared_ptr<JsonW> p_json, p_object, p_jarray;

    p_jarray = std::make_shared<JsonW>();

    // null is a standard json string, see README.md for detail
    p_jarray->add(std::make_shared<JsonW>(u8"null"));

    // 10 is a standard json string, see README.md for detail
    p_jarray->add(std::make_shared<JsonW>(u8"10"));

    p_object = std::make_shared<JsonW>();

    // "data" is a standard json string, see README.md for detail
    p_object->add(u8"data", std::make_shared<JsonW>(u8"\"data\"")); 

    // assign p_jarray into p_object, it is not deep copy.
    // p_object just copy the address of p_jarray
    p_object->add("array", p_jarray);

    p_json = std::make_shared<JsonW>();

    // assign p_object into p_json, it is not deep copy.
    // p_json just copy the address of p_object
    p_json->add("object", p_object);

    std::cout << "p_json:" << p_json->text() << std::endl;

```

# API Reference

## Constructor and Destructor

Caller can always call *valid()* to check if JsonW successcully constructed. Here are all public constructors.

+ default constructor
  default type for JsonW is always NULLVALUE
+ copy constructor
  Deep copy another JsonW
+ construct by utf8 file input stream 
+ construct by utf8 string
  Caller can use std::string or const char* to feed in the utf8 data
+ construct by ucs string (std::wstring / const wchar_t*)
+ destrcutor

``` c++

    // Construct a NULLVALUE json
    JsonW();
    
    // Construct a json by deep copy another json
    explicit JsonW(const JsonW& rhs);
    
    // Construct a json from an text file encoded by utf8
    explicit JsonW(std::ifstream& fin);
    
    // Construct a json directly from c-ctyle utf8 string
    explicit JsonW(const char* utf8str);
    
    // Construct a json directly from utf8 string with length
    JsonW(const char* utf8data, size_t length);
    
    // Construct a json directly from c-ctyle ucs string
    explicit JsonW(const wchar_t* wstr);
    
    // Construct a json directly from ucs string with length
    JsonW(const wchar_t* ucsdata, size_t size);    
    
    // destructor
    ~JsonW();

```

## Simple Data Accessor

``` c++

    // check if json is valid
    bool valid() const;
    
    // get type of jsonw, possible types are:
    // JsonW::OBJECT
    // JsonW::ARRAY
    // JsonW::INTEGER
    // JsonW::FLOAT
    // JsonW::STRING
    // JsonW::BOOLEAN
    // JsonW::NULLVALUE
    // JsonW::BAD
    int type() const;

    // get the integer value if type is INTEGER
    long long integer() const;
    
    // get the floating value if type is FLOAT
    long double frac() const;
    
    // get the string value in ucs encoding if type is STRING
    std::wstring wstr() const;
    
    // get the string value in utf8 encoding if type is STRING
    std::string str() const;
    
    // get the boolean value if type is BOOLEAN
    bool boolean() const;
    
    // get the number of data in this json value
    // for INTEGER, FLOAT, STRING and BOOLEAN, return 1
    // for ARRAY, return the number of values inside it
    // for OBJECT, return the number of name-value pairs inside it
    // others return 0
    size_t size() const;
    
    // set json value to 'integer' and type to INTEGER
    void integer(long long integer);
    
    // set json value to 'frac' and type to FLOAT
    void frac(long double frac);
    
    // set json value to 'str (wstr)' and type to STRING
    void wstr(const std::wstring& wstr);
    void wstr(const wchar_t* wstr);
    void wstr(const wchar_t* wstr, size_t length);
    void str(const std::string& str);
    void str(const char* str);
    void str(const char* str, size_t length);
    
    // set json value to 'boolean' and type to BOOLEAN
    void boolean(bool boolean);
    
    // set json value to null and type to NULLVALUE
    void reset();
    
    // set value based on standard utf8 json text
    void json(const std::string& text);
    void json(const char* text);
    void json(const char* text, size_t size);

```

## Json Object Accessor

``` c++
    
    // store all available keys in either ucs or utf8 enconding in vector
    void wkeys(std::vector<std::wstring>& keys) const;
    void keys(std::vector<std::string>& keys) const;
    
    // get the pointer of json object via specific key, return NULL if
    // no such entry or 'this' is not an json object
    // different from assignment operator using deep copy (for 
    // example, 'JsonW jvalue = jobject["key"]'),
    // the object returned from get() is the actual object stored inside
    // 'this'. See 'Avoiding deep copying' section for detail.
    std::shared_ptr<JsonW> get(const std::wstring& wkey) const;
    std::shared_ptr<JsonW> get(const std::string& key) const;
    
    // add a name-value pair into json object
    // after adding the jvalue, 'this' will take care of the
    // memory releasing inside its destructor.
    bool add(std::wstring wkey, std::shared_ptr<JsonW> jvalue);
    bool add(std::string key, std::shared_ptr<JsonW> jvalue);
    
    // add name-value pair int to json object, which value is integer
    bool add(std::wstring wkey, long long integer);
    bool add(std::wstring wkey, long integer);
    bool add(std::wstring wkey, int integer);
    bool add(std::wstring wkey, short integer);
    bool add(std::string key, long long integer);
    bool add(std::string key, long integer);
    bool add(std::string key, int integer);
    bool add(std::string key, short integer);
    
    // add name-value pair int to json object, which value is floating
    bool add(std::wstring wkey, long double frac);
    bool add(std::wstring wkey, double frac);
    bool add(std::wstring wkey, float frac);
    bool add(std::string key, long double frac);
    bool add(std::string key, double frac);
    bool add(std::string key, float frac);
    
    // add name-value pair int to json object, which value is string
    bool add(std::wstring wkey, std::wstring wstr);
    bool add(std::string key, std::string str);
    
    // add name-value pair int to json object, which value is boolean
    bool add(std::wstring wkey, bool boolean );
    bool add(std::string key, bool boolean);
    
    // NOTE:
    // DO NOT use 'add(u"key", NULL)' to assign null value, the keyword 'NULL' is 0.
    // Use 'add(u8"key", std::make_shared<JsonW>())' instead
    
    // delete a name-pair value inside json object by the name
    // return false if no such value
    bool erase(std::wstring wkey);
    bool erase(std::string key);

```

## Json Array Accessor

``` c++

    // get the pointer of json array via index, return NULL if
    // no such entry or 'this' is not an json array
    // different from assignment operator using deep copy (for 
    // example, 'JsonW jvalue = jarray[3]'),
    // the object returned from get() is the actual object stored inside
    // 'this'. See 'Avoiding deep copying' section for detail.
    std::shared_ptr<JsonW> get(size_t idx) const;
    
    // add a value into json array
    // after adding the jvalue, 'this' will take care of the
    // memory releasing inside its destructor.
    bool add(std::shared_ptr<JsonW> junit);
    
    // add an integer into array
    bool add(long long integer);
    bool add(long integer);
    bool add(int integer);
    bool add(short integer);
    
    // add a floating into array
    bool add(long double frac);
    bool add(double frac);
    bool add(float frac);
    
    // add a string into array
    bool add(std::wstring wstr);
    bool add(std::string str);
    
    // add a boolean into array
    bool add(bool boolean);
    
    // NOTE:
    // DO NOT use 'add(NULL)' to assign null value, the keyword 'NULL' is 0.
    // Use 'add(std::make_shared<JsonW>())' instead
    
    // delete a value inside json array by index, 
    // return false if no such value
    bool erase(size_t idx);

```

## Format and Output

``` c++

    // format json data into utf8 text in json standard
    // set singleline to false to make the return data
    // more readable
    std::string text( bool singleline = true ) const;
    
    // format json data into ucs text
    // set singleline to false to make the return data
    // more readable
    std::string wtext( bool singleline = true ) const;

```

# Known issues and TODO

1. *JsonW* does NOT support the big number. The Json contains number that greater than LLONG_MAX/DBL_MAX  or less than LLONG_MIN/DBL_MIN  is treated as invalid during creation.
2. *JsonW* does NOT handle the memory overflow when reading or creating super massive Json object. If you try to feed several terabytes data in it, the behavior is undefined.
