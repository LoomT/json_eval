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
unique_ptr<Node> parseExpression(const string& expression, string::size_type& pos);

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
    throw invalid_argument("Not an operator");
}

inline int operatorPrecedence(const NodeAction& c) {
    if(c == ADD || c == SUBTRACT) return 1;
    if(c == MULTIPLY || c == DIVIDE) return 2;
    if(c == RAISE) return 3;
    throw invalid_argument("Not an operator");
}

inline char operatorAssociativity(const NodeAction& c) {
    if(c == ADD || c == SUBTRACT || c == MULTIPLY || c == DIVIDE) return 'L';
    if(c == RAISE) return 'R';
    throw invalid_argument("Not an operator");
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
unique_ptr<Node> parseRestOfPath(const string& expression, string::size_type& pos) { // NOLINT(*-no-recursion)
    if(pos == expression.size() || isArithmeticOperator(expression[pos])
        || expression[pos] == ']' || expression[pos] == ',' || expression[pos] == ')') {
        auto leaf = make_unique<Node>(IDENTIFIER);
        return leaf;
    }
    if(expression[pos] == '.') { // access member from this identifier
        auto parent = make_unique<Node>(GET_MEMBER);
        const string identifier = parseIdentifier(expression, ++pos);
        const unique_ptr<Node> child = parseRestOfPath(expression, pos);
        child->value = identifier;
        parent->children.push_back(move(*child));
        return parent;
    }
    if(expression[pos] == '[') {
        auto parent = make_unique<Node>(GET_SUBSCRIPT);
        auto middle = Node(ONLY_SUBSCRIPT);
        middle.subscript = parseExpression(expression, ++pos);
        pos++;
        if(expression[pos] == '.' || expression[pos] == '[') {
            const unique_ptr<Node> leaf = parseRestOfPath(expression, pos);
            middle.children = move(leaf->children);
            middle.action = leaf->action;
        }
        parent->children.push_back(move(middle));
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
    while(pos < expression.size()) {
        result.push_back(move(*parseExpression(expression, pos)));
        if(pos == expression.size())
            throw ExpressionParseException("Missing closing bracket",
                    expression.c_str(), pos);
        if(expression[pos] == ',') {
            pos++;
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
unique_ptr<Node> parseOperand(const string& expression, string::size_type& pos) { // NOLINT(*-no-recursion)

    while(pos < expression.size() && expression[pos] != ')' && expression[pos] != ',') {
        const char& c = expression[pos];
        if(isalpha(c) || c == '_' || c == '$') {
            // check if the identifier is actually a function
            if(const string identifier = parseIdentifier(expression, pos);
                funcMap.contains(identifier) && pos < expression.size() && expression[pos] == '(') {
                auto func = make_unique<Node>(funcMap.at(identifier));
                func->children = parseFunction(expression, ++pos);
                return func;
            } else {
                unique_ptr<Node> path = parseRestOfPath(expression, pos);
                path->value = identifier;
                return path;
            }
        }
        if(isdigit(c) || c == '-') {
            size_t intPos;
            size_t floatPos;
            const string numberToParse = expression.substr(pos);
            const long long intNumber = stoll(numberToParse, &intPos);
            const double floatNumber = stod(numberToParse, &floatPos);
            pos += floatPos; // number as float cannot be smaller than same number as integer
            if(intPos == floatPos) {
                auto number = make_unique<Node>(INT_LITERAL);
                number->value = intNumber;
                return number;
            } else {
                auto number = make_unique<Node>(FLOAT_LITERAL);
                number->value = floatNumber;
                return number;
            }
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
unique_ptr<Node> parseExpression(const string& expression, string::size_type& pos) {
    // "a.b[0] + a.b[1] * a.b[a.b[0] + a.b[1]][0] / 2^2"
    vector<unique_ptr<Node>> parsedExpression;

    int depth = 0;
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
            if(c == '-' && (parsedExpression.empty() || isArithmeticOperator(parsedExpression.back()->action)
                && parsedExpression.back()->children.empty())) {
                parsedExpression.push_back(parseOperand(expression, pos));
            } else {
                parsedExpression.emplace_back(make_unique<Node>(operatorMap.at(c)));
                pos++;
            }
        } else {
            parsedExpression.push_back(parseOperand(expression, pos));
        }
    }
    if(depth != 0) throw ExpressionParseException("Missing closing brackets", expression.c_str(), pos);
    if(parsedExpression.size() == 1 && !isArithmeticOperator(parsedExpression.front()->action))
        return std::move(parsedExpression.front());

    stack<unique_ptr<Node>> operators;
    stack<unique_ptr<Node>> operands;

    for(unique_ptr<Node>& node : parsedExpression) {
        if(!isArithmeticOperator(node->action) || !node->children.empty()) operands.push(move(node));
        else {
            while(!operators.empty() && operatorPrecedence(node->action) <= operatorPrecedence(operators.top()->action)
            && operatorAssociativity(node->action) == 'L') {
                if(operands.size() < 2) throw ExpressionParseException("Too many operators", expression.c_str(), pos);
                unique_ptr<Node> operand = std::move(operators.top());
                operators.pop();
                unique_ptr<Node> b = std::move(operands.top());
                operands.pop();
                unique_ptr<Node> a = std::move(operands.top());
                operands.pop();
                operand->children.push_back(move(*a));
                operand->children.push_back(move(*b));
                operands.push(move(operand));
            }
            operators.push(move(node));
        }
    }

    while(!operators.empty()) {
        if(operands.size() < 2) throw ExpressionParseException("Too many operators", expression.c_str(), pos);
        unique_ptr<Node> operand = std::move(operators.top());
        operators.pop();
        unique_ptr<Node> b = std::move(operands.top());
        operands.pop();
        unique_ptr<Node> a = std::move(operands.top());
        operands.pop();
        operand->children.push_back(move(*a));
        operand->children.push_back(move(*b));
        operands.push(move(operand));
    }

    if(operands.size() == 1) return std::move(operands.top());
    throw ExpressionParseException("Too many operands", expression.c_str(), pos);
}

/**
 * Parses the string expression into a linked list
 * that can be more easily executed
 *
 * @param expression complete string expression
 * @return root node of the expression tree/linked list
 */
Node parseExpression(string expression) {
    string::size_type pos = 0;
    erase_if(expression, [](const unsigned char c){return iswspace(c);});
    Node result = std::move(*parseExpression(expression, pos));
    if(pos < expression.size()) throw ExpressionParseException("Unexpected character", expression.c_str(), pos);
    return result;
}

/**
 * FOR DEBUGGING
 *
 * @param node node to print
 */
string toString(Node node) { // TODO subscript, other stuff
    stringstream ss;
    stack<pair<int, Node>> stack;
    stack.emplace(0, move(node));
    while(!stack.empty()) {
        auto [ident, n] = std::move(stack.top());
        stack.pop();
        for(int i = 0; i < ident; i++) {
            ss << "    ";
        }
        if(n.action == INT_LITERAL) ss << get<long long>(n.value);
        else if(n.action == FLOAT_LITERAL) ss << get<double>(n.value);
        else ss << get<string>(n.value);
        if(n.subscript != nullptr) ss << '[' << ']';
        ss << endl;
        for(Node& c : n.children) {
            // stack.emplace(ident+1, Node(c));
        }
    }
    return ss.str();
}