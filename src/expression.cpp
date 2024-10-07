#include "expression.h"

#include <complex>
#include <memory>
#include <queue>
#include <stack>
#include <unordered_map>

using namespace std;

const static unordered_map<string, NodeAction> funcMap =
    {{"max", MAX}, {"min", MIN}, {"size", SIZE}};

const static unordered_map<char, NodeAction> operatorMap =
    {{'+', ADD}, {'-', SUBTRACT}, {'*', MULTIPLY}, {'/', DIVIDE}, {'^', RAISE}};

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
 * @return true iff it's +, -, * or /
 */
inline bool isArithmeticOperator(const char& c) {
    if(c == '+' || c == '-' || c == '*' || c == '/' || c == '^') return true;
    return false;
}

inline bool isArithmeticOperator(const NodeAction& c) {
    if(c == ADD || c == SUBTRACT || c == MULTIPLY || c == DIVIDE || c == RAISE) return true;
    return false;
}

inline int operatorPrecedence(const char& c) {
    if(c == '+' || c == '-') return 1;
    if(c == '*' || c == '/') return 2;
    if(c == '^') return 3;
    throw invalid_argument("operatorPrecedence error");
}

inline int operatorPrecedence(const NodeAction& c) {
    if(c == ADD || c == SUBTRACT) return 1;
    if(c == MULTIPLY || c == DIVIDE) return 2;
    if(c == RAISE) return 3;
    throw invalid_argument("operatorPrecedence error");
}

inline char operatorAssociativity(const NodeAction& c) {
    if(c == ADD || c == SUBTRACT || c == MULTIPLY || c == DIVIDE) return 'L';
    if(c == RAISE) return 'R';
    throw invalid_argument("operatorPrecedence error");
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
    if(!isalpha(expression[pos]) && expression[pos] != '_' && expression[pos] != '$')
        throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
    const string::size_type wordStart = pos;
    while(pos < expression.size()
          && (isalpha(expression[pos]) || isdigit(expression[pos])
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
 * Parses JSON path, number literal or function into a linked list
 * that can be more easily executed
 *
 * @param expression complete string expression
 * @param pos current position in the string. Moves the pos
 * @return root node of the operand (JSON path, number literal, function) tree/linked list
 */
Node parseOperand(const string& expression, string::size_type& pos) { // NOLINT(*-no-recursion)

    while(pos < expression.size() && expression[pos] != ')' && expression[pos] != ',') {
        const char& c = expression[pos];
        if(iswspace(c)) continue;
        if(isalpha(c) || c == '_' || c == '$') {
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
        if(isdigit(c) || c == '-') {
            size_t len;
            const int numberLiteral = stoi(expression.substr(pos), &len);
            pos += len;
            Node number;
            number.literal = numberLiteral;
            number.action = NUMBER_LITERAL;
            return number;
        }
        throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
    }
    throw ExpressionParseException("Unexpected stuff", expression.c_str(), pos);
}

/**
 * Parses the whole expression into a linked list
 * that can be more easily executed
 *
 * @param expression complete string expression
 * @param pos current position in the string. Moves the pos
 * @return root node of the expression tree/linked list
 */
Node parseExpression(const string& expression, string::size_type& pos) {
    // "a.b[0] + a.b[1] * a.b[a.b[0] + a.b[1]][0] / 2^2"
    deque<Node> parsedExpression;

    int depth = 0;
    pos = skipWS(expression, pos);
    while(pos < expression.size() && expression[pos] != ']' && expression[pos] != ','
        && (expression[pos] != ')' || depth > 0)) {
        const char& c = expression[pos];
        if(c == '(') {
            depth++;
            parsedExpression.push_back(parseExpression(expression, ++pos));
        } else if(c == ')') {
            if(depth == 0) throw ExpressionParseException("Unexpected closing bracket", expression.c_str(), pos);
            depth--;
            pos++;
        } else if(isArithmeticOperator(c)) {
            if(c == '-' && (parsedExpression.empty() || isArithmeticOperator(parsedExpression.back().action))) {
                parsedExpression.push_back(parseOperand(expression, pos));
            } else {
                parsedExpression.emplace_back(operatorMap.at(c));
                pos++;
            }
        } else {
            parsedExpression.push_back(parseOperand(expression, pos));
        }
        pos = skipWS(expression, pos);
    }
    if(depth != 0) throw ExpressionParseException("Missing closing brackets", expression.c_str(), pos);
    if(parsedExpression.size() == 1 && !isArithmeticOperator(parsedExpression.front().action))
        return parsedExpression.front();

    stack<Node> operators;
    stack<Node> operands;

    for(Node& node : parsedExpression) {
        if(!isArithmeticOperator(node.action)) operands.push(node);
        else {
            while(!operators.empty() && operatorPrecedence(node.action) <= operatorPrecedence(operators.top().action)
            && operatorAssociativity(node.action) == 'L') {
                if(operands.size() < 2) throw ExpressionParseException("Too many operators", expression.c_str(), pos);
                Node operand = operators.top();
                operators.pop();
                Node b = operands.top();
                operands.pop();
                Node a = operands.top();
                operands.pop();
                operand.children.push_back(a);
                operand.children.push_back(b);
                operands.push(operand);
            }
            operators.push(node);
        }
    }

    while(!operators.empty()) {
        if(operands.size() < 2) throw ExpressionParseException("Too many operators", expression.c_str(), pos);
        Node operand = operators.top();
        operators.pop();
        Node b = operands.top();
        operands.pop();
        Node a = operands.top();
        operands.pop();
        operand.children.push_back(a);
        operand.children.push_back(b);
        operands.push(operand);
    }

    if(operands.size() == 1) return operands.top();
    throw ExpressionParseException("Too many operands", expression.c_str(), pos);
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