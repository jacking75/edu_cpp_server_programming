# uv-cpp
- uv-cpp is a simple interface, high-performance network library based on C++11.
- [GitHub](https://github.com/wlgq2/uv-cpp )    
  
  
## Features  
- C 스타일 함수 포인터 대신 C++11 함수/바인드 스타일 콜백.
- TCP 및 UDP 래퍼.
- DNSandHttp：DNS 쿼리 및 http 지원, 기수 트리에 기반한 http 라우팅.
- TimerandTimerWheel：시간 복잡도 O(1)의 하트비트 타임아웃 판단.
- Async：libuv 비동기 래퍼를 사용하지만 여러 번 호출하지만 콜백은 한 번만 호출되는 문제를 최적화했다.
- 패킷 및 패킷 버퍼：Tcp 데이터 스트림의 패킷을 주고받는다. 사용자 정의 데이터 패킷 구조(예: uvnsq) 지원
- 로그 인터페이스.  
  

## Build Instructions
- VS2017 (windows)
- Codeblocks (linux)
- CMake (linux)  
   

## Quick start
A simple echo server  
```
#include <iostream>
#include <uv/include/uv11.h>

int main(int argc, char** args)
{
    uv::EventLoop* loop = uv::EventLoop::DefaultLoop();
	
    uv::TcpServer server(loop);
    server.setMessageCallback([](uv::TcpConnectionPtr ptr,const char* data, ssize_t size)
    {
        ptr->write(data, size, nullptr);
    });
    //server.setTimeout(60); //heartbeat timeout.
	
    uv::SocketAddr addr("0.0.0.0", 10005, uv::SocketAddr::Ipv4);
    server.bindAndListen(addr);
    loop->run();
}
```  
  
A simple http service router which based on radix tree.  
```
int main(int argc, char** args)
{
    uv::EventLoop loop;
    uv::http::HttpServer::SetBufferMode(uv::GlobalConfig::BufferMode::CycleBuffer);

    uv::http::HttpServer server(&loop);
	
    //example:  127.0.0.1:10010/test
    server.Get("/test",std::bind(&func1,std::placeholders::_1,std::placeholders::_2));
    
    //example:  127.0.0.1:10010/some123abc
    server.Get("/some*",std::bind(&func2, std::placeholders::_1, std::placeholders::_2));
    
    //example:  127.0.0.1:10010/value:1234
    server.Get("/value:",std::bind(&func3, std::placeholders::_1, std::placeholders::_2));
    
    //example:  127.0.0.1:10010/sum?param1=100&param2=23
    server.Get("/sum",std::bind(&func4, std::placeholders::_1, std::placeholders::_2));
    
    uv::SocketAddr addr("127.0.0.1", 10010);
    server.bindAndListen(addr);
    loop.run();
}
```  





