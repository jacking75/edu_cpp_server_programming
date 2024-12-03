#include "redisHandler.h"
//#include "pch.h"
#include "User.h"
#include <print>


// redis 연결 설정
acl::redis_client* setup_redis() {
    acl::string redis_addr("127.0.0.1:6389");
    acl::redis_client* client = new acl::redis_client(redis_addr.c_str(), 10, 10);

    acl::redis redis;
    redis.set_client(client);

    if (!redis.ping()) {
        std::print("Redis 연결 실패!\n");
        delete client;
        return nullptr;
    }

    std::print("Redis 연결 성공!\n");
    return client;
}

// redis return 반환하는 함수 (주소 입력받기)
acl::redis* setup_redis(std::string_view redis_addr) {
    acl::redis_client* client = new acl::redis_client(redis_addr.data(), 10, 10);
    acl::redis* redis = new acl::redis();
    redis->set_client(client);

    if (!redis->ping()) {
        std::print("Redis 연결 실패!\n");
        delete redis;
        delete client;
        return nullptr;
    }

    std::print("Redis 연결 성공!\n");
    return redis;
}

void handle_string(acl::redis& redis) {
    std::string key, value;
    int ttl = -1;

    std::print("key를 입력하세요: ");
    std::cin >> key;
    std::print("value를 입력하세요: ");
    std::cin >> value;
    std::print("TTL(초 단위로 입력, 기본값 -1: 만료되지 않음): ");
    std::cin >> ttl;

    redis.set(key.c_str(), value.c_str());
    if (ttl > 0) {
        redis.expire(key.c_str(), ttl);
    }
    std::print("String 저장 완료!\n");
}

void handle_get(acl::redis& redis) {
    std::string key;
    acl::string value;

    std::print("key를 입력하세요: ");
    std::cin >> key;

    if (redis.get(key.c_str(), value)) {
        if (value.empty()) {
            std::print("key '{}'에 해당하는 값이 없습니다.\n", key);
        }
        else {
            std::print("key: {}, value: {}\n", key, value.c_str());
        }
    }
    else {
        std::print("Redis에서 key '{}'를 가져오는 중 오류가 발생했습니다.\n", key);
    }
}

void handle_list_push(acl::redis_list& redis) {
    std::string key, value;
    char action;

    std::print("key를 입력하세요: ");
    std::cin >> key;
    std::print("왼쪽(L) 또는 오른쪽(R) 입력을 선택하세요: ");
    std::cin >> action;

    while (true) {
        std::print("리스트에 추가할 값을 입력하세요 (종료하려면 q 입력): ");
        std::cin >> value;
        if (value == "q") break;

        bool success = false;
        if (action == 'L' || action == 'l') {
            success = redis.lpush(key.c_str(), value.c_str(), nullptr) >= 0;
        }
        else if (action == 'R' || action == 'r') {
            success = redis.rpush(key.c_str(), value.c_str(), nullptr) >= 0;
        }

        if (success) {
            std::print("리스트에 값이 성공적으로 추가되었습니다: {}\n", value);
        }
        else {
            std::print("리스트에 값을 추가하는 중 오류가 발생했습니다.\n");
        }
    }
}

void handle_list_get(acl::redis_list& redis) {
    std::string key;
    int start, end;

    std::print("key를 입력하세요: ");
    std::cin >> key;
    std::print("조회 시작 인덱스를 입력하세요: ");
    std::cin >> start;
    std::print("조회 종료 인덱스를 입력하세요: ");
    std::cin >> end;

    std::vector<acl::string> list_values;

    redis.clear();
    if (redis.lrange(key.c_str(), start, end, &list_values)) {
        if (list_values.empty()) {
            std::print("리스트가 비어있습니다.\n");
        }
        else {
            std::print("리스트 값들: \n");
            for (size_t i = 0; i < list_values.size(); ++i) {
                std::print("Index {}: {}\n", start + i, list_values[i].c_str());
            }
        }
    }
    else {
        std::print("Redis에서 리스트 '{}'를 조회하는 중 오류가 발생했습니다.\n", key);
    }
}

void handle_set(acl::redis& redis) {
    std::string key, value;

    std::print("key를 입력하세요: ");
    std::cin >> key;

    while (true) {
        std::print("집합에 추가할 값을 입력하세요 (종료하려면 q 입력): ");
        std::cin >> value;
        if (value == "q") break;

        redis.sadd(key.c_str(), value.c_str());
    }

    std::print("Set 저장 완료!\n");
}

