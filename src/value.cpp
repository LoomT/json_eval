#include "value.h"

#include <format>
#include <sstream>

using namespace std;

string objectToString(const unordered_map<string, ValueJSON>& obj) { // NOLINT(*-no-recursion)
    stringstream ss;
    stringstream::pos_type pos;
    ss << "{ ";
    for (auto &[key, value] : obj) {
        ss << '"' << key << "\": " << toString(value);
        pos = ss.tellp();
        ss << ", ";
    }
    ss.seekp(pos);
    ss << " }";
    return ss.str();
}

string arrayToString(const vector<ValueJSON>& array) { // NOLINT(*-no-recursion)
    stringstream ss;
    stringstream::pos_type pos;
    ss << "[ ";
    for (const ValueJSON& item : array) {
        ss << toString(item);
        pos = ss.tellp();
        ss << ", ";
    }
    ss.seekp(pos);
    ss << " ]";
    return ss.str();
}

string toString(const ValueJSON& value) { // NOLINT(*-no-recursion)
    switch(value.type) {
        case typeNULL: return "null";
        case STRING: return "\"" + get<string>(value.value) + "\"";
        case INT: return to_string(get<long long>(value.value));
        case FLOAT: return format("{}", get<double>(value.value)); // to_string does not remove trailing zeroes pre C++26
        case OBJECT: return objectToString(get<unordered_map<string, ValueJSON>>(value.value));
        case ARRAY: return arrayToString(get<vector<ValueJSON>>(value.value));
        case BOOL: {
            if(get<bool>(value.value)) return "true";
            return "false";
        }
    }
    return "error";
}