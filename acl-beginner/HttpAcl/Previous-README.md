# HTTP Module acl
Acl에서 제공하는 HTTP Module을 활용한 Http Server 코드


<br><br>


## 기본 Sample 코드 : http_ex.cpp 코드 분석
`http_ex.cpp` 은 acl에서 제공하는 http 모듈을 활용한 기본적인 예시 코드입니다.

<br>

```cpp
#include "acl_cpp/lib_acl.hpp"  // Must before http_server.hpp
#include "fiber/http_server.hpp"
```
- `acl_cpp/lib_acl.hpp`: ACL C++ 라이브러리의 기본 헤더를 포함합니다. 이 헤더는 네트워킹, 파일 처리, 문자열 처리 등 다양한 기능을 제공합니다.
- `fiber/http_server.hpp`: HTTP 서버를 구성하는 데 필요한 클래스를 정의한 헤더 파일입니다.

```cpp
static char* var_cfg_debug_msg;
static acl::master_str_tbl var_conf_str_tab[] = {
  { "debug_msg", "test_msg", &var_cfg_debug_msg },
  { 0, 0, 0 }
};

static int  var_cfg_io_timeout;
static acl::master_int_tbl var_conf_int_tab[] = {
  { "io_timeout", 120, &var_cfg_io_timeout, 0, 0 },
  { 0, 0 , 0 , 0, 0 }
};
```
- **설정 변수 초기화**: `var_cfg_debug_msg`와 `var_cfg_io_timeout`은 서버의 설정 값을 저장하는 변수입니다.
- `acl::master_str_tbl` 및 `acl::master_int_tbl` 구조체 배열을 통해 문자열과 정수 설정을 저장하고 있습니다. 이러한 설정 값은 서버가 초기화될 때 로드됩니다.

```cpp
int main() {
    const char* addr = "0.0.0.0|8194";
    const char* conf = NULL;

    acl::acl_cpp_init();
```
- `addr`: 서버가 바인딩할 IP 주소와 포트를 나타냅니다. `0.0.0.0`은 모든 네트워크 인터페이스에서 요청을 수신하는 것을 의미하며, `8194`는 서버가 리스닝할 포트 번호입니다.
- `conf`: 서버 설정 파일을 지정할 수 있지만, 여기서는 사용하지 않습니다.
- `acl::acl_cpp_init()`: ACL 라이브러리의 초기화를 수행합니다.

```cpp
    acl::http_server server;

    // Call the methods in acl::master_base class.
    server.set_cfg_int(var_conf_int_tab).set_cfg_str(var_conf_str_tab);
```
- `acl::http_server`: HTTP 서버 객체를 생성합니다.
- `set_cfg_int`, `set_cfg_str`: 초기화 단계에서 설정된 정수와 문자열 값을 서버 객체에 설정합니다.

```cpp
    server.on_proc_init([&addr] {
        printf("---> after process init: addr=%s, io_timeout=%d\r\n", addr, var_cfg_io_timeout);
    }).on_proc_exit([] {
        printf("---> before process exit\r\n");
    }).on_proc_sighup([](acl::string& s) {
        s = "+ok";
        printf("---> process got sighup\r\n");
        return true;
    }).on_thread_accept([](acl::socket_stream& conn) {
        printf("---> accept %d\r\n", (long long)conn.sock_handle());
        return true;
    })
```
- **이벤트 핸들러 등록**: 서버의 수명 주기 동안 특정 이벤트가 발생할 때 호출되는 핸들러들을 설정합니다.
    - `on_proc_init`: 서버 프로세스가 초기화된 후 호출됩니다.
    - `on_proc_exit`: 서버 프로세스가 종료되기 전에 호출됩니다.
    - `on_proc_sighup`: `SIGHUP` 시그널을 수신했을 때 호출됩니다.
      + `SIGHUP` 시그널 : 통신 접속이 끊어졌을 때 프로세스들에게 보내는 시그널
    - `on_thread_accept`: 클라이언트 연결이 수락되었을 때 호출됩니다.

