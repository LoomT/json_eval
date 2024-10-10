#include "../src/expression.h"

#include <gtest/gtest.h>
#include <complex>

TEST(JSONPath, verySimple) {
    const Node actual = parseExpression("a");
    EXPECT_STREQ("a", get<std::string>(actual.value).c_str());
    EXPECT_EQ(IDENTIFIER, actual.action);
}

TEST(JSONPath, simple) {
    const Node actual = parseExpression("a1_");
    EXPECT_STREQ("a1_", get<std::string>(actual.value).c_str());
    EXPECT_EQ(IDENTIFIER, actual.action);
}

TEST(JSONPath, underscoreStart) {
    const Node actual = parseExpression("_a1_");
    EXPECT_STREQ("_a1_", get<std::string>(actual.value).c_str());
    EXPECT_EQ(IDENTIFIER, actual.action);
}

TEST(JSONPath, unexpectedStart) {
    EXPECT_THROW(parseExpression("?a1"), ExpressionParseException);
}

TEST(JSONPath, path2) {
    const Node actual = parseExpression("a.b");
    EXPECT_STREQ("a", get<std::string>(actual.value).c_str());
    EXPECT_EQ(GET_MEMBER, actual.action);
    EXPECT_STREQ("b", get<std::string>(actual.children.at(0).value).c_str());
    EXPECT_EQ(IDENTIFIER, actual.children.at(0).action);
}

TEST(JSONPath, path4) {
    const Node actual = parseExpression("a.b._c.d1");
    const Node& child1 = actual.children.at(0);
    EXPECT_STREQ("b", get<std::string>(child1.value).c_str());
    EXPECT_EQ(GET_MEMBER, child1.action);
    const Node& child2 = child1.children.at(0);
    EXPECT_STREQ("_c", get<std::string>(child2.value).c_str());
    EXPECT_EQ(GET_MEMBER, child2.action);
    const Node& child3 = child2.children.at(0);
    EXPECT_STREQ("d1", get<std::string>(child3.value).c_str());
    EXPECT_EQ(IDENTIFIER, child3.action);
}

TEST(JSONPath, numberInPath) {
    EXPECT_THROW(parseExpression("a.1"), ExpressionParseException);
}

TEST(NumberLiteral, simpleNumber) {
    const Node actual = parseExpression("1");
    EXPECT_EQ(1, get<long long>(actual.value));
    EXPECT_EQ(INT_LITERAL, actual.action);
}

TEST(Subscript, simple) {
    const Node actual = parseExpression("a[1]");
    EXPECT_STREQ("a", get<std::string>(actual.value).c_str());
    EXPECT_EQ(GET_SUBSCRIPT, actual.action);
    const Node& child = actual.children.at(0);
    EXPECT_EQ(ONLY_SUBSCRIPT, child.action);
    EXPECT_EQ(1, get<long long>(child.subscript->value));
    EXPECT_EQ(INT_LITERAL, child.subscript->action);
}

TEST(Subscript, simpleExpressionInSubscript) {
    const Node actual = parseExpression("a[b]");
    EXPECT_STREQ("a", get<std::string>(actual.value).c_str());
    EXPECT_EQ(GET_SUBSCRIPT, actual.action);
    const Node& child = actual.children.at(0);
    EXPECT_EQ(ONLY_SUBSCRIPT, child.action);
    EXPECT_STREQ("b", get<std::string>(child.subscript->value).c_str());
    EXPECT_EQ(IDENTIFIER, child.subscript->action);
}

TEST(Subscript, memberOfArrayValue) {
    const Node actual = parseExpression("a[b].c");
    EXPECT_STREQ("a", get<std::string>(actual.value).c_str());
    EXPECT_EQ(GET_SUBSCRIPT, actual.action);
    const Node& child1 = actual.children.at(0);
    EXPECT_STREQ("b", get<std::string>(child1.subscript->value).c_str());
    EXPECT_EQ(IDENTIFIER, child1.subscript->action);
    EXPECT_EQ(GET_MEMBER, child1.action);
    const Node& child2 = child1.children.at(0);
    EXPECT_STREQ("c", get<std::string>(child2.value).c_str());
    EXPECT_EQ(IDENTIFIER, child2.action);
}

