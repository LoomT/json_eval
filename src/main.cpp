#include <iostream>

#include "JSON.h"

using namespace std; // only std allowed anyway

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./json_eval <json_file> <expression>\n"
                "Example: ./json_eval test.json \"a.b[1]\"" << endl;
        return -1;
    }
    const auto json = JSON(argv[1]);
    const string input = argv[2];

    cout << toString(json.evaluate(input));

    return 0;
}