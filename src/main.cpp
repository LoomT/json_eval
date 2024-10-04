#include <iostream>
#include <stack>

#include "execute.h"
#include "JSON.h"
#include "expression.h"

using namespace std; // only std allowed anyway

void printParsedExpression(unique_ptr<Node> node);

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./json_eval <json file> <expression>\n"
                "Example: ./json_eval test.json \"a.b[1]\"" << endl;
        return -1;
    }
    const unordered_map<string, ValueJSON> map = parseJSON(argv[1]);
    const string input = argv[2];

    const unique_ptr<Node> result = parseExpression(input);
    cout << toString(executeExpression(map, *result)) << endl;

    return 0;
}