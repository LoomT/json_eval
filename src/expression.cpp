#include "expression.h"

#include <complex>

using namespace std;

bool isLetter(const char c) {
    if((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) return true;
    return false;
}

node parseExpression(const string& expression) {
    // a
    node result;
    string::size_type pos = 0;

    while(pos < expression.size()) {
        const char& c = expression[pos];
        if(isLetter(c) || c == '_') {
            const string::size_type wordStart = pos;
            while(pos < expression.size()
                && (isLetter(expression[pos]) || isdigit(expression[pos]) || expression[pos] == '_')) pos++;
            if(pos == expression.size() || iswspace(expression[pos])) {
                node leaf;
                leaf.variable = expression.substr(wordStart, pos - wordStart);
                leaf.action = VARIABLE;
                result = leaf;
            }
        }
        else throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
    }
    return result;
}