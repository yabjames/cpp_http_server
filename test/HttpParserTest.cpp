#include <gtest/gtest.h>
#include "../include/HttpParser.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

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

TEST(HttpParserTest, ShouldHaveRootPath) {
	const std::string path = "/";
	const std::vector<std::string_view> segments = HttpParser::split_path(path);

	EXPECT_EQ(segments.size(), 1);
	EXPECT_EQ(segments[0], "/");
}

TEST(HttpParserTest, ShouldHavePathParameters) {
	try {
		HttpServer server{};
		server.get_mapping(
			"/foo/{id}/{user}", [](HttpServer::Request &req,
			                       HttpServer::Response &res) {
				std::stringstream ss;
				ss << "id: " << req.path_params.get_path_param("id").value() <<
					"\n";
				ss << "user: " << req.path_params.get_path_param("user").value()
					<< "\n";
				res.body = ss.str();
			}
			);

		server.start_listening(8081);

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		int sock = socket(AF_INET, SOCK_STREAM, 0);

		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(8081);
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		ASSERT_EQ(
			connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
			0);

		const std::string request = "GET /foo/123/james HTTP/1.1\r\n"
			"Host: localhost\r\n"
			"Connection: keep-alive\r\n"
			"Content-Length: 5\r\n"
			"\r\n"
			"hello";

		send(sock, request.c_str(), request.size(), 0);

		char buffer[1024]{};
		const ssize_t bytes = recv(sock, buffer, sizeof(buffer), 0);
		const std::string result = std::string(buffer);

		EXPECT_GT(bytes, 0);
		EXPECT_TRUE(result.find("id: 123") != std::string::npos);
		EXPECT_TRUE(result.find("user: james") != std::string::npos);

		close(sock);
	} catch (const std::exception &e) {
		FAIL() << "Exception occurred: " << e.what();
	}
}

TEST(HttpParserTest, ShouldHandleNoPathParameters) {
	try {
		HttpServer server{};
		server.get_mapping(
			"/foo/{id}/{user}", [](HttpServer::Request &req,
			                       HttpServer::Response &res) {
				std::stringstream ss;
				try {
					ss << req.path_params.get_path_param("foo").value() << "\n";
					res.body = ss.str();
				} catch (const std::bad_optional_access &e) {
					res.body = "could not get path parameter foo";
				}
			}
			);

		server.start_listening(8081);

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		int sock = socket(AF_INET, SOCK_STREAM, 0);

		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(8081);
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		ASSERT_EQ(
			connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
			0);

		const std::string request = "GET /foo/123/james HTTP/1.1\r\n"
			"Host: localhost\r\n"
			"Connection: keep-alive\r\n"
			"Content-Length: 5\r\n"
			"\r\n"
			"hello";

		send(sock, request.c_str(), request.size(), 0);

		char buffer[1024]{};
		const ssize_t bytes = recv(sock, buffer, sizeof(buffer), 0);
		const std::string result = std::string(buffer);

		EXPECT_GT(bytes, 0);
		EXPECT_FALSE(result.find("id: 123") != std::string::npos);
		EXPECT_FALSE(result.find("user: james") != std::string::npos);
		EXPECT_TRUE(
			result.find("could not get path parameter foo") != std::string::
			npos);

		close(sock);
	} catch (const std::exception &e) {
		FAIL() << "Exception occurred: " << e.what();
	}
}