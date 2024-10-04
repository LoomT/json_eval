#include "execute.h"

#include <cfloat>

using namespace std;

/**
 *
 * @param JSON entire JSON object
 * @param expression expression to execute
 * @param currentObj current object function is in
 * @return evaluated expression on currentObj
 */
ValueJSON executeExpression(const unordered_map<string, ValueJSON> &JSON, const Node &expression,
                            const unordered_map<string, ValueJSON> &currentObj);

/**
 *
 * @param JSON entire JSON object
 * @param expression expression to execute
 * @return evaluated expression on JSON
 */
inline ValueJSON executeExpression(const unordered_map<string, ValueJSON>& JSON, const Node& expression) {
    return executeExpression(JSON, expression, JSON);
}

/**
 *
 * @param JSON entire JSON object
 * @param expression intermediary array node
 * @param array array of ValueJSONs to pick from
 * @return evaluated subscript expression
 */
ValueJSON getItemFromArray(const unordered_map<string, ValueJSON> &JSON, const Node &expression, // NOLINT(*-no-recursion)
                           const vector<ValueJSON> &array) {

    const ValueJSON sub = executeExpression(JSON, *expression.subscript);
    if(sub.type != NUMBER) throw executeException("Subscript should be a number", expression);
    const int index = static_cast<int>(get<double>(sub.value));
    if(index > array.size()) throw range_error("Index out of range");
    ValueJSON arrayItem = array.at(index);
    if(expression.action == ONLY_SUBSCRIPT) return arrayItem;
    const auto next = expression.children.at(0);
    if(expression.action == GET_MEMBER) {
        if(arrayItem.type != OBJECT) throw executeException("Variable should be an object", expression);
        const unordered_map<string, ValueJSON> obj = get<unordered_map<string, ValueJSON>>(arrayItem.value);
        return executeExpression(JSON, next, obj);
    }
    if(expression.action == GET_SUBSCRIPT) {
        if(arrayItem.type != ARRAY) throw executeException("Variable should be an array", expression);
        return getItemFromArray(JSON, next, get<vector<ValueJSON>>(arrayItem.value));
    }
    throw runtime_error("Unexpected action");
}


/**
 *
 * @param JSON entire JSON object
 * @param expression max function node
 * @return evaluated max function on JSON
 */
ValueJSON getMax(const unordered_map<string, ValueJSON> &JSON, const Node &expression) { // NOLINT(*-no-recursion)
    vector<ValueJSON> arguments;
    for(const Node& child : expression.children) {
        arguments.push_back(executeExpression(JSON, child));
    }
    if(arguments.size() == 1 && arguments.at(0).type == ARRAY) {
        const vector<ValueJSON> arrayValues = get<vector<ValueJSON>>(arguments.at(0).value);
        if(arrayValues.empty()) throw executeException("Array should not be empty in max function", expression);
        double result = -DBL_MAX;
        for(const ValueJSON& child : arrayValues) {
            if(child.type != NUMBER) throw executeException("Array should only contain numbers in max function", expression);
            result = max(result, get<double>(child.value));
        }
        return {NUMBER, result};
    } else {
        double result = -DBL_MAX;
        for(const ValueJSON& child : arguments) {
            if(child.type != NUMBER) throw executeException("Arguments should only be numbers in max function", expression);
            result = max(result, get<double>(child.value));
        }
        return {NUMBER, result};
    }
}

/**
 *
 * @param JSON entire JSON object
 * @param expression min function node
 * @return evaluated min function on JSON
 */
ValueJSON getMin(const unordered_map<string, ValueJSON> &JSON, const Node &expression) { // NOLINT(*-no-recursion)
    vector<ValueJSON> arguments;
    for(const Node& child : expression.children) {
        arguments.push_back(executeExpression(JSON, child));
    }
    if(arguments.size() == 1 && arguments.at(0).type == ARRAY) {
        const vector<ValueJSON> arrayValues = get<vector<ValueJSON>>(arguments.at(0).value);
        if(arrayValues.empty()) throw executeException("Array should not be empty in min function", expression);
        double result = DBL_MAX;
        for(const ValueJSON& child : arrayValues) {
            if(child.type != NUMBER) throw executeException("Array should only contain numbers in min function", expression);
            result = min(result, get<double>(child.value));
        }
        return {NUMBER, result};
    } else {
        double result = DBL_MAX;
        for(const ValueJSON& child : arguments) {
            if(child.type != NUMBER) throw executeException("Arguments should only be numbers in min function", expression);
            result = min(result, get<double>(child.value));
        }
        return {NUMBER, result};
    }
}

/**
 *
 * @param JSON entire JSON object
 * @param expression size function node
 * @return evaluated size function on JSON
 */
ValueJSON getSize(const unordered_map<string, ValueJSON> &JSON, const Node &expression) { // NOLINT(*-no-recursion)
    vector<ValueJSON> arguments;
    for(const Node& child : expression.children) {
        arguments.push_back(executeExpression(JSON, child));
    }
    if(arguments.size() != 1) throw executeException("Size function can only have one argument", expression);
    switch(const ValueJSON& argument = arguments.at(0); argument.type) {
        case STRING: return {NUMBER, static_cast<double>(get<string>(argument.value).size())};
        case ARRAY: return {NUMBER, static_cast<double>(get<vector<ValueJSON>>(argument.value).size())};
        case OBJECT: return {NUMBER, static_cast<double>(get<unordered_map<string, ValueJSON>>(argument.value).size())};
        default: throw executeException("Wrong type for size function", expression);
    }
}

/**
 *
 * @param JSON entire JSON object
 * @param expression expression to execute
 * @param currentObj current object function is in
 * @return evaluated expression on currentObj
 */
ValueJSON executeExpression(const unordered_map<string, ValueJSON>& JSON, const Node& expression, // NOLINT(*-no-recursion)
    const unordered_map<string, ValueJSON>& currentObj) {
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
            if(currentObj.at(variable).type != OBJECT) throw executeException("Variable should be an object", expression);
            const unordered_map<string, ValueJSON> obj = get<unordered_map<string, ValueJSON>>(currentObj.at(variable).value);
            const auto next = expression.children.at(0);
            return executeExpression(JSON, next, obj);
        }
        case GET_SUBSCRIPT: {
            const string& variable = expression.identifier;
            if(currentObj.at(variable).type != ARRAY) throw executeException("Variable should be an array", expression);
            const vector<ValueJSON> array = get<vector<ValueJSON>>(currentObj.at(variable).value);
            const Node next = expression.children.at(0);

            return getItemFromArray(JSON, next, array);
        }
        case ONLY_SUBSCRIPT:
            throw executeException("Subscript should not be here", expression);
        case MAX: {
            return getMax(JSON, expression);
        }
        case MIN: {
            return getMin(JSON, expression);
        }
        case SIZE: {
            return getSize(JSON, expression);
        }
    }
    throw executeException("Switch case leaked!", expression);
}
