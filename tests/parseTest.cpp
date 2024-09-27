#include <gtest/gtest.h>

#include "../parse.h"

using namespace std;

TEST(SimpleParse, uglyStrings) {
    const string filePath = string(TEST_DATA_DIR) + "/uglyStrings.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_EQ(3, result.size());
}

TEST(ComplexValidParse, bigNoArrays) {
    const string filePath = string(TEST_DATA_DIR) + "/bigNoArrays.json";
    ASSERT_NO_THROW(parseJSON(filePath));
}

TEST(EscapedChar, escapedQuoteInKey) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedQuoteInKey.json";
    const unordered_map<string, ValueJSON> result = parseJSON(filePath);
    ASSERT_TRUE(result.contains("escaped\""));
}

TEST(EscapedChar, escapedLastQuoteInKey) {
    const string filePath = string(TEST_DATA_DIR) + "/escapedChar/escapedLastQuoteInKey.json";
    ASSERT_THROW(parseJSON(filePath), ParseException);
}