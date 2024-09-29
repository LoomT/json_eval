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

TEST(JSONPath, path2) {
    const node actual = parseExpression("a.b");
    EXPECT_STREQ("a", actual.variable.c_str());
    EXPECT_EQ(GET_MEMBER, actual.action);
    EXPECT_STREQ("b", actual.children.at(0).variable.c_str());
    EXPECT_EQ(VARIABLE, actual.children.at(0).action);
}

TEST(JSONPath, path4) {
    const node actual = parseExpression("a.b._c.d1");
    const node child1 = actual.children.at(0);
    EXPECT_STREQ("b", child1.variable.c_str());
    EXPECT_EQ(GET_MEMBER, child1.action);
    const node child2 = child1.children.at(0);
    EXPECT_STREQ("_c", child2.variable.c_str());
    EXPECT_EQ(GET_MEMBER, child2.action);
    const node child3 = child2.children.at(0);
    EXPECT_STREQ("d1", child3.variable.c_str());
    EXPECT_EQ(VARIABLE, child3.action);
}

TEST(JSONPath, numberInPath) {
    EXPECT_THROW(parseExpression("a.1"), ExpressionParseException);
}