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
    double totalTime = 0;
    constexpr int runs = 25;
    for(int i = 0; i < runs; i++) {
        const auto start = chrono::steady_clock::now();
        const auto json = JSON(argv[1]);
        const string input = argv[2];

        cout << toString(json.evaluate(input)) << endl;
        const auto end = chrono::steady_clock::now();
        const chrono::duration<double, milli> diff = end - start;
        totalTime += diff.count();
    }
    cout << "average execution time of " << runs << " runs: " << totalTime / runs << " ms" << endl;

    return 0;
}