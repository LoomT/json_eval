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
    std::string expression;
    const std::string::size_type pos;

public:
    explicit ExpressionParseException(const char* msg, const char* expression, const std::string::size_type pos)
        : message(msg), expression(expression), pos(pos) {}

    [[nodiscard]] const char* what() const noexcept override
    {
        std::string str = message + '\n' + expression + '\n';
        for(int i = 0; i < pos; i++) {
            str += ' ';
        }
        str += "^\n";
        return str.data();
    }
};

node parseExpression(const std::string& expression);

#endif //EXPRESSION_H