void handle_hash(acl::redis& redis) {
    std::string key, field, value;

    std::print("key를 입력하세요: ");
    std::cin >> key;

    while (true) {
        std::print("해시 필드를 입력하세요 (종료하려면 q 입력): ");
        std::cin >> field;
        if (field == "q") break;

        std::print("해시 필드의 값을 입력하세요: ");
        std::cin >> value;

        redis.hset(key.c_str(), field.c_str(), value.c_str());
    }

    std::print("Hash 저장 완료!\n");
}

void handle_zset(acl::redis& redis) {
    std::string key, value;
    std::map<acl::string, double> members;
    double score;

    std::print("key를 입력하세요: ");
    std::cin >> key;

    acl::string acl_key(key.c_str());

    while (true) {
        std::print("Sorted Set에 추가할 값을 입력하세요 (종료하려면 q 입력): ");
        std::cin >> value;
        if (value == "q") break;

        while (true) {
            std::print("해당 값의 점수를 입력하세요: ");
            std::cin >> score;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::print("유효한 숫자를 입력하세요!\n");
            }
            else {
                break;  // 유효한 숫자가 입력된 경우 루프를 빠져나감
            }
        }

        acl::string acl_value(value.c_str());
        members[acl_value] = score;
    }

    if (!members.empty()) {
        redis.zadd(acl_key, members);
        std::print("Sorted Set 저장 완료!\n");
    }
    else {
        std::print("저장할 멤버가 없습니다.\n");
    }
}



void handle_set_json(acl::redis_client& client) {
    std::string key, name, email;
    int age;

    std::print("Enter key: ");
    std::cin >> key;
    std::print("Enter name: ");
    std::cin >> name;
    std::print("Enter age: ");
    std::cin >> age;
    std::print("Enter email: ");
    std::cin >> email;

    User user = { name, age, email };

    if (set_json_data(client, key, user)) {
        std::print("User data saved successfully in Redis!\n");
    }
    else {
        std::print("Failed to save user data in Redis.\n");
    }
}

void handle_get_json(acl::redis_client& client) {
    std::string key;
    std::print("Enter key: ");
    std::cin >> key;

    try {
        User user = json_to_user(client, key);
        std::print("Name: {}, Age: {}, Email: {}\n", user.name, user.age, user.email);
    }
    catch (const std::exception& e) {
        std::print("{}\n", e.what());
    }
}

bool set_json_data(acl::redis_client& client, std::string_view key, const User& user) {
    acl::json json;
    user_to_json(user, json);

    acl::string json_str;
    json.build_json(json_str);

    acl::redis_command cmd(&client);
    std::vector<const char*> argv = {
        "JSON.SET", key.data(), "$", json_str.c_str() 
    };
    std::vector<size_t> lens = { 
        strlen("JSON.SET"), key.size(), strlen("$"), json_str.size() 
    };

    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), lens.data());
    return result && result->get_type() == acl::REDIS_RESULT_STATUS;
}

[[nodiscard]] std::string get_json_field(acl::redis_client& client, std::string_view key, std::string_view field) {
    acl::redis_command cmd(&client);
    std::vector<const char*> argv = { "JSON.GET", key.data(), field.data() };
    std::vector<size_t> lens = { strlen("JSON.GET"), key.size(), field.size() };

    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), lens.data());

    if (!result || result->get_type() != acl::REDIS_RESULT_STRING) {
        throw std::runtime_error("Failed to retrieve JSON field from Redis");
    }
    return result->get(0);
}

void set_with_ttl(acl::redis& cmd, std::string_view key, std::string_view value, int ttl) {
    if (!cmd.setex(key.data(), value.data(), ttl)) {
        std::print("Failed to set key with TTL in Redis\n");
    }
    else {
        std::print("Data saved with {} seconds TTL.\n", ttl);
    }
}

void get_value(acl::redis& cmd, std::string_view key) {
    acl::string value;
    if (cmd.get(key.data(), value)) {
        if (value.empty()) {
            std::print("key '{}'에 해당하는 값이 존재하지 않습니다.\n", key);
        }
        else {
            std::print("Value for key '{}': {}\n", key, value.c_str());
        }
    }
    else {
        std::print("Redis에서 key '{}'를 가져오는 중 오류가 발생했습니다.\n", key);
    }
}
