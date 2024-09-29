#include "expression.h"

#include <complex>

using namespace std;

bool isLetter(const char c) {
    if((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) return true;
    return false;
}

bool isArithmeticOperator(const char c) {
    if(c == '+' || c == '-' || c == '*' || c == '/') return true;
    return false;
}

node parsePath(const string& expression, string::size_type& pos) {
    if(pos == expression.size()) throw ExpressionParseException("Missing member", expression.c_str(), pos);
    if(!isLetter(expression[pos]) && expression[pos] != '_')
        throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
    node result;
    const string::size_type wordStart = pos;
    while(pos < expression.size()
          && (isLetter(expression[pos]) || isdigit(expression[pos]) || expression[pos] == '_')) pos++;
    if(pos == expression.size() || iswspace(expression[pos]) || isArithmeticOperator(expression[pos])) {
        node leaf;
        leaf.variable = expression.substr(wordStart, pos - wordStart);
        leaf.action = VARIABLE;
        return leaf;
    }
    if(expression[pos] == '.') { // access member from this identifier
        node parent;
        parent.variable = expression.substr(wordStart, pos - wordStart);
        parent.action = GET_MEMBER;
        parent.children.push_back(parsePath(expression, ++pos));
        return parent;
    }
    throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
}

node parseExpression(const string& expression, string::size_type pos) {
    // a.b
    node result;

    while(pos < expression.size()) {
        const char& c = expression[pos];
        if(isLetter(c) || c == '_') {
            const string::size_type wordStart = pos;
            while(pos < expression.size()
                && (isLetter(expression[pos]) || isdigit(expression[pos]) || expression[pos] == '_')) pos++;
            if(pos == expression.size() || iswspace(expression[pos]) || isArithmeticOperator(expression[pos])) {
                node leaf;
                leaf.variable = expression.substr(wordStart, pos - wordStart);
                leaf.action = VARIABLE;
                result = leaf;
            } else if(expression[pos] == '.') { // access member from this identifier
                node parent;
                parent.variable = expression.substr(wordStart, pos - wordStart);
                parent.action = GET_MEMBER;
                parent.children.push_back(parsePath(expression, ++pos));
                result = parent;
            }
        }
        else throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
    }
    return result;
}