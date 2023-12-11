# RIORING
  
  
## 원래 저장소에 있던 README.md의 내용  
https://github.com/newbiediver/rioring  
  
[![Windows-Build](https://github.com/newbiediver/rioring/actions/workflows/build_windows.yml/badge.svg)](https://github.com/newbiediver/rioring/actions/workflows/build_windows.yml)
[![Linux-Build](https://github.com/newbiediver/rioring/actions/workflows/build_linux.yml/badge.svg)](https://github.com/newbiediver/rioring/actions/workflows/build_linux.yml)
[![Dockerized](https://github.com/newbiediver/rioring/actions/workflows/build_docker.yml/badge.svg)](https://github.com/newbiediver/rioring/actions/workflows/build_docker.yml)
![Version](https://img.shields.io/badge/Version-0.2.1-orange.svg)

[https://github.com/newbiediver/rioring](https://github.com/newbiediver/rioring)

C++17 크로스 플랫폼 비동기 네트워킹 i/o 라이브러리. <br />
C++17 Cross platform asynchronous networking i/o library.

윈도우 플랫폼과 리눅스 플랫폼에서 사용가능 한 가장 최신 모델을 사용합니다.<br />
This library use the latest models available on the Windows and Linux platforms.

윈도우 플랫폼에서 Registered i/o, 리눅스 플랫폼에서는 io_uring 을 기본 모델로 사용합니다.<br />
The default model is Registered i/o on the Windows platform and io_uring on the Linux platform.

이 i/o 라이브러리는 내장된 socket object 만 지원합니다. 파일 디스크립터는 지원하지 않습니다.<br />
This i/o library does support only built-in socket object. No file descriptor supported.

***노트: 기여자는 dev branch 에 PR 요청하시기 바랍니다. 곧 기여자 정책에 대해서 업데이트 될 예정입니다.*** <br />
***NOTE: Please contributor request PR to dev branch. We will update about contributor policy soon.***

## Requirement
***노트: clang 컴파일러는 테스트 해보지 않았습니다.***<br />
***NOTE: clang compiler has not tested.***
### 1. Win32
```
  Minumum - MSVC 16.0 (Visual Studio 2019)
  Recommend - MSVC 17.0 (Visual Studio 2022)
```
  
### 2. Linux
```
  Minumum - gcc 9
  Recommend - gcc 11
```
   
### 3. Arch
```
  x64/amd64
```

### 4. Environments
```
* Windows
  - >= Windows 8, Windows Server 2012 SP3
* Linux
  - >= Kernel 5.13
  - >= liburing 2.1 (Ex> Installable 2.1 on ubuntu apt)
```

## Features
### 1. Most latest network I/O service
```shell
Registered I/O - Windows platform
I/O Uring - Linux platform
```
### 2. Built-in network socket
```shell
TCP - Socket and server system
UDP - Socket and server system
```
### 3. Thread scheduling
```shell
Thread generator - Generate thread wrapper
Thread object - Thread as a object
```

### 4. Buffer
```shell
Safe unlimited buffer - Never overflow buffer
Double buffer - Switchable unlimited buffer
```

### 5. Many helpers
```shell
Built-in address resolver
Etc...
```

## Build & Install
### 1. Windows Platform
```shell
# 소스코드 클론
# Clone source code
C:\dev> git clone https://github.com/newbiediver/rioring.git ./source

# 빌드
# Build
C:\your\path> cd source
C:\your\path\source> cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/your/path/rioring -B .\build
C:\your\path\source> cmake --build .\build

# 설치
# Install
C:\your\path\source> cmake --install .\build\src

# C:\your\path\rioring\include <-- header files path
# C:\your\path\rioring\lib <-- library file path
```

### 2. Linux Platform
```shell
# liburing 라이브러리 설치 
# Install liburing library (Ex> Debian/Ubuntu)
~/your/path $ sudo apt install -y liburing-dev

# 소스코드 클론
# Clone source code
~/your/path $ git clone https://github.com/newbiediver/rioring.git ./source

# 빌드
# Build
~/your/path $ cd source
~/your/path/source $ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -B ./build
~/your/path/source $ cmake --build ./build

# 설치
# Install
~/your/path/source $ sudo cmake --install ./build/src 

# /usr/local/include <-- header files path
# /usr/local/lib <-- library file path
```

## Example - Echo server
### *NOTE: Please refer example directory to see other examples*
```c++
#include <iostream>
#include <unordered_set>
#include <rioring.h>

using namespace rioring;
using namespace std::chrono_literals;

class event_receiver {
public:
    event_receiver() = default;
    ~event_receiver() = default;

    void on_accept_client( tcp_socket_ptr &new_socket ) {
        // register events for new socket at this time.
        // Normally receive and close events and or error event.
        // If you need sending completion event, you can also register 'set_send_complete_event'.

        new_socket->set_close_event( [event = this]( auto && socket ) {
            event->on_disconnect_client( std::forward< decltype( socket ) >( socket ) );
        } );

        new_socket->set_receive_event( []( auto &&socket, auto &&buffer, auto &&addr ) {
            event_receiver::on_read_message( std::forward< decltype( socket ) >( socket ), 
                    std::forward< decltype( buffer ) >( buffer ),
                    std::forward< decltype( addr ) >( addr ) );
        } );

        std::cout << "Connected client " << new_socket->remote_ipv4() << ":" << new_socket->remote_port() << std::endl;

        std::scoped_lock sc{ container_lock };
        activating_sockets.insert( new_socket );
    }

    static void on_read_message( socket_ptr &socket, io_buffer *buffer, sockaddr *addr [[maybe_unused]] ) {
        // just echo
        auto tcp = to_tcp_socket_ptr( socket );
        tcp->send( *(*buffer), buffer->size() );

        // pop contents from buffer
        buffer->pop( buffer->size() );
    }

    void on_disconnect_client( socket_ptr &socket ) {
        std::cout << "Disconnect client " << socket->remote_ipv4() << ":" << socket->remote_port() << std::endl;

        std::scoped_lock sc{ container_lock };
        activating_sockets.erase( socket );
    }

    void leave_clients() {
        std::scoped_lock sc{ container_lock };
        for ( auto & socket : activating_sockets ) {
            auto tcp = to_tcp_socket_ptr( socket );
            tcp->close();
        }
    }

    void wait() const {
        while ( !activating_sockets.empty() ) {
            std::this_thread::sleep_for( 1ms );
        }
    }

    critical_section container_lock;
    std::unordered_set< socket_ptr > activating_sockets;
};

int main() {
    constexpr unsigned short port = 8000;
    event_receiver receiver;
    std::string in;

    // Initialize i/o service.
    io_service io;
    if ( !io.run( static_cast< int >( std::thread::hardware_concurrency() ) ) ) {
        std::cerr << "failed to run io service" << std::endl;
        return 1;
    }
    std::cout << "started io service" << std::endl;

    // Initialize tcp listener.
    tcp_server_ptr server = tcp_server::create( &io );
    if ( !server ) {
        std::cerr << "failed to create server object" << std::endl;
        return 1;
    }

    // accept event.
    server->set_accept_event( [event = &receiver]( auto && new_socket ) {
        event->on_accept_client( std::forward< decltype( new_socket )>( new_socket ) );
    } );

    // run this server.
    if ( !server->run( port ) ) {
        std::cerr << "failed to open server port" << std::endl;
        return 1;
    }
    std::cout << "started server. port: " << port << std::endl;

    std::cout << "Enter 'exit' to terminate server" << std::endl;
    while ( true ) {
        std::cin >> in;
        if ( in == "exit" ) {
            break;
        }
    }

    receiver.leave_clients();
    receiver.wait();

    server->stop();
    io.stop();

    return 0;
}
```

## Dependencies
1. **Win32 - Nothing. Just winsock.**
2. **Linux - [axboe/liburing](https://github.com/axboe/liburing)**

## Using docker image
```dockerfile
# cmake g++-11 ninja-build included
# you can use rioring include & rioring.a library file 
FROM ghcr.io/newbiediver/rioring:latest
...
```

## Next Step
1. ~~Implement UDP socket~~ - updated
2. ~~Support docker image~~ - updated
3. **Support package manager** - developing

# License
*MIT license Copyright (c) 2022 NewbieDiver*
