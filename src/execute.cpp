#include "execute.h"

using namespace std;

ValueJSON executeExpression(const unordered_map<string, ValueJSON>& JSON, const Node& expression, const unordered_map<string, ValueJSON>& currentObj);
inline ValueJSON executeExpression(const unordered_map<string, ValueJSON>& JSON, const Node& expression) {
    return executeExpression(JSON, expression, JSON);
}

ValueJSON getItemFromArray(const unordered_map<string, ValueJSON>& JSON, const Node& expression, const vector<ValueJSON>& array) {

    const ValueJSON sub = executeExpression(JSON, *expression.subscript);
    if(sub.type != NUMBER) throw runtime_error("Subscript is not a number");
    const int index = static_cast<int>(get<double>(sub.value));
    if(index > array.size()) throw runtime_error("Index out of range");
    ValueJSON arrayItem = array.at(index);
    if(expression.action == ONLY_SUBSCRIPT) return arrayItem;
    const auto next = expression.children.at(0);
    if(expression.action == GET_MEMBER) {
        if(arrayItem.type != OBJECT) throw runtime_error("Variable is not an object");
        const unordered_map<string, ValueJSON> obj = get<unordered_map<string, ValueJSON>>(arrayItem.value);
        return executeExpression(JSON, next, obj);
    }
    if(expression.action == GET_SUBSCRIPT) {
        if(arrayItem.type != ARRAY) throw runtime_error("Variable is not an array");
        return getItemFromArray(JSON, next, get<vector<ValueJSON>>(arrayItem.value));
    }
    throw runtime_error("Unexpected action");
}

ValueJSON executeExpression(const unordered_map<string, ValueJSON>& JSON, const Node& expression, const unordered_map<string, ValueJSON>& currentObj) {
    switch (expression.action) {
        case IDENTIFIER: {
            const string& variable = expression.identifier;
            return currentObj.at(variable);
        }
        case NUMBER_LITERAL: { //TODO add ints to ValueJSON
            return {NUMBER, static_cast<double>(expression.literal)};
        }
        case GET_MEMBER: {
            const string& variable = expression.identifier;
            if(currentObj.at(variable).type != OBJECT) throw runtime_error("Variable is not an object");
            const unordered_map<string, ValueJSON> obj = get<unordered_map<string, ValueJSON>>(currentObj.at(variable).value);
            const auto next = expression.children.at(0);
            return executeExpression(JSON, next, obj);
        }
        case GET_SUBSCRIPT: {
            const string& variable = expression.identifier;
            if(currentObj.at(variable).type != ARRAY) throw runtime_error("Variable is not an array");
            const vector<ValueJSON> array = get<vector<ValueJSON>>(currentObj.at(variable).value);
            const Node next = expression.children.at(0);

            return getItemFromArray(JSON, next, array);
        }
        case ONLY_SUBSCRIPT:
            break;
        case MAX:
            break;
        case MIN:
            break;
        case SIZE:
            break;
    }
    throw runtime_error("No action");
}