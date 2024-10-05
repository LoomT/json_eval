#ifndef parseJSON_H
#define parseJSON_H
#include <string>
#include <unordered_map>
#include "value.h"

/**
 *
 * @param filePath file path of the JSON to parse
 * @return hashmap representation of the JSON file
 */
std::unordered_map<std::string, ValueJSON> parseJSON(const std::string& filePath);

class JSONParseException final : public std::exception {
    std::string message;

    public:
    explicit JSONParseException(const char* msg)
        : message(msg) {}

    [[nodiscard]] const char* what() const noexcept override
    {
        return message.c_str();
    }
};

#endif //parseJSON_H
