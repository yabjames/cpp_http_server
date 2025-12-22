#include <gtest/gtest.h>
#include "../include/HttpParser.h"

class HttpParserTest : public ::testing::Test {
  public:
	static constexpr int port{8081};
};

TEST(HttpParserTest, ShouldSplitPath) {
    std::string path = "/foo/foo2";
    std::vector<std::string_view> segments = HttpParser::split_path(path);

    EXPECT_EQ(segments.size(), 2);
    EXPECT_EQ(segments[0], "foo");
    EXPECT_EQ(segments[1], "foo2");
}
