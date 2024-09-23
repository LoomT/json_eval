#include "parse.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

string openFile(const string& filePath) {
    ifstream in(filePath);
    if(!in.good()) {
        throw runtime_error{"Could not open " + filePath};
    }
    in.seekg(0, ios::end);
    long long size = in.tellg();
    string buffer(size, ' ');
    in.seekg(0);
    in.read(&buffer[0], size);
    return buffer;
}

void hello() {
    cout << "Hello world!" << endl << openFile("yep.txt") << endl;
}