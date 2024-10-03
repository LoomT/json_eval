#include "JSON.h"
#include <iostream>
#include <fstream>

#include "value.h"

using namespace std;

unordered_map<string, ValueJSON> parseObject(string json);
ValueJSON parseValue(const string& json);

/**
 *
 * @param filePath file path of JSON
 * @return contents of the JSON file in string
 */
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
inline string skipWS(const string& str) {
    if(str.empty()) return str;
    string::size_type pos = 0;
    while(isspace(str[pos])) {
        pos++;
    }
    return &str[pos];
}

/**
 *
 * @param str string
 * @param amount characters to skip
 * @return str with the amount of characters skipped
 */
inline string skip(const string& str, const string::size_type amount) {
    return &str[amount];
}

/**
 *
 * @param str JSON string starting with number value
 * @return JSON string with the number value skipped
 */
string skipNumber(const string& str) {
    bool dot = false;
    for(int i = 0; i < str.size(); i++) {
        if(i == 0 && str[i] == '-') continue;
        if(isdigit(str[i])) continue;
        if(str[i] == '.') {
            if(dot) throw JSONParseException("Multiple dots in a number");
            dot = true;
            continue;
        }
        if(str[i] == 'e' || str[i] == 'E') {
            i++;
            if(str[i] == '-' || str[i] == '+') i++;
            if(isdigit(str[i])) {
                continue;
            }
            throw JSONParseException("Unexpected character after exponent");
        }
        return &str[i];
    }
    return str;
}

/**
 *
 * @param str JSON string starting with object or array value
 * @param isObject true to skip an object, false for array
 * @return JSON string with the object or array value skipped
 */
string skipObjectOrArray(const string& str, const bool isObject) {
    int stack = 1;
    bool inString = false;  // track if we are inside a string literal
    for (size_t i = 1; i < str.size(); ++i) {
        const char c = str[i];
        if (inString) {
            if (c == '"') inString = false;
            else if (c == '\\') i++; // skip the next escaped char
        } else {
            if (c == '"') {
                inString = true;  // starting or ending a string literal
            } else if (c == (isObject ? '{' : '[')) {
                stack++;
            } else if (c == (isObject ? '}' : ']')) {
                stack--;
                if (stack == 0) {
                    return &str[++i];  // found the matching closing brace
                }
            }
        }
    }
    throw JSONParseException("No matching closing brace found.");
}

/**
 *
* @param str JSON string starting with string value
 * @return JSON string with the string value skipped
 */
string skipString(const string& str) {
    if(str[0] != '"') throw JSONParseException("Missing quotation mark before");
    for(int i = 1; i < str.size(); i++) {
        if(str[i] == '"') return &str[i+1];
        if(str[i] == '\\') i++;
    }
    throw JSONParseException("Cant skip");
}

/**
 *
 * @param str JSON string starting with value
 * @return JSON string with the value skipped
 */
string skipValue(const string& str) {
    if(str[0] == '"') return skipString(str);
    //null, true or false
    if(str[0] == 'n' || str[0] == 't') return &str[4];
    if(str[0] == 'f') return &str[5];
    if(str[0] == '-' || isdigit(str[0])) return skipNumber(str);
    if(str[0] == '{') return skipObjectOrArray(str, true);
    if(str[0] == '[') return skipObjectOrArray(str, false);

    throw JSONParseException("Error while skipping value");
}

/**
 * Extracts a string
 * 
 * @param json string that contains a string to be parsed in front,
 * the wanted string should be surrounded with "
 * @return the extracted string
 */
string parseString(const string& json) {
    if(json[0] != '"') throw JSONParseException("Missing key opening quotation mark '\"'");
    string result;
    for(int i = 1; i < json.size(); i++) {
        if(json[i] == '"') return result;

        if(json[i] == '\\') { //TODO revise escape sequence parsing
            i++;
            result += json[i];
            continue;
        }

        result += json[i];
    }
    throw JSONParseException("Missing string closing quotation mark '\"'");
}

/**
 *
 * @param json JSON string with array in front
 * @return vector representation of the JSON array value
 */
