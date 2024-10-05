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