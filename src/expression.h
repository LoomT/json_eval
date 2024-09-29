#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <string>
#include <vector>

enum nodeAction {
    VARIABLE,
    NUMBER_LITERAL,
    GET_MEMBER,
    SUBSCRIPT,
    MAX,
    MIN,
    SIZE
};

struct node {
    int literal = 0;
    std::string variable;
    nodeAction action;
    std::vector<node> children; // list for functions with multiple args, otherwise get first
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

node parseExpression(const std::string& expression, std::string::size_type pos = 0);

#endif //EXPRESSION_H
