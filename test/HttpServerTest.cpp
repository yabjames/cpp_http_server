#include "HttpServer.h"
#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <string>

class HttpServerTest : public ::testing::Test {
  public:
	static constexpr int port{8081};
};

TEST(ServerTest, ConstructorDestructorTest) { HttpServer server{}; }

TEST(HttpServerTest, AcceptsHttpRequest) {
	HttpServer server{};
	server.get_mapping("/",
					   [](const HttpServer::Request &,
						  HttpServer::Response &res) { res.body = "test"; });
	server.start_listening(HttpServerTest::port);

	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	int sock = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(HttpServerTest::port);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ASSERT_EQ(connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
			  0);

	const char *request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
						  "Host: localhost\r\n"
						  "Connection: keep-alive\r\n"
						  "Content-Length: 0\r\n"
						  "\r\n";
	send(sock, request, strlen(request), 0);

	char buffer[1024];
	const ssize_t bytes = recv(sock, buffer, sizeof(buffer), 0);

	EXPECT_GT(bytes, 0); // Server sent response

	close(sock);
}

TEST(HttpServerTest, AcceptGetRequest) {
	HttpServer server{};
	server.get_mapping(
		"/hello", [](const HttpServer::Request &, HttpServer::Response &res) {
			res.body = "hello, world";
		});
	server.start_listening(HttpServerTest::port);

	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	int sock = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(HttpServerTest::port);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ASSERT_EQ(connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
			  0);

	const char *request = "GET /hello HTTP/1.1\r\n\r\n";
	send(sock, request, strlen(request), 0);

	char buffer[1024]{};
	const ssize_t bytes = recv(sock, buffer, sizeof(buffer), 0);
	std::string result = std::string(buffer);

	EXPECT_GT(bytes, 0);
	ASSERT_TRUE(result.find("hello, world") != std::string::npos);

	close(sock);
}

TEST(HttpServerTest, IgnoreGetReqBody) {
	HttpServer server{};
	server.get_mapping("/hello",
					   [](const HttpServer::Request &req,
						  HttpServer::Response &res) { res.body = req.body; });
	server.start_listening(HttpServerTest::port);

	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	int sock = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(HttpServerTest::port);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ASSERT_EQ(connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
			  0);

	const char *request = "GET /hello HTTP/1.1\r\n\r\nhello, world";
	send(sock, request, strlen(request), 0);

	char buffer[1024]{};
	const ssize_t bytes = recv(sock, buffer, sizeof(buffer), 0);
	const std::string result = std::string(buffer);

	EXPECT_GT(bytes, 0);

	// Should not find "hello, world" as setting the request body is ignored
	ASSERT_FALSE(result.find("hello, world") != std::string::npos);

	close(sock);
}

TEST(HttpServerTest, DoesntIgnorePostReqBody) {
	try {
		HttpServer server{};
		server.post_mapping(
			"/foo", [](const HttpServer::Request &req,
					   HttpServer::Response &res) { res.body = req.body; });
		server.start_listening(HttpServerTest::port);

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		int sock = socket(AF_INET, SOCK_STREAM, 0);

		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(HttpServerTest::port);
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		ASSERT_EQ(
			connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
			0);

		std::string request = "POST /foo HTTP/1.1\r\n"
							  "Host: localhost\r\n"
							  "Connection: keep-alive\r\n"
							  "Content-Length: 5\r\n"
							  "\r\n"
							  "hello";

		send(sock, request.c_str(), request.size(), 0);

		char buffer[1024]{};
		const ssize_t bytes = recv(sock, buffer, sizeof(buffer), 0);
		std::string result = std::string(buffer);

		EXPECT_GT(bytes, 0);
		ASSERT_TRUE(result.find("hello") != std::string::npos);

		close(sock);
	} catch (const std::exception &e) {
		FAIL() << "Exception occurred: " << e.what();
	}
}

