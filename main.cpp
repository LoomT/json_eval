#include <iostream>
#include <variant>

#include "parse.h"

using namespace std; // only std allowed anyway

int main() {
    unordered_map<string, ValueJSON> map = parseJSON("yep.txt");
    cout << map.size() << endl;
    for(auto &[key, value] : map) {
        cout << key << ": ";
        switch(value.type) {
            case typeNULL: cout << "null" << endl; break;
            case STRING: cout << "\"" << get<string>(value.value) << "\"" << endl; break;
            case NUMBER: cout << get<double>(value.value) << endl; break;
            case OBJECT:
                cout << "object of size " << get<unordered_map<string, ValueJSON>>(value.value).size() << endl;
                break;
            case ARRAY: cout << "array of size " << get<vector<ValueJSON>>(value.value).size() << endl; break;
            case BOOL: cout << get<bool>(value.value) << endl; break;
        }
    }
    return 0;
}
