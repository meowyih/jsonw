#ifndef OCTILLION_JSONW_HEADER
#define OCTILLION_JSONW_HEADER

#include <iostream>
#include <queue>
#include <string>
#include <map>

namespace octillion
{
    class JsonTokenW;

    class JsonValueW;
    class JsonObjectW;
    class JsonArrayW;

    class JsonTextW;
}

// JsonTokenW presents a token in json data. It also has static member that can parse the json from text to token. 
// However, JsonW caller does not need to access this class in almost all the cases. See README.md for detail.
class octillion::JsonTokenW
{
public:
    enum class Type
    {
        NumberInt,
        NumberFrac,
        String,
        LeftCurlyBracket,
        RightCurlyBracket,
        LeftSquareBracket,
        RightSquareBracket,
        Colon,
        Comma,
        Boolean,
        Null,
        Bad
    };

public:
    JsonTokenW(std::wistream& ins);

public:
    enum Type type() { return type_; }
    int integer() { return integer_; }
    double frac() { return frac_; }
    std::wstring wstring() { return wstring_; }
    bool boolean() { return boolean_; }

    friend std::wostream& operator<<(std::wostream& out, const octillion::JsonTokenW& token);

public:
    static bool parse(std::wistream& ins, std::queue<JsonTokenW>& tokens);

private:
    static bool isskippable(wchar_t character);
    static bool findnext(std::wistream& ins);    

private:
    enum Type type_ = Type::Bad;
    int integer_ = 0;
    double frac_ = 0.0;
    std::wstring wstring_;
    bool boolean_ = true;
};

// JsonValueW represents a value in json. This class provide many different 
// constructor to create different type of value, includes number, boolean, null,
// string, json object and json array.
class octillion::JsonValueW
{
public:
    enum class Type
    {
        NumberInt,
        NumberFrac,
        String,
        Boolean,
        Null,
        JsonObject,
        JsonArray,
        Bad
    };
public:

    // constructors for number, boolean, string, null, json object and json array
    JsonValueW(int integer) { type_ = Type::NumberInt; integer_ = integer;  }
    JsonValueW(double frac) { type_ = Type::NumberFrac; frac_ = frac; }
    JsonValueW(bool boolean) { type_ = Type::Boolean; boolean_ = boolean; }
    JsonValueW(std::wstring wstring) { type_ = Type::String; wstring_ = wstring; }
    JsonValueW(const wchar_t* wstring) { type_ = Type::String; wstring_ = wstring; }
    JsonValueW(std::string string);
    JsonValueW(const char* string);
    JsonValueW(JsonObjectW* object) { type_ = Type::JsonObject; object_ = object; }
    JsonValueW(JsonArrayW* array) { type_ = Type::JsonArray; array_ = array; }
    JsonValueW() { type_ = Type::Null; } // null value
    
    // destructor
    ~JsonValueW();
    
    // special constructor that can create value from a sequence of token
    JsonValueW(std::queue<JsonTokenW>& queue);
    
    // members for checking type and if value is valid
    Type type() { return type_; }
    bool valid() { return (type_ != Type::Bad); }

    // accessor members
    JsonObjectW* object() { return object_; }
    JsonArrayW* array() { return array_; }
    int integer() { return integer_; }
    double frac() { return frac_; }
    bool boolean() { return boolean_; }
    std::wstring wstring() { return wstring_; }
    std::string string();

    friend JsonObjectW;
    friend JsonArrayW;
    friend JsonTextW;

private:
    Type type_ = Type::Bad;
    JsonObjectW* object_ = NULL;
    JsonArrayW* array_ = NULL;
    int integer_ = 0;
    double frac_ = 0.0;
    bool boolean_ = true;
    std::wstring wstring_;
};

// JsonObjectW represents an object in json, it was designed to contains multple
// key/value pairs. 
class octillion::JsonObjectW
{
public:
    JsonObjectW();
    JsonObjectW(std::queue<JsonTokenW>& queue);
    ~JsonObjectW();

