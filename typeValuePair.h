#ifndef TYPEVALUEPAIR_H
#define TYPEVALUEPAIR_H
#include <string>
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

class typeValuePair {
    public:
        const typeJSON type;

        explicit typeValuePair(const typeJSON type)
            : type(type) {
        }
};

class valueString : public typeValuePair {
    public:
        const std::string value;

        explicit valueString(const typeJSON type, std::string  value)
            : typeValuePair(type), value(std::move(value)) {}

};


#endif //TYPEVALUEPAIR_H
