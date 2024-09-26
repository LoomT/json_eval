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

string skipValue(const string& str) {
    if(str[0] == '"') {
        bool escaped = false;
        for(int i = 1; i < str.size(); i++) {
            if(str[i] == '"' && !escaped) return &str[i+1];

            if(str[i] == '\\' && !escaped) escaped = true;
            else escaped = false;
        }
    }
    if(str[0] == 'n') return &str[4];
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

ValueJSON parseValue(const string& json) {
    if(json[0] == 'n') {
        if(json[1] == 'u' && json[2] == 'l' && json[3] == 'l') {
            ValueJSON value;
            value.type = typeNULL;
            return value;
        }
        throw ParseException("Unexpected value type");
    }
    if(json[0] == '"') {
        ValueJSON value;
        value.type = STRING;
        value.value = parseString(json);
        return value;
    }
    if(json[0] == '{') {
        ValueJSON value;
        value.type = OBJECT;
        value.value = parseObject(json);
        return value;
    }
    throw ParseException("Unexpected value type");
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
        ValueJSON value = parseValue(json);
        if(!object.insert({key, value}).second) throw ParseException("Duplicate keys");
        json = skipWS(skipValue(json));
        if(json[0] == ',') {
            json = skipWS(skip(json, 1));
            continue;
        } else if(json[0] != '}') {
            throw ParseException("Missing ',' after value");
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