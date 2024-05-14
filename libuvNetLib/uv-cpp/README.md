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