
#include <iostream>
#include <sstream>
#include <queue>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <locale>

#include <codecvt>

#include "jsonw.hpp"

// global operator overriding
namespace octillion
{
    std::wostream& operator<<(std::wostream& out, const octillion::JsonTokenW& token)
    {
        switch (token.type_)
        {
        case JsonTokenW::Type::NumberInt:
            out << token.integer_;
            return out;
        case JsonTokenW::Type::NumberFrac:
            out << token.frac_;
            return out;
        case JsonTokenW::Type::String:
            out << token.wstring_;
            return out;
        case JsonTokenW::Type::LeftCurlyBracket:
            out << L"{";
            return out;
        case JsonTokenW::Type::RightCurlyBracket:
            out << L"}";
            return out;
        case JsonTokenW::Type::LeftSquareBracket:
            out << L"[";
            return out;
        case JsonTokenW::Type::RightSquareBracket:
            out << L"]";
            return out;
        case JsonTokenW::Type::Colon:
            out << L":";
            return out;
        case JsonTokenW::Type::Comma:
            out << L",";
            return out;
        case JsonTokenW::Type::Boolean:
            if (token.boolean_)
            {
                out << L"true";
            }
            else
            {
                out << L"false";
            }
            return out;
        case JsonTokenW::Type::Null:
            out << L"null";
            return out;
        case JsonTokenW::Type::Bad:
            out << L"BadToken";
            return out;
        }

        return out;
    }
}

