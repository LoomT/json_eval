#include "expression.h"

#include <complex>
#include <memory>
#include <stack>
#include <unordered_map>

using namespace std;

static unordered_map<string, NodeAction> funcMap =
    {{"max", MAX}, {"min", MIN}, {"size", SIZE}};

/**
 * Parses the string expression into a linked list
 * that can be more easily executed
 *
 * @param expression complete string expression
 * @param pos current position in the string
 * @return root node of the expression tree/linked list
 */
Node parseExpression(const string& expression, string::size_type& pos);

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
 *
 * @param expression complete string expression
 * @param pos current position in expression
 * @return position with whitespace skipped
 */
inline string::size_type skipWS(const string& expression, string::size_type pos) {
    while(iswspace(expression[pos])) pos++;
    return pos;
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
 * @param pos current position in expression at start of path. Moves the pos
 * @return rest of the path as node
 */
Node parseRestOfPath(const string& expression, string::size_type& pos) { // NOLINT(*-no-recursion)
    if(pos == expression.size() || iswspace(expression[pos]) || isArithmeticOperator(expression[pos])
        || expression[pos] == ']' || expression[pos] == ',' || expression[pos] == ')') {
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
        middle.subscript = make_shared<Node>(parseExpression(expression, ++pos));
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
 *
 * @param expression complete string expression
 * @param pos current position in expression, move the pos
 * @return input parameters of the function
 */
vector<Node> parseFunction(const string& expression, string::size_type& pos) { // NOLINT(*-no-recursion)
    vector<Node> result;
    pos = skipWS(expression, pos);
    while(pos < expression.size()) {
        result.emplace_back(parseExpression(expression, pos));
        pos = skipWS(expression, pos);
        if(pos == expression.size())
            throw ExpressionParseException("Missing closing bracket",
                    expression.c_str(), pos);
        if(expression[pos] == ',') {
            pos = skipWS(expression, ++pos);
            if(pos == expression.size())
                throw ExpressionParseException("Missing closing bracket",
                        expression.c_str(), pos);
            if(expression[pos] == ')')
                throw ExpressionParseException("Unexpected closing bracket after a comma",
                    expression.c_str(), pos);
        } else {
            if(expression[pos] != ')')
                throw ExpressionParseException("Missing closing bracket",
                    expression.c_str(), pos);
            pos++;
            return result;
        }
    }
    return result;
}

/**
 * Parses the string expression into a linked list
 * that can be more easily executed
 *
 * @param expression complete string expression
 * @param pos current position in the string. Moves the pos
 * @return root node of the expression tree/linked list
 */
Node parseExpression(const string& expression, string::size_type& pos) { // NOLINT(*-no-recursion)

    while(pos < expression.size() && expression[pos] != ')' && expression[pos] != ',') {
        const char& c = expression[pos];
        if(iswspace(c)) continue;
        if(isLetter(c) || c == '_' || c == '$') {
            // check if the identifier is actually a function
            if(const string identifier = parseIdentifier(expression, pos);
                funcMap.contains(identifier) && pos < expression.size() && expression[pos] == '(') {
                Node func;
                func.action = funcMap.at(identifier);
                func.children = parseFunction(expression, ++pos);
                return func;
            } else {
                Node path = parseRestOfPath(expression, pos);
                path.identifier = identifier;
                return path;
            }
        }
        else if(isdigit(c) || c == '-') {
            auto* len = new size_t;
            const int numberLiteral = stoi(expression.substr(pos), len);
            pos += *len;
            Node number;
            number.literal = numberLiteral;
            number.action = NUMBER_LITERAL;
            return number;
        }
        else throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
    }
    throw ExpressionParseException("Unexpected stuff", expression.c_str(), pos);
}

/**
 * Parses the string expression into a linked list
 * that can be more easily executed
 *
 * @param expression complete string expression
 * @return root node of the expression tree/linked list
 */
Node parseExpression(const std::string& expression) {
    string::size_type pos = 0;
    return parseExpression(expression, pos);
}

/**
 *
 * @param node node to print
 */
string toString(Node node) {
    stringstream ss;
    stack<pair<int, Node>> stack;
    stack.emplace(0, move(node));
    while(!stack.empty()) {
        auto [ident, n] = std::move(stack.top());
        stack.pop();
        for(int i = 0; i < ident; i++) {
            ss << "    ";
        }
        if(n.action == NUMBER_LITERAL) ss << n.literal;
        ss << n.identifier;
        if(n.subscript != nullptr) ss << '[' << n.subscript->literal << ']';
        ss << endl;
        for(Node& c : n.children) {
            stack.emplace(ident+1, Node(c));
        }
    }
    return ss.str();
}