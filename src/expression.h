#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <memory>
#include <string>
#include <vector>

enum NodeAction {
    IDENTIFIER,
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
    std::string identifier;
    std::shared_ptr<Node> subscript; // this could ideally be a unique pointer
    std::vector<Node> children; // list for functions with multiple args, otherwise get first
};

/**
 * Parses the string expression into a linked list
 * that can be more easily executed
 *
 * @param expression complete string expression
 * @return root node of the expression tree/linked list
 */
Node parseExpression(const std::string& expression);

/**
 *
 * @param node (local) root expression node
 * @return string representation of the node
 */
std::string toString(Node node);

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

#endif //EXPRESSION_H
