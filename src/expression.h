#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <memory>
#include <string>
#include <vector>

enum NodeAction {
    VARIABLE,
    NUMBER_LITERAL,
    GET_MEMBER,
    GET_SUBSCRIPT,
    ONLY_SUBSCRIPT,
    MAX,
    MIN,
    SIZE
};

struct Node {
    int literal = 0;
    NodeAction action;
    std::string variable;
    std::shared_ptr<Node> subscript;
    std::vector<Node> children; // list for functions with multiple args, otherwise get first
};

class ExpressionParseException final : public std::exception {
    std::string message;

public:
    explicit ExpressionParseException(const char* msg, const char* expression, const std::string::size_type pos) {
        message = std::string(msg) + '\n' + expression + '\n';
        for(int i = 0; i < pos; i++) {
            message += ' ';
        }
        message += '^';
    }

    [[nodiscard]] const char* what() const noexcept override
    {
        return message.c_str();
    }
};

std::unique_ptr<Node> parseExpression(const std::string& expression);

#endif //EXPRESSION_H
