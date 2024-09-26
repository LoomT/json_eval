#include <gtest/gtest.h>

#include "../parse.h"

using namespace std;

TEST(SimpleParse, uglyStrings) {
    const string filePath = string(TEST_DATA_DIR) + "/uglyStrings.json";
    cout << objectToString(parseJSON(filePath));
}