#include "execute.h"

#include <cfloat>
#include <cmath>

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
inline ValueJSON executeExpression(const unordered_map<string, ValueJSON>& JSON, const Node& expression) { // NOLINT(*-no-recursion)
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

    ValueJSON sub;
    try {
        sub = executeExpression(JSON, *expression.subscript);
    } catch (pathException& e) {
        e.appendPathFront("[");
        e.appendPathBack("]");
        throw;
    }

    if(sub.type != INT) throw pathException("Subscript should be an integer", "");
    const long long index = get<long long>(sub.value);
    if(index < 0 || index > array.size())
        throw pathException("Index was out of bounds for array of size " + to_string(array.size()), '[' + to_string(index) + ']');
    ValueJSON arrayItem = array.at(index);
    if(expression.action == ONLY_SUBSCRIPT) return arrayItem;
    const auto next = expression.children.at(0);
    if(expression.action == GET_MEMBER) {
        if(arrayItem.type != OBJECT) throw pathException("This path should be an object", '[' + to_string(index) + ']');
        const unordered_map<string, ValueJSON> obj = get<unordered_map<string, ValueJSON>>(arrayItem.value);
        try {
            return executeExpression(JSON, next, obj);
        } catch (pathException& e) {
            e.appendPathFront('[' + to_string(index) + ']');
            throw;
        }
    }
    if(expression.action == GET_SUBSCRIPT) {
        if(arrayItem.type != ARRAY) throw pathException("This path should be an array", '[' + to_string(index) + ']');
        try {
            return getItemFromArray(JSON, next, get<vector<ValueJSON>>(arrayItem.value));
        } catch (pathException& e) {
            e.appendPathFront('[' + to_string(index) + ']');
            throw;
        }
    }
    throw executeException("Unexpected action");
}

inline double extractDouble(const ValueJSON& number) {
    if(number.type == INT)
        return static_cast<double>(get<long long>(number.value));
    else if(number.type == FLOAT)
        return get<double>(number.value);
    throw executeException("Unexpected type in extractDouble");
}

