#include "expression.h"

#include <complex>
#include <memory>

using namespace std;

/**
 * Parses the string expression into a linked list
 * that can be more easily executed
 *
 * @param expression complete string expression
 * @param pos current position in the string
 * @return root node of the expression tree/linked list
 */
unique_ptr<Node> parseExpression(const string& expression, string::size_type& pos);

/**
 *
 * @param c character to check
 * @return true iff c is a letter
 */
inline bool isLetter(const char c) {
    if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return true;
    return false;
}

/**
 *
 * @param c character to check
 * @return true iff it's +, -, * or /
 */
inline bool isArithmeticOperator(const char c) {
    if(c == '+' || c == '-' || c == '*' || c == '/') return true;
    return false;
}

/**
 * Extracts the identifier, moves the pos to end of valid identifier.
 * If id is invalid the pos will be at an invalid position
 *
 * @param expression complete string expression
 * @param pos current position in expression, should be start of identifier
 * @return extracted valid identifier
 */
string parseIdentifier(const string& expression, string::size_type& pos) {
    if(pos == expression.size())
        throw ExpressionParseException("Expected identifier here", expression.c_str(), pos);
    if(!isLetter(expression[pos]) && expression[pos] != '_' && expression[pos] != '$')
        throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
    const string::size_type wordStart = pos;
    while(pos < expression.size()
          && (isLetter(expression[pos]) || isdigit(expression[pos])
              || expression[pos] == '_' || expression[pos] == '$')) pos++;
    return expression.substr(wordStart, pos - wordStart);
}

/**
 *
 * @param expression complete string expression
 * @param pos current position in expression at start of path
 * @return rest of the path as node
 */
Node parseRestOfPath(const string& expression, string::size_type& pos) { // NOLINT(*-no-recursion)
    if(pos == expression.size() || iswspace(expression[pos]) || isArithmeticOperator(expression[pos]) || expression[pos] == ']') {
        Node leaf;
        leaf.action = IDENTIFIER;
        return leaf;
    }
    if(expression[pos] == '.') { // access member from this identifier
        Node parent;
        parent.action = GET_MEMBER;
        const string identifier = parseIdentifier(expression, ++pos);
        Node child = parseRestOfPath(expression, pos);
        child.identifier = identifier;
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
            Node leaf = parseRestOfPath(expression, pos);

            middle.children = leaf.children;
            middle.action = leaf.action;
        }
        parent.children.push_back(middle);
        return parent;
    }
    throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
}

/**
 * Parses the string expression into a linked list
 * that can be more easily executed
 *
 * @param expression complete string expression
 * @param pos current position in the string
 * @return root node of the expression tree/linked list
 */
unique_ptr<Node> parseExpression(const string& expression, string::size_type& pos) {
    unique_ptr<Node> result;

    while(pos < expression.size()) {
        const char& c = expression[pos];
        if(iswspace(c)) continue;
        if(isLetter(c) || c == '_' || c == '$') {
            const string identifier = parseIdentifier(expression, pos);
            Node path = parseRestOfPath(expression, pos);
            path.identifier = identifier;
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

/**
 * Parses the string expression into a linked list
 * that can be more easily executed
 *
 * @param expression complete string expression
 * @return root node of the expression tree/linked list
 */
unique_ptr<Node> parseExpression(const std::string& expression) {
    string::size_type pos = 0;
    return parseExpression(expression, pos);
}