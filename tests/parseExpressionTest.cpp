#include <complex>
#include <gtest/gtest.h>

#include "../src/expression.h"

TEST(JSONPath, verySimple) {
    const std::unique_ptr<node> actual = parseExpression("a");
    EXPECT_STREQ("a", actual->variable.c_str());
    EXPECT_EQ(VARIABLE, actual->action);
}

TEST(JSONPath, simple) {
    const std::unique_ptr<node> actual = parseExpression("a1_");
    EXPECT_STREQ("a1_", actual->variable.c_str());
    EXPECT_EQ(VARIABLE, actual->action);
}

TEST(JSONPath, underscoreStart) {
    const std::unique_ptr<node> actual = parseExpression("_a1_");
    EXPECT_STREQ("_a1_", actual->variable.c_str());
    EXPECT_EQ(VARIABLE, actual->action);
}

TEST(JSONPath, unexpectedStart) {
    EXPECT_THROW(parseExpression("?a1"), ExpressionParseException);
}

TEST(JSONPath, path2) {
    const std::unique_ptr<node> actual = parseExpression("a.b");
    EXPECT_STREQ("a", actual->variable.c_str());
    EXPECT_EQ(GET_MEMBER, actual->action);
    EXPECT_STREQ("b", actual->children.at(0).variable.c_str());
    EXPECT_EQ(VARIABLE, actual->children.at(0).action);
}

TEST(JSONPath, path4) {
    const std::unique_ptr<node> actual = parseExpression("a.b._c.d1");
    const node child1 = actual->children.at(0);
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

TEST(NumberLiteral, simpleNumber) {
    const std::unique_ptr<node> actual = parseExpression("1");
    EXPECT_EQ(1, actual->literal);
    EXPECT_EQ(NUMBER_LITERAL, actual->action);
}

TEST(Subscript, simple) {
    const std::unique_ptr<node> actual = parseExpression("a[1]");
    EXPECT_STREQ("a", actual->variable.c_str());
    EXPECT_EQ(GET_SUBSCRIPT, actual->action);
    const node child = actual->children.at(0);
    EXPECT_EQ(ONLY_SUBSCRIPT, child.action);
    EXPECT_EQ(1, child.subscript->literal);
    EXPECT_EQ(NUMBER_LITERAL, child.subscript->action);
}

TEST(Subscript, simpleExpressionInSubscript) {
    const std::unique_ptr<node> actual = parseExpression("a[b]");
    EXPECT_STREQ("a", actual->variable.c_str());
    EXPECT_EQ(GET_SUBSCRIPT, actual->action);
    const node child = actual->children.at(0);
    EXPECT_EQ(ONLY_SUBSCRIPT, child.action);
    EXPECT_STREQ("b", child.subscript->variable.c_str());
    EXPECT_EQ(VARIABLE, child.subscript->action);
}

TEST(Subscript, memberOfArrayValue) {
    const std::unique_ptr<node> actual = parseExpression("a[b].c");
    EXPECT_STREQ("a", actual->variable.c_str());
    EXPECT_EQ(GET_SUBSCRIPT, actual->action);
    const node child1 = actual->children.at(0);
    EXPECT_STREQ("b", child1.subscript->variable.c_str());
    EXPECT_EQ(VARIABLE, child1.subscript->action);
    EXPECT_EQ(GET_MEMBER, child1.action);
    const node child2 = child1.children.at(0);
    EXPECT_STREQ("c", child2.variable.c_str());
    EXPECT_EQ(VARIABLE, child2.action);
}

// maybe a bit too many assertions
TEST(Subscript, complexPathMix) {
    const std::unique_ptr<node> actual = parseExpression("a.b[c.d2[221].ee][1].f");
    const node b = actual->children.at(0);
    EXPECT_STREQ("b", b.variable.c_str());
    EXPECT_EQ(GET_SUBSCRIPT, b.action);
    const node firstSubscript = b.children.at(0);
    EXPECT_STREQ("c", firstSubscript.subscript->variable.c_str());
    EXPECT_EQ(GET_MEMBER, firstSubscript.subscript->action);
    const node d2 = firstSubscript.subscript->children.at(0);
    EXPECT_STREQ("d2", d2.variable.c_str());
    EXPECT_EQ(GET_SUBSCRIPT, d2.action);
    const node innerSubscript = d2.children.at(0);
    EXPECT_EQ(221, innerSubscript.subscript->literal);
    EXPECT_EQ(NUMBER_LITERAL, innerSubscript.subscript->action);
    EXPECT_EQ(GET_MEMBER, innerSubscript.action);
    const node ee = innerSubscript.children.at(0);
    EXPECT_STREQ("ee", ee.variable.c_str());
    EXPECT_EQ(VARIABLE, ee.action);
    const node secondSubscript = firstSubscript.children.at(0);
    EXPECT_EQ(1, secondSubscript.subscript->literal);
    EXPECT_EQ(NUMBER_LITERAL, secondSubscript.subscript->action);
    const node f = secondSubscript.children.at(0);
    EXPECT_STREQ("f", f.variable.c_str());
    EXPECT_EQ(VARIABLE, f.action);
}

TEST(IllegalPath, extraOpeningBracket) {
    EXPECT_THROW(parseExpression("a[[1]"), ExpressionParseException);
}

TEST(IllegalPath, extraClosingBracket) {
    EXPECT_THROW(parseExpression("a[1]]"), ExpressionParseException);
}

TEST(IllegalPath, extraDot) {
    EXPECT_THROW(parseExpression("a..b"), ExpressionParseException);
}