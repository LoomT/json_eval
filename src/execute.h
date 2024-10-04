#ifndef EXECUTE_H
#define EXECUTE_H
#include <utility>

#include "expression.h"
#include "value.h"

inline ValueJSON executeExpression(const std::unordered_map<std::string, ValueJSON>& JSON, const Node& expression);

class executeException final : public std::exception {
    std::string message;
    Node badNode;

public:
    explicit executeException(const char* msg, Node badNode)
        : message(msg), badNode(std::move(badNode)) {}

    [[nodiscard]] const char* what() const noexcept override
    {
        return (message + '\n' + toString(badNode)).c_str();
    }
};

#endif //EXECUTE_H
