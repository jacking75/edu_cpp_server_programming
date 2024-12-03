# ACL (Advanced C/C++ Library) HTTP Server 기능 정리

ACL(Advanced C/C++ Library)은 고성능 C/C++ 기반의 네트워크 프로그래밍 라이브러리로, 다양한 서버 및 클라이언트 통신 기능을 제공합니다. <br><br>

그 중 특히 HTTP 서버와 관련된 기능을 동기 및 비동기 방식으로 모두 제공하며, <br>
`socket_stream`을 이용한 전통적인 HTTP 서버 구현부터 `fiber`(경량 스레드)를 활용한 고성능 비동기 서버 구현까지 지원합니다. <br>

이 문서는 ACL에서 제공하는 HTTP 서버 기능을 두 가지 방식(전통적인 동기 방식 및 Fiber 기반 비동기 방식)으로 정리합니다. 

---

## 1. 전통적인 HTTP 서버 방식

### 1.1 개요
ACL의 전통적인 HTTP 서버 방식은 **Blocking I/O 모델**을 기반으로 동작합니다. <br>
이 방식은 클라이언트와 서버 간의 통신이 순차적으로 이루어지며, 각 요청이 완료될 때까지 서버는 해당 작업에 대해 블로킹됩니다. <br>
이 방식은 구현이 비교적 간단하고 직관적이지만, 다수의 클라이언트를 동시에 처리할 때 성능이 제한될 수 있습니다. <br>
- 구현 소스코드 : [acl/lib_acl_cpp/src/http](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/src/http)
- sample 소스코드 : [acl/lib_acl_cpp/samples/http](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/http)

<br> <br> 

### 1.2 주요 클래스와 구성 요소
- **`acl::socket_stream`**: 클라이언트와의 연결을 담당하는 TCP 소켓 스트림.
  + 서버는 이 객체를 사용하여 클라이언트의 요청을 수신하고 응답을 전송합니다.
- **`acl::HttpServlet`**: HTTP 요청을 처리하는 클래스.
  + 이 클래스를 상속받아 GET, POST 등의 요청을 처리하는 메서드를 구현합니다.
  + 구현 : [`HttpServlet.cpp`](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/src/http/HttpServlet.cpp)
- **`acl::HttpServletRequest`**: HTTP 요청 데이터를 파싱하고 제공하는 클래스.
- **`acl::HttpServletResponse`**: HTTP 응답을 작성하고 전송하는 클래스.

<br> <br> 

### 1.3 구현 방식

#### 서버 설정 및 요청 처리 흐름
1. **소켓 열기**: 서버는 지정된 포트에서 `acl::server_socket`을 통해 소켓을 열고 클라이언트의 연결 요청을 대기합니다.
2. **연결 수락**: 클라이언트가 연결을 시도하면, 서버는 `accept()` 메서드를 통해 새로운 연결을 수락하고, 해당 연결에 대한 스트림(`acl::socket_stream`)을 생성합니다.
3. **HTTP 요청 처리**: 클라이언트로부터 HTTP 요청이 들어오면 `acl::HttpServletRequest`가 요청 데이터를 파싱하고, 서버는 해당 요청에 맞는 처리 메서드(`doGet`, `doPost` 등)를 호출하여 응답을 생성합니다.
4. **응답 전송**: `acl::HttpServletResponse`를 통해 클라이언트로 응답을 전송하고, 연결을 종료합니다.

<br> 

#### acl::HttpServlet
`HttpServlet` 클래스는 HTTP 요청과 응답을 처리하는 기본 클래스입니다. <br> 
각 요청 메서드(GET, POST 등)를 처리하기 위한 메서드를 오버라이딩하여 사용자가 직접 요청 처리를 구현할 수 있습니다. <br> 

- **`doRun()`**: 클라이언트 요청이 들어오면 `start()` 메서드를 호출하여 요청을 처리합니다. 이 과정에서 HTTP 메서드(GET, POST 등)를 파악하고, 해당 메서드에 맞는 핸들러(`doGet`, `doPost` 등)를 호출합니다.
- **`start()`**: 실제 HTTP 요청을 처리하는 메서드로, 소켓 스트림을 통해 데이터를 읽어들이고 적절한 메서드를 호출하여 응답을 생성합니다.

<br> 

  
#### acl::HttpServletRequest
`HttpServletRequest` 클래스는 HTTP 요청 데이터를 파싱하고 제공하는 역할을 합니다.  <br> 
클라이언트로부터 받은 요청 메서드(GET, POST 등), 헤더 정보, 파라미터 등을 다룹니다. <br> 

