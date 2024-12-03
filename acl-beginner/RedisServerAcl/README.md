## Acl을 활용하여 Redis 실습 프로젝트

### 목차
1. [Acl Redis](#acl-redis)
2. [실습 목표](#실습-목표)
3. [코드 설명](#코드-설명)
    1. [Redis 연결 설정](#1-redis-연결-설정)
    2. [String 자료형 실습](#2-string-자료형-실습)
    3. [List 자료형 실습](#3-list-자료형-실습)
    4. [Set 자료형 실습](#4-set-자료형-실습)
    5. [Hash 자료형 실습](#5-hash-자료형-실습)
    6. [Sorted Set (ZSet) 자료형 실습](#6-sorted-set-zset-자료형-실습)
4. [Redis JSON 사용](#redis-json-사용)
    1. [User 구조체와 JSON 변환](#1-user-구조체와-json-변환)
    2. [Redis JSON 데이터 저장](#2-redis-json-데이터-저장)
    3. [Redis JSON 데이터 가져오기](#3-redis-json-데이터-가져오기)
5. [코드 예제 실행](#코드-예제-실행)
6. [리팩토링 C++23](#리팩토링-c23)

---

## Acl Redis

* [🖱️Acl redis - One C++ redis client library in Acl](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md)
* ACL은 다양한 기능을 제공하는 C++ 라이브러리로, 네트워크 프로그래밍과 코루틴, **Redis**와 같은 데이터베이스와의 상호작용 등을 쉽게 할 수 있게 해줍니다.
* Redis에 대한 자세한 설명이 필요하시다면 ["📄Redis가 지원하는 자료형 with Acl"](../Document/Acl-Redis-Data.md) 문서를 참고하세요.

---

## 실습 목표

1. Redis와 연결 설정: Redis 서버에 연결하고 기본 설정을 확인합니다.
2. String 자료형 실습: 키-값 쌍을 저장하고 TTL(Time To Live)을 설정하며 값을 조회합니다.
3. List 자료형 실습: 리스트에 값을 추가하고 특정 범위의 값을 조회합니다.
4. Set 자료형 실습: 집합(Set)에 값을 추가하고 조회합니다.
5. Hash 자료형 실습: 해시(Hash)에 필드와 값을 저장하고 조회합니다.
6. Sorted Set (ZSet) 자료형 실습: 정렬된 집합에 값과 점수를 저장하고 조회합니다.
7. Redis JSON 사용: JSON 데이터를 Redis에 저장하고 가져옵니다.


### 프로젝트 구조

```
RedisServerAcl/
├── main.cpp                # 메인 파일로, 프로그램의 시작점 및 주요 실행 흐름을 포함
├── redisHandler.cpp        # Redis와의 상호작용을 위한 함수 구현 파일
├── redisHandler.h          # Redis 관련 함수 선언을 포함한 헤더 파일
├── User.cpp                # User 구조체와 관련된 JSON 변환 함수 구현 파일
├── User.h                  # User 구조체 선언 및 관련 함수 선언 파일
├── pch.h                   # 프로젝트 전반에서 사용되는 공통 헤더 파일 (프리컴파일 헤더)

```

---

## 코드 설명

### 1. Redis 연결 설정

#### `redisHandler.h`
```cpp
#include <iostream>
#include <acl_cpp/lib_acl.hpp>

// Redis 연결 설정
acl::redis_client* setup_redis();
acl::redis* setup_redis(const char* redis_addr);
```

#### `redisHandler.cpp`
```cpp
#include "redisHandler.h"
#include <iostream>

acl::redis_client* setup_redis() {
    acl::string redis_addr("127.0.0.1:6379");
    acl::redis_client* client = new acl::redis_client(redis_addr.c_str(), 10, 10);

    acl::redis redis;
    redis.set_client(client);

    if (redis.ping() == false) {
        std::cerr << "Redis 연결 실패!" << std::endl;
        delete client;
        return nullptr;
    }

    std::cout << "Redis 연결 성공!" << std::endl;
    return client;
}
```

- **`setup_redis()`**: Redis 서버에 연결하는 함수입니다. 연결에 성공하면 클라이언트를 반환하고, 실패하면 에러 메시지를 출력합니다.

---

### 2. String 자료형 실습

#### `redisHandler.h`
```cpp
void handle_string(acl::redis& redis);
void handle_get(acl::redis& redis);
```

#### `redisHandler.cpp`
```cpp
void handle_string(acl::redis& redis) {
    std::string key, value;
    int ttl = -1;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;
    std::cout << "value를 입력하세요: ";
    std::cin >> value;
    std::cout << "TTL(초 단위로 입력, 기본값 -1: 만료되지 않음): ";
    std::cin >> ttl;

    acl::string acl_key(key.c_str());
    acl::string acl_value(value.c_str());

    redis.set(acl_key, acl_value);
    if (ttl > 0) {
        redis.expire(acl_key, ttl);
    }
    std::cout << "String 저장 완료!" << std::endl;
}

void handle_get(acl::redis& redis) {
    std::string key;
    acl::string value;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;

    acl::string acl_key(key.c_str());

    if (redis.get(acl_key, value)) {
        if (value.empty()) {
            std::cout << "key '" << acl_key.c_str() << "'에 해당하는 값이 없습니다." << std::endl;
        }
        else {
            std::cout << "key: " << acl_key.c_str() << ", value: " << value.c_str() << std::endl;
        }
    }
    else {
        std::cerr << "Redis에서 key '" << acl_key.c_str() << "'를 가져오는 중 오류가 발생했습니다." << std::endl;
    }
}
```

- **`handle_string()`**: 사용자가 입력한 키와 값을 Redis에 저장합니다. TTL을 설정하여 데이터의 만료 시간을 지정할 수 있습니다.
- **`handle_get()`**: 사용자가 입력한 키를 사용해 Redis에 저장된 값을 조회하고 출력합니다.

---

### 3. List 자료형 실습

#### `redisHandler.h`
```cpp
void handle_list_push(acl::redis_list& redis);
void handle_list_get(acl::redis_list& redis);
```

#### `redisHandler.cpp`
```cpp
void handle_list_push(acl::redis_list& redis) {
    std::string key, value;
    char action;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;
    std::cout << "왼쪽(L) 또는 오른쪽(R) 입력을 선택하세요: ";
    std::cin >> action;

    acl::string acl_key(key.c_str());

    while (true) {
        std::cout << "리스트에 추가할 값을 입력하세요 (종료하려면 q 입력): ";
        std::cin >> value;
        if (value == "q") break;

        acl::string acl_value(value.c_str());

        bool success = false;
        if (action == 'L' || action == 'l') {
            success = redis.lpush(acl_key, acl_value, NULL) >= 0;
        }
        else if (action == 'R' || action == 'r') {
            success = redis.rpush(acl_key, acl_value, NULL) >= 0;
        }

        if (success) {
            std::cout << "리스트에 값이 성공적으로 추가되었습니다: " << acl_value.c_str() << std::endl;
        }
        else {
            std::cerr << "리스트에 값을 추가하는 중 오류가 발생했습니다." << std::endl;
        }
    }
}

void handle_list_get(acl::redis_list& redis) {
    std::string key;
    int start, end;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;
    std::cout << "조회 시작 인덱스를 입력하세요: ";
    std::cin >> start;
    std::cout << "조회 종료 인덱스를 입력하세요: ";
    std::cin >> end;

    acl::string acl_key(key.c_str());
    std::vector<acl::string> list_values;

    redis.clear();
    if (redis.lrange(acl_key.c_str(), start, end, &list_values)) {
        if (list_values.empty()) {
            std::cout << "리스트가 비어있습니다." << std::endl;
        }
        else {
            std::cout << "리스트 값들: " << std::endl;
            for (size_t i = 0; i < list_values.size(); ++i) {
                std::cout << "Index " << start + i << ": " << list_values[i].c_str() << std::endl;
            }
        }
    }
    else {
        std::cerr << "Redis에서 리스트 '" << acl_key.c_str() << "'를 조회하는 중 오류가 발생했습니다." << std::endl;
    }
}
```

- **`handle_list_push()`**: 사용자가 입력한 값을 리스트의 왼쪽(LPUSH) 또는 오른쪽(RPUSH)에 추가합니다.
- **`handle_list_get()`**: 사용자가 지정한 범위 내에서 리스트의 값을 조회하고 출력합니다.

---

### 4. Set 자료형 실습

#### `redisHandler.h`
```cpp
void handle_set(acl::redis& redis);
```

#### `redisHandler.cpp`
```cpp
void handle_set(acl::redis& redis) {
    std::string key, value;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;

    acl::string acl_key(key.c_str());

    while (true) {
        std::cout << "

집합에 추가할 값을 입력하세요 (종료하려면 q 입력): ";
        std::cin >> value;
        if (value == "q") break;

        acl::string acl_value(value.c_str());
        redis.sadd(acl_key, acl_value);
    }

    std::cout << "Set 저장 완료!" << std::endl;
}
```

- **`handle_set()`**: 사용자가 입력한 값을 Redis의 Set에 추가합니다. `q`를 입력하면 종료됩니다.

---

### 5. Hash 자료형 실습

#### `redisHandler.h`
```cpp
void handle_hash(acl::redis& redis);
```

#### `redisHandler.cpp`
```cpp
void handle_hash(acl::redis& redis) {
    std::string key, field, value;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;

    acl::string acl_key(key.c_str());

    while (true) {
        std::cout << "해시 필드를 입력하세요 (종료하려면 q 입력): ";
        std::cin >> field;
        if (field == "q") break;

        std::cout << "해시 필드의 값을 입력하세요: ";
        std::cin >> value;

        acl::string acl_field(field.c_str());
        acl::string acl_value(value.c_str());

        redis.hset(acl_key, acl_field, acl_value);
    }

    std::cout << "Hash 저장 완료!" << std::endl;
}
```

- **`handle_hash()`**: 해시 자료형에 필드와 값을 저장합니다. 필드와 값을 반복적으로 입력할 수 있습니다.

---

### 6. Sorted Set (ZSet) 자료형 실습

#### `redisHandler.h`
```cpp
void handle_zset(acl::redis& redis);
```

#### `redisHandler.cpp`
```cpp
void handle_zset(acl::redis& redis) {
    std::string key, value;
    std::map<acl::string, double> members;
    double score;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;

    acl::string acl_key(key.c_str());

    while (true) {
        std::cout << "Sorted Set에 추가할 값을 입력하세요 (종료하려면 q 입력): ";
        std::cin >> value;
        if (value == "q") break;

        std::cout << "해당 값의 점수를 입력하세요: ";
        std::cin >> score;

        acl::string acl_value(value.c_str());
        members[acl_value] = score;
    }

    if (!members.empty()) {
        redis.zadd(acl_key, members);
        std::cout << "Sorted Set 저장 완료!" << std::endl;
    }
    else {
        std::cout << "저장할 멤버가 없습니다." << std::endl;
    }
}
```

- **`handle_zset()`**: 정렬된 집합(ZSet)에 값과 점수를 추가합니다. 점수는 멤버의 순서를 결정하는데 사용됩니다.

---

## Redis JSON 사용

Redis는 JSON 데이터를 저장하기 위한 별도의 RedisJSON 모듈을 지원합니다. <br>
이 모듈을 활용하면 구조체 데이터를 JSON 형식으로 Redis에 저장하고 조회할 수 있습니다. 
<br>
RedisJSON 모듈에 대한 설명이 궁금하다면 [📄Redis JSON이란?](../Document/Acl-Redis-Json.md) 문서를 참고하세요. <br>
이번 실습에서는 Acl을 통해 `User` 구조체를 JSON으로 변환하고, 이를 Redis에 저장하거나 조회하는 방법을 다룹니다.
<br>

ACL의 JSON 모듈과 acl::redis_command 클래스는 Redis와 JSON 데이터를 쉽게 다룰 수 있도록 도와줍니다.

- JSON 모듈: User 구조체를 ACL의 acl::json 객체로 변환하고, 이 객체를 Redis에 저장하는 데 사용됩니다.
- Redis Command: acl::redis_command 객체는 JSON.SET, JSON.GET 같은 Redis 명령어를 실행하는 데 사용되며, JSON 데이터를 효율적으로 저장하고 조회할 수 있도록 지원합니다.

### 1. User 구조체와 JSON 변환

먼저, 구조체를 JSON으로 변환하는 함수를 정의합니다. ACL 라이브러리의 JSON 기능을 활용하여, `User` 구조체의 데이터를 ACL JSON 객체로 변환합니다. <br>
이는 Redis에 구조체 데이터를 저장하기 위한 준비 단계입니다.

#### `User.h`
```cpp
#ifndef USER_H
#define USER_H

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
User json_to_user(acl::redis_client& client, const std::string& key);

#endif // USER_H
```

#### `User.cpp`
```cpp
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
User json_to_user(acl::redis_client& client, const std::string& key) {
    User user;

    user.name = get_json_field(client, key, "name");
    user.age = std::stoi(get_json_field(client, key, "age"));
    user.email = get_json_field(client, key, "email");

    return user;
}
```

- **`user_to_json()`**: `User` 구조체를 ACL의 JSON 객체로 변환합니다. `acl::json` 객체의 루트에 구조체 데이터를 추가합니다.
- **`json_to_user()`**: Redis에서 개별 JSON 필드를 가져와 `User` 구조체로 변환합니다.

---

### 2. Redis JSON 데이터 저장
이 과정에서는 ACL의 `acl::redis_command` 클래스를 사용하여 Redis의 `JSON.SET` 명령을 실행하고, 변환된 JSON 데이터를 Redis에 저장합니다.

#### `redisHandler.h`
```cpp
bool set_json_data(acl::redis_client& client, const std::string& key, const User& user);
```

#### `redisHandler.cpp`
```cpp
bool set_json_data(acl::redis_client& client, const std::string& key, const User& user) {
    acl::json json;
    user_to_json(user, json);

    acl::string json_str;
    json.build_json(json_str);

    acl::redis_command cmd(&client);
    std::vector<const char*> argv = { "JSON.SET", key.c_str(), "$", json_str.c_str() };
    std::vector<size_t> lens = { strlen("JSON.SET"), key.size(), strlen("$"), json_str.size() };

    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), lens.data());
    return result && result->get_type() == acl::REDIS_RESULT_STATUS;
}
```

- **`set_json_data()`**: `User` 구조체를 JSON으로 변환한 후, 이를 Redis에 저장합니다. ACL의 `acl::redis_command` 객체를 사용해 `JSON.SET` 명령어를 실행합니다. 
- **`acl::redis_command`**: 이 클래스는 Redis 명령어를 실행하는 데 사용됩니다. `cmd.request()` 함수를 호출하여 Redis의 `JSON.SET` 명령어를 전달하고, 결과를 처리합니다.

---

### 3. Redis JSON 데이터 가져오기

#### `redisHandler.h`
```cpp
std::string get_json_field(acl::redis_client& client, const std::string& key, const std::string& field);
```

#### `redisHandler.cpp`
```cpp
std::string get_json_field(acl::redis_client& client, const std::string& key, const std::string& field) {
    acl::redis_command cmd(&client);
    std::vector<const char*> argv = { "JSON.GET", key.c_str(), field.c_str() };
    std::vector<size_t> lens = { strlen("JSON.GET"), key.size(), field.size() };

    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), lens.data());

    if (!result || result->get_type() != acl::REDIS_RESULT_STRING) {
        throw std::runtime_error("Failed to retrieve JSON field from Redis");
    }
    return result->get(0);
}
```

- **`get_json_field()`**
      + Redis에 저장된 JSON 데이터에서 특정 필드를 가져오는 함수입니다.
      + `acl::redis_command` 객체를 사용하여 `JSON.GET` 명령을 사용하여 Redis에서 필드 값을 조회합니다.
- **`acl::redis_command`**: 이 클래스는 `cmd.request()` 함수로 `JSON.GET` 명령을 실행하고, Redis로부터 해당 필드의 값을 받아옵니다.

---

이처럼 ACL의 기능을 활용하면 Redis와의 상호작용을 보다 효율적으로 수행할 수 있으며, 특히 구조체와 같은 복잡한 데이터 타입도 쉽게 처리할 수 있습니다.

---

## 코드 예제 실행

### `main.cpp`
```cpp
#include <iostream>
#include "redisHandler.h"

int main() {
    acl::acl_cpp_init();  // ACL 초기화

    acl::redis_client* client = setup_redis();
    if (!client) {
        return -1;
    }

    acl::redis redis;
    redis.set_client(client);

    while (true) {
        std::string command;
        std::cout << "\n[Redis Test with Acl] 명령어를 입력하세요 (set, get, list_push, list_get, hash, zset, set_json, get_json, exit): ";
        std::cin >> command;

        if (command == "exit") {
            break;
        }
        else if (command == "set") {
            handle_string(redis);
        }
        else if (command == "get") {
            handle_get(redis);
        }
        else if (command == "list_push") {
            handle_list_push(redis);
        }
        else if (command == "list_get") {
            handle_list_get(redis);
        }
        else if (command == "hash") {
            handle_hash(redis);
        }
        else if (command == "zset") {
            handle_zset(redis);
        }
        else if (command == "set_json") {
            handle_set_json(*client);
        }
        else if (command == "get_json") {
            handle_get_json(*client);
        }
        else {
            std::cout << "지원하지 않는 명령어입니다." << std::endl;
        }
    }

    delete client;
    return 0;
}
```

- **`acl::acl_cpp_init()`**: ACL 라이브러리 초기화 함수입니다. 이 함수를 호출하여 ACL 내부의 여러 기능을 사용할 준비를 합니다.

---

이 코드를 통해 다양한 Redis 자료형을 연습하고, 데이터를 저장하고 조회하는 실습을 진행할 수 있습니다. <br>
JSON 데이터를 사용하는 부분에서는 `User` 구조체를 ACL의 JSON 객체로 변환하고 이를 Redis에 저장하는 방법을 실습할 수 있습니다.

---


## 리팩토링 C++23

이 리팩토링 작업을 통해 Redis 관련 코드가 최신 C++23 표준에 맞게 개선되었습니다. 
- `std::print`를 통한 출력 간소화
- `std::string_view`를 활용한 메모리 최적화

#### 1. **`std::print`로 출력 메시지 변경**

- 기존의 `std::cout`를 사용하여 출력하던 부분을 `std::print`로 대체했습니다.
- `std::print`는 C++23에서 도입된 새로운 출력 함수로, 서식화된 출력이 간결하고 직관적으로 작성될 수 있도록 도와줍니다.

**예시 변경 사항**:
- **이전 코드**:
  ```cpp
  std::cout << "key를 입력하세요: ";
  std::cin >> key;
  ```
- **변경 후**:
  ```cpp
  std::print("key를 입력하세요: ");
  std::cin >> key;
  ```

- **이전 코드**:
  ```cpp
  std::cout << "Sorted Set 저장 완료!" << std::endl;
  ```
- **변경 후**:
  ```cpp
  std::print("Sorted Set 저장 완료!\n");
  ```

#### 2. **문자열 처리 방식 통일**

- `std::string`을 사용하여 사용자 입력을 받아 처리한 후, Redis에 키나 값을 저장할 때 `acl::string`으로 변환하는 방식으로 일관되게 처리했습니다.
- `std::string_view`를 활용하여 함수 인자로 문자열을 전달할 때 메모리 복사와 할당을 최소화했습니다.

**예시 변경 사항**:
- **이전 코드**:
  ```cpp
  acl::string acl_key(key.c_str());
  ```
- **변경 후**:
  ```cpp
  // 해당 부분은 std::string -> acl::string 변환 부분 그대로 유지
  ```

#### 3. **코드의 가독성 및 일관성 향상**

- 코드의 가독성을 높이기 위해 출력 메시지에 대한 서식을 통일했습니다.
- `std::print`를 사용하면서, 각 메시지의 끝에 `\n`을 추가하여 줄바꿈을 명확히 표현했습니다.

**예시 변경 사항**:
- **이전 코드**:
  ```cpp
  std::cout << "리스트에 값이 성공적으로 추가되었습니다: " << value << std::endl;
  ```
- **변경 후**:
  ```cpp
  std::print("리스트에 값이 성공적으로 추가되었습니다: {}\n", value);
  ```

#### 4. **기타 개선 사항**

- **오류 메시지 개선**: 기존의 `std::cerr` 부분은 그대로 유지하되, 오류 메시지 출력을 `std::print`로 변경할 수 있는 부분에서는 변경했습니다.
- **`[[nodiscard]]` 적용**: 반환 값을 무시해서는 안 되는 중요한 함수들에 `[[nodiscard]]` 속성을 추가하여, 실수로 반환 값을 무시하는 상황을 방지했습니다.




---

## 추가 자료

Acl Redis를 더욱 깊이 이해하고 사용하려면 아래 링크들을 참조하세요.

- [🖱️cluster example for redis3.0+](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md#22-redis-client-cluster-example-for-redis30)
- [🖱️Using redis client cluster in multi-threads](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md#23-using-redis-client-cluster-in-multi-threads)
- [🖱️redis pipeline in multi-threads](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md#24-use-redis-pipeline-in-multi-threads)
