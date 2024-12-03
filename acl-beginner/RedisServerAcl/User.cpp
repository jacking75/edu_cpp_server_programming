#include "User.h"
#include <iostream>
#include <stdexcept>
#include "redisHandler.h"

// User 구조체를 ACL JSON 객체로 변환하는 함수
void user_to_json(const User& user, acl::json& json) {
    acl::json_node& root = json.get_root();
    root.add_text("name", user.name.c_str());
    root.add_number("age", user.age);
    root.add_text("email", user.email.c_str());
}

// Redis에서 전체 User 구조체 데이터를 가져오는 함수
[[nodiscard]] User json_to_user(acl::redis_client& client, std::string_view key) {
    User user;

    user.name = get_json_field(client, key, "name");
    user.age = std::stoi(get_json_field(client, key, "age"));
    user.email = get_json_field(client, key, "email");

    return user;
}
