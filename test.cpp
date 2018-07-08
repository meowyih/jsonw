
#include <iostream>

#include "jsonw.hpp"

using namespace octillion;

// show how to read json from buffer contains utf8 data
void read_json_from_utf8_data();

// show how to read json from buffer contains usc (wchar_t) data
void read_json_from_usc_data();

// show how to read json from utf8 file
void read_json_from_utf8_file();

// show how to create json as below programatically
// {
//     "txt1": "some text1",
//     "num1": 123,
//     "array":
//     [
//         true,
//         {
//             "txt2": "some text2",
//             "num2": 456
//         }
//     ]
// }
void create_json_programatically();

// show how to get the value from json text and work with it
void how_to_work_with_value();

// show how to work with object
void how_to_work_with_object();

// show how to work with array
void how_to_work_with_array();

// memory management - avoiding deep copy to save memory
void how_to_avoid_deep_copy();

int main()
{
    read_json_from_utf8_data();
    read_json_from_usc_data();
    read_json_from_utf8_file();
    create_json_programatically();
    how_to_work_with_value();
    how_to_work_with_object();
    how_to_work_with_array();
    how_to_avoid_deep_copy();

    // see README.md for the memory leak detection
#ifdef  OCTILLION_JSONW_ENABLE_MEMORY_LEAK_DETECTION
    octillion::JsonW::memory_leak_detect_result();
#endif

    return 0;
}

void read_json_from_utf8_data()
{
    // literal 'u8' is not necessary if compiler uses utf8 as default 
    char jsondata[] = u8"{\"name\":\"meowyih\",\"age\":123}";

    // initialize JsonTextW object using jsondata
    JsonW json(jsondata);

    // json in utf8
    std::cout << json << std::endl;
}

void read_json_from_usc_data()
{
    // literal 'u8' is not necessary if compiler uses utf8 as default 
    wchar_t jsondata[] = L"{\"name\":\"meowyih\",\"age\":123}";

    // initialize JsonTextW object using jsondata
    JsonW json(jsondata);

    // json in utf8
    std::cout << json << std::endl;
}

void read_json_from_utf8_file()
{
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
}

// show how to create json as below programatically
// {
//     "txt1": "some text1",
//     "num1": 123,
//     "array":
//     [
//         true,
//         {
//             "txt2": null,
//             "num2": 0.456
//         }
//     ]
// }
void create_json_programatically()
{
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
}

void how_to_work_with_value()
{
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
}

void how_to_work_with_object()
{
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
}

void how_to_work_with_array()
{
    // create a test json text using ascii (utf8) string
    octillion::JsonW jarray(u8"[12,13,-42,20]");

    std::cout << "json array contains " << jarray.size() << " value(s) in it" << std::endl;

    // list each value (again, for demo purpose, we 'magically' know all value in are integer)
    for (size_t i = 0; i < jarray.size(); i++)
    {
        std::cout << "item-" << i << ":" << jarray[i].integer() << std::endl;
    }
}

// memory management - avoiding deep copy to save memory
void how_to_avoid_deep_copy()
{ 
    JsonW json, jobject, jarray;
    
    jarray[1] = 10; // JsonW automatically assign NULL to jarray[0]
    jobject["data"] = "data"; 

    // deep copy jarray into jobject
    jobject["array"] = jarray;

    // deep copy jobject (as well as jarray) into json
    json["object"] = jobject;

    std::cout << "json:" << json << std::endl;

    // it is not a problem when data size is small, however,
    // if JsonW contains huge data, says 100MB. Deep copy might
    // be a problem if the memory usage is a concern.

    // If caller has such concern, do not use assignment 
    // operaotr (i.e. '='). Use size(JsonW::OBJECT)/get()/add()/keys() for json object
    // and size(JsonW::ARRAY)/get()/add() for json array
    JsonW *p_json, *p_object, *p_jarray;

    p_jarray = new JsonW();

    // null is a standard json string, see README.md for detail
    p_jarray->add(new JsonW("null"));

    // 10 is a standard json string, see README.md for detail
    p_jarray->add(new JsonW("10"));

    p_object = new JsonW();

    // "data" is a standard json string, see README.md for detail
    p_object->add("data", new JsonW("\"data\"")); 

    // assign p_jarray into p_object, it is not deep copy.
    // p_object just copy the address of p_jarray
    p_object->add("array", p_jarray);

    p_json = new JsonW();

    // assign p_object into p_json, it is not deep copy.
    // p_json just copy the address of p_object
    p_json->add("object", p_object);

    std::cout << "p_json:" << p_json->text() << std::endl;

    // when delete the p_json, all the JsonW objects in it
    // would be deleted, includes p_jobject and p_jarray.
    delete p_json;
}