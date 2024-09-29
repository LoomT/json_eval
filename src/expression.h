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

node parseExpression(const std::string& expression);

#endif //EXPRESSION_H
