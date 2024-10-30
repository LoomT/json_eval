#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <memory>
#include <string>
#include <variant>
#include <vector>

enum NodeAction {
    IDENTIFIER,
    INT_LITERAL,
    FLOAT_LITERAL,
    GET_MEMBER,
    GET_SUBSCRIPT,
    ONLY_SUBSCRIPT,
    MAX,
    MIN,
    SIZE,
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    RAISE // TODO this should be a function pow() since ^ is reserved for xor
};

struct Node {
    std::variant<long long, double, std::string> value; // could be a number literal or identifier
    NodeAction action;
    std::unique_ptr<Node> subscript;
    std::vector<Node> children; // list for functions with multiple args, otherwise get first

    explicit Node(const NodeAction& action): action(action) {}
};

/**
 * Parses the string expression into a linked list
 * that can be more easily executed
 *
 * @param expression complete string expression
 * @return root node of the expression tree/linked list
 */
Node parseExpression(std::string expression);

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