```cpp
    .Get("/", [](acl::HttpRequest&, acl::HttpResponse& res) {
        std::string buf("hello world1!\r\n");
        res.setContentLength(buf.size());
        return res.write(buf.c_str(), buf.size());
    }).Post("/json", [](acl::HttpRequest& req, acl::HttpResponse& res) {
        acl::json* json = req.getJson();
        if (json) {
            return res.write(*json);
        } else {
            std::string buf = "no json got\r\n";
            res.setContentLength(buf.size());
            return res.write(buf.c_str(), buf.size());
        }
    })
```
- **HTTP 요청 핸들러 등록**: 특정 경로에 대한 HTTP 메서드(GET, POST 등) 요청을 처리하는 핸들러들을 설정합니다.
    - `Get("/")`: 루트 경로로 GET 요청이 들어오면 "hello world1!" 메시지를 반환합니다.
    - `Post("/json")`: "/json" 경로로 POST 요청이 들어오면 JSON 데이터를 처리하고 반환합니다.

```cpp
    .run_alone(addr, conf);

    return 0;
}
```
- `run_alone`: 서버를 지정된 주소와 포트에서 단독으로 실행합니다.


----

이 코드에서 HTTP 서버가 비동기적으로 요청을 처리하는 방식에 대해 주요 구성 요소와 함께 설명합니다.


### 코드의 주요 구성 요소

1. **서버 설정 및 초기화**:
   ```cpp
   acl::acl_cpp_init();
   acl::http_server server;
   server.set_cfg_int(var_conf_int_tab).set_cfg_str(var_conf_str_tab);
   ```

   이 부분에서는 `acl_cpp_init()`을 통해 ACL 라이브러리를 초기화하고, HTTP 서버 객체를 생성하여 설정값을 적용합니다.

2. **HTTP 요청 처리 설정**:
   ```cpp
   server.on_proc_init([&addr] {
        printf("---> after process init: addr=%s, io_timeout=%d\r\n", addr, var_cfg_io_timeout);
    }).on_proc_exit([] {
        printf("---> before process exit\r\n");
    }).on_proc_sighup([](acl::string& s) {
        s = "+ok";
        printf("---> process got sighup\r\n");
        return true;
    }).on_thread_accept([](acl::socket_stream& conn) {
        printf("---> accept %d\r\n", conn.sock_handle());
        return true;
    }).Get("/", [](acl::HttpRequest&, acl::HttpResponse& res) {
        std::string buf("hello world1!\r\n");
        res.setContentLength(buf.size());
        return res.write(buf.c_str(), buf.size());
    }).Post("/json", [](acl::HttpRequest& req, acl::HttpResponse& res) {
        acl::json* json = req.getJson();
        if (json) {
            return res.write(*json);
        } else {
            std::string buf = "no json got\r\n";
            res.setContentLength(buf.size());
            return res.write(buf.c_str(), buf.size());
        }
    });
   ```

   여기서 서버가 초기화된 후 실행할 작업(`on_proc_init`), 서버 종료 시 실행할 작업(`on_proc_exit`), 시그널 수신 시 실행할 작업(`on_proc_sighup`), 클라이언트 연결 수락 시 실행할 작업(`on_thread_accept`), GET 및 POST 요청에 대한 핸들러를 설정합니다.

3. **서버 실행**:
   ```cpp
   server.run_alone(addr);
   ```

   이 부분에서 서버가 지정된 주소(`addr`)에서 단독 실행됩니다. 이 메서드는 `master_fiber` 클래스에서 상속된 메서드로, 서버의 비동기 실행을 담당합니다.

### 코루틴과 비동기 처리