octillion::JsonTokenW::JsonTokenW(std::wistream& ins)
{
    type_ = Type::Bad;
    wchar_t character;

    if (!ins.good())
    {
        return;
    }

    character = ins.peek();

    // return false if no more data to read
    if (character == std::char_traits<wchar_t>::eof())
    {
        return;
    }

    // handle single character token
    switch (character)
    {
    case L'{': type_ = Type::LeftCurlyBracket;  ins.get(); return;
    case L'}': type_ = Type::RightCurlyBracket; ins.get(); return;
    case L'[': type_ = Type::LeftSquareBracket;   ins.get(); return;
    case L']': type_ = Type::RightSquareBracket;  ins.get(); return;
    case L':': type_ = Type::Colon;              ins.get(); return;
    case L',': type_ = Type::Comma;              ins.get(); return;
    }

    // handle number
    if (iswdigit(character) || character == L'-')
    {
        std::wstring numberstr;
        std::wstring expstr;
        bool containdot = false;
        bool containexp = false;
        bool negativeexp = false;
        int exponent = 0;

        // negative value
        if (character == L'-')
        {
            std::cerr << "found -" << std::endl;
            numberstr.push_back(character);
            ins.get();
            character = ins.peek();
        }

        // if start with 0, it must followed by . or standalone zero
        if (character == L'0')
        {
            numberstr.push_back(character);
            ins.get();
            character = ins.peek();

            if (character != L'.')
            {
                type_ = Type::NumberInt;
                integer_ = 0;
                return;
            }

            containdot = true;
            numberstr.push_back(character);
            ins.get();
            character = ins.peek();
        }

        while (iswdigit(character) || character == L'.' || character == L'e')
        {
            // handle .
            if (character == L'.')
            {
                if (containdot || numberstr.length() == 0)
                {
                    return;
                }
                else if (numberstr.length() == 1 && !iswdigit(numberstr.at(0)))
                {
                    return;
                }
                else
                {
                    containdot = true;
                }
            }

            if (character == L'e' || character == L'E')
            {
                containexp = true;
                break;
            }

            numberstr.push_back(character);
            ins.get();
            character = ins.peek();
        }

        // last character in numberstr has to be a digit
        if (numberstr.length() == 0 || !iswdigit(numberstr.back()))
        {
            return;
        }

        // first digit cannot be zero except frac or zero
        if (!containdot && numberstr.at(0) == L'0' && numberstr.length() > 1)
        {
            return;
        }

        // handle exponent notation
        if (character == L'e' || character == L'E')
        {
            ins.get();
            character = ins.peek();
            if (character == L'-')
            {
                negativeexp = true;
                ins.get();
                character = ins.peek();
            }

            if (character == L'+')
            {
                ins.get();
                character = ins.peek();
            }

            while (iswdigit(character))
            {
                expstr.push_back(character);
                ins.get();
                character = ins.peek();
            }

            if (expstr.length() == 0)
            {
                return;
            }

            try
            {
                exponent = std::stoi(expstr);
            }
            catch (const std::out_of_range& oor)
            {
                std::cerr << "Out of Range error: " << oor.what() << '\n';
                type_ = Type::Bad;
                return;
            }
        }

        if (containdot)
        {
            try
            {
                frac_ = std::stold(numberstr);
            }
            catch (const std::out_of_range&)
            {
                type_ = Type::Bad;
                return;
            }

            type_ = Type::NumberFrac;
        }
        else
        {
            try
            {
                integer_ = std::stoi(numberstr);
            }
            catch (const std::out_of_range&)
            {
                type_ = Type::Bad;
                return;
            }

            type_ = Type::NumberInt;
        }

        if (containexp)
        {
            double multiplier;
            if (negativeexp)
            {
                multiplier = std::pow(10, -1 * exponent);
            }
            else
            {
                multiplier = std::pow(10, exponent);
            }

            if (multiplier == HUGE_VAL || multiplier == -HUGE_VAL)
            {
                std::cerr << "Out of Range error" << std::endl;
                type_ = Type::Bad;
                return;
            }

            if (type_ == Type::NumberInt)
            {
                frac_ = 1.0 * integer_;
                type_ = Type::NumberFrac;
            }

            frac_ = frac_ * multiplier;
        }

        return;
    }

    // handle 'true'
    if (character == L't')
    {
        ins.get();
        character = ins.peek();
        if (character == std::char_traits<wchar_t>::eof() || character != L'r')
        {
            return;
        }

        ins.get();
        character = ins.peek();
        if (character == std::char_traits<wchar_t>::eof() || character != L'u')
        {
            return;
        }

        ins.get();
        character = ins.peek();
        if (character == std::char_traits<wchar_t>::eof() || character != L'e')
        {
            return;
        }

        ins.get();
        type_ = Type::Boolean;
        boolean_ = true;
        return;
    }

    // handle 'false'
    if (character == L'f')
    {
        ins.get();
        character = ins.peek();
        if (character == std::char_traits<wchar_t>::eof() || character != L'a')
        {
            return;
        }

        ins.get();
        character = ins.peek();
        if (character == std::char_traits<wchar_t>::eof() || character != L'l')
        {
            return;
        }

        ins.get();
        character = ins.peek();
        if (character == std::char_traits<wchar_t>::eof() || character != L's')
        {
            return;
        }

        ins.get();
        character = ins.peek();
        if (character == std::char_traits<wchar_t>::eof() || character != L'e')
        {
            return;
        }

        ins.get();
        type_ = Type::Boolean;
        boolean_ = false;
        return;
    }

    // handle 'null'
    if (character == L'n')
    {
        ins.get();
        character = ins.peek();
        if (character == std::char_traits<wchar_t>::eof() || character != L'u')
        {
            return;
        }

        ins.get();
        character = ins.peek();
        if (character == std::char_traits<wchar_t>::eof() || character != L'l')
        {
            return;
        }

        ins.get();
        character = ins.peek();
        if (character == std::char_traits<wchar_t>::eof() || character != L'l')
        {
            return;
        }

        ins.get();
        type_ = Type::Null;
        return;
    }

    // the only remaining possible token is string, must start with \"
    if (character != L'\"')
    {
        return;
    }

    // consume \"
    ins.get();
    character = ins.peek();

    // handle string
    bool backslash = false;
    std::wstring strbuf;

    while (character != std::char_traits<wchar_t>::eof())
    {
        if (backslash)
        {
            // previous character is backslash
            if (character == L'u')
            {
                // special case for \u
                std::wstring hexbuf(L"0x");
                ins.get();
                hexbuf.push_back(ins.get());
                hexbuf.push_back(ins.get());
                hexbuf.push_back(ins.get());
                hexbuf.push_back(ins.get());

                if (hexbuf.at(2) == std::char_traits<wchar_t>::eof() ||
                    hexbuf.at(3) == std::char_traits<wchar_t>::eof() ||
                    hexbuf.at(4) == std::char_traits<wchar_t>::eof() ||
                    hexbuf.at(5) == std::char_traits<wchar_t>::eof())
                {
                    return;
                }

                try
                {
                    unsigned int charvalue = std::stoul(hexbuf, NULL, 16);
                    strbuf.push_back((wchar_t)charvalue);
                    character = ins.peek();
                    backslash = false;
                    continue;
                }
                catch (const std::invalid_argument&)
                {
                    return;
                }
                catch (const std::out_of_range&)
                {
                    return;
                }
            }

            // other single character cases
            switch (character)
            {
            case L'\"': strbuf.push_back(L'\"'); break;
            case L'\\': strbuf.push_back(L'\\'); break;
            case L'/': strbuf.push_back(L'/'); break;
            case L'b':  strbuf.push_back((wchar_t)0x08); break;
            case L'f':  strbuf.push_back((wchar_t)0x0c); break;
            case L'n':  strbuf.push_back(L'\n'); break;
            case L'r':  strbuf.push_back(L'\r'); break;
            case L't':  strbuf.push_back(L'\t'); break;
            }

            ins.get();
            character = ins.peek();
            backslash = false;
            continue;
        }
        else if (character == L'\\')
        {
            // special , set flag and fo next round
            backslash = true;
            ins.get();
            character = ins.peek();
            continue;
        }
        else if (character == L'\r' || character == L'\n')
        {
            // unexpected EOL
            return;
        }
        else if (character == L'\"')
        {
            ins.get();
            type_ = Type::String;
            wstring_ = strbuf;
            return;
        }
        else
        {
            strbuf.push_back(character);
            ins.get();
            character = ins.peek();
        }
    }

    // unexpected EOF
    return;
}

