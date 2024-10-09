#ifndef JSON_H
#define JSON_H
#include <string>
#include <unordered_map>

#include "parseJSON.h"
#include "value.h"
#include "execute.h"
#include "expression.h"

class JSON {
    std::unordered_map<std::string, ValueJSON> data;

public:
    /**
     * Constructs a JSON object with the contents of the JSON file
     * 
     * @param filePath file path of the JSON file
     */
    explicit JSON(const std::string& filePath) {
        data = parseFileJSON(filePath);
    }

    /**
     * 
     * @param expression expression to evaluate on the JSON this object was created with
     * @return evaluated result
     */
    ValueJSON evaluate(const std::string& expression) const {
        return executeExpression(data, parseExpression(expression));
    }
};

#endif //JSON_H
