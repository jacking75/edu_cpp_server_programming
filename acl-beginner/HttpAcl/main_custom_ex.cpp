#include "acl_cpp/lib_acl.hpp"
#include "fiber/http_server.hpp"
#include <print>

int main_sample() {
    acl::acl_cpp_init(); // ACL 라이브러리 초기화
    const char* addr = "0.0.0.0:8080"; // 서버 주소와 포트 설정

    // HTTP 서버 객체 생성
    acl::http_server server;

    // GET /
    server.Get("/", [](acl::HttpRequest&, acl::HttpResponse& res) {
        std::string response = "Hello, World!";
        res.setContentType("text/plain");
        res.setContentLength(response.size());
        return res.write(response.c_str(), response.size());
    });

    // POST /data
    server.Post("/data", [](acl::HttpRequest& req, acl::HttpResponse& res) {
        acl::string* body = req.getBody();
        std::string response = "Received: ";
        if (body != nullptr) {
            response += body->c_str(); // acl::string을 const char*로 변환하여 사용
        }
        else {
            response += "(no data)";
        }
        res.setContentType("text/plain");
        res.setContentLength(response.size());
        return res.write(response.c_str(), response.size());
    });

    // POST /json
    server.Post("/json", [](acl::HttpRequest& req, acl::HttpResponse& res) {
        acl::string* body = req.getBody();
        std::string body_content = body ? body->c_str() : "(empty)";
        std::cout << "Request Body: " << body_content << std::endl;

        // 직접 JSON 파싱 시도
        acl::json json;
        if (json.update(body_content.c_str())) { // JSON을 수동으로 파싱
            std::cout << "Parsed JSON: " << json.to_string() << std::endl;
            return res.write(json); // JSON 응답 전송
        }
        else {
            std::string buf = "failed to parse json\r\n";
            std::cout << "Failed to parse JSON" << std::endl;
            res.setContentLength(buf.size());
            return res.write(buf.c_str(), buf.size());
        }
    });


    // 서버 실행
    server.run_alone(addr);

    return 0;
}