TEST(HttpServerTest, AllUniqueReqMethods) {
	// this will test all different http methods with the same route name
	HttpServer server{};

	server.get_mapping("/foo",
					   [](const HttpServer::Request &req,
						  HttpServer::Response &res) { res.body = "0"; });
	server.post_mapping("/foo",
						[](const HttpServer::Request &req,
						   HttpServer::Response &res) { res.body = "1"; });
	server.put_mapping("/foo",
					   [](const HttpServer::Request &req,
						  HttpServer::Response &res) { res.body = "2"; });
	server.patch_mapping("/foo",
						 [](const HttpServer::Request &req,
							HttpServer::Response &res) { res.body = "3"; });
	server.options_mapping("/foo",
						   [](const HttpServer::Request &req,
							  HttpServer::Response &res) { res.body = "4"; });
	server.head_mapping("/foo",
						[](const HttpServer::Request &req,
						   HttpServer::Response &res) { res.body = "5"; });
	server.delete_mapping("/foo",
						  [](const HttpServer::Request &req,
							 HttpServer::Response &res) { res.body = "6"; });
	server.connect_mapping("/foo",
						   [](const HttpServer::Request &req,
							  HttpServer::Response &res) { res.body = "7"; });
	server.trace_mapping("/foo",
						 [](const HttpServer::Request &req,
							HttpServer::Response &res) { res.body = "8"; });

	server.start_listening(HttpServerTest::port);

	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(HttpServerTest::port);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	for (int i = 0; i < 9; i++) {
		const std::string methods[9] = {"GET",	  "POST",	 "PUT",
										"PATCH",  "OPTIONS", "HEAD",
										"DELETE", "CONNECT", "TRACE"};
		std::string request = methods[i] + " /foo HTTP/1.1\r\n"
										   "Host: localhost\r\n"
										   "Connection: keep-alive\r\n"
										   "Content-Length: 0\r\n"
										   "\r\n";

		int listener_fd = socket(AF_INET, SOCK_STREAM, 0);
		ASSERT_EQ(connect(listener_fd, reinterpret_cast<sockaddr *>(&addr),
						  sizeof(addr)),
				  0);
		send(listener_fd, request.c_str(), request.size(), 0);

		char buffer[1024]{};
		const ssize_t bytes = recv(listener_fd, buffer, sizeof(buffer), 0);
		std::string result = std::string(buffer);

		EXPECT_GT(bytes, 0);
		ASSERT_TRUE(result.find(std::to_string(i)) != std::string::npos);
		ASSERT_TRUE(close(listener_fd) != -1);
	}
}

TEST(HttpServerTest, HandleNonExistentGetRoute) {
	HttpServer server{};
	server.start_listening(HttpServerTest::port);
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(HttpServerTest::port);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	std::string request = "GET /foo HTTP/1.1\r\n"
						  "Host: localhost\r\n"
						  "Connection: keep-alive\r\n"
						  "Content-Length: 0\r\n"
						  "\r\n";

	int listener_fd = socket(AF_INET, SOCK_STREAM, 0);
	ASSERT_EQ(
		connect(listener_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
		0);
	send(listener_fd, request.c_str(), request.size(), 0);

	char buffer[1024]{};
	const ssize_t bytes = recv(listener_fd, buffer, sizeof(buffer), 0);
	std::string result = std::string(buffer);

	EXPECT_GT(bytes, 0);
	ASSERT_TRUE(result.find("404 Not Found") != std::string::npos);
	ASSERT_TRUE(close(listener_fd) != -1);
}

/*
 * This test covers a different branch than HandleNonExistentGetRoute
 * because POST requests can handle the request body
 */
TEST(HttpServerTest, HandleNonExistentPostRoute) {
	HttpServer server{};
	server.start_listening(HttpServerTest::port);
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(HttpServerTest::port);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	std::string request = "POST /foo HTTP/1.1\r\n"
						  "Host: localhost\r\n"
						  "Connection: keep-alive\r\n"
						  "Content-Length: 0\r\n"
						  "\r\n";

	int listener_fd = socket(AF_INET, SOCK_STREAM, 0);
	ASSERT_EQ(
		connect(listener_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
		0);
	send(listener_fd, request.c_str(), request.size(), 0);

	char buffer[1024]{};
	const ssize_t bytes = recv(listener_fd, buffer, sizeof(buffer), 0);
	std::string result = std::string(buffer);

	EXPECT_GT(bytes, 0);
	ASSERT_TRUE(result.find("404 Not Found") != std::string::npos);
	ASSERT_TRUE(close(listener_fd) != -1);
}

TEST(HttpServerTest, HandleNonExistentHttpMethod) {
	HttpServer server{};
	server.start_listening(HttpServerTest::port);
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(HttpServerTest::port);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	std::string request = "FOO /foo HTTP/1.1\r\n"
						  "Host: localhost\r\n"
						  "Connection: keep-alive\r\n"
						  "Content-Length: 0\r\n"
						  "\r\n";

	int listener_fd = socket(AF_INET, SOCK_STREAM, 0);
	ASSERT_EQ(
		connect(listener_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
		0);
	send(listener_fd, request.c_str(), request.size(), 0);

	char buffer[1024]{};
	const ssize_t bytes = recv(listener_fd, buffer, sizeof(buffer), 0);
	std::string result = std::string(buffer);

	EXPECT_GT(bytes, 0);
	ASSERT_TRUE(result.find("500 Error") != std::string::npos);
	ASSERT_TRUE(close(listener_fd) != -1);
}

TEST(HttpServerTest, ListenThrowsIfSocketInvalid) {
	HttpServer server{};
	EXPECT_THROW(server.listen(-1), std::runtime_error);
}
