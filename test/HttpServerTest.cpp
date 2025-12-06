#include "HttpServer.h"
#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <string>

class HttpServerTest : public ::testing::Test {};

TEST(ServerTest, ConstructorDestructorTest) {
    HttpServer server {};
}

TEST(HttpServerTest, ServerStartsAndAcceptsRequests) {
    HttpServer server {};

    // Start server in non-blocking mode
    server.start_listening(8080);

    // Send real HTTP request using curl
    int result = system("curl -s http://localhost:8080 > /dev/null");

    EXPECT_EQ(result, 0);
}

TEST(HttpServerTest, AcceptsHttpRequest) {
    HttpServer server;
    server.start_listening(8081);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8081);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ASSERT_EQ(connect(sock, (sockaddr*)&addr, sizeof(addr)), 0);

    const char* request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    send(sock, request, strlen(request), 0);

    char buffer[1024];
    int bytes = recv(sock, buffer, sizeof(buffer), 0);

    EXPECT_GT(bytes, 0);  // Server sent response

    close(sock);
}

TEST(HttpServerTest, AcceptGetRequest) {
    HttpServer server;
    server.get_mapping("/hello", [](const HttpServer::Request&, HttpServer::Response& res){
        res.body = "hello, world";
    });
    server.start_listening(8082);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8082);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ASSERT_EQ(connect(sock, (sockaddr*)&addr, sizeof(addr)), 0);

    const char* request = "GET /hello HTTP/1.1\r\n\r\n";
    send(sock, request, strlen(request), 0);

    char buffer[1024] {};
    int bytes = recv(sock, buffer, sizeof(buffer), 0);
    std::string result = std::string(buffer);

    EXPECT_GT(bytes, 0);
    ASSERT_TRUE(result.find("hello, world") != std::string::npos);

    close(sock);
}
