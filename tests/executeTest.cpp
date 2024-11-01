#include <gtest/gtest.h>

#include "../src/JSON.h"

using namespace std;

TEST(TrivialPath, first) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(2, get<long long>(json.evaluate("a.b[1]").value));
}

TEST(TrivialPath, second) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_STREQ("\"test\"", toString(json.evaluate("a.b[2].c")).c_str());
}

TEST(TrivialPath, third) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_STREQ("[ 1, 2, { \"c\": \"test\" }, [ 11, 12 ] ]", toString(json.evaluate("a.b")).c_str());
}

TEST(Path, subExpression) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_STREQ("\"test\"", toString(json.evaluate("a.b[a.b[1]].c")).c_str());
}

TEST(FunctionMax, max) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(2, get<long long>(json.evaluate("max(a.b[0], a.b[1])").value));
}

TEST(FunctionMin, min) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(11, get<long long>(json.evaluate("min(a.b[3])").value));
}

TEST(FunctionSize, first) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(1, get<long long>(json.evaluate("size(a)").value));
}

TEST(FunctionSize, second) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(4, get<long long>(json.evaluate("size(a.b)").value));
}

TEST(FunctionSize, third) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(4, get<long long>(json.evaluate("size(a.b[a.b[1]].c)").value));
}

TEST(FunctionMax, withLiterals) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(15, get<long long>(json.evaluate("max(a.b[0], 10, a.b[1], 15)").value));
}

TEST(Arithmetic, addAtPaths) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(3, get<long long>(json.evaluate("a.b[0] + a.b[1]").value));
}

TEST(Arithmetic, addLiterals) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(4, get<long long>(json.evaluate("1  +3").value));
}

TEST(Arithmetic, addWithLiterals) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(6, get<long long>(json.evaluate("1 + a.b[1] + 3").value));
}

TEST(Arithmetic, addNegativeLiteral) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(-2, get<long long>(json.evaluate("1 + -3").value));
}

TEST(Arithmetic, mix) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(6, get<long long>(json.evaluate("a.b[0] + a.b[ 1 ] * a.b[a.b[0] + a.b[1]][0] / 2^2").value));
}

TEST(Arithmetic, mixFloat) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_FLOAT_EQ(4.6, get<double>(json.evaluate("a.b[0] + a.b[ 1 ] * a.b[a.b[0] + a.b[1]][0] / 5.5 - 0.4").value));
}

TEST(Arithmetic, justParentheses) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(5, get<long long>(json.evaluate("(5)").value));
}

TEST(Arithmetic, parentheses) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(9, get<long long>(json.evaluate("(1+2 * (3 + 2*-1))^2").value));
}

TEST(Arithmetic, mixParentheses) {
    const string filePath = string(TEST_DATA_DIR) + "/test.json";
    const JSON json = JSON(filePath);
    ASSERT_EQ(15, get<long long>(json.evaluate("a.b[0] + a.b[ 1 ] * a.b[a.b[0] + a.b[1]][0] / 2^2 + (1+2 * (3 + 2*-1))^2").value));
}