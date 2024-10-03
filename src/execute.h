#ifndef EXECUTE_H
#define EXECUTE_H
#include "expression.h"
#include "value.h"

inline ValueJSON executeExpression(const std::unordered_map<std::string, ValueJSON>& JSON, const Node& expression);

#endif //EXECUTE_H
