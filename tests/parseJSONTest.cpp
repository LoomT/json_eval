#include <gtest/gtest.h>

#include "../src/parse.h"
#include "../src/value.h"

using namespace std;

//Simple valid
TEST(ParseSimple, empty) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/empty.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(result.size(), 0);
}

TEST(ParseSimple, string) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/string.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(1, result.size());
    ASSERT_TRUE(result.contains("string"));
    ASSERT_EQ(STRING, result.at("string").type);
    ASSERT_EQ("something", get<string>(result.at("string").value));
}

TEST(ParseSimple, number) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/number.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(1, result.size());
    ASSERT_TRUE(result.contains("number"));
    ASSERT_EQ(NUMBER, result.at("number").type);
    ASSERT_EQ(5, get<double>(result.at("number").value));
}

TEST(ParseSimple, null) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/null.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(1, result.size());
    ASSERT_TRUE(result.contains("shouldBeNull"));
    ASSERT_EQ(typeNULL, result.at("shouldBeNull").type);
}

TEST(ParseSimple, boolTrue) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/boolTrue.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(1, result.size());
    ASSERT_TRUE(result.contains("bool"));
    ASSERT_EQ(BOOL, result.at("bool").type);
    ASSERT_TRUE(get<bool>(result.at("bool").value));
}

TEST(ParseSimple, boolFalse) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/boolFalse.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(1, result.size());
    ASSERT_TRUE(result.contains("bool"));
    ASSERT_EQ(BOOL, result.at("bool").type);
    ASSERT_FALSE(get<bool>(result.at("bool").value));
}

TEST(ParseSimple, objectEmpty) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/objectEmpty.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(1, result.size());
    ASSERT_TRUE(result.contains("object"));
    ASSERT_EQ(OBJECT, result.at("object").type);
    const unordered_map<string, ValueJSON> obj = get<unordered_map<string, ValueJSON>>(result.at("object").value);
    ASSERT_EQ(0, obj.size());
}

TEST(ParseSimple, arrayEmpty) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/arrayEmpty.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(1, result.size());
    ASSERT_TRUE(result.contains("array"));
    ASSERT_EQ(ARRAY, result.at("array").type);
    const vector<ValueJSON> array = get<vector<ValueJSON>>(result.at("array").value);
    ASSERT_EQ(0, array.size());
}

TEST(ParseComplexValid, bigNoArrays) {
    const string filePath = string(TEST_DATA_DIR) + "/complex/bigNoArrays.json";
    ASSERT_NO_THROW(parseJSON(filePath));
}

TEST(ParseComplexValid, everything) {
    const string filePath = string(TEST_DATA_DIR) + "/complex/everything.json";
    ASSERT_NO_THROW(parseJSON(filePath));
}

// Escaped char
TEST(ParseEscapedChar, escapedQuoteInKey) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedQuoteInKey.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_TRUE(result.contains("escaped\""));
}

TEST(ParseEscapedChar, escapedLastQuoteInKey) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedLastQuoteInKey.json";
    ASSERT_THROW(parseJSON(filePath), ParseException);
}

// Edge cases
TEST(EdgeCase, emptyKey) {
    const string filePath = string(TEST_DATA_DIR) + "/edgeCases/emptyKey.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(1, result.size());
    ASSERT_TRUE(result.contains(""));
}

TEST(EdgeCase, emptyStringValue) {
    const string filePath = string(TEST_DATA_DIR) + "/edgeCases/emptyStringValue.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ("", get<string>(result.at("empty").value));
}

// Unformatted JSON
TEST(Unformatted, uglyStrings) {
    const string filePath = string(TEST_DATA_DIR) + "/unformatted/uglyStrings.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(3, result.size());
}

// Array
TEST(ParseArray, simple) {
    const string filePath = string(TEST_DATA_DIR) + "/array/simple.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(1, result.size());
    ASSERT_TRUE(result.contains("array"));
    const vector<ValueJSON> array = get<vector<ValueJSON>>(result.at("array").value);
    ASSERT_EQ(3, array.size());
    ASSERT_EQ("one", get<string>(array.at(0).value));
    ASSERT_EQ("two", get<string>(array.at(1).value));
    ASSERT_EQ("three", get<string>(array.at(2).value));
}

TEST(ParseArray, oneItem) {
    const string filePath = string(TEST_DATA_DIR) + "/array/oneItem.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_TRUE(result.contains("array"));
    const vector<ValueJSON> array = get<vector<ValueJSON>>(result.at("array").value);
    ASSERT_EQ(1, array.size());
    ASSERT_EQ("one", get<string>(array.at(0).value));
}

TEST(ParseArray, differentTypeItems) {
    const string filePath = string(TEST_DATA_DIR) + "/array/differentTypeItems.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_TRUE(result.contains("array"));
    const vector<ValueJSON> array = get<vector<ValueJSON>>(result.at("array").value);
    ASSERT_EQ(7, array.size());
    ASSERT_STREQ("one", get<string>(array.at(0).value).c_str());
    ASSERT_EQ(typeNULL, array.at(1).type);
    ASSERT_EQ(3, get<double>(array.at(2).value));
    ASSERT_TRUE(get<bool>(array.at(3).value));
    ASSERT_FALSE(get<bool>(array.at(4).value));
    const unordered_map<string, ValueJSON> obj = get<unordered_map<string, ValueJSON>>(array.at(5).value);
    ASSERT_EQ(0, obj.size());
    const vector<ValueJSON> vect = get<vector<ValueJSON>>(array.at(6).value);
    ASSERT_EQ(0, vect.size());
}
