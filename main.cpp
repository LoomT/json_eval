#include <iostream>

#include "parse.h"

using namespace std; // only std allowed anyway

int main() {
    unordered_map<string, Value> map = parseJSON("yep.txt");
    cout << map.size() << endl;
    for(auto &[key, value] : map) {
        cout << key << " " << value.type << endl;
    }
    return 0;
}