코루틴을 사용한 비동기 처리는 주로 `master_fiber` 클래스와 `http_server_impl` 클래스에 의해 구현됩니다. 
`master_fiber`는 기본적으로 서버가 요청을 처리하는 동안 블로킹되지 않도록 설계되어 있습니다. 
이 비동기 처리 방식은 다음과 같은 방식으로 코드에서 구현됩니다.

#### 1. **클라이언트 연결 수락 (`on_accept`)**

`on_thread_accept`에서 클라이언트의 연결을 수락할 때, 서버는 이 작업을 비동기적으로 수행합니다. 
연결이 수락되면, `http_server_impl` 클래스의 `on_accept` 메서드가 호출됩니다.

```cpp
void on_accept(socket_stream& conn) override {
    if (thread_accept_ && !thread_accept_(conn)) {
        return;
    }

    acl::session* session;
    if (redis_) {
        session = new redis_session(*redis_, 0);
    } else {
        session = new memcache_session("127.0.0.1|11211");
    }

    http_servlet servlet(handlers_, &conn, session);
    servlet.setLocalCharset("utf-8");

    while (servlet.doRun()) {}

    delete session;
}
```

- **비동기 작업**: 여기서 `servlet.doRun()`이 비동기적으로 호출되며, 이는 HTTP 요청을 처리하는 동안 다른 클라이언트의 요청도 병렬로 처리될 수 있도록 합니다.

#### 2. **HTTP 요청 처리 (`http_servlet::doRun`)**

`http_servlet` 클래스는 실제로 HTTP 요청을 처리합니다. `doRun` 메서드는 각 요청을 비동기적으로 처리할 수 있도록 설계되었습니다.

```cpp
bool http_servlet::doRun() {
    acl::HttpRequest request;
    acl::HttpResponse response;

    if (parseRequest(request) && handlers_[request.getMethod()][request.getPath()]) {
        handlers_[request.getMethod()][request.getPath()](request, response);
    }

    response.send(conn_);
    return conn_->keep_alive();
}
```

- **비동기 요청 처리**: `doRun`은 클라이언트의 요청을 받아들이고, 비동기적으로 요청을 처리합니다. 이 과정에서 각 요청은 별도의 코루틴으로 처리되며, 이는 서버가 다수의 요청을 동시에 처리할 수 있게 합니다.

- **코루틴 활용**: 코루틴은 요청을 중단(`yield`)했다가 필요한 시점에 재개(`resume`)하여 처리할 수 있는 구조를 제공합니다. 예를 들어, 데이터베이스에서 데이터를 가져오거나, 네트워크 통신을 하는 동안 다른 요청이 처리될 수 있습니다.

### 3. **서버 실행**

`run_alone` 메서드는 서버를 단독 실행하며, 내부적으로는 서버의 메인 루프를 돌면서 클라이언트 연결을 비동기적으로 처리하는 코루틴 기반의 실행 방식을 사용합니다.

### 결론

이 코드에서의 코루틴 사용은 비동기적으로 다수의 클라이언트 요청을 처리하기 위한 구조입니다. 
코루틴을 활용하여 각 요청이 독립적으로 처리되며, 서버는 블로킹 없이 다수의 요청을 동시에 처리할 수 있습니다. 
주요 비동기 처리 부분은 `on_accept` 메서드에서의 연결 수락과 `doRun` 메서드에서의 요청 처리에서 이루어집니다. 
이로 인해, 서버는 높은 성능을 유지하면서 다수의 클라이언트를 동시에 처리할 수 있게 됩니다.


----------

### 1. `run_alone` 함수