bool octillion::JsonTokenW::isskippable(wchar_t character)
{
    if (character == L' ' || character == L'\r' || character == L'\n' || character == L'\t')
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool octillion::JsonTokenW::findnext(std::wistream& ins)
{
    wchar_t character;
    if (!ins.good())
    {
        return false;
    }

    character = ins.peek();

    // return false if no more data to read
    if (character == std::char_traits<wchar_t>::eof())
    {
        return false;
    }

    // skip white space 
    while (isskippable(character))
    {
        ins.get();
        character = ins.peek();

        if (character == std::char_traits<wchar_t>::eof())
        {
            return false;
        }
    }

    // check next character is valid begin character for token 
    if (character == L'[' || character == L']' ||
        character == L'{' || character == L'}' ||
        character == L':' || iswdigit(character) ||
        character == L',' || character == L'\"' ||
        character == L'-' || character == L't' ||
        character == L'f' || character == L'n')
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool octillion::JsonTokenW::parse(std::wistream& ins, std::queue<JsonTokenW>& tokens)
{
    bool success = findnext(ins);

    while (success)
    {
        JsonTokenW token(ins);

        if (token.type() == JsonTokenW::Type::Bad)
        {
            std::queue<JsonTokenW>().swap(tokens); // clear
            return false;
        }
        else
        {
            tokens.push(token);
        }

        success = findnext(ins);
    }
    return true;
}

octillion::JsonValueW::JsonValueW(std::queue<JsonTokenW>& queue)
{
    object_ = NULL;
    array_ = NULL;
    type_ = Type::Bad;

    if (queue.empty())
    {
        return;
    }

    switch (queue.front().type())
    {
    case JsonTokenW::Type::LeftCurlyBracket: // JsonObject
        object_ = new JsonObjectW(queue);
        if (!object_->valid())
        {
            delete object_;
            object_ = NULL;
        }
        else
        {
            type_ = Type::JsonObject;
        }
        return;
    case JsonTokenW::Type::RightCurlyBracket:
        return;
    case JsonTokenW::Type::LeftSquareBracket: // JsonArray
        array_ = new JsonArrayW(queue);
        if (!array_->valid())
        {
            delete array_;
            array_ = NULL;
        }
        else
        {
            type_ = Type::JsonArray;
        }
        return;
    case JsonTokenW::Type::RightSquareBracket: return; // invalid
    case JsonTokenW::Type::NumberInt:
        integer_ = queue.front().integer();
        queue.pop();
        type_ = Type::NumberInt;
        return;
    case JsonTokenW::Type::NumberFrac:
        frac_ = queue.front().frac();
        queue.pop();
        type_ = Type::NumberFrac;
        return;
    case JsonTokenW::Type::String:
        wstring_ = queue.front().wstring();
        queue.pop();
        type_ = Type::String;
        return;
    case JsonTokenW::Type::Boolean:
        boolean_ = queue.front().boolean();
        queue.pop();
        type_ = Type::Boolean;
        return;
    case JsonTokenW::Type::Null:
        queue.pop();
        type_ = Type::Null;
        return;
    case JsonTokenW::Type::Colon: return; // invalid
    case JsonTokenW::Type::Comma: return; // invalid
    case JsonTokenW::Type::Bad: return; // invalid
    }
}

octillion::JsonValueW::JsonValueW(std::string string)
{
    type_ = Type::String;
    
    // convert to wstring
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    wstring_ = conv.from_bytes(string.data());
}

octillion::JsonValueW::JsonValueW(const char* string)
{
    type_ = Type::String;

    // convert to wstring
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    wstring_ = conv.from_bytes(string);
}

std::string octillion::JsonValueW::string()
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstring());
}

