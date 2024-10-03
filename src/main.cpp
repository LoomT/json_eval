#include <iostream>
#include <stack>

#include "execute.h"
#include "JSON.h"
#include "expression.h"

using namespace std; // only std allowed anyway

void printParsedExpression(unique_ptr<Node> node);

int main(int argc, char* argv[]) {
    const unordered_map<string, ValueJSON> map = parseJSON("yep.json");
    const string input = "a[f][0]";
    const unique_ptr<Node> result = parseExpression(input);
    cout << toString(executeExpression(map, *result)) << endl;

    return 0;
}

void printParsedExpression(unique_ptr<Node> node) {
    stack<pair<int, unique_ptr<Node>>> stack;
    stack.emplace(0, move(node));
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
}