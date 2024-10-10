#include <chrono>
#include <iostream>
#include <numeric>

#include "JSON.h"

using namespace std; // only std allowed anyway

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./json_eval <json file> <expression>\n"
                "Example: ./json_eval test.json \"a.b[1]\"" << endl;
        return -1;
    }
    constexpr int runs = 5000;
    auto times = array<double, runs>();
    for(int i = 0; i < runs; i++) {
        const auto start = chrono::steady_clock::now();
        const auto json = JSON(argv[1]);
        const string input = argv[2];

        toString(json.evaluate(input));
        const auto end = chrono::steady_clock::now();
        const chrono::duration<double, milli> diff = end - start;
        times[i] = diff.count();
    }
    sort(times.begin(), times.end());
    cout << "Number of runs: " << runs << endl;
    cout << "Fastest execution time: " << times[0] << " ms" << endl;
    cout << "Faster quartile execution time: " << times[runs/4] << " ms" << endl;
    cout << "Median execution time: " << times[runs/2] << " ms" << endl;
    cout << "Slower quartile execution time: " << times[runs - runs/4 - 1] << " ms" << endl;
    cout << "Slowest execution time: " << times[runs - 1] << " ms" << endl;
    cout << "Average execution time: " << accumulate(times.begin(), times.end(), 0.0) / runs << " ms" << endl;
    cout << "IQR: " << times[runs - runs/4 - 1] - times[runs/4] << " ms" << endl;

    return 0;
}