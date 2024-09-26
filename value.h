#ifndef VALUE_H
#define VALUE_H
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

enum typeJSON {
    STRING,
    NUMBER, //TODO split into int and float?
    OBJECT,
    ARRAY,
    BOOL,
    typeNULL // NULL taken
};

struct ValueJSON {
    typeJSON type;
    std::variant<std::string, double, bool, std::unordered_map<std::string, ValueJSON>, std::vector<ValueJSON>> value;
};

std::string toString(const ValueJSON& value, int level = 0);
std::string objectToString(const std::unordered_map<std::string, ValueJSON>& obj, int level = 0);

#endif //VALUE_H
