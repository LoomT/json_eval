#ifndef VALUE_H
#define VALUE_H
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum TypeJSON {
    STRING,
    INT,
    FLOAT,
    OBJECT,
    ARRAY,
    BOOL,
    typeNULL // NULL taken
};

struct ValueJSON {
    TypeJSON type;
    std::variant<std::string, long long, double, bool, std::unordered_map<std::string, ValueJSON>, std::vector<ValueJSON>> value;

    ValueJSON(const TypeJSON type,
        const std::variant<std::string, long long, double, bool, std::unordered_map<std::string, ValueJSON>, std::vector<ValueJSON>
        > &value)
        : type(type),
          value(value) {
    }

    ValueJSON() = default;
};

std::string toString(const ValueJSON& value);

#endif //VALUE_H
