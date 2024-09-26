#include "parse.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "typeValuePair.h"

using namespace std;

unordered_map<string, ValueJSON> parseObject(string json);

string openFile(const string& filePath) {
    ifstream in(filePath);
    if(!in.good()) {
        throw runtime_error{"Could not open " + filePath};
    }
    in.seekg(0, ios::end);
    const long long size = in.tellg();
    string buffer(size, ' ');
    in.seekg(0);
    in.read(&buffer[0], size);
    return buffer;
}

/**
 *
 * strips leading whitespace by moving the pointer on the given string
 *
 * @param str string to strip
 * @return string with whitespace skipped
 */
string skipWS(const string& str) {
    if(str.empty()) return str;
    string::size_type pos = 0;
    while(isspace(str[pos])) {
        pos++;
    }
    return &str[pos];
}

string skip(const string& str, const string::size_type amount) {
    return &str[amount];
}

string skipNumber(const string& str) {
    bool dot = false;
    for(int i = 0; i < str.size(); i++) {
        if(i == 0 && str[i] == '-') continue;
        if(isdigit(str[i])) continue;
        if(str[i] == '.') {
            if(dot) throw ParseException("Multiple dots in a number");
            dot = true;
            continue;
        }
        if(str[i] == 'e' || str[i] == 'E') {
            i++;
            if(str[i] == '-' || str[i] == '+') i++;
            if(isdigit(str[i])) {
                i++;
                continue;
            }
            throw ParseException("Unexpected character after exponent");
        }
        return &str[i];
    }
    return str;
}

string skipValue(const string& str) {
    if(str[0] == '"') {
        bool escaped = false;
        for(int i = 1; i < str.size(); i++) {
            if(str[i] == '"' && !escaped) return &str[i+1];

            if(str[i] == '\\' && !escaped) escaped = true;
            else escaped = false;
        }
    }
    //null, true or false
    if(str[0] == 'n' || str[0] == 't') return &str[4];
    if(str[0] == 'f') return &str[5];
    if(str[0] == '-' || isdigit(str[0])) return skipNumber(str);

    throw ParseException("Cant skip");
}

string parseString(const string& json) {
    if(json[0] != '"') throw ParseException("Missing key opening \"");
    string result;
    bool escaped = false;
    for(int i = 1; i < json.size(); i++) {
        if(json[i] == '"' && !escaped) return result;

        if(json[i] == '\\' && !escaped) escaped = true;
        else escaped = false;

        result += json[i];
    }
    throw ParseException("Missing key closing \"");
}

ValueJSON parseValue(const string& json) { // NOLINT(*-no-recursion)
    ValueJSON value;
    switch(json[0]) {
        case 'n': {
            if(json.size() >= 4 && json[1] == 'u' && json[2] == 'l' && json[3] == 'l') {
                value.type = typeNULL;
                break;
            }
            throw ParseException("Unexpected value type");
        }
        case '"': {
            value.type = STRING;
            value.value = parseString(json);
            break;
        }
        case '{': {
            value.type = OBJECT;
            value.value = parseObject(json);
            break;
        }
        case 't': {
            if(json.size() >= 4 && json[1] == 'r' && json[2] == 'u' && json[3] == 'e') {
                value.type = BOOL;
                value.value = true;
                break;
            }
            throw ParseException("Unexpected value type");
        }
        case 'f': {
            if(json.size() >= 5 && json[1] == 'a' && json[2] == 'l' && json[3] == 's' && json[4] == 'e') {
                value.type = BOOL;
                value.value = false;
                break;
            }
            throw ParseException("Unexpected value type");
        }
        case '-':case '0':case '1':case '2':case '3':case '4':
        case '5':case '6':case '7':case '8':case '9': {
            value.type = NUMBER;
            value.value = stod(json);
            break;
        }

        default: throw ParseException("Unexpected value type");
    }
    return value;
}

unordered_map<string, ValueJSON> parseObject(string json) { // NOLINT(*-no-recursion)
    unordered_map<string, ValueJSON> object;
    if(json[0] != '{') throw ParseException("Missing object opening curly brace '{'");
    json = skipWS(skip(json, 1));
    while(json[0] != '}') {
        const string key = parseString(json);
        json = skipWS(skip(json, key.size() + 2));
        if(json[0] != ':') throw ParseException("Missing ':' between key and value");
        json = skipWS(skip(json, 1));
        if(ValueJSON value = parseValue(json); !object.insert({key, value}).second)
            throw ParseException("Duplicate keys");
        json = skipWS(skipValue(json));
        if(json[0] == ',') {
            json = skipWS(skip(json, 1));
            if(json[0] == '}') throw ParseException("Unexpected ',' after last value");
        } else if(json[0] != '}') {
            const string message = "Key: " + key + " Error: missing ',' after value";
            throw ParseException(message.c_str());
        }
    }
    return object;
}

unordered_map<std::string, ValueJSON> parseJSON(const string& filePath) {
    cout << openFile(filePath) << endl;
    const string json = skipWS(openFile(filePath));
    if(json.size() < 2) throw ParseException("JSON file is less than 2 characters");
    return parseObject(json);
}