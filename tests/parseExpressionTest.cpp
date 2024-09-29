#include <complex>
#include <gtest/gtest.h>

#include "../src/expression.h"

TEST(JSONPath, verySimple) {
    const node actual = parseExpression("a");
    EXPECT_STREQ("a", actual.variable.c_str());
    EXPECT_EQ(VARIABLE, actual.action);
}

TEST(JSONPath, simple) {
    const node actual = parseExpression("a1_");
    EXPECT_STREQ("a1_", actual.variable.c_str());
    EXPECT_EQ(VARIABLE, actual.action);
}

TEST(JSONPath, underscoreStart) {
    const node actual = parseExpression("_a1_");
    EXPECT_STREQ("_a1_", actual.variable.c_str());
    EXPECT_EQ(VARIABLE, actual.action);
}

TEST(JSONPath, unexpectedStart) {
    EXPECT_THROW(parseExpression("?a1"), ExpressionParseException);
}