#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "utils.h"

TEST(UtilsTest, ReplaceAll) {
    const std::string str = "This is a tests. This is only a tests.";
    const std::string from = "tests";
    const std::string to = "sample";
    const std::string expected = "This is a sample. This is only a sample.";

    EXPECT_EQ(replace_all(str, from, to), expected);
}

TEST(UtilsTest, IsJson) {
    const std::string json_str = R"({"key": "value"})";
    const std::string non_json_str = "key=value";

    EXPECT_TRUE(is_json(json_str));
    EXPECT_FALSE(is_json(non_json_str));
}

TEST(UtilsTest, LoadAdditionalPayloads) {
    const std::string payload_file = "../files/payloads.txt";
    load_additional_payloads(payload_file);

    EXPECT_GT(payloads.size(), 0);
}

TEST(UtilsTest, GetDomainFromUrl) {
    const std::string url = "https://example.com/path?query=string";
    const std::string expected_domain = "example.com";

    EXPECT_EQ(get_domain_from_url(url), expected_domain);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
