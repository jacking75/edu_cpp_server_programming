//#include "pch.h"
#include "redisHandler.h"
#include <print>

int main() {
    acl::acl_cpp_init();
    acl::log::stdout_open(true);

    // Redis 설정 함수 호출
    std::string_view redis_addr = "127.0.0.1:6389";  // RedisJSON 서버 주소
    acl::redis* redis = setup_redis(redis_addr);
    if (!redis) {
        return -1;  // Redis 연결 실패 시 종료
    }

    acl::redis_client* client = redis->get_client();  // redis_client 객체 가져오기

    while (true) {
        std::string command;
        std::print("\n[Redis Test with Acl] 명령어를 입력하세요 (set, get, list_push, list_get, hash, zset, set_json, get_json, exit): ");
        std::cin >> command;

        if (command == "exit") {
            break;
        }
        else if (command == "set") {
            handle_string(*redis);
        }
        else if (command == "get") {
            handle_get(*redis);
        }
        else if (command == "list_push") {
            handle_list_push(*redis);
        }
        else if (command == "list_get") {
            handle_list_get(*redis);
        }
        else if (command == "hash") {
            handle_hash(*redis);
        }
        else if (command == "zset") {
            handle_zset(*redis);
        }
        else if (command == "set_json") {
            handle_set_json(*client);
        }
        else if (command == "get_json") {
            handle_get_json(*client);
        }
        else {
            std::print("지원하지 않는 명령어입니다.\n");
        }
    }

    return 0;
}