octillion::JsonValueW::~JsonValueW()
{
    if (object_ != NULL)
    {
        delete object_;
    }

    if (array_ != NULL)
    {
        delete array_;
    }
}

octillion::JsonObjectW::JsonObjectW()
{
    valid_ = false;
}

bool octillion::JsonObjectW::add(std::wstring key, JsonValueW* value)
{
    auto it = wvalues_.find(key);

    if (it != wvalues_.end())
    {
        valid_ = false;
        return false;
    }

    wvalues_[key] = value;
    valid_ = true;
    return true;
}

bool octillion::JsonObjectW::add(std::string key, JsonValueW* value)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wkey = conv.from_bytes(key.data());
    
    return add( wkey, value );
}

bool octillion::JsonObjectW::add(std::wstring key, JsonArrayW* array)
{
    bool ret;
    JsonValueW* value = new JsonValueW(array);

    ret = add(key, value);
    if (!ret)
    {
        delete value;
    }

    return ret;
}

bool octillion::JsonObjectW::add(std::string key, JsonArrayW* array)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wkey = conv.from_bytes(key.data());
    
    return add( wkey, array );
}

bool octillion::JsonObjectW::add(std::wstring key, JsonObjectW* object)
{
    bool ret;
    JsonValueW* value = new JsonValueW(object);

    ret = add(key, value);
    if (!ret)
    {
        delete value;
    }

    return ret;
}

bool octillion::JsonObjectW::add(std::string key, JsonObjectW* object)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wkey = conv.from_bytes(key.data());
    
    return add( wkey, object );
}

bool octillion::JsonObjectW::add(std::wstring key, std::wstring wstring)
{
    bool ret;
    JsonValueW* value = new JsonValueW(wstring);

    ret = add(key, value);
    if (!ret)
    {
        delete value;
    }

    return ret;
}

bool octillion::JsonObjectW::add(std::string key, std::string string)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wkey = conv.from_bytes(key.data());
    std::wstring wstring = conv.from_bytes(string.data());
    
    return add( wkey, wstring );
}

bool octillion::JsonObjectW::add(std::wstring key, const wchar_t* wstring)
{
    bool ret;
    JsonValueW* value = new JsonValueW(wstring);

    ret = add(key, value);
    if (!ret)
    {
        delete value;
    }

    return ret;
}

bool octillion::JsonObjectW::add(std::string key, const char* string)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wkey = conv.from_bytes(key.data());
    std::wstring wstring = conv.from_bytes(string);
    
    return add( wkey, wstring );
}

bool octillion::JsonObjectW::add(std::wstring key, int integer)
{
    bool ret;
    JsonValueW* value = new JsonValueW(integer);

    ret = add(key, value);
    if (! ret)
    {
        delete value;
    }

    return ret;
}

bool octillion::JsonObjectW::add(std::string key, int integer)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wkey = conv.from_bytes(key.data());
    
    return add( wkey, integer );
}

bool octillion::JsonObjectW::add(std::wstring key, double frac)
{
    bool ret;
    JsonValueW* value = new JsonValueW(frac);

    ret = add(key, value);
    if (!ret)
    {
        delete value;
    }

    return ret;
}

bool octillion::JsonObjectW::add(std::string key, double frac)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wkey = conv.from_bytes(key.data());
    
    return add( wkey, frac );
}