```cpp
bool master_fiber::run_alone(const char* addrs, const char* path /* = NULL */) {
    // 주소가 없으면 경고 메시지 출력 후 종료
    if (addrs == NULL || *addrs == 0) {
        if (path == NULL || *path == 0) {
            printf("%s: addrs NULL and path NULL\r\n", addrs);
            return false;
        }
        printf("%s: addrs NULL and try to use master_service in %s\r\n",
            __FUNCTION__, path);
    }

    daemon_mode_ = false;

    int argc = 0;
    const char *argv[9];
    const char *file_path = acl_process_path();

    // 서버 실행을 위한 명령 인수 배열 설정
    argv[argc++] = file_path ? file_path : "unknown";
    argv[argc++] = "-L";
    argv[argc++] = addrs;
    if (path && *path) {
        argv[argc++] = "-f";
        argv[argc++] = path;
    }

    // 비동기 서버 실행을 시작
    run(argc, (char **) argv);
    return true;
}
```

`run_alone` 함수는 서버를 실행하는 데 필요한 인수들을 설정한 후, `run` 함수를 호출하여 서버를 시작합니다. 여기서 `run` 함수는 비동기적으로 서버를 관리하고 요청을 처리하기 위한 핵심 함수입니다.

### 2. `run` 함수

```cpp
void master_fiber::run(int argc, char** argv) {
    // 각 프로세스는 한 번만 실행 가능
    acl_assert(has_called == false);
    has_called = true;

    // 비동기 서버의 메인 루프를 시작
    acl_fiber_server_main(argc, argv, service_on_accept, this,
        ACL_MASTER_SERVER_PRE_INIT, service_pre_jail,
        ACL_MASTER_SERVER_POST_INIT, service_init,
        ACL_MASTER_SERVER_PRE_EXIT, service_pre_exit,
        ACL_MASTER_SERVER_EXIT, service_exit,
        ACL_MASTER_SERVER_ON_LISTEN, service_on_listen,
        ACL_MASTER_SERVER_THREAD_INIT, thread_init,
        ACL_MASTER_SERVER_THREAD_INIT_CTX, this,
        ACL_MASTER_SERVER_SIGHUP, service_on_sighup,
        ACL_MASTER_SERVER_BOOL_TABLE, conf_.get_bool_cfg(),
        ACL_MASTER_SERVER_INT64_TABLE, conf_.get_int64_cfg(),
        ACL_MASTER_SERVER_INT_TABLE, conf_.get_int_cfg(),
        ACL_MASTER_SERVER_STR_TABLE, conf_.get_str_cfg(),
        ACL_MASTER_SERVER_END);
}
```

이 `run` 함수는 서버의 비동기적 실행을 시작하는 주요 함수입니다. `acl_fiber_server_main`을 호출하여 서버 실행의 핵심 이벤트 루프를 설정하고, 각종 초기화와 종료 처리 함수들을 등록합니다.

### 3. `acl_fiber_server_main` 함수

```cpp
void acl_fiber_server_main(int argc, char *argv[],
    void (*service)(void*, ACL_VSTREAM*), void *ctx, int name, ...)
{
    // ... (기본 설정 및 초기화 코드)

    // 서버의 이벤트 루프를 시작
    servers_start(__servers, acl_var_fiber_threads);
}
```

`acl_fiber_server_main` 함수는 서버 실행의 중심이 되는 이벤트 루프를 설정하고, 비동기 서버의 초기화 작업을 수행합니다. 이 함수는 주어진 인수와 설정을 기반으로, 클라이언트의 요청을 비동기적으로 처리할 준비를 합니다.

### 4. 비동기 처리 및 코루틴 활용

서버가 비동기로 작동하는 핵심은 `servers_start` 함수에서 이루어집니다. 이 함수는 여러 개의 스레드 또는 코루틴을 생성하고, 각각의 코루틴이 클라이언트의 연결 요청을 비동기적으로 처리합니다.

```cpp
void servers_start(servers_t* servers, int thread_count) {
    // 서버를 위한 이벤트 루프 및 코루틴 설정
    for (int i = 0; i < thread_count; i++) {
        // 각 스레드에서 코루틴을 실행
        acl_create_thread(fiber_loop, servers);
    }
}
```

여기서 `fiber_loop`은 각 코루틴이 클라이언트 요청을 처리하는 함수입니다. 이 함수는 비동기적으로 여러 요청을 처리하며, I/O 작업 중에도 다른 요청들을 효율적으로 처리할 수 있습니다.

