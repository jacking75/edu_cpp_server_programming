# Acl - Coroutine (코루틴)

**코루틴**은 비동기 프로그래밍을 간편하게 만들어주는 도구입니다. <br>
일반적인 스레드와는 달리, 하나의 스레드 내에서 여러 코루틴을 생성하고 실행할 수 있으며, <br>
이들 코루틴은 서로 독립적으로 실행됩니다.<br>
코루틴을 사용하면 프로그램의 흐름을 중단하지 않고 비동기 작업을 수행할 수 있습니다.

<br>

## 코루틴(Coroutine)이란?

‘Co’routine(Co- + routine)으로 일종의 가벼운 스레드(Light-weight thread)로 동시성 작업을 간편하게 할 수 있게 해주는 역할을 합니다. <br>
### `즉, 코루틴은 실행의 지연과 재개를 허용함으로써,`
### `비선점적인 멀티태스킹을 위한 서브 루틴을 일반화한 컴퓨터 프로그램 구성요소라 볼 수 있습니다.` <br><br>


**비선점적인 멀티태스킹** <br>
+ **비선점형** : 다른 태스크가 끝나지 않으면 다른 프로세스는 CPU를 사용할 수 없음
  - CPU에 A프로세스가 있다면, A의 상태가 자체적으로 bolck되거나 terminated 되어야만 B를 실행할 수 있음.
  - 즉, 스케쥴러가 CPU에 있는 프로세스를 선점, 제어할 수 없음
  - 코루틴
+ **선점형** : 하나의 프로세스가 다른 프로세스 대신에 CPU를 차지할 수 있음
  - running인 상태여도 스케쥴러가 이를 중단 (ready) 시키고 다른 프로세스로 교체할 수 있음
  - 스레드

<br>

### `따라서 코루틴은 병행성(동시성)은 제공하지만, 병렬성은 제공하지 않는다. `
* 병행성(동시성) = 논리적으로 작업이 병렬로 실행되는 것처럼 보이는 것
* 병렬성 = 물리적으로 작업이 병렬로 실행되는 것



<br>


선점형으로 비동기 처리 시 아래와 같은 문제가 발생
- 코드의 복잡성 : 독립적인 스레드에서 각 루틴이 동작하므로 동시성 제어를 위한 복잡한 코드가 필요하고 코드 흐름 파악이 어렵다.
- Context switching 비용 : 스레드 사용시 Context switching 리소스 비용으로 성능 저하가 발생할 수 있다.

<br>

이런 문제 해결을 위해 비선점적 멀티 태스킹을 사용한 언어적 지원 방법이 **코루틴**입니다.

<br>


코루틴의 장점:
  - **쉽고 가독성 높은 비동기 처리**
    + 코루틴을 사용하면 예측 가능한 프로그래밍을 할 수 있기 때문에, 비교적 코드 흐름 파악이 쉬어 비동기 처리임에도 개발자가 가독성 높게 여러 작업을 처리할 수 있습니다.
  - **동시성 구현 및 경량 스레드**
    + 동시성 프로그래밍(2개 이상의 프로세스가 동시에 계산을 진행하는 상태)
    + 경량 스레드: 스레드보다 더 적은 자원을 사용하여 효율적인 비동기 작업을 수행할 수 있습니다.

---------------------------------
## `lib_fiber`

* acl의 `lib_fiber`는 windows 기준 **select, poll, iocp, GUI Message** 지원
* lib fiber를 사용하면 이벤트 중심 모델을 사용하는 기존 비동기 프레임워크보다 <br> **고성능과 대용량 네트워크 애플리케이션 서비스를 더 쉽게 작성 가능**
  + 또한 동기화 객체도 지원하고 있다. [📄lib_fiber가 지원하는 동기화 객체](./Cpp-SynchronizationObject.md)
* +) 코루틴 방식으로 Windows에서 GUI 프레임워크(MFC, wtl 등)로 작성된 Windows GUI 앱의 네트워크 모듈도 작성 가능

<br>

