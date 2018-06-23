#ifndef OCTILLION_JSON_HEADER
#define OCTILLION_JSON_HEADER

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

class octillion::JsonTokenW
{
public:
    enum class Type
    {
        NumberInt = 1,
        NumberFrac = 2,
        String = 3,
        LeftCurlyBracket = 4,
        RightCurlyBracket = 5,
        LeftSquareBracket = 6,
        RightSquareBracket = 7,
        Colon = 8,
        Comma = 9,
        Boolean = 10,
        Null = 11,
        Bad = 12
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
    enum Type type_;
    int integer_;
    double frac_;
    std::wstring wstring_;
    bool boolean_;
};

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
    JsonValueW(int integer) { type_ = Type::NumberInt; integer_ = integer;  }
    JsonValueW(double frac) { type_ = Type::NumberFrac; frac_ = frac; }
    JsonValueW(bool boolean) { type_ = Type::Boolean; boolean_ = boolean; }
    JsonValueW(std::wstring wstring) { type_ = Type::String; wstring_ = wstring; }
    JsonValueW(wchar_t* wstring) { type_ = Type::String; wstring_ = wstring; }
    JsonValueW(std::string string);
    JsonValueW(char* string);
    JsonValueW(JsonObjectW* object) { type_ = Type::JsonObject; object_ = object; }
    JsonValueW(JsonArrayW* array) { type_ = Type::JsonArray; array_ = array; }
    JsonValueW() { type_ = Type::Null; }

    JsonValueW(std::queue<JsonTokenW>& queue);
    ~JsonValueW();

    Type type() { return type_; }
    bool valid() { return (type_ != Type::Bad); }

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
    Type type_;
    JsonObjectW* object_;
    JsonArrayW* array_;
    int integer_;
    double frac_;
    bool boolean_;
    std::wstring wstring_;
};

class octillion::JsonObjectW
{
public:
    JsonObjectW();
    JsonObjectW(std::queue<JsonTokenW>& queue);
    ~JsonObjectW();

    bool add(std::wstring key, JsonValueW* value);
    bool add(std::wstring key, JsonArrayW* array);
    bool add(std::wstring key, JsonObjectW* object);
    bool add(std::wstring key, std::wstring wstring );
    bool add(std::wstring key, wchar_t* wstring);
    bool add(std::wstring key, int integer);
    bool add(std::wstring key, double frac);
    bool add(std::wstring key, bool boolean);
    bool add(std::wstring key);
    
    bool add(std::string key, JsonValueW* value);
    bool add(std::string key, JsonArrayW* array);
    bool add(std::string key, JsonObjectW* object);
    bool add(std::string key, std::string string );
    bool add(std::string key, char* string);
    bool add(std::string key, int integer);
    bool add(std::string key, double frac);
    bool add(std::string key, bool boolean);
    bool add(std::string key);
    
    bool valid() { return valid_; };

    size_t size() { return wvalues_.size(); }
    std::vector<std::wstring> wkeys();
    std::vector<std::string> keys();

    JsonValueW* find( std::wstring wkey );
    JsonValueW* find( std::string key );

    friend JsonArrayW;
    friend JsonValueW;
    friend JsonTextW;

private:
    bool valid_;
    std::map<std::wstring, JsonValueW*> wvalues_;
};

class octillion::JsonArrayW
{
public:
    JsonArrayW();
    JsonArrayW(std::queue<JsonTokenW>& queue);
    ~JsonArrayW();

    bool add(JsonValueW* value);
    bool add(JsonArrayW* array);
    bool add(JsonObjectW* object);
    bool add(std::string string);
    bool add(char* string);
    bool add(int integer);
    bool add(double frac);
    bool add(bool boolean);
    bool add();
    bool valid() { return valid_; };

    size_t size() { return values_.size(); }
    JsonValueW* at(size_t idx) { return values_.at(idx); }

    friend JsonObjectW;
    friend JsonValueW;
    friend JsonTextW;

private:
    bool valid_;
    std::vector<JsonValueW*> values_;
};

class octillion::JsonTextW
{
public:
    JsonTextW(std::wistream& ins);
    JsonTextW(wchar_t* wstr);
    JsonTextW(char* utf8str);
    
    JsonTextW(JsonValueW* value);
    JsonTextW(JsonObjectW* object);
    JsonTextW(JsonArrayW* array);
    ~JsonTextW();
    bool valid() { return valid_; };
    
    JsonValueW* value() { return value_; }

    std::wstring wstring();
    std::string string();

private:
    void init(std::wistream& ins);

private:
    static std::wstring wstring(const JsonValueW* const value);
    static std::wstring wstring(const JsonObjectW* const object);
    static std::wstring wstring(const JsonArrayW* const array);

private:
    bool valid_;
    JsonValueW* value_;
};

#endif // OCTILLION_JSON_HEADER