vector<ValueJSON> parseArray(string json) { // NOLINT(*-no-recursion)
    vector<ValueJSON> result;
    json = skip(json, 1);
    while(json[0] != ']') {
        json = skipWS(json);
        result.push_back(parseValue(json));
        json = skipWS(skipValue(json));
        if(json[0] == ',') {
            json = skipWS(skip(json, 1));
            if(json[0] == ']') throw JSONParseException("Unexpected ',' after last value");
        } else if(json[0] != ']') {
            throw JSONParseException("Error: missing ',' after value");
        }
    }
    return result;
}

/**
 * Parses string JSON value into ValueJSON type
 *
 * @param json string with value to parse in front
 * @return parsed value
 */
ValueJSON parseValue(const string& json) { // NOLINT(*-no-recursion)
    ValueJSON value;
    switch(json[0]) {
        case 'n': {
            if(json.size() >= 4 && json[1] == 'u' && json[2] == 'l' && json[3] == 'l') {
                value.type = typeNULL;
                break;
            }
            throw JSONParseException("Unexpected value type");
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
        case '[': {
            value.type = ARRAY;
            value.value = parseArray(json);
            break;
        }
        case 't': {
            if(json.size() >= 4 && json[1] == 'r' && json[2] == 'u' && json[3] == 'e') {
                value.type = BOOL;
                value.value = true;
                break;
            }
            throw JSONParseException("Unexpected value type");
        }
        case 'f': {
            if(json.size() >= 5 && json[1] == 'a' && json[2] == 'l' && json[3] == 's' && json[4] == 'e') {
                value.type = BOOL;
                value.value = false;
                break;
            }
            throw JSONParseException("Unexpected value type");
        }
        case '-':case '0':case '1':case '2':case '3':case '4':
        case '5':case '6':case '7':case '8':case '9': {
            value.type = NUMBER;
            value.value = stod(json);
            break;
        }

        default: throw JSONParseException("Unexpected value type");
    }
    return value;
}

/**
 * Checks if the string can be a valid C++ variable name
 * Only letters, numbers and underscores are allowed. The first character cannot be a digit.
 *
 * @param key string key
 * @return true if valid, false otherwise
 */
inline bool isKeyValid(const string& key) {
    if(key.empty()) return false;
    // check if the first character is a letter
    if(isdigit(key[0])) return false;
    for(int i = 1; i < key.size(); i++) {
        const char c = key[i];
        if(c >= 65 && c <= 90 || c >= 97 && c <= 122) continue;
        if(isdigit(c)) continue;
        if(c == '_') continue;
        return false;
    }
    return true;
}

/**
 * Parses JSON object into a hashmap
 *
 * @param json JSON object string
 * @return hashmap representation of the JSON object
 */
unordered_map<string, ValueJSON> parseObject(string json) { // NOLINT(*-no-recursion)
    unordered_map<string, ValueJSON> object;
    if(json[0] != '{') throw JSONParseException("Missing object opening curly brace '{'");
    json = skipWS(skip(json, 1));
    while(json[0] != '}') {
        const string key = parseString(json);
        if(!isKeyValid(key)) throw JSONParseException(("Invalid key syntax for key " + key).c_str());
        json = skipWS(skipString(json));
        if(json[0] != ':') throw JSONParseException("Missing ':' between key and value");
        json = skipWS(skip(json, 1));
        if(ValueJSON value = parseValue(json); !object.insert({key, value}).second)
            throw JSONParseException("Duplicate keys");
        json = skipWS(skipValue(json));
        if(json[0] == ',') {
            json = skipWS(skip(json, 1));
            if(json[0] == '}') throw JSONParseException("Unexpected ',' after last value");
        } else if(json[0] != '}') {
            const string message = "Key: " + key + " Error: missing ',' after value";
            throw JSONParseException(message.c_str());
        }
    }
    return object;
}

/**
 *
 * @param filePath file path of the JSON to parse
 * @return hashmap representation of the JSON file
 */
unordered_map<std::string, ValueJSON> parseJSON(const string& filePath) {
    cout << "Parsing this string:\n" << openFile(filePath) << endl << endl;
    const string json = skipWS(openFile(filePath));
    if(json.size() < 2) throw JSONParseException("JSON file is less than 2 characters");
    return parseObject(json);
}