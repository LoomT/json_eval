#include "../src/parseJSON.h"
#include "../src/value.h"

#include <gtest/gtest.h>

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
    ASSERT_STREQ("something", get<string>(result.at("string").value).c_str());
}

TEST(ParseSimple, integer) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/number.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_TRUE(result.contains("integer"));
    ASSERT_EQ(INT, result.at("integer").type);
    ASSERT_EQ(5, get<long long>(result.at("integer").value));
}

TEST(ParseSimple, negativeInt) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/number.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_TRUE(result.contains("negativeInt"));
    ASSERT_EQ(INT, result.at("negativeInt").type);
    ASSERT_EQ(-6, get<long long>(result.at("negativeInt").value));
}

TEST(ParseSimple, floating) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/number.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_TRUE(result.contains("floating"));
    ASSERT_EQ(FLOAT, result.at("floating").type);
    ASSERT_FLOAT_EQ(0.12, get<double>(result.at("floating").value));
}

TEST(ParseSimple, negativeFloat) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/number.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_TRUE(result.contains("negativeFloat"));
    ASSERT_EQ(FLOAT, result.at("negativeFloat").type);
    ASSERT_FLOAT_EQ(-12.002, get<double>(result.at("negativeFloat").value));
}

TEST(ParseSimple, scaled) {
    const string filePath = string(TEST_DATA_DIR) + "/simple/number.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_TRUE(result.contains("scaled"));
    ASSERT_EQ(FLOAT, result.at("scaled").type);
    ASSERT_FLOAT_EQ(1.0321e-5, get<double>(result.at("scaled").value));
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
TEST(ParseEscapedChar, escapedLastQuoteInKey) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedLastQuoteInKey.json";
    ASSERT_THROW(parseJSON(filePath), JSONParseException);
}

TEST(ParseEscapedChar, escapedEscapeInValue) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedInValue.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_STREQ("esc\\ape", get<string>(result.at("escape").value).c_str());
}

TEST(ParseEscapedChar, escapedForwardSlashInValue) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedInValue.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_STREQ("/", get<string>(result.at("forward").value).c_str());
}

TEST(ParseEscapedChar, escapedNewLineInValue) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedInValue.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_STREQ("top\nbottom", get<string>(result.at("newline").value).c_str());
}

TEST(ParseEscapedChar, escapedTabInValue) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedInValue.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_STREQ("\ttabbed", get<string>(result.at("tab").value).c_str());
}

TEST(ParseEscapedChar, escapedBackSpaceInValue) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedInValue.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_STREQ("b\b", get<string>(result.at("backspace").value).c_str());
}

TEST(ParseEscapedChar, escapedFormFeedInValue) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedInValue.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_STREQ("\f", get<string>(result.at("form_feed").value).c_str());
}

TEST(ParseEscapedChar, escapedCarriageReturnInValue) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedInValue.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_STREQ("\r", get<string>(result.at("carriage").value).c_str());
}

TEST(ParseEscapedChar, escapedSmileyInValue) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedInValue.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_STREQ("\u0002", get<string>(result.at("smiley").value).c_str());
}

TEST(ParseEscapedChar, escapedQuotesInValue) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedInValue.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_STREQ("\"citation\"", get<string>(result.at("quote").value).c_str());
}

// Key in objects
TEST(EdgeCase, emptyKey) {
    const string filePath = string(TEST_DATA_DIR) + "/key/emptyKey.json";
    ASSERT_THROW(parseJSON(filePath), JSONParseException);
}

TEST(ParseEscapedChar, escapedQuoteInKey) {
    const string filePath = string(TEST_DATA_DIR) + "/key/escapedQuoteInKey.json";
    ASSERT_THROW(parseJSON(filePath), JSONParseException);
}

// Edge cases
TEST(EdgeCase, emptyStringValue) {
    const string filePath = string(TEST_DATA_DIR) + "/edgeCases/emptyStringValue.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_STREQ("", get<string>(result.at("empty").value).c_str());
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
    ASSERT_STREQ("one", get<string>(array.at(0).value).c_str());
    ASSERT_STREQ("two", get<string>(array.at(1).value).c_str());
    ASSERT_STREQ("three", get<string>(array.at(2).value).c_str());
}

TEST(ParseArray, oneItem) {
    const string filePath = string(TEST_DATA_DIR) + "/array/oneItem.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_TRUE(result.contains("array"));
    const vector<ValueJSON> array = get<vector<ValueJSON>>(result.at("array").value);
    ASSERT_EQ(1, array.size());
    ASSERT_STREQ("one", get<string>(array.at(0).value).c_str());
}

TEST(ParseArray, differentTypeItems) {
    const string filePath = string(TEST_DATA_DIR) + "/array/differentTypeItems.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_TRUE(result.contains("array"));
    const vector<ValueJSON> array = get<vector<ValueJSON>>(result.at("array").value);
    ASSERT_EQ(7, array.size());
    ASSERT_STREQ("one", get<string>(array.at(0).value).c_str());
    ASSERT_EQ(typeNULL, array.at(1).type);
    ASSERT_EQ(3, get<long long>(array.at(2).value));
    ASSERT_TRUE(get<bool>(array.at(3).value));
    ASSERT_FALSE(get<bool>(array.at(4).value));
    const unordered_map<string, ValueJSON> obj = get<unordered_map<string, ValueJSON>>(array.at(5).value);
    ASSERT_EQ(0, obj.size());
    const vector<ValueJSON> vect = get<vector<ValueJSON>>(array.at(6).value);
    ASSERT_EQ(0, vect.size());
}