bool octillion::JsonObjectW::add(std::wstring key, bool boolean)
{
    bool ret;
    JsonValueW* value = new JsonValueW(boolean);

    ret = add(key, value);
    if (!ret)
    {
        delete value;
    }

    return ret;
}

bool octillion::JsonObjectW::add(std::string key, bool boolean)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wkey = conv.from_bytes(key.data());
    
    return add( wkey, boolean );
}

bool octillion::JsonObjectW::add(std::wstring key)
{
    bool ret;
    JsonValueW* value = new JsonValueW();

    ret = add(key, value);
    if (!ret)
    {
        delete value;
    }

    return ret;
}

bool octillion::JsonObjectW::add(std::string key)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wkey = conv.from_bytes(key.data());
    
    return add( wkey );
}

octillion::JsonObjectW::JsonObjectW(std::queue<JsonTokenW>& queue)
{
    valid_ = false;

    // Object must start with LeftCurlyBracket:'{' and minimum size is 2 '{' + '}'
    if (queue.size() < 2 ||
        queue.front().type() != JsonTokenW::Type::LeftCurlyBracket)
    {
        return;
    }

    queue.pop();
    while (!queue.empty())
    {
        std::wstring key;
        switch (queue.front().type())
        {
        case JsonTokenW::Type::LeftCurlyBracket: // invalid
            return;
        case JsonTokenW::Type::RightCurlyBracket:
            valid_ = true;
            queue.pop();
            return;
        case JsonTokenW::Type::LeftSquareBracket: // invalid
        case JsonTokenW::Type::RightSquareBracket: // invalid
        case JsonTokenW::Type::NumberInt: // invalid
        case JsonTokenW::Type::NumberFrac: // invalid
        case JsonTokenW::Type::Boolean:
        case JsonTokenW::Type::Null:
            return;
        case JsonTokenW::Type::String:
            key = queue.front().wstring();
            if (key.length() == 0)
            {
                return;
            }
            else if (wvalues_.count(key) > 0)
            {
                return; // not allow duplicate key
            }

            queue.pop();
            if (queue.empty())
            {
                return;
            }
            else if (queue.front().type() != JsonTokenW::Type::Colon)
            {
                return;
            }

            queue.pop();
            if (queue.empty())
            {
                return;
            }
            else
            {
                JsonValueW* value = new JsonValueW(queue);

                switch (value->type())
                {
                case JsonValueW::Type::NumberInt:
                case JsonValueW::Type::NumberFrac:
                case JsonValueW::Type::String:
                case JsonValueW::Type::Boolean:
                case JsonValueW::Type::Null:
                    break;
                }

                if (value->type() == JsonValueW::Type::Bad)
                {
                    return;
                }
                else
                {
                    wvalues_[key] = value;
                }
            }

            if (!queue.empty() && queue.front().type() == JsonTokenW::Type::Comma)
            {
                // consume comma and expect next key-data set
                queue.pop();
            }

            break;

        case JsonTokenW::Type::Colon: return; // invalid
        case JsonTokenW::Type::Comma: return; // invalid
        case JsonTokenW::Type::Bad: return;
        }
    }
}

std::vector<std::wstring> octillion::JsonObjectW::wkeys()
{
    std::vector<std::wstring> vector;
    auto it = wvalues_.begin();

    while (it != wvalues_.end())
    {
        vector.push_back(it->first);
        it++;
    }

    return vector;
}

std::vector<std::string> octillion::JsonObjectW::keys()
{
    std::vector<std::string> vector;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    auto it = wvalues_.begin();

    while (it != wvalues_.end())
    {
        vector.push_back(conv.to_bytes(it->first));
        it++;
    }

    return vector;
}

octillion::JsonValueW* octillion::JsonObjectW::find( std::wstring wkey )
{
    std::map<std::wstring, JsonValueW*>::iterator it = wvalues_.find(wkey);
    if (it != wvalues_.end())
    {
        return it->second;
    }
    else
    {
        return NULL;
    }
}

octillion::JsonValueW* octillion::JsonObjectW::find( std::string key )
{
    // convert to wstring
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wkey = conv.from_bytes(key.data());        
    return find(wkey);
}

octillion::JsonObjectW::~JsonObjectW()
{
    for (auto const& value : wvalues_)
    {
        delete value.second;
    }

    wvalues_.clear();
}

octillion::JsonArrayW::JsonArrayW()
{
    valid_ = false;
}