* 여러 플랫폼 (Linux, MacOS, Windows, iOS 및 Android)에서 실행 가능
* x86, Arm 아키텍처 지원
* select/poll/epoll/kqueue/iocp/win32 GUI 메시지 지원
* DNS 프로토콜은 acl 코루틴으로 구현되었으므로 코루틴 모델에서도 DNS API를 사용할 수 있습니다.
* Unix 및 Windows에서 시스템 IO API 후킹
  + 읽기 API: read/readv/recv/recvfrom/recvmsg
  + 쓰기 API: write/writev/send/sendto/sendmsg/sendfile64
  + **소켓 API: socket/listen/accept/connect/setsockopt**
  + 이벤트 API: select/poll/epoll_create/epoll_ctl/epoll_wait
  + DNS API: gethostbyname/gethostbyname_r/getaddrinfo/freeaddrinfo
* 메모리 사용량 최소화를 위한 공유 스택 모드 지원
* 동기화 프리미티브
  + 코루틴 뮤텍스, 셈포어는 코루틴 간에 사용 가능
  + 코루틴과 스레드 간에 코루틴 이벤트 사용 가능
 
<br>

원문 출처 : [lib_fiber/README_en.md](https://github.com/acl-dev/acl/blob/master/lib_fiber/README_en.md)

--------------------

## 예제1 : 기초 예제
- [📁CoroutineTCP 소스코드](../TCPCoroutineServerAcl)
: ACL 라이브러리의 코루틴 기능을 사용하여 TCP 서버를 구현한 것입니다. 

### 1. `run_tcp_coroutine_server` 함수의 정의

```cpp
void run_tcp_coroutine_server() {
    const char* addr = "127.0.0.1:8088";
    acl::server_socket server;
    if (!server.open(addr)) {
        std::cerr << "Failed to open server socket on " << addr << std::endl;
        return;
    }

    std::cout << "Coroutine TCP Server is running on " << addr << std::endl;
```

- **서버 소켓 생성**: `acl::server_socket server;` 서버 소켓 객체를 생성 (이 소켓은 클라이언트의 연결을 기다림)
- **소켓 열기**: `server.open(addr)`를 통해 지정된 주소(`127.0.0.1:8088`)에서 서버 소켓을 열어 클라이언트 연결을 받을 준비 시작


### 2. 코루틴을 사용한 클라이언트 연결 처리

```cpp
    go[&]{
        while (true) {
            acl::socket_stream* conn = server.accept();
            if (conn) {
                go[=] {
                    char buf[256];
                    while (true) {
                        int ret = conn->read(buf, sizeof(buf), false);
                        if (ret <= 0) {  // 데이터 읽기 실패 또는 연결 종료 시
                            std::cerr << "Failed to read data from client or connection closed." << std::endl;
                            break;
                        }

                        buf[ret] = '\0';  // 문자열로 처리하기 위해 null-terminator 추가
                        std::cout << "Received from client: " << buf << std::endl;

                        if (conn->write(buf, ret) != ret) {
                            std::cerr << "Failed to send data to client." << std::endl;
                            break;
                        }
                    }
                    delete conn;  // 연결 종료 시 소켓 스트림 삭제
                };
            }
        }
    };
```

#### 2.1 **최초 코루틴 생성 (서버 역할)**

- **코루틴 시작**: `go[&]{}`는 코루틴을 시작하는 구문
  + 코루틴은 비동기적 작업을 쉽게 구현할 수 있게 해주는 경량 스레드입니다.
  + 여기서 `&`는 캡처 리스트로, 코루틴 내부에서 외부의 `server` 객체를 사용 가능하도록 함

- **클라이언트 연결 수락**: `server.accept()`는 클라이언트의 연결을 기다리며, 연결이 수락되면 `acl::socket_stream* conn` 객체를 반환
  + 이 객체를 사용해 클라이언트와 데이터를 주고받음

#### 2.2 **클라이언트마다의 코루틴 생성**

- **새로운 클라이언트 코루틴**: `go[=]{}`를 사용하여 클라이언트가 연결될 때마다 새로운 코루틴 생성
  + 이 코루틴은 클라이언트의 요청을 처리하고 응답을 보내는 역할
  + `=`는 캡처 리스트로, 외부 변수(여기서는 `conn`)을 복사하여 코루틴 내에서 사용할 수 있도록 함

#### 2.3 **클라이언트 데이터 처리**

- **데이터 읽기 및 에코**: `conn->read(buf, sizeof(buf), false)`를 통해 클라이언트로부터 데이터를 읽습니다.
  + 읽은 데이터는 `buf`에 저장되고, 이를 다시 클라이언트로 재전송
  
  - **데이터 읽기 오류 처리**: `ret <= 0` 조건은 데이터 읽기 실패 또는 클라이언트가 연결을 끊은 경우를 의미
    + 이때 코루틴은 `break`를 통해 루프를 탈출하고, 연결을 종료합니다.

- **문자열 처리**: 데이터를 문자열로 처리하기 위해 `buf[ret] = '\0';`을 사용해 null-terminator 추가

- **에코 실패 처리**: 만약 `conn->write`에서 실패하면 오류 메시지를 출력하고 연결을 종료합니다.

- **자원 해제**: 마지막으로 `delete conn;`을 통해 연결된 클라이언트 소켓 객체를 삭제하여 메모리를 해제합니다.

### 3. 코루틴 스케줄러 시작

```cpp
    acl::fiber::schedule();  // 코루틴 스케줄러 시작
}
```

- **코루틴 스케줄러 실행**: `acl::fiber::schedule()`을 호출하여 코루틴 스케줄링을 시작합니다. 이 함수는 코루틴이 일정하게 실행되도록 관리합니다.


<br><br>

----------------

## 예제2 : 코루틴 스레드 세이프를 위해 Acl에서 제공하는 다양한 기능

이 예제들은 `acl` 라이브러리를 활용하여 코루틴과 스레드 간 동기화를 다루는 방법을 보여줍니다. <br>

이 중 코루틴 동기화 관련한 **fiber_mutex 사용법**은 [📄fiber_mutex 코루틴 동기화](./Coroutine-fiber_mutex.md) 를 참고하세요. <br><br>

아래 예제는 `mutex`, `semaphore`, `condition variable`, `event`, 및 `thread-safe queue`와 같은 동기화 기법을 사용하여 다중 코루틴 환경에서 안전한 자원 관리를 구현합니다.<br>

### Coroutine 관련 예제 소스 코드
* [samples](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c++)
  + event_mutex [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/event_mutex)
  + fiber_sem_cpp [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/fiber_sem_cpp)
  + thread_cond [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_cond)
  + thread_event [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_event)
  + thread_lock [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_lock)
  + thread_mutex [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_mutex)
  + thread_tbox [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_tbox)


<br>

<details>

  ### 1. **Mutex (event_mutex)**
  
  이 예제는 `acl::event_mutex`를 사용하여 다중 스레드와 코루틴 간에 안전한 자원 접근을 보장하는 방법을 보여줍니다. 이 코드는 여러 스레드와 코루틴이 동일한 자원을 잠금(lock)하고 이를 해제(unlock)하여 작업을 수행하는 방식으로 구성됩니다.
  
  - **핵심 개념**: `acl::event_mutex`는 **스레드**와 **코루틴** 모두에서 사용할 수 있는 뮤텍스로, 자원에 대한 접근을 제어하여 동시에 여러 스레드 또는 코루틴이 동일한 자원에 접근하지 못하도록 합니다.
  - **사용법**: 각 코루틴은 `lock()`을 호출하여 자원을 잠그고, 작업이 끝난 후 `unlock()`을 호출하여 자원을 해제합니다.
  - **실행 흐름**:
    - 각 코루틴은 뮤텍스를 잠그고(`lock()`), 작업을 수행한 뒤 뮤텍스를 해제(`unlock()`)합니다.
    - `__show` 플래그를 사용하여 각 코루틴의 잠금 및 해제 상태를 출력할 수 있습니다.
    - `__counter`는 코루틴이 작업을 수행할 때마다 증가합니다.
  
  ```cpp
  acl::event_mutex lock;
  myfiber* fb = new myfiber(lock, nfibers);
  fb->start();
  acl::fiber::schedule_with(__event_type);
  ```
  
  <br>
  
  
  ### 2. **Semaphore (fiber_sem_cpp)**
  
  이 예제는 `acl::fiber_sem`을 사용하여 코루틴 간 동기화를 수행하는 방법을 보여줍니다. 세마포어는 특정 수의 코루틴만 자원에 접근할 수 있도록 허용하며, 나머지 코루틴은 대기합니다.
  
  - **핵심 개념**: 세마포어는 정해진 수의 "티켓"을 관리하여 제한된 수의 코루틴만 자원에 접근할 수 있도록 합니다. `wait()`는 티켓을 소비하고, `post()`는 티켓을 반환합니다.
  - **사용법**: 코루틴은 `wait()`을 호출하여 세마포어의 티켓을 소비하고, 작업이 끝나면 `post()`를 호출하여 티켓을 반환합니다.
  - **실행 흐름**:
    - 각 코루틴은 세마포어의 티켓을 얻기 위해 `wait()`을 호출하고, 작업을 완료한 후 `post()`로 티켓을 반환합니다.
    - `sem_async` 플래그를 통해 비동기 세마포어(`acl::fiber_sem_t_async`)를 사용할 수 있습니다.
  
  ```cpp
  acl::fiber_sem* sem = new acl::fiber_sem(1);
  myfiber* f = new myfiber(sem);
  f->start();
  ```
  
  <br>
  
  
  ### 3. **Condition Variable (thread_cond)**
  
  이 예제는 `acl::fiber_cond`와 `acl::fiber_event`를 사용하여 생산자-소비자 패턴을 구현한 코드입니다. 조건 변수와 이벤트를 사용하여 코루틴 간 동기화와 신호 전달을 관리합니다.
  
  - **핵심 개념**: 조건 변수는 특정 조건이 만족될 때까지 코루틴이 대기할 수 있게 하고, 이벤트를 사용해 신호를 전달합니다. `notify()`는 대기 중인 코루틴을 깨우고, `wait()`는 특정 조건이 만족될 때까지 대기합니다.
  - **사용법**: 생산자는 작업을 완료한 후 조건 변수를 통해 소비자에게 신호를 보냅니다. 소비자는 조건 변수가 신호를 받을 때까지 대기하고, 작업을 수행합니다.
  - **실행 흐름**:
    - 생산자 스레드는 `notify()`를 사용해 소비자를 깨우고, 소비자는 조건 변수가 신호를 받을 때까지 대기합니다.
    - `timeout`이 설정되면 대기 시간이 초과되었을 때 작업을 중단할 수 있습니다.
  
  ```cpp
  acl::fiber_cond cond;
  acl::fiber_event event;
  producer* prod = new producer(event, cond);
  consumer* cons = new consumer(event, cond, timeout);
  ```
  
  <br>
  
  
  ### 4. **Event Lock (thread_lock)**
  
  이 예제는 `acl::fiber_event`를 사용하여 코루틴이 자원을 안전하게 사용할 수 있도록 자원 접근을 제어하는 방법을 보여줍니다. 이 방식은 자원을 잠금(lock)하고 해제(unlock)하는 방식으로 자원을 보호합니다.
  
  - **핵심 개념**: 이벤트를 통해 코루틴 간의 자원 접근을 조율합니다. `wait()`는 자원을 잠그고, `notify()`는 자원을 해제하는 역할을 합니다.
  - **사용법**: 각 코루틴은 `wait()`를 호출하여 자원을 잠그고, 작업이 완료되면 `notify()`를 호출하여 자원을 해제합니다.
  - **실행 흐름**:
    - 코루틴은 이벤트를 잠그고(`wait()`), 작업이 완료된 후 이벤트를 해제(`notify()`)합니다. 이 과정을 통해 자원을 안전하게 보호할 수 있습니다.
  
  ```cpp
  acl::fiber_event lock;
  myfiber* fb = new myfiber(lock, nfibers);
  fb->start();
  acl::fiber::schedule_with(__event_type);
  ```
  
  <br>
  
  
  ### 5. **Thread-Safe Queue (thread_tbox)**
  
  이 예제는 `acl::fiber_tbox`를 사용하여 코루틴 간에 안전하게 데이터를 전달하는 방법을 보여줍니다. 생산자는 데이터를 큐에 푸시하고, 소비자는 큐에서 데이터를 팝합니다.
  
  - **핵심 개념**: `fiber_tbox`는 코루틴 간 안전한 데이터 전달을 위한 쓰레드 세이프 큐입니다. 생산자는 데이터를 생성하여 큐에 삽입하고, 소비자는 큐에서 데이터를 제거하여 처리합니다.
  - **사용법**: 생산자는 `push()`를 사용해 데이터를 큐에 추가하고, 소비자는 `pop()`을 사용해 데이터를 큐에서 가져옵니다.
  - **실행 흐름**:
    - 생산자는 큐에 데이터를 추가하고, 소비자는 해당 데이터를 가져와 처리합니다. 타임아웃을 설정할 수 있어 일정 시간이 지나면 데이터 수신을 중단할 수 있습니다.
  
  ```cpp
  acl::fiber_tbox<myobj> tbox;
  producer* prod = new producer(tbox);
  consumer* cons = new consumer(tbox, timeout);
  ```
  
  <br>

</details>

 <br><br> <br><br>