### 5. 클라이언트 연결 처리 (`service_on_accept`)

```cpp
static void service_on_accept(void* ctx, ACL_VSTREAM* stream) {
    // 새로운 클라이언트 연결에 대해 코루틴으로 처리
    master_fiber* master = (master_fiber*) ctx;
    socket_stream conn(stream);
    
    master->on_accept(conn);
}
```

`service_on_accept`은 클라이언트 연결이 발생할 때 호출됩니다. 이 함수는 새로운 연결에 대해 `master_fiber`의 `on_accept` 메서드를 호출하여, 클라이언트 요청을 비동기적으로 처리할 수 있도록 합니다. 이 `on_accept` 메서드는 각 클라이언트에 대해 독립적으로 실행되며, 동시에 여러 클라이언트를 처리할 수 있습니다.

### 결론

이 일련의 함수 호출 과정은 `run_alone`에서 시작하여 `acl_fiber_server_main`에 이르기까지 서버가 비동기적으로 작동하는 과정을 설정합니다. 각 클라이언트의 연결 요청은 코루틴으로 처리되며, I/O 작업이 비동기적으로 이루어집니다. 이로 인해 서버는 고효율의 비동기 네트워크 서버로 동작할 수 있게 됩니다.

---
# HTTP server에서 비동기 처리



지금까지 설명한 내용을 바탕으로, `ACL` 라이브러리의 HTTP 서버 구현에서 비동기 처리의 핵심은 **코루틴**을 활용한 **비동기 I/O 처리**입니다. 
이를 구체적으로 정리하면 다음과 같습니다:

### 1. **비동기 처리의 핵심 위치**

- **코루틴 기반 서버 루프**: 서버가 실행되는 동안 모든 클라이언트 연결과 요청은 비동기적으로 처리됩니다. 이 비동기 처리는 `acl_fiber_server_main` 함수에서 설정되며, `servers_start` 함수에서 각 코루틴이 실행됩니다.
- **클라이언트 연결 처리**: 클라이언트 연결이 수락되면, 각 연결에 대해 `service_on_accept` 함수가 호출되고, 이 함수는 코루틴으로 실행되어 비동기적으로 요청을 처리합니다. 이 과정에서 각 클라이언트 연결은 독립적으로 비동기 처리되므로 서버는 여러 클라이언트의 요청을 동시에 처리할 수 있습니다.
- **HTTP 요청 처리**: 각 클라이언트 요청은 `http_servlet` 클래스의 `doRun` 메서드에서 비동기적으로 처리됩니다. 여기서도 코루틴이 사용되어 요청이 중단되거나 재개되는 시점에 다른 요청을 처리할 수 있습니다.

### 2. **구체적인 비동기 흐름**

- **`run_alone` → `run` → `acl_fiber_server_main`**: 이 일련의 함수 호출을 통해 서버의 비동기 실행 환경이 설정됩니다.
- **`service_on_accept`**: 클라이언트 연결이 발생할 때 비동기적으로 처리하기 위해 코루틴을 사용합니다.
- **`http_servlet::doRun`**: HTTP 요청을 처리하는 비동기 메서드로, 코루틴을 통해 각 요청이 독립적으로 처리됩니다.

### 3. **결론: 비동기 처리가 적용된 구체적인 위치**