/**
 * Returns maximum value, integer if all arguments are also integers, floating point number otherwise
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
    vector<ValueJSON> values;
    string exceptionMessage = "Arguments should only be numbers in max function";
    if(arguments.size() == 1 && arguments.at(0).type == ARRAY) {
        values = get<vector<ValueJSON>>(arguments.at(0).value);
        if(values.empty()) throw executeException("Array should not be empty in max function");
        exceptionMessage = "Array should only contain numbers in max function";

    } else {
        values = arguments;
        exceptionMessage = "Arguments should only be numbers in max function";
    }

    bool onlyIntegers = true;
    for(const ValueJSON& child : values) {
        if(child.type != INT && child.type != FLOAT) throw executeException(exceptionMessage);
        if(child.type != INT) onlyIntegers = false;
    }
    if(onlyIntegers) {
        long long result = INT_MIN;
        for(const ValueJSON& child : values) {
            result = max(result, get<long long>(child.value));
        }
        return {INT, result};
    } else {
        double result = -DBL_MAX;
        for(const ValueJSON& child : values) {
            result = max(result, extractDouble(child));
        }
        return {FLOAT, result};
    }
}

/**
 * Returns minimum value, integer if all arguments are also integers, floating point number otherwise
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
    vector<ValueJSON> values;
    string exceptionMessage = "Arguments should only be numbers in min function";
    if(arguments.size() == 1 && arguments.at(0).type == ARRAY) {
        values = get<vector<ValueJSON>>(arguments.at(0).value);
        if(values.empty()) throw executeException("Array should not be empty in min function");
        exceptionMessage = "Array should only contain numbers in min function";

    } else {
        values = arguments;
        exceptionMessage = "Arguments should only be numbers in min function";
    }

    bool onlyIntegers = true;
    for(const ValueJSON& child : values) {
        if(child.type != INT && child.type != FLOAT) throw executeException(exceptionMessage);
        if(child.type != INT) onlyIntegers = false;
    }
    if(onlyIntegers) {
        long long result = INT_MAX;
        for(const ValueJSON& child : values) {
            result = min(result, get<long long>(child.value));
        }
        return {INT, result};
    } else {
        double result = DBL_MAX;
        for(const ValueJSON& child : values) {
            result = min(result, extractDouble(child));
        }
        return {FLOAT, result};
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
    if(arguments.size() != 1) throw executeException("Size function can only have one argument");
    switch(const ValueJSON& argument = arguments.at(0); argument.type) {
        case STRING: return {INT, static_cast<long long>(get<string>(argument.value).size())};
        case ARRAY: return {INT, static_cast<long long>(get<vector<ValueJSON>>(argument.value).size())};
        case OBJECT: return {INT, static_cast<long long>(get<unordered_map<string, ValueJSON>>(argument.value).size())};
        default: throw executeException("Wrong type for size function");
    }
}

pair<ValueJSON, ValueJSON> getOperands(const unordered_map<string, ValueJSON>& JSON, const Node& expression) {
    if(expression.children.size() != 2) throw executeException("Wrong number of operands for a binary operator");
    ValueJSON a = executeExpression(JSON, expression.children.at(0));
    ValueJSON b = executeExpression(JSON, expression.children.at(1));
    return {a, b};
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
            const string& identifier = expression.identifier;
            if(!currentObj.contains(identifier)) throw pathException("No such key in JSON", identifier);
            return currentObj.at(identifier);
        }
        case NUMBER_LITERAL: {
            return {INT, (expression.literal)};
        }
        case GET_MEMBER: {
            const string& identifier = expression.identifier;
            if(!currentObj.contains(identifier)) throw pathException("No such key in JSON", identifier);
            if(currentObj.at(identifier).type != OBJECT) throw pathException("This path should be an object", identifier);
            const unordered_map<string, ValueJSON> obj = get<unordered_map<string, ValueJSON>>(currentObj.at(identifier).value);
            const auto next = expression.children.at(0);
            try {
                return executeExpression(JSON, next, obj);
            } catch (pathException& e) {
                e.appendPathFront(identifier + '.');
                throw;
            }
        }
        case GET_SUBSCRIPT: {
            const string& identifier = expression.identifier;
            if(!currentObj.contains(identifier)) throw pathException("No such key in JSON", identifier);
            if(currentObj.at(identifier).type != ARRAY) throw pathException("This path should be an array", identifier);
            const vector<ValueJSON> array = get<vector<ValueJSON>>(currentObj.at(identifier).value);
            const Node next = expression.children.at(0);

            try {
                return getItemFromArray(JSON, next, array);
            } catch (pathException& e) {
                e.appendPathFront(identifier);
                throw;
            }
        }
        case ONLY_SUBSCRIPT:
            throw executeException("Grave error, switch case ONLY_SUBSCRIPT should be impossible!");
        case MAX: {
            return getMax(JSON, expression);
        }
        case MIN: {
            return getMin(JSON, expression);
        }
        case SIZE: {
            return getSize(JSON, expression);
        }
        case ADD: {
            const auto& [a, b] = getOperands(JSON, expression);
            if(a.type == INT && b.type == INT)
                return {INT, get<long long>(a.value) + get<long long>(b.value)};
            return {FLOAT, extractDouble(a) + extractDouble(b)};
        }
        case SUBTRACT: {
            const auto& [a, b] = getOperands(JSON, expression);
            if(a.type == INT && b.type == INT)
                return {INT, get<long long>(a.value) - get<long long>(b.value)};
            return {FLOAT, extractDouble(a) - extractDouble(b)};
        }
        case MULTIPLY: {
            const auto& [a, b] = getOperands(JSON, expression);
            if(a.type == INT && b.type == INT)
                return {INT, get<long long>(a.value) * get<long long>(b.value)};
            return {FLOAT, extractDouble(a) * extractDouble(b)};
        }
        case DIVIDE: {
            const auto& [a, b] = getOperands(JSON, expression);
            if(a.type == INT && b.type == INT)
                return {INT, get<long long>(a.value) / get<long long>(b.value)};
            return {FLOAT, extractDouble(a) / extractDouble(b)};
        }
        case RAISE: {
            const auto& [a, b] = getOperands(JSON, expression);
            if(a.type == INT && b.type == INT)
                return {INT, llround(pow(get<long long>(a.value), get<long long>(b.value)))};
            return {FLOAT, pow(extractDouble(a), extractDouble(b))};
        }
    }
    throw executeException("Grave error, switch case leaked!");
}
