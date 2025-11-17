#include "../include/HttpServer.h"

int main() {

    HttpServer server {};

    server.get_mapping("/test", [](const HttpServer::Request&, HttpServer::Response& res){
        res.body = "testing new api route";
    });

    server.get_mapping("/test2", [](const HttpServer::Request&, HttpServer::Response& res){
        res.body = "this is the other route";
    });

    server.post_mapping("/post", [](const HttpServer::Request& req, HttpServer::Response& res){
        res.body = "post api route: " + req.body;
    });

    server.listen(3490);
}