    // members to add a key/value pairs into object. The key type is ucs encoding string.
    bool add(std::wstring key, JsonValueW* value);
    bool add(std::wstring key, JsonArrayW* array);
    bool add(std::wstring key, JsonObjectW* object);
    bool add(std::wstring key, std::wstring wstring );
    bool add(std::wstring key, const wchar_t* wstring);
    bool add(std::wstring key, int integer);
    bool add(std::wstring key, double frac);
    bool add(std::wstring key, bool boolean);
    bool add(std::wstring key); // null value
    
    // members to add a key/value pairs into object. The key type is utf8 encoding string.
    bool add(std::string key, JsonValueW* value);
    bool add(std::string key, JsonArrayW* array);
    bool add(std::string key, JsonObjectW* object);
    bool add(std::string key, std::string string );
    bool add(std::string key, const char* string);
    bool add(std::string key, int integer);
    bool add(std::string key, double frac);
    bool add(std::string key, bool boolean);
    bool add(std::string key); // null value
    
    // check if object is valid
    bool valid() { return valid_; };

    // return the number of key/value pairs in the object
    size_t size() { return wvalues_.size(); }
    
    // return all available keys in either ucs or utf8 enconding
    std::vector<std::wstring> wkeys();
    std::vector<std::string> keys();

    // access the value via ucs or utf8 key, the return data is NULL if key does not exist
    JsonValueW* find( std::wstring wkey );
    JsonValueW* find( std::string key );

    friend JsonArrayW;
    friend JsonValueW;
    friend JsonTextW;

private:
    bool valid_ = false;
    std::map<std::wstring, JsonValueW*> wvalues_;
};


// JsonArrayW represents an array in json. It was designed to contains multiple values.
class octillion::JsonArrayW
{
public:
    JsonArrayW();
    JsonArrayW(std::queue<JsonTokenW>& queue);
    ~JsonArrayW();

    // add a value with different type
    bool add(JsonValueW* value);
    bool add(JsonArrayW* array);
    bool add(JsonObjectW* object);
    bool add(std::string string);
    bool add(const char* string);
    bool add(std::wstring wstring);
    bool add(const wchar_t* wstring);
    bool add(int integer);
    bool add(double frac);
    bool add(bool boolean);
    bool add(); // add null value
    
    // check if array is valid
    bool valid() { return valid_; };

    // return the number of values in array
    size_t size() { return values_.size(); }
    
    // accessor member 
    JsonValueW* at(size_t idx) { return values_.at(idx); }

    friend JsonObjectW;
    friend JsonValueW;
    friend JsonTextW;

private:
    bool valid_ = false;
    std::vector<JsonValueW*> values_;
};

// JsonTextW represents a json text defined in RFC7159, which contains exactly one value.
// This class provide members to read json data from wistream, ucs data and utf8 data.
// This class also provide members to create json from value, object or array since
// An object or an array is also a value in json standard.
class octillion::JsonTextW
{
public:
    // create JsonTextW from wistream, ucs string or utf8 string
    JsonTextW(std::wistream& ins);
    JsonTextW(const wchar_t* wstr);
    JsonTextW(const wchar_t* uscdata, size_t length );
    JsonTextW(const char* utf8str);
    JsonTextW(const char* utf8data, size_t length );

    // create JsonTextW from value, object or array.
    JsonTextW(JsonValueW* value);
    JsonTextW(JsonObjectW* object);
    JsonTextW(JsonArrayW* array);
    
    // destructor
    ~JsonTextW();
    
    // check if json data is valid
    bool valid() { return valid_; };

    // return the single value in JsonTextW    
    JsonValueW* value() { return value_; }

    // return the json text in ucs or utf8, don't forget the standard json must be utf8 enconding
    std::wstring wstring();
    std::string string();

private:
    void init(std::wistream& ins);

private:
    static std::wstring wstring(const JsonValueW* const value);
    static std::wstring wstring(const JsonObjectW* const object);
    static std::wstring wstring(const JsonArrayW* const array);

private:
    bool valid_ = false;
    JsonValueW* value_ = NULL;
};

#endif // OCTILLION_JSONW_HEADER