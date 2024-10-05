#include <iostream>

#include "parseJSON.h"
#include "expression.h"
#include "JSON.h"

using namespace std; // only std allowed anyway

void printParsedExpression(unique_ptr<Node> node);

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./json_eval <json file> <expression>\n"
                "Example: ./json_eval test.json \"a.b[1]\"" << endl;
        return -1;
    }
    const JSON json = JSON(argv[1]);
    const string input = argv[2];

    cout << toString(json.evaluate(input)) << endl;

    return 0;
}