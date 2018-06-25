# JsonW - C++ Wide Character Json Tool

Author   | Email
---------|------------------
Yih Horng|yhorng75@gmail.com

# About JsonW

*JsonW* is a *_C++11_* Json R/W tool that underlying using wchar_t/std::wstring to store text value. It is part of the [octillion-cubes](https://github.com/meowyih/octillion-cubes) project.

I know there are already plenty of C++ Json libraries out there. The reason I reinvented the wheel is becasue it is fun and I'm too lazy to to read other people's license note. :grinning:

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

Make sure the compiler supports C++11 and can find the header file _jsonw.hpp_ and source file _jsonw.cpp_.

# Usage

_All the sample code in this section is also available in main.cpp._

## Read json from utf8 data

``` c++

    // literal 'u8' is not necessary if compiler uses utf8 as default 
    char jsondata[] = u8"{\"name\":\"meowyih\",\"age\":123}";

    // initialize JsonTextW object using jsondata
    octillion::JsonTextW* json = new octillion::JsonTextW(jsondata);

    // json in utf8
    std::cout << json->string() << std::endl;

    delete json;
    
```

## Read json from utf8 file

``` c++

    // 'sample.json' is a utf-8 encoded file without BOM
    std::string jsonfile = "sample.json";
    
    // initialize JsonTextW object using wistream (wifstream)
    std::wifstream wfin(jsonfile);
    
    if (!wfin.good())
    {
        std::cout << "bad ifstream" << std::endl;
        return;
    }
    
    // initialize JsonTextW object using wfin
    octillion::JsonTextW* json = new octillion::JsonTextW(wfin);
    
    // json in utf8
    std::cout << json->string() << std::endl; 
    
    delete json;
    
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

    // create the outer JsonObjectW
    octillion::JsonObjectW* object1 = new octillion::JsonObjectW();
    object1->add(u8"txt1", u8"some text1");
    object1->add(u8"num1", 123);

    // create the array
    octillion::JsonArrayW* array = new octillion::JsonArrayW();
    array->add(true);

    // create the JsonObjectW inside array
    octillion::JsonObjectW* object2 = new octillion::JsonObjectW();
    object2->add(u8"txt2", u8"some text2");
    object2->add(u8"num2", 456);
    array->add(object2);

    // add array into outer object
    object1->add(u8"array", array);

    // create the json text object that contains a value (object is also a value)
    octillion::JsonTextW* json = new octillion::JsonTextW(object1);

    // display the json text in utf8
    std::cout << json->string() << std::endl;

    // release the memory
    delete json;

```

## Work with value

According to the [RFC7159](https://www.rfc-editor.org/info/rfc7159) json text contains exactly one _value_. This example shows how to extract that JsonValueW from JsonTextW. Then handle the data in it based on the different type of value.

``` c++

    // create a test json text using ascii (utf8) string
    octillion::JsonTextW* json = new octillion::JsonTextW(u8"{\"name\":\"meowyih\",\"age\":123}");

    // check if json is valid
    if (!json->valid())
    {
        std::cout << "error: invalid json" << std::endl;
        delete json;
        return;
    }

    // get the type of value in json
    octillion::JsonValueW* value = json->value();
    
    switch (value->type())
    {
    case octillion::JsonValueW::Type::String:
        std::cout << "string: " << value->string() << std::endl;
        break;
    case octillion::JsonValueW::Type::NumberInt:
        std::cout << "number(int): " << value->integer() << std::endl;
        break;
    case octillion::JsonValueW::Type::NumberFrac:
        std::cout << "number(double): " << value->frac() << std::endl;
        break;
    case octillion::JsonValueW::Type::Boolean:
        std::cout << "boolean: " << value->boolean() << std::endl;
        break;
    case octillion::JsonValueW::Type::Null:
        std::cout << "null" << std::endl;
        break;
    case octillion::JsonValueW::Type::JsonArray:
        std::cout << "array size: " << value->array()->size() << std::endl;
        break;
    case octillion::JsonValueW::Type::JsonObject:
        std::cout << "object size: " << value->object()->size() << std::endl;
        break;
    default:
        std::cout << "error: unknown type" << std::endl;
    }

    delete json;
    
```

## Work with object

An _object_ contains multiple key-value pairs. Here is an example shows how to get all keys in _object_, as well as retrieving the _value_ via the key. 

``` c++

    // create a test json text using ascii (utf8) string
    octillion::JsonTextW* json = new octillion::JsonTextW(u8"{\"last\":\"Lee\",\"first\":\"Peter\"}");

    // check if json is valid
    if (!json->valid())
    {
        std::cout << "error: invalid json" << std::endl;
        delete json;
        return;
    }

    // check if json has object
    if (json->value()->type() != octillion::JsonValueW::Type::JsonObject)
    {
        std::cout << "error: json does not contain object" << std::endl;
        delete json;
        return;
    }

    // list all keys in object in std::wstring type and std::string type
    octillion::JsonObjectW* object = json->value()->object();
    std::vector<std::wstring> wkeys = object->wkeys();
    std::vector<std::string> keys = object->keys();

    // display name/value pair in ucs (wchar_t), normally programmer should 
    // check the value type before handling it, but for demo purpose, we 
    // 'magically' knows it is string type since it was created by our own.
    for (size_t i = 0; i < wkeys.size(); i++)
    {
        std::wstring wkey = wkeys.at(i);
        std::wstring wvalue = object->find(wkey)->wstring();
        std::wcout << L"wkey-" << i << L":" << wkey << L" value:" << wvalue << std::endl;
    }

    // display name/value pair in utf8 (char)
    for (size_t i = 0; i < keys.size(); i++)
    {
        std::string key = keys.at(i);
        std::string value = object->find(key)->string();
        std::cout << "key-" << i << ":" << key << " value:" << value << std::endl;
    }

    // don't forget to release the memory
    delete json;
    
```

## Work with array

An _array_ contains multiple values. Here is an example shows how to access all _values_ inside an _array_.

``` c++
    
    // create a test json text using ascii (utf8) string
    octillion::JsonTextW* json = new octillion::JsonTextW(u8"[12,13,-42,20]");

    // normally, programmer should check if the value in json is array,
    // but for demo purpose, we 'magically' know the value is array type.
    octillion::JsonValueW* value = json->value();
    octillion::JsonArrayW* array = value->array();

    std::cout << "array size is " << array->size() << std::endl;

    // list each value (again, for demo purpose, we 'magically' know all value in are integer)
    for (size_t i = 0; i < array->size(); i++)
    {
        std::cout << "item-" << i << ":" << array->at(i)->integer() << std::endl;
    }

    // don't forget to release memory
    delete json;
    
```    

# Memory Management

_All the sample code in this section is also available in main.cpp._

Whenever the destructor of a *JsonTextW*, a *JsonObjectW*, a *JsonArrayW* or a *JsonValueW* been called, it releases all the sub-objects inside it. 

For example:

``` c++

    // create three JsonValueW, 'value1', 'value2' and 'value3', dynamically
    octillion::JsonValueW* value1 = new octillion::JsonValueW( 170 );
    octillion::JsonValueW* value2 = new octillion::JsonValueW( u8"peterw" );
    octillion::JsonValueW* value3 = new octillion::JsonValueW( 75 );
    
    // create an JsonObjectW, named 'object', and add three name/value pairs in it
    octillion::JsonObjectW* object = new octillion::JsonObjectW();
    object->add( u8"height", value1 );
    object->add( u8"name", value2 );
    object->add( u8"weight", value3 );
    
    // create another JsonValueW, named 'value4', contains 'object'
    octillion::JsonValueW* value4 = new octillion::JsonValueW( object );
    
    // create an JsonTextW that contains 'value4'
    octillion::JsonTextW* json = new octillion::JsonTextW( value4 );
    
    // the code below calls JsonTextW's destructor and all value1, value2, 
    // value3, object and value4 will be deleted as well
    delete json;

```

Note: the code above is just to demostrate how memory management works. You can write code in much simpler way as below.

``` c++

    // add three name/value pairs into object
    octillion::JsonObjectW* object = new octillion::JsonObjectW();
    object->add( u8"height", 170 );
    object->add( u8"name", u8"peterw" );
    object->add( u8"weight", 75 );

    // Since an object is also a value in json, you can create a JsonTextW via JsonObjectW
    octillion::JsonTextW* json = new octillion::JsonTextW( object );

    delete json;

```

# Known issues and TODO

1. *JsonW* does NOT support the big number. The Json contains number that greater than INT_MAX/HUGE_VAL or less than INT_MIN/-HUGE_VAL is treated as invalid during creation.
2. *JsonW* does NOT handle the memory overflow when reading or creating super massive Json object. If you try to feed several terabytes data in it, the behavior is undefined.
3. *JsonTextW* does NOT support _pretty format_ output yet. When calling _string()_ or _wstring()_, the retrurns data is a single-line text in utf8 or usc encoding.
