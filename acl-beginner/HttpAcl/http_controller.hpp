#pragma once
#include "acl_cpp/lib_acl.hpp"
#include "http_service.hpp"

class HttpController {
public:
    void configure_routes(acl::http_server& server) {
        server.Get("/", [](acl::HttpRequest&, acl::HttpResponse& res) {
            HttpService service;
            std::string response = service.process_root_request();
            res.setContentLength(response.size());
            return res.write(response.c_str(), response.size());
            });

        server.Post("/json", [](acl::HttpRequest& req, acl::HttpResponse& res) {
            HttpService service;
            acl::json* json = req.getJson();
            std::string response = service.process_json_request(json);
            res.setContentLength(response.size());
            return res.write(response.c_str(), response.size());
            });
    }
};