- 비동기 처리는 **서버의 메인 루프**와 **클라이언트 연결 및 요청 처리**에서 이루어집니다. 
- 코루틴을 활용하여 I/O 작업을 비동기적으로 처리함으로써, 서버는 여러 클라이언트의 요청을 동시에 처리할 수 있습니다.
- 이러한 비동기 처리가 서버의 성능을 높이고, 동시에 다수의 클라이언트를 처리할 수 있도록 설계되었습니다.


  1. **`run_alone` 함수 호출**:
     ```cpp
     server.run_alone(addr);
     ```
     - 이 함수 호출은 `master_fiber` 클래스의 `run_alone` 메서드를 실행합니다. 이 메서드는 내부적으로 서버를 비동기적으로 실행하기 위한 준비를 합니다.
     - `run_alone` 함수는 `run` 메서드를 호출하여 서버의 메인 루프를 시작합니다. 이 메인 루프에서 모든 클라이언트 요청을 비동기적으로 처리합니다.
  
  2. **비동기 요청 처리의 흐름**:
     - 서버가 클라이언트의 연결을 수락하고, 각 요청을 처리할 때 비동기적으로 동작합니다. 이 비동기 처리의 핵심은 코루틴(`fiber`)을 활용하여 이루어집니다.
     - 클라이언트의 연결이 수락될 때 `on_thread_accept`에서 설정한 콜백이 호출되며, 이때 비동기적으로 요청을 처리합니다.
  
  3. **클라이언트 요청 처리의 비동기성**:
     - `on_thread_accept`에서 연결이 수락되면, 연결된 클라이언트의 요청은 `http_server_impl` 클래스의 `on_accept` 메서드에서 처리됩니다.
     - `on_accept` 메서드는 각 요청을 처리할 때 `http_servlet` 객체를 생성하여, `doRun` 메서드를 호출합니다.
  
  ### `doRun` 메서드의 관련성
  
  - **`doRun` 메서드**:
    ```cpp
    bool http_servlet::doRun() {
        acl::HttpRequest request;
        acl::HttpResponse response;
  
        if (parseRequest(request) && handlers_[request.getMethod()][request.getPath()]) {
            handlers_[request.getMethod()][request.getPath()](request, response);
        }
  
        response.send(conn_);
        return conn_->keep_alive();
    }
    ```
    - 이 메서드는 HTTP 요청을 실제로 처리하는 역할을 합니다. 이 메서드는 서버가 각 클라이언트의 요청을 처리할 때 호출되며, 비동기적으로 동작합니다.
    - 비동기적인 I/O 작업은 이 메서드에서 요청을 처리하고 응답을 전송하는 동안에 이루어집니다.

  ### 결론
  
  - **비동기 처리의 핵심**: 이 코드에서 비동기 처리는 `run_alone` 메서드 호출로 시작됩니다.
    이 메서드는 서버를 비동기적으로 실행하며, 클라이언트의 연결과 요청을 처리할 때 코루틴을 활용합니다.
  - **`doRun` 메서드의 역할**: `doRun` 메서드는 서버가 클라이언트의 HTTP 요청을 처리할 때 호출되며, 각 요청을 비동기적으로 처리합니다.
    이 메서드는 이 코드에서 설정한 HTTP 핸들러 (`Get`, `Post` 등)들이 호출되는 곳입니다.
  - **비동기 처리의 전체 흐름**: `run_alone` → `run` → `acl_fiber_server_main` → `service_on_accept` → `http_servlet::doRun`으로 이어지는 과정에서
    비동기 처리와 코루틴이 사용되어 서버가 동시에 다수의 클라이언트 요청을 처리할 수 있게 됩니다.
  
  따라서, 이 코드에서 비동기적 처리가 이루어지는 구체적인 함수는 `run_alone`과 그 내부에서 호출되는 `http_servlet::doRun` 메서드입니다.

  ---

따라서, 이 코드에서 비동기 구현은 **클라이언트의 연결 수락부터 요청 처리까지의 모든 과정**에 적용되어 있습니다. 
이는 `master_fiber` 클래스와 이를 상속받은 `http_server_impl` 클래스의 비동기 처리 메커니즘에 의해 구현됩니다.


<br> <br>

- 기본적인 Acl의 HTTP Module (server)를 다루고 있는 문서는 다음과 같습니다. [📄Acl HTTP Server 기능 정리](../Document/Acl-HTTP-Server.md)
















