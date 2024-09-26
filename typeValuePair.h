#ifndef TYPEVALUEPAIR_H
#define TYPEVALUEPAIR_H
#include <string>
#include <utility>
#include <vector>

enum typeJSON {
    STRING,
    NUMBER, //TODO split into int and float?
    OBJECT,
    ARRAY,
    BOOL,
    typeNULL // NULL taken
};

struct Value {
    std::string string;
    double number;
    std::unordered_map<std::string, Value> object;
    std::vector<Value> array;
    bool boolean;
    typeJSON type;
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