bool octillion::JsonArrayW::add(JsonValueW* value)
{
    if (value == NULL)
    {
        return false;
    }

    valid_ = true;
    values_.push_back( value );

    return true;
}

octillion::JsonArrayW::JsonArrayW(std::queue<JsonTokenW>& queue)
{
    valid_ = false;

    // Object must start with LeftCurlyBracket:'[' and minimum size is 2 '[' + ']'
    if (queue.size() < 2 ||
        queue.front().type() != JsonTokenW::Type::LeftSquareBracket)
    {
        std::cerr << "Incomplete array" << std::endl;
        return;
    }

    queue.pop();
    while (!queue.empty())
    {
        switch (queue.front().type())
        {
        case JsonTokenW::Type::RightSquareBracket:
            valid_ = true;
            queue.pop();
            return;
        case JsonTokenW::Type::LeftCurlyBracket: // JsonData (object type)
        case JsonTokenW::Type::LeftSquareBracket: // JsonData (array type)
        case JsonTokenW::Type::NumberInt: // JsonData (int type)
        case JsonTokenW::Type::NumberFrac: // JsonData (frac type)
        case JsonTokenW::Type::Boolean:
        case JsonTokenW::Type::Null:
        {
            JsonValueW* value = new JsonValueW(queue);
            if (value->type() == JsonValueW::Type::Bad)
            {
                return;
            }

            values_.push_back(value);

            // if followed by comma, continually read next JsonData
            if (!queue.empty() && queue.front().type() == JsonTokenW::Type::Comma)
            {
                queue.pop();
                continue;
            }
            else if (!queue.empty() && queue.front().type() == JsonTokenW::Type::RightSquareBracket)
            {
                queue.pop();
                valid_ = true;
                return;
            }

            return;
        }

        // invalid
        case JsonTokenW::Type::RightCurlyBracket: return;
        case JsonTokenW::Type::Colon: return;
        case JsonTokenW::Type::Comma: return;
        case JsonTokenW::Type::Bad: return;
        }
    }
}

bool octillion::JsonArrayW::add(JsonArrayW* array)
{
    return add(new JsonValueW(array));
}

bool octillion::JsonArrayW::add(JsonObjectW* object)
{
    return add(new JsonValueW(object));
}

bool octillion::JsonArrayW::add(std::string string)
{
    return add(new JsonValueW(string));
}

bool octillion::JsonArrayW::add(const char* string)
{
    return add(new JsonValueW(string));
}

bool octillion::JsonArrayW::add(std::wstring wstring)
{
    return add(new JsonValueW(wstring));
}

bool octillion::JsonArrayW::add(const wchar_t* wstring)
{
    return add(new JsonValueW(wstring));
}

bool octillion::JsonArrayW::add(int integer)
{
    return add(new JsonValueW(integer));
}

bool octillion::JsonArrayW::add(double frac)
{
    return add(new JsonValueW(frac));
}

bool octillion::JsonArrayW::add(bool boolean)
{
    return add(new JsonValueW(boolean));
}

bool octillion::JsonArrayW::add()
{
    return add(new JsonValueW());
}

octillion::JsonArrayW::~JsonArrayW()
{
    for (auto const& value : values_)
    {
        delete value;
    }

    values_.clear();
}

void octillion::JsonTextW::init(std::wistream& ins)
{
    std::queue<octillion::JsonTokenW> tokens;

    value_ = NULL;

    // new std::codecvt_utf8<wchar_t> 
    // The constructed locale object takes over responsibility for deleting this facet object.
    // http://www.cplusplus.com/reference/locale/locale/locale/
    ins.imbue(std::locale(ins.getloc(), new std::codecvt_utf8<wchar_t>));
    octillion::JsonTokenW::parse(ins, tokens);
    octillion::JsonValueW* value = new octillion::JsonValueW(tokens);

    if ( value->type_ == JsonValueW::Type::Bad )
    {
        valid_ = false;
        delete value;
    }
    else
    {
        valid_ = true;
        value_ = value;
    }
}

octillion::JsonTextW::JsonTextW(std::wistream& ins)
{
    init(ins);
}

octillion::JsonTextW::JsonTextW(const wchar_t* wstr )
{
    // convert to wstringbuf
    std::wstringbuf strBuf(wstr);
    
    // convert to wistream
    std::wistream wins(&strBuf);
    
    // constructor with wistream parameter 
    init(wins);
}

