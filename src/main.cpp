#include <iostream>
#include <stack>

#include "parse.h"
#include "expression.h"

using namespace std; // only std allowed anyway

int main() {
    // const unordered_map<string, ValueJSON> map = parseJSON("bigNoArrays.json");
    // cout << objectToString(map);
    const string input = "a.b[12].c";
    const node result = parseExpression(input);
    stack<pair<int, node>> stack;
    stack.emplace(0, result);
    while(!stack.empty()) {
        auto [ident, n] = stack.top();
        stack.pop();
        for(int i = 0; i < ident; i++) {
            cout << "    ";
        }
        if(n.action == NUMBER_LITERAL) cout << n.literal;
        cout << n.variable << " " << n.action << endl;
        for(const node& c : n.children) {
            stack.emplace(ident+1, c);
        }
    }
    return 0;
}
