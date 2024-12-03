# 예제 코드 및 설명 문서
* [🖱️lib_fiber 예제 소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c++)

<br>

## 빠른 시작 예제
출처 : [🖱️README.md#Quick-Start](https://github.com/acl-dev/acl?tab=readme-ov-file#4-quick-start)
* The first demo with Acl
  
* Simple TCP server
    ```cpp 
    #include <thread>
    #include "pch.h"
    
    void run(void) {
        const char* addr = "127.0.0.1:8088";
        acl::server_socket server; // server_socket 클래스 인스턴스 생성
        if (!server.open(addr)) {  // addr 주소로 서버 소켓을 열고, Bind해서 클라이언트의 연결 요청을 대기 상태(listen)로 만들기
            return; // 실패 시 바로 종료
        }
    
        while (true) {
            acl::socket_stream* conn = server.accept(); // Wait for connection. 클라이언트의 연결 요청 대기 
                                                        // -> 만약 클라이언트가 연결 시도하면, 새로운 socket_stream 객체 반환
            if (conn == NULL) {
                break;
            }
            std::thread thread([=] {  // Start one thread to handle the connection (스레드 생성)
                char buf[256];  // 최대 256 바이트 데이터
                int ret = conn->read(buf, sizeof(buf), false);  // Read data 
                if (ret > 0) {
                    conn->write(buf, ret);  // 읽은 데이터 다시 전송
                }
                delete conn; // 연결 끝나면, socket_stream 객체 삭제
                });
            thread.detach();  // 생성된 스레드 detach (생성된 스레드는 메인 스레드와 독립적으로 실행)
                              // => 메인 스레드가 종료되어도 스레드가 독립적으로 계속 실행.
        }
    }

    
    ```
  
<br>


<br>

* Coroutine TCP server  [📄코루틴이란?](./Coroutine.md)
    
    ```cpp 
    #include "acl_cpp/lib_acl.hpp"
    #include "fiber/go_fiber.hpp"
    
    void run(void) {
      const char* addr = "127.0.0.1:8088";
      acl::server_socket server;
      if (!server.open(addr)) {
        return;
      }
    
      go[&] {  // Create one server coroutine to wait for connection.
        while (true) {
          acl::socket_stream* conn = server.accept();
          if (conn) {
            go[=] {  // Create one client coroutine to handle the connection.
              char buf[256];
              int ret = conn->read(buf, sizeof(buf), false);
              if (ret > 0) {
                (void) conn->write(buf, ret);
              }
              delete conn;
            };
          }
        }
      };
    
      acl::fiber::schedule();  // Start the coroutine scheculde process.
    }

    
    ```

<br>

---

## Server-Client 예제 코드
실습 코드 : [🖱️Server-Client 테스트 코드](https://github.com/acl-dev/acl/blob/master/lib_fiber/README_en.md#samples)

* Coroutine server with c++
* Coroutine server with c++11
* Windows GUI sample

<br>

---

## ACL 심층 예제: Coroutine, MySQL, Redis 실습 가이드

Acl가 지원해주는 주요 기능을 더 자세히 다루는 예제 (심층 예제)

<br>

* Coroutine 심층 사용법
  + 사용 시, 멀티 스레드 환경에서의 사용을 위한 mutex, lock 등등
* mysql 사용법
* redis 심층 사용법
  + json 형식으로 저장하기 등등

<br>


## 예제 소스코드 정리

### Coroutine 관련 예제
* [samples](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c++)
  + event_mutex [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/event_mutex)
  + fiber_sem_cpp [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/fiber_sem_cpp)
  + thread_cond [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_cond)
  + thread_event [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_event)
  + thread_lock [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_lock)
  + thread_mutex [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_mutex)
  + thread_tbox [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_tbox)
    
* 위 Samples은 [acl project](https://github.com/acl-dev/acl/) library의 APIs 사용

<br>

### MYSQL 관련 예제
* lib_fiber/samples-c++/mysql [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/mysql)

<br>

* **2.52 mysql: mysql 클라이언트 프로그램** [소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/mysql)
* **2.53 mysql2: mysql 클라이언트 프로그램** [소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/mysql2)

<br>

### Redis 관련 예제
* lib_fiber/samples-c++/
  - redis [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/redis)
  - redis_channel [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/redis_channel)
  - redis_pipeline [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/redis_pipeline)
  - redis_sem [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/redis_sem)
  - redis_threads [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/redis_threads)

<br>

* **Redis 관련 파일** [소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/redis)
  - 2.75 redis/redis_connection: redis 연결 클라이언트 프로그램
  - 2.76 redis/redis_hash: redis 해시 테이블과 유사한 클라이언트 애플리케이션
  - 2.77 redis/redis_hyperloglog: redis 하이퍼로그 클라이언트 프로그램
  - 2.78 redis/redis_key: redis 키 클라이언트 응용 프로그램
  - 2.79 redis/redis_list: redis 리스트 클라이언트 프로그램
  - 2.80 redis/redis_manager: redis 연결 풀 클러스터 관리 클라이언트 프로그램
  - 2.81 redis/redis_pool: redis 연결 풀 클라이언트 프로그램
  - 2.82 redis/redis_pubsub: redis 게시-구독 클라이언트 프로그램
  - 2.83 redis/redis_server: redis 서버 관리 클라이언트 애플리케이션
  - 2.84 REDIS/REDIS_SET: REDIS 수집 클라이언트 애플리케이션
  - 2.85 redis/redis_string: redis 문자열 클라이언트 애플리케이션
  - 2.86 redis/redis_trans: redis 트랜잭션 클라이언트
  - 2.87 redis/redis_zset: redis 정렬 집합 클라이언트 프로그램
  - 2.88 redis/redis_zset_pool: redis ordered-set 클라이언트, ordered-set 객체를 사용하여 대용량 데이터 블록을 저장하고 연결 풀링을 지원합니다.

<br>