octillion::JsonTextW::JsonTextW(const wchar_t* ucsdata, size_t size )
{
    // convert to std::wstring
    std::wstring wstr( ucsdata, size );
    
    // convert to wstringbuf
    std::wstringbuf strBuf(wstr);
    
    // convert to wistream
    std::wistream wins(&strBuf);
    
    // constructor with wistream parameter 
    init(wins);
}

octillion::JsonTextW::JsonTextW(const char* utf8str)
{
    // convert to wstring
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wstr = conv.from_bytes(utf8str);

    // convert to wstringbuf
    std::wstringbuf strBuf(wstr.data());

    // convert to wistream
    std::wistream wins(&strBuf);
    
    // constructor with std::wstring parameter
    init(wins);
}

octillion::JsonTextW::JsonTextW(const char* utf8data, size_t length )
{
    // convert to std::string
    std::string utf8str( utf8data, length );
    
    // convert to wstring
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wstr = conv.from_bytes(utf8str);

    // convert to wstringbuf
    std::wstringbuf strBuf(wstr.data());

    // convert to wistream
    std::wistream wins(&strBuf);
    
    // constructor with std::wstring parameter
    init(wins);
}

octillion::JsonTextW::JsonTextW(JsonValueW* value)
{
    value_ = value;
}

octillion::JsonTextW::JsonTextW(JsonObjectW* object)
{
    JsonValueW* value = new JsonValueW(object);
    value_ = value;
}

octillion::JsonTextW::JsonTextW(JsonArrayW* array)
{
    JsonValueW* value = new JsonValueW(array);
    value_ = value;
}

octillion::JsonTextW::~JsonTextW()
{
    if (value_ != NULL)
    {
        delete value_;
    }
}

std::wstring octillion::JsonTextW::wstring()
{
    return wstring(value_);
}

std::string octillion::JsonTextW::string()
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstring(value_));
}

std::wstring octillion::JsonTextW::wstring(const JsonValueW* const value)
{
    std::wstring str;

    if (value == NULL || value->type_ == JsonValueW::Type::Bad)
    {
        return str;
    }

    switch (value->type_)
    {
    case JsonValueW::Type::NumberInt:
        return std::to_wstring( value->integer_ );
    case JsonValueW::Type::NumberFrac:
        return std::to_wstring(value->frac_);
    case JsonValueW::Type::Boolean:
        if (value->boolean_)
        {
            return L"true";
        }
        else
        {
            return L"false";
        }
    case JsonValueW::Type::Null:
        return L"null";
    case JsonValueW::Type::String:

        for (int i = 0; i < (int) value->wstring_.length(); i++)
        {
            wchar_t wchar = value->wstring_.at(i);

            switch (wchar)
            {
            case 0x22: str.append(L"\\\""); break;
            case 0x5C: str.append(L"\\\\"); break;
            case 0x2F: str.append(L"\\/"); break;
            case 0x08: str.append(L"\\b"); break;
            case 0x0C: str.append(L"\\f"); break;
            case 0x0A: str.append(L"\\n"); break;
            case 0x0D: str.append(L"\\r"); break;
            case 0x09: str.append(L"\\t"); break;
            default: str = str + wchar;
            }
        }

        return L"\"" + str + L"\"";
    case JsonValueW::Type::JsonObject:
        return wstring(value->object_);
    case JsonValueW::Type::JsonArray:
        return wstring(value->array_);
    case JsonValueW::Type::Bad:
        return str;
    }
    return str;
}

std::wstring octillion::JsonTextW::wstring(const JsonObjectW* const object)
{
    std::wstring str;
    bool first = true;
    if (!object->valid_)
    {
        return str;
    }

    str = L"{";
    for (auto const &value : object->wvalues_)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            str = str + L",";
        }

        str = str + L"\"" + value.first + L"\"" + L":" + wstring(value.second);
    }

    str = str + L"}";

    return str;
}

std::wstring octillion::JsonTextW::wstring(const JsonArrayW* const array)
{
    std::wstring str;
    bool first = true;

    str = L"[";
    for (auto const &value : array->values_)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            str = str + L",";
        }

        str = str + wstring(value);
    }
    str = str + L"]";

    return str;
}