// maybe a bit too many assertions
TEST(Subscript, complexPathMix) {
    const Node actual = parseExpression("a.b[c.d2[221].ee][1].f");
    const Node& b = actual.children.at(0);
    EXPECT_STREQ("b", get<std::string>(b.value).c_str());
    EXPECT_EQ(GET_SUBSCRIPT, b.action);
    const Node& firstSubscript = b.children.at(0);
    EXPECT_STREQ("c", get<std::string>(firstSubscript.subscript->value).c_str());
    EXPECT_EQ(GET_MEMBER, firstSubscript.subscript->action);
    const Node& d2 = firstSubscript.subscript->children.at(0);
    EXPECT_STREQ("d2", get<std::string>(d2.value).c_str());
    EXPECT_EQ(GET_SUBSCRIPT, d2.action);
    const Node& innerSubscript = d2.children.at(0);
    EXPECT_EQ(221, get<long long>(innerSubscript.subscript->value));
    EXPECT_EQ(INT_LITERAL, innerSubscript.subscript->action);
    EXPECT_EQ(GET_MEMBER, innerSubscript.action);
    const Node& ee = innerSubscript.children.at(0);
    EXPECT_STREQ("ee", get<std::string>(ee.value).c_str());
    EXPECT_EQ(IDENTIFIER, ee.action);
    const Node& secondSubscript = firstSubscript.children.at(0);
    EXPECT_EQ(1, get<long long>(secondSubscript.subscript->value));
    EXPECT_EQ(INT_LITERAL, secondSubscript.subscript->action);
    const Node& f = secondSubscript.children.at(0);
    EXPECT_STREQ("f", get<std::string>(f.value).c_str());
    EXPECT_EQ(IDENTIFIER, f.action);
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

TEST(MaxFunction, empty) {
    EXPECT_THROW(parseExpression("max()"), ExpressionParseException);
}

TEST(MaxFunction, oneArg) {
    const Node actual = parseExpression("max(1)");
    EXPECT_EQ(MAX, actual.action);
    EXPECT_EQ(1, actual.children.size());
    EXPECT_EQ(INT_LITERAL, actual.children.at(0).action);
    EXPECT_EQ(1, get<long long>(actual.children.at(0).value));
}

TEST(MaxFunction, twoArgs) {
    const Node actual = parseExpression("max(a, 3)");
    EXPECT_EQ(MAX, actual.action);
    EXPECT_EQ(2, actual.children.size());
    EXPECT_EQ(IDENTIFIER, actual.children.at(0).action);
    EXPECT_STREQ("a", get<std::string>(actual.children.at(0).value).c_str());
    EXPECT_EQ(3, get<long long>(actual.children.at(1).value));
}

TEST(MaxFunction, moreArgs) {
    const Node actual = parseExpression("max(d.e[a], -123 ,  haha)");
    EXPECT_EQ(MAX, actual.action);
    EXPECT_EQ(3, actual.children.size());
    EXPECT_EQ(GET_MEMBER, actual.children.at(0).action);
    EXPECT_STREQ("d", get<std::string>(actual.children.at(0).value).c_str());
    EXPECT_EQ(-123, get<long long>(actual.children.at(1).value));
    EXPECT_EQ(IDENTIFIER, actual.children.at(2).action);
    EXPECT_STREQ("haha", get<std::string>(actual.children.at(2).value).c_str());
}

TEST(MaxFunction, extraTrailingComma) {
    EXPECT_THROW(parseExpression("max(ab,)"), ExpressionParseException);
}

TEST(MaxFunction, extraLeadingComma) {
    EXPECT_THROW(parseExpression("max(, ab)"), ExpressionParseException);
}

TEST(MinFunction, empty) {
    EXPECT_THROW(parseExpression("min( )"), ExpressionParseException);
}

TEST(MinFunction, oneArg) {
    const Node actual = parseExpression("min(1)");
    EXPECT_EQ(MIN, actual.action);
    EXPECT_EQ(1, actual.children.size());
    EXPECT_EQ(INT_LITERAL, actual.children.at(0).action);
    EXPECT_EQ(1, get<long long>(actual.children.at(0).value));
}

TEST(MinFunction, twoArgs) {
    const Node actual = parseExpression("min(a, 3)");
    EXPECT_EQ(MIN, actual.action);
    EXPECT_EQ(2, actual.children.size());
    EXPECT_EQ(IDENTIFIER, actual.children.at(0).action);
    EXPECT_STREQ("a", get<std::string>(actual.children.at(0).value).c_str());
    EXPECT_EQ(3, get<long long>(actual.children.at(1).value));
}

TEST(MinFunction, twoArgsExtraWS) {
    const Node actual = parseExpression("min( a  , 3   )");
    EXPECT_EQ(MIN, actual.action);
    EXPECT_EQ(2, actual.children.size());
    EXPECT_EQ(IDENTIFIER, actual.children.at(0).action);
    EXPECT_STREQ("a", get<std::string>(actual.children.at(0).value).c_str());
    EXPECT_EQ(3, get<long long>(actual.children.at(1).value));
}

TEST(MinFunction, moreArgs) {
    const Node actual = parseExpression("min(d.e[a], -123 ,  haha)");
    EXPECT_EQ(MIN, actual.action);
    EXPECT_EQ(3, actual.children.size());
    EXPECT_EQ(GET_MEMBER, actual.children.at(0).action);
    EXPECT_STREQ("d", get<std::string>(actual.children.at(0).value).c_str());
    EXPECT_EQ(-123, get<long long>(actual.children.at(1).value));
    EXPECT_EQ(IDENTIFIER, actual.children.at(2).action);
    EXPECT_STREQ("haha", get<std::string>(actual.children.at(2).value).c_str());
}

TEST(SizeFunction, empty) {
    EXPECT_THROW(parseExpression("size(  )"), ExpressionParseException);
}

TEST(SizeFunction, oneArg) {
    const Node actual = parseExpression("size(ab)");
    EXPECT_EQ(SIZE, actual.action);
    EXPECT_EQ(1, actual.children.size());
    EXPECT_EQ(IDENTIFIER, actual.children.at(0).action);
    EXPECT_STREQ("ab", get<std::string>(actual.children.at(0).value).c_str());
}

TEST(SizeFunction, oneArgExtraWS) {
    const Node actual = parseExpression("size( ab  )");
    EXPECT_EQ(SIZE, actual.action);
    EXPECT_EQ(1, actual.children.size());
    EXPECT_EQ(IDENTIFIER, actual.children.at(0).action);
    EXPECT_STREQ("ab", get<std::string>(actual.children.at(0).value).c_str());
}