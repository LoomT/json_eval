#include "expression.h"

#include <complex>
#include <memory>

using namespace std;

unique_ptr<Node> parseExpression(const string& expression, string::size_type& pos);

bool isLetter(const char c) {
    if((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) return true;
    return false;
}

bool isArithmeticOperator(const char c) {
    if(c == '+' || c == '-' || c == '*' || c == '/') return true;
    return false;
}

string parseIdentifier(const string& expression, string::size_type& pos) {
    if(pos == expression.size()) throw ExpressionParseException("Expected identifier here", expression.c_str(), pos);
    if(!isLetter(expression[pos]) && expression[pos] != '_')
        throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
    Node result;
    const string::size_type wordStart = pos;
    while(pos < expression.size()
          && (isLetter(expression[pos]) || isdigit(expression[pos]) || expression[pos] == '_')) pos++;
    return expression.substr(wordStart, pos - wordStart);
}

Node parseNextOnPath(const string& expression, string::size_type& pos) {
    if(pos == expression.size() || iswspace(expression[pos]) || isArithmeticOperator(expression[pos]) || expression[pos] == ']') {
        Node leaf;
        leaf.action = VARIABLE;
        return leaf;
    }
    if(expression[pos] == '.') { // access member from this identifier
        Node parent;
        parent.action = GET_MEMBER;
        const string identifier = parseIdentifier(expression, ++pos);
        Node child = parseNextOnPath(expression, pos);
        child.variable = identifier;
        parent.children.push_back(child);
        return parent;
    }
    if(expression[pos] == '[') {
        Node parent;
        parent.action = GET_SUBSCRIPT;
        Node middle;
        middle.subscript = parseExpression(expression, ++pos);
        pos++;
        if(expression[pos] != '.' && expression[pos] != '[') {
            middle.action = ONLY_SUBSCRIPT;
        } else {
            Node leaf = parseNextOnPath(expression, pos);

            middle.children = leaf.children;
            middle.action = leaf.action;
        }
        parent.children.push_back(middle);
        return parent;
    }
    throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
}

unique_ptr<Node> parseExpression(const string& expression, string::size_type& pos) {
    // a.b
    unique_ptr<Node> result;

    while(pos < expression.size()) {
        const char& c = expression[pos];
        if(iswspace(c)) continue;
        if(isLetter(c) || c == '_') {
            const string identifier = parseIdentifier(expression, pos);
            Node path = parseNextOnPath(expression, pos);
            path.variable = identifier;
            return make_unique<Node>(path);
        }
        else if(isdigit(c)) {
            auto* len = new size_t;
            const int numberLiteral = stoi(expression.substr(pos), len);
            pos += *len;
            Node number;
            number.literal = numberLiteral;
            number.action = NUMBER_LITERAL;
            return make_unique<Node>(number);
        }
        else throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
    }
    return result;
}

unique_ptr<Node> parseExpression(const std::string& expression) {
    string::size_type pos = 0;
    return parseExpression(expression, pos);
}