<br> 

#### acl::HttpServletResponse
`HttpServletResponse` 클래스는 서버가 클라이언트에게 응답할 데이터를 생성하고 전송하는 데 사용됩니다. <br>  
상태 코드, 헤더 설정, 콘텐츠 길이 등을 관리하며, 데이터를 클라이언트로 전송합니다. <br> 

<br> 

### Blocking I/O
Blocking I/O 모델에서는 서버가 클라이언트로부터 데이터를 요청받을 때, 해당 I/O 작업이 완료될 때까지 해당 작업을 실행하는 쓰레드가 블로킹됩니다. <br> 
즉, 데이터를 읽거나 쓰는 작업이 완료되기 전까지는 해당 쓰레드는 다른 작업을 수행하지 못하고 대기 상태에 놓입니다. <br> <br> 

이 코드에서 사용하는 `socket_stream` 클래스는 소켓 기반의 I/O 처리를 담당하는데, 이 소켓 클래스는 기본적으로 Blocking I/O 모델로 동작합니다. <br> 
예를 들어, 클라이언트로부터 데이터를 수신할 때 `read()` 또는 `recv()` 함수가 호출되면, 데이터가 도착할 때까지 해당 함수는 반환되지 않고 블로킹됩니다.<br> 

다음과 같은 코드에서 이를 확인할 수 있습니다: <br> 


```cpp
socket_stream* in;
socket_stream* out;

in = NEW socket_stream();
in->open(ACL_VSTREAM_IN);

out = NEW socket_stream();
out->open(ACL_VSTREAM_OUT);
```

위의 코드에서 소켓 스트림이 열리면, 서버는 클라이언트로부터 데이터를 수신할 때까지 블로킹됩니다. <br> 
`HttpServlet::start()` 메서드에서 이 소켓 스트림을 사용하여 클라이언트의 요청을 읽어들이고 응답을 작성합니다.<br> 


<br> <br> 

---

## 2. Fiber 기반 HTTP 서버 방식

### 2.1 개요
ACL은 경량 스레드(Lightweight Threads)인 **Fiber**를 사용한 비동기 I/O 모델을 제공합니다.  <br> 
이 방식은 전통적인 동기식 방식의 성능 병목을 해소하고, 대량의 동시 접속을 효율적으로 처리할 수 있도록 설계되었습니다. <br>  <br> 

Fiber 기반의 HTTP 서버는 각각의 연결 요청을 별도의 Fiber로 처리함으로써, 성능을 극대화하고 I/O 블로킹을 피할 수 있습니다. <br> 

- 구현 소스코드 :
  + [acl/lib_acl_cpp/src/http](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/src/http)
  + [fiber/http_server.hpp](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/include/fiber/http_server.hpp)
    * http_server_impl 를 상속받아서 사용중
  + [fiber/master_fiber.hpp](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/include/fiber/detail/http_server_impl.hpp)
    * master_fiber 를 상속 받아서 사용중
  + [fiber/master_fiber.hpp](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/include/fiber/master_fiber.hpp)
    * HTTP 서버와 관련된 코루틴 기반의 비동기 실행을 관리하는 기반 클래스!

<br>
  
- sample 소스코드 :
  + [acl/lib_fiber/samples-c++/https](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/https_server)
  + [acl/lib_fiber/samples-c++/httpd](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/httpd)

<br> <br> 

### 2.2 주요 클래스와 구성 요소
- **`acl::fiber`**: 경량 스레드로서, 비동기 I/O 작업을 처리할 수 있는 기본 단위입니다.
- **`acl_fiber_create()`**: 새로운 Fiber를 생성하여 HTTP 요청을 비동기적으로 처리하는 함수입니다.
- **`acl::HttpServlet`**: 전통적인 HTTP 서버와 마찬가지로 Fiber 기반 서버에서도 HTTP 요청 처리를 담당합니다.
- **`acl::memcache_session`**: Fiber와 함께 사용될 수 있는 비동기식 세션 관리 클래스입니다.

<br> <br> 

### 2.3 구현 방식

#### Fiber 기반 HTTP 서버의 처리 흐름
1. **Fiber 생성 및 스케줄링**: 
   - `acl_fiber_create()` 함수를 사용해 각 클라이언트 요청을 처리할 Fiber를 생성합니다. Fiber는 경량 스레드로, 빠르게 생성되고 실행됩니다.
   - 생성된 Fiber는 각각 클라이언트와의 연결을 비동기적으로 처리합니다. 이 방식은 성능을 극대화하고 서버의 I/O 작업이 블로킹되지 않도록 합니다.

2. **비동기 I/O 처리**:
   - Fiber는 I/O 작업을 대기하면서도 비동기적으로 요청을 처리할 수 있습니다. 즉, 하나의 Fiber가 I/O 작업을 기다리는 동안에도 다른 Fiber는 계속해서 요청을 처리할 수 있습니다.
   - 이 방식은 서버가 다수의 클라이언트로부터 들어오는 요청을 동시에 처리할 수 있도록 하며, 시스템의 자원을 효율적으로 사용하게 만듭니다.

3. **Fiber 스케줄링**:
   - Fiber의 실행 순서를 관리하기 위해 `acl_fiber_schedule()` 함수를 사용하여 모든 Fiber를 스케줄링합니다.
   - 이 스케줄러는 커널 이벤트나 `poll`, `select` 등의 방식으로 Fiber의 실행을 관리하며, 각 Fiber가 완료될 때까지 동작을 지속합니다.
   
4. **클라이언트 요청 처리**:
   - 각 클라이언트의 요청은 Fiber에서 처리되며, Fiber는 각 클라이언트와의 통신이 종료될 때까지 실행됩니다.
   - 클라이언트로부터의 HTTP 요청은 `HttpServlet`에서 처리되고, 해당 요청에 대한 응답이 완료되면 Fiber는 종료됩니다.


  ```cpp
  static void http_server(ACL_FIBER*, void* ctx) {
      acl::socket_stream* conn = (acl::socket_stream*) ctx;
      acl::memcache_session session("127.0.0.1:11211");
      http_servlet servlet(conn, &session);
  
      while (servlet.doRun()) {
          // 요청 처리
      }
  
      delete conn;
  }
  
  int main() {
      acl::server_socket server;
      server.open("127.0.0.1:9001");
  
      while (true) {
          acl::socket_stream* client = server.accept();
          acl_fiber_create(http_server, client, 128000); // Fiber 생성
      }
  
      acl_fiber_schedule(); // Fiber 스케줄링
      return 0;
  }
  ```

<br> 

#### 장단점

- **장점**:
  - **경량 스레드(Fiber)**: Fiber는 일반적인 스레드보다 훨씬 경량화되어 있어, 수천 개의 Fiber를 동시에 생성하고 관리할 수 있습니다. 이를 통해 다수의 클라이언트 요청을 동시에 처리할 수 있습니다.
  - **비동기 처리**: Fiber를 사용하면 각 클라이언트의 요청을 비동기적으로 처리할 수 있습니다. 이는 각 Fiber가 I/O 작업 중에도 다른 작업을 계속 처리할 수 있도록 하여, 서버가 더 높은 동시성 성능을 제공할 수 있게 합니다.
  - **성능 향상**: 다수의 클라이언트 요청을 처리할 때, 전통적인 멀티스레드보다 훨씬 적은 자원으로 고성능 처리가 가능합니다. I/O 작업에서 발생하는 블로킹 문제를 해결할 수 있습니다.

- **단점**:
  - **구현 복잡성**: Fiber 기반 비동기 프로그래밍은 동기 방식에 비해 구현의 복잡도가 높아집니다. Fiber의 스케줄링과 동작을 정확하게 관리해야 하며, 잘못된 설계는 성능 저하를 초래할 수 있습니다.
  - **Fiber 관리**: Fiber의 생성, 스케줄링, 실행 상태를 세밀하게 관리해야 합니다. 이러한 관리가 복잡할 수 있으며, 디버깅 과정도 까다로울 수 있습니다.


---



ACL은 **전통적인 동기 방식**과 **Fiber 기반의 비동기 방식**을 모두 지원하여 다양한 서버 구현 시나리오에 대응할 수 있는 유연성을 제공합니다. <br> 
전통적인 HTTP 서버는 단순한 구조와 사용이 쉬운 반면, 대규모 동시 연결 처리에는 성능적인 한계가 있습니다. <br> 
반면 Fiber 기반 HTTP 서버는 이러한 한계를 극복하며, 대규모 네트워크 트래픽을 효과적으로 처리할 수 있는 강력한 방법을 제공합니다.<br> <br> 

이를 통해 ACL은 다양한 요구 사항에 맞춰 HTTP 서버를 쉽게 확장하고 최적화할 수 있는 기능을 제공하며, 특히 Fiber 기반 서버는 고성능 시스템을 구축하는 데 매우 유용한 도구입니다.
