#pragma once
#include <string>
#include "acl_cpp/lib_acl.hpp"
#include "http_model.hpp"

class HttpService {
public:
    std::string process_root_request() {
        return "hello world1!\r\n";
    }

    std::string process_json_request(acl::json* json) {
        if (json) {
            return json->to_string();
        }
        else {
            return "no json got\r\n";
        }
    }
};
