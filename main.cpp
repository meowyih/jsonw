// NOTE: see README.md for detail

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "jsonw.hpp"

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

// show how the memory management works, see README.md for detail
void show_how_memory_management_works();

// much simpler version of show_how_memory_management_works()
void show_how_memory_management_works_simple();

int main()
{
    read_json_from_utf8_data();
    read_json_from_usc_data();
    read_json_from_utf8_file();
    create_json_programatically();
    how_to_work_with_value();
    how_to_work_with_object();
    how_to_work_with_array();
    show_how_memory_management_works();
    show_how_memory_management_works_simple();
    return 0;
}

void read_json_from_utf8_data()
{
    // literal 'u8' is not necessary if compiler uses utf8 as default 
    char jsondata[] = u8"{\"name\":\"meowyih\",\"age\":123}";

    // initialize JsonTextW object using jsondata
    octillion::JsonTextW* json = new octillion::JsonTextW(jsondata);

    // json in utf8
    std::cout << json->string() << std::endl;

    delete json;
}

void read_json_from_usc_data()
{
    // literal 'u8' is not necessary if compiler uses utf8 as default 
    wchar_t jsondata[] = L"{\"name\":\"meowyih\",\"age\":123}";

    // initialize JsonTextW object using jsondata
    octillion::JsonTextW* json = new octillion::JsonTextW(jsondata);

    // json in utf8
    std::cout << json->string() << std::endl;

    delete json;
}

void read_json_from_utf8_file()
{
    // 'sample.json' must utf-8 format without BOM
    std::string jsonfile = "F:\\VSProject\\JsonParser\\Debug\\sample.json";

    // initialize JsonTextW object using wistream (wifstream)
    std::wifstream wfin(jsonfile);

    if (!wfin.good())
    {
        std::wcout << L"bad ifstream" << std::endl;
        return;
    }

    // initialize JsonTextW object using wfin
    octillion::JsonTextW* json = new octillion::JsonTextW(wfin);

    // json in utf8
    std::cout << json->string() << std::endl;

    delete json;
}

void create_json_programatically()
{
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
}

void how_to_work_with_value()
{
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
}

void how_to_work_with_object()
{
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
}

void how_to_work_with_array()
{
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
}

void show_how_memory_management_works()
{
    // create three JsonValueW, 'value1', 'value2' and 'value3', dynamically
    octillion::JsonValueW* value1 = new octillion::JsonValueW(170);
    octillion::JsonValueW* value2 = new octillion::JsonValueW(u8"peterw");
    octillion::JsonValueW* value3 = new octillion::JsonValueW(75);

    // create an JsonObjectW, named 'object', and add three name/value pairs in it
    octillion::JsonObjectW* object = new octillion::JsonObjectW();
    object->add(u8"height", value1);
    object->add(u8"name", value2);
    object->add(u8"weight", value3);

    // create another JsonValueW, named 'value4', contains 'object'
    octillion::JsonValueW* value4 = new octillion::JsonValueW(object);

    // create an JsonTextW that contains 'value4'
    octillion::JsonTextW* json = new octillion::JsonTextW(value4);

    // the code below calls JsonTextW's destructor and all value1, value2, 
    // value3, object and value4 will be deleted as well
    delete json;
}

void show_how_memory_management_works_simple()
{
    // add three name/value pairs into object
    octillion::JsonObjectW* object = new octillion::JsonObjectW();
    object->add(u8"height", 170);
    object->add(u8"name", u8"peterw");
    object->add(u8"weight", 75);

    // Since an object is also a value in json, you can create a JsonTextW via JsonObjectW
    octillion::JsonTextW* json = new octillion::JsonTextW(object);

    delete json;
}
