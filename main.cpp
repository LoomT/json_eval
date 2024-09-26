#include <iostream>
#include "parse.h"

using namespace std; // only std allowed anyway

int main() {
    const unordered_map<string, ValueJSON> map = parseJSON("bigObject.json");
    cout << objectToString(map);
    return 0;
}
