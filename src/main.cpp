#include <chrono>
#include <iostream>

#include "parseJSON.h"
#include "expression.h"
#include "JSON.h"

using namespace std; // only std allowed anyway

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./json_eval <json file> <expression>\n"
                "Example: ./json_eval test.json \"a.b[1]\"" << endl;
        return -1;
    }
    const chrono::time_point<chrono::high_resolution_clock> start = chrono::high_resolution_clock::now();
    const JSON json = JSON(argv[1]);
    const string input = argv[2];

    cout << toString(json.evaluate(input)) << endl;
    const chrono::time_point<chrono::high_resolution_clock> end = chrono::high_resolution_clock::now();
    const chrono::duration<double, milli> diff = end - start;
    cout << diff.count() << " ms" << endl;
    return 0;
}