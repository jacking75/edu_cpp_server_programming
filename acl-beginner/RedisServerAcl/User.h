#pragma once
#include <string>
#include <acl_cpp/lib_acl.hpp>

// User 구조체 정의
struct User {
    std::string name;
    int age;
    std::string email;
};

// JSON 변환 관련 함수 선언
void user_to_json(const User& user, acl::json& json);
[[nodiscard]] User json_to_user(acl::redis_client& client, std::string_view key);
