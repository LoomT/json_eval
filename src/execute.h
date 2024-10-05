#ifndef EXECUTE_H
#define EXECUTE_H
#include <utility>

#include "expression.h"
#include "value.h"

/**
 *
 * @param JSON entire JSON object
 * @param expression expression to execute
 * @return evaluated expression on JSON
 */
ValueJSON executeExpression(const std::unordered_map<std::string, ValueJSON>& JSON, const Node& expression);

class executeException : public std::exception {
    std::string message;

public:
    explicit executeException(std::string msg)
        : message(move(msg)) {}

    [[nodiscard]] const char* what() const noexcept override
    {
        return message.c_str();
    }
};

class pathException final : public executeException {
    std::string path;
    std::string full;

public:
    explicit pathException(std::string msg, std::string path)
        : executeException(move(msg)), path(move(path)) {}

    [[nodiscard]] const char* what() const noexcept override
    {
        return full.c_str();
    }

    void appendPathFront(std::string front) {
        path = move(front) + path;
        full = std::string(executeException::what()) + "\nWrong path: " + path;
    }

    void appendPathBack(std::string back) {
        path = path + move(back);
        full = std::string(executeException::what()) + "\nWrong path: " + path;
    }
};


#endif //EXECUTE_H
