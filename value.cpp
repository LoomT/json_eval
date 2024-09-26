#include "value.h"

#include <iostream>

using namespace std;

string objectToString(const unordered_map<string, ValueJSON>& obj, const int level) {
    string result = "{\n";
    for (auto &[key, value] : obj) {
        for(int i = 0; i < level; i++) result += "    ";
        result += "    " + key + ": " + toString(value, level) + ",\n";
    }
    result.erase(result.size() - 2, 1);
    for(int i = 0; i < level; i++) result += "    ";
    result += "}";
    return result;
}

string toString(const ValueJSON& value, const int level) {
    switch(value.type) {
        case typeNULL: return "null";
        case STRING: return "\"" + get<string>(value.value) + "\"";
        case NUMBER: return to_string(get<double>(value.value));
        case OBJECT: return objectToString(get<unordered_map<string, ValueJSON>>(value.value), level + 1);
        case ARRAY: return "to be implemented";
        case BOOL: {
            if(get<bool>(value.value)) return "true";
            return "false";
        }
    }
    return "error";
}