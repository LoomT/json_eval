#include "parseJSON.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>

#include "value.h"

using namespace std;

unordered_map<string, ValueJSON> parseObject(const string& json, string::size_type& pos);
ValueJSON parseValue(const string& json, string::size_type& pos);

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
 * Extracts a string
 * 
 * @param json string that contains a string to be parsed in front,
 * the wanted string should be surrounded with "
 * @return the extracted string
 */
string parseString(const string& json, string::size_type& pos) {
    if(json[pos] != '"') throw JSONParseException("Missing key opening quotation mark '\"'");
    pos++;
    stringstream result;
    for(; pos < json.size(); pos++) {
        if(json[pos] == '"') {
            pos++;
            return result.str();
        }

        if(json[pos] == '\\') {
            pos++;
            if(pos >= json.size()) throw JSONParseException("Reached EOF while parsing string");
            switch(const char c = json[pos]) {
                case '\\':
                case '/':
                case '"':
                    result << c;
                    break;
                case 'b':
                    result << '\b';
                    break;
                case 'f':
                    result << '\f';
                    break;
                case 'n':
                    result << '\n';
                    break;
                case 'r':
                    result << '\r';
                    break;
                case 't':
                    result << '\t';
                    break;
                case 'u': {
                    stringstream ss;
                    for(int j = 0; j < 4; j++) {
                        pos++;
                        if(pos >= json.size()) throw JSONParseException("Reached EOF while parsing string");
                        const char h = json[pos];
                        ss << hex << h;
                    }
                    if(int j; ss >> j)
                        result << static_cast<char>(j);
                    else throw JSONParseException("Error while parsing universal character name");
                    break;
                }
                default: throw JSONParseException("Unexpected escape sequence");
            }
        }

        else result << json[pos];
    }
    throw JSONParseException("Missing string closing quotation mark '\"'");
}

/**
 *
 * @param json JSON string with array in front
 * @return vector representation of the JSON array value
 */
vector<ValueJSON> parseArray(const string& json, string::size_type& pos) { // NOLINT(*-no-recursion)
    vector<ValueJSON> result;
    pos++;
    while(json[pos] != ']') {
        result.push_back(parseValue(json, pos));
        // pos += skipValue(json, pos);
        if(json[pos] == ',') {
            pos++;
            if(json[pos] == ']') throw JSONParseException("Unexpected ',' after last value");
        } else if(json[pos] != ']') {
            throw JSONParseException("Error: missing ',' after value");
        }
    }
    pos++;
    return result;
}

/**
 * Parses string JSON value into ValueJSON type
 *
 * @param json string with value to parse in front
 * @return parsed value
 */
ValueJSON parseValue(const string& json, string::size_type& pos) { // NOLINT(*-no-recursion)
    ValueJSON value;
    switch(json[pos]) {
        case 'n': {
            if(json.size() >= pos + 4 && json[pos+1] == 'u' && json[pos+2] == 'l' && json[pos+3] == 'l') {
                value.type = typeNULL;
                pos += 4;
                break;
            }
            throw JSONParseException("Unexpected value type");
        }
        case '"': {
            value.type = STRING;
            value.value = parseString(json, pos);
            break;
        }
        case '{': {
            value.type = OBJECT;
            value.value = parseObject(json, pos);
            break;
        }
        case '[': {
            value.type = ARRAY;
            value.value = parseArray(json, pos);
            break;
        }
        case 't': {
            if(json.size() >= pos + 4 && json[pos + 1] == 'r' && json[pos + 2] == 'u' && json[pos + 3] == 'e') {
                value.type = BOOL;
                value.value = true;
                pos += 4;
                break;
            }
            throw JSONParseException("Unexpected value type");
        }
        case 'f': {
            if(json.size() >= pos + 5 && json[pos + 1] == 'a' && json[pos + 2] == 'l' && json[pos + 3] == 's' && json[pos + 4] == 'e') {
                value.type = BOOL;
                value.value = false;
                pos += 5;
                break;
            }
            throw JSONParseException("Unexpected value type");
        }
        case '-':
            if(json.size() < pos + 2 || !isdigit(json[pos + 1])) throw JSONParseException("Negative sign should be followed by a number");
        // falls through if there is a digit after -
        case '0':case '1':case '2':case '3':case '4':
        case '5':case '6':case '7':case '8':case '9': {
            size_t intPos;
            size_t floatPos;

            long long intNumber = stoll(json.substr(pos), &intPos);
            double floatNumber = stod(json.substr(pos), &floatPos);
            // if intPos == floatPos then there was no fraction part which means it's an integer
            if(intPos == floatPos) {
                value.type = INT;
                value.value = intNumber;
            } else {
                value.type = FLOAT;
                value.value = floatNumber;
            }
            pos += floatPos;
            break;
        }

        default: throw JSONParseException("Unexpected value type");
    }
    return value;
}

/**
 * Checks if the string can be a valid C++ identifier
 * Only letters, numbers, underscores and dollar signs are allowed. The first character cannot be a digit.
 *
 * @param key string key
 * @return true if valid, false otherwise
 */
inline bool isKeyValid(const string& key) {
    if(key.empty()) return false;
    // check if the first character is a letter
    if(const char c = key[0];
        !isalpha(c) && c != '_' && c != '$') return false;
    for(int i = 1; i < key.size(); i++) {
        if(const char c = key[i];
            isalpha(c) || isdigit(c) || c == '_' || c == '$') continue;
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
unordered_map<string, ValueJSON> parseObject(const string& json, string::size_type& pos) { // NOLINT(*-no-recursion)
    unordered_map<string, ValueJSON> object;
    if(json[pos] != '{') throw JSONParseException("Missing object opening curly brace '{'");
    pos++;
    while(json[pos] != '}') {
        const string key = parseString(json, pos);
        if(!isKeyValid(key)) throw JSONParseException(("Invalid key syntax for key " + key).c_str());
        // pos += skipString(json, pos);
        if(json[pos] != ':') throw JSONParseException("Missing ':' between key and value");
        pos++;
        if(ValueJSON value = parseValue(json, pos); !object.insert({key, value}).second)
            throw JSONParseException("Duplicate keys");
        // pos += skipValue(json, pos);
        if(json[pos] == ',') {
            pos++;
            if(json[pos] == '}') throw JSONParseException("Unexpected ',' after last value");
        } else if(json[pos] != '}') {
            const string message = "Key: " + key + " Error: missing ',' after value";
            throw JSONParseException(message.c_str());
        }
    }
    pos++;
    return object;
}

/**
 *
 * @param filePath file path of the JSON to parse
 * @return hashmap representation of the JSON file
 */
unordered_map<std::string, ValueJSON> parseFileJSON(const string& filePath) {
    string json = openFile(filePath);
    erase_if(json, [](const unsigned char c){return iswspace(c);});
    if(json.size() < 2) throw JSONParseException("JSON file is less than 2 characters");
    string::size_type pos = 0;
    return parseObject(json, pos);
}

unordered_map<std::string, ValueJSON> parseJSON(string json) {
    erase_if(json, [](const unsigned char c){return iswspace(c);});
    if(json.size() < 2) throw JSONParseException("JSON file is less than 2 characters");
    string::size_type pos = 0;
    return parseObject(json, pos);
}