#ifndef JSON_EVAL_PARSE_H
#define JSON_EVAL_PARSE_H
#include <string>
#include <unordered_map>
#include "value.h"

class ParseException final : public std::exception {
private:
    std::string message;

public:
    explicit ParseException(const char* msg)
        : message(msg) {}

    [[nodiscard]] const char* what() const noexcept override
    {
        return message.c_str();
    }
};

std::unordered_map<std::string, ValueJSON> parseJSON(const std::string& filePath);

#endif //JSON_EVAL_PARSE_H
