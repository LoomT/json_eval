#include <iostream>
#include <stack>

#include "JSON.h"
#include "expression.h"

using namespace std; // only std allowed anyway

int main() {
    // const unordered_map<string, ValueJSON> map = parseJSON("bigNoArrays.json");
    // cout << objectToString(map);
    const string input = "a.b[12][c.d]";
    unique_ptr<Node> result = parseExpression(input);
    stack<pair<int, unique_ptr<Node>>> stack;
    stack.emplace(0, move(result));
    while(!stack.empty()) {
        auto [ident, n] = std::move(stack.top());
        stack.pop();
        for(int i = 0; i < ident; i++) {
            cout << "    ";
        }
        if(n->action == NUMBER_LITERAL) cout << n->literal;
        cout << n->variable;
        if(n->subscript != nullptr) cout << '[' << n->subscript->literal << ']';
        cout << endl;
        for(Node& c : n->children) {
            stack.emplace(ident+1, make_unique<Node>(c));
        }
    }
    return 0;
}
