# Redis 주요 기능 정리 및 ACL 실습 코드

이 문서에서는 Redis의 주요 기능을 소개하고, ACL 라이브러리를 사용하여 해당 기능을 실습하는 방법을 설명합니다. <br>
[Redis](https://redis.io)는 고성능의 인메모리 데이터 저장소로, 다양한 자료구조를 지원하여 빠르고 유연한 데이터 처리를 제공합니다. <br>
각 기능별로 Redis의 주요 명령어를 설명하고, ACL 라이브러리를 이용한 실습 코드를 함께 정리하였습니다. <br>
- **테스트 가능한 실습 코드는 [📁RedisServerAcl 디렉토리](../RedisServerAcl)에 위치하고 있습니다!**

## 목차
1. Redis 개요
2. Redis 주요 자료구조 및 명령어
   - 2.1 String
   - 2.2 List
   - 2.3 Set
   - 2.4 Hash
   - 2.5 Sorted Set (ZSet)
3. ACL을 활용한 Redis 실습 코드
   - 3.1 Redis 연결 설정
   - 3.2 String 자료형 실습
   - 3.3 List 자료형 실습
   - 3.4 Set 자료형 실습
   - 3.5 Hash 자료형 실습
   - 3.6 Sorted Set (ZSet) 자료형 실습

---

## 1. Redis 개요

Redis는 인메모리 데이터베이스로, 고속의 데이터 읽기 및 쓰기가 가능합니다. 또한 다양한 자료구조를 지원하여 캐싱, 메시지 브로커, 순위 시스템 등 여러 분야에 활용됩니다.

주요 자료구조로는 **String**, **List**, **Set**, **Hash**, **Sorted Set(ZSet)** 등이 있으며, 각 자료구조는 특정한 목적을 위해 사용됩니다.

[🌐Redis 자료구조](https://redis.io/docs/latest/develop/data-types/)

---

## 2. Redis 주요 자료구조 및 명령어

### 2.1 **String**
- **설명**: Redis의 기본 자료구조로, 단순한 문자열 값을 저장하는데 사용됩니다.
- **명령어**:
  - `SET key value`: 문자열 값을 저장
  - `GET key`: 문자열 값을 조회
  - **예시**:
    ```redis
    SET user:1 "John"
    GET user:1  // 결과: John
    ```

#### 2.2 **List**
- **설명**: 연결 리스트로, FIFO(First-In, First-Out) 또는 LIFO(Last-In, First-Out) 구조로 데이터를 처리합니다.
- **명령어**:
  - `LPUSH key value`: 리스트의 왼쪽(앞)에 값 추가
  - `RPUSH key value`: 리스트의 오른쪽(뒤)에 값 추가
  - `LPOP key`: 리스트의 왼쪽(앞)의 값 제거 및 반환
  - **예시**:
    ```redis
    LPUSH tasks "Task1"
    LPUSH tasks "Task2"
    LPOP tasks  // 결과: Task2
    ```

#### 2.3 **Set**
- **설명**: 중복 없는 집합을 관리하는 자료구조로, 순서가 없는 유일한 값들의 집합입니다.
- **명령어**:
  - `SADD key value`: 집합에 값 추가
  - `SMEMBERS key`: 집합의 모든 값을 반환
  - **예시**:
    ```redis
    SADD fruits "Apple"
    SADD fruits "Banana"
    SMEMBERS fruits  // 결과: Apple, Banana
    ```

#### 2.4 **Hash**
- **설명**: 필드와 값의 쌍으로 이루어진 해시 테이블을 저장합니다.
- **명령어**:
  - `HSET key field value`: 해시의 필드에 값 저장
  - `HGET key field`: 필드의 값 조회
  - **예시**:
    ```redis
    HSET user:1 name "John"
    HSET user:1 age 30
    HGET user:1 name  // 결과: John
    ```

#### 2.5 **Sorted Set (ZSet)**
- **설명**: 값에 점수를 할당하여 순서대로 정렬되는 집합입니다.
- **명령어**:
  - `ZADD key score value`: 정렬된 집합에 점수와 함께 값 추가
  - `ZRANGE key start stop`: 범위 내의 값을 조회
  - **예시**:
    ```redis
    ZADD leaderboard 100 "Player1"
    ZADD leaderboard 200 "Player2"
    ZRANGE leaderboard 0 -1  // 결과: Player1, Player2
    ```

---

### 3. ACL을 활용한 Redis 실습 코드

**테스트 가능한 실습 코드는 [📁RedisServerAcl 디렉토리](../RedisServerAcl)에 위치하고 있습니다!**
- [Redis 실습 acl github 소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/redis)

<br>

#### 3.1 **Redis 연결 설정**
ACL 라이브러리를 사용하여 Redis 서버에 연결하는 코드입니다.

```cpp
#include "acl_cpp/lib_acl.hpp"

int main() {
    // Redis 연결 설정
    acl::redis_client client("127.0.0.1:6379", 10, 10);
    acl::redis redis;
    redis.set_client(&client);

    // 연결 확인
    if (redis.ping() == false) {
        std::cerr << "Failed to connect to Redis" << std::endl;
        return -1;
    }
    std::cout << "Connected to Redis!" << std::endl;

    return 0;
}
```

#### 3.2 **String 자료형 실습**
String 자료형의 SET, GET 명령을 사용한 예제입니다.

```cpp
// String 값 설정
redis.set("key1", "value1");
// String 값 조회
std::string value;
redis.get("key1", value);
std::cout << "key1: " << value << std::endl;
```

#### 3.3 **List 자료형 실습**
List 자료형의 LPUSH, LPOP 명령을 사용한 예제입니다.

```cpp
// 리스트 값 추가
redis.lpush("tasks", "Task1", NULL);
redis.lpush("tasks", "Task2", NULL);

// TTL 추가
redis.expire(tasks, ttl);

// 리스트 값 가져오기
std::string task;
redis.lpop("tasks", task);
std::cout << "Next task: " << task << std::endl;
```

#### 3.4 **Set 자료형 실습**
Set 자료형의 SADD, SMEMBERS 명령을 사용한 예제입니다.

```cpp
// 집합에 값 추가
redis.sadd("fruits", "Apple");
redis.sadd("fruits", "Banana");

// 집합 값 가져오기
acl::string buf;
acl::redis_result result;
redis.smembers("fruits", result);

for (size_t i = 0; i < result.size(); ++i) {
    std::cout << result.get(i).c_str() << std::endl;
}
```

#### 3.5 **Hash 자료형 실습**
Hash 자료형의 HSET, HGET 명령을 사용한 예제입니다.

```cpp
// 해시에 값 추가
redis.hset("user:1", "name", "John");
redis.hset("user:1", "age", "30");

// 해시에서 값 가져오기
std::string name;
redis.hget("user:1", "name", name);
std::cout << "User name: " << name << std::endl;
```

#### 3.6 **Sorted Set (ZSet) 자료형 실습**
Sorted Set의 ZADD, ZRANGE 명령을 사용한 예제입니다.

```cpp
// 정렬된 집합에 값 추가
redis.zadd("leaderboard", 100, "Player1");
redis.zadd("leaderboard", 200, "Player2");

// 정렬된 집합 값 조회
acl::redis_result result;
redis.zrange("leaderboard", 0, -1, result);

for (size_t i = 0; i < result.size(); ++i) {
    std::cout << result.get(i).c_str() << std::endl;
}
```

