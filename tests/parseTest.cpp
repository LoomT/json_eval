#include <gtest/gtest.h>

#include "../parse.h"
#include "../value.h"

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


TEST(SimpleParse, uglyStrings) {
    const string filePath = string(TEST_DATA_DIR) + "/unformatted/uglyStrings.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(3, result.size());
}