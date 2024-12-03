# CPP-Socket-acl

이 프로젝트는 Visual C++ 환경에서 오픈 소스 네트워크 프레임워크인 Acl(Advanced C/C++ Library)을 활용한 소켓 프로그래밍 학습을 목표로 하고 있습니다.

<br>

# 목차
1. [ACL 이란?](#acl-이란)
   - ACL 간단 소개
   - ACL 주요 기능 소개
   - 이 깃허브에서 다루는 실습 기능 소개

2. [ACL 빌드 및 정적 라이브러리로 추가하는 방법](#acl-빌드-및-정적-라이브러리로-추가하는-방법)

3. [ACL 기능 별 실습](#acl-기능-별-실습)
   - [TCP Echo 서버 (TCP Echo Server + CppConsoleClient)](#1-tcp-echo-server--cppconsoleclient)
   - [Coroutine을 사용한 TCP Echo 서버 (TCP Coroutine Echo Server + CppConsoleClient)](#2-tcp-coroutine-echo-server--cppconsoleclient)
   - [Redis 실습 (Redis Server Acl)](#3-redis)
   - [MySQL 실습 (Mysql Server Acl)](#4-mysql)
   - [동기화 객체 실습 (Sync Objects Acl)](#5-acl-synchronization-object)
   - [HTTP Server 실습 (Http Acl)](#6-http-server)

4. [ACL 채팅 서버 실습 (with C# WinForms client)](#acl-채팅-서버-실습-with-c-winforms-client)
   - 실습 목표 소개
   - 주요 기능
      + Redis를 사용한 로그인
      + 채팅 기능
      + C#과 C++ 사이의 패킷 직렬화 기법
      + 동기화 객체
   - 실습 캡쳐

5. [C++23을 활용한 코드 리팩토링](#c23-리팩토링-개념-및-수정-사항-정리)

6. [문서](#문서)
  
<br>

---

# ACL 이란?

[The Acl (Advanced C/C++ Library)](https://github.com/acl-dev/acl/tree/master)은 강력한 다중 플랫폼 네트워크 통신 라이브러리 및 서비스 프레임워크입니다.

지원되는 플랫폼: Linux, Windows, Solaris, FreeBSD, MacOS, iOS, AndroidOS

<br>

HTTP/SMTP/ICMP//MQTT/Redis/Memcached/Beanstalk/Handler Socket과 같은 일반적인 프로토콜이 Acl에서 구현되며, 

코덱 라이브러리(XML/JSON/MIME/BASE64/UUCODE/QPCODE/RFC2047/RFC1035)도 포함되어 있습니다. 

또한 Acl은 Mysql, Postgresql, Sqlite와 같이 널리 사용되는 데이터베이스를 위한 통합 추상 인터페이스도 제공합니다. 

Acl 라이브러리를 사용하면 데이터베이스 애플리케이션을 더 쉽고, 빠르고, 안전하게 작성할 수 있습니다.

<br>

## ACL 주요 기능 소개
ACL 라이브러리에서 제공하는 주요 기능은 다음과 같습니다:

1. **Basic Network Module**
   - 가장 기본적인 스트리밍 통신 모듈로, 네트워크 스트리밍을 지원할 뿐만 아니라 파일 스트리밍도 지원

2. **Coroutine (코루틴)**
   - 비동기 코드를 작성할 수 있는 경량 스레드인 코루틴 지원
   - 이에 따라 lib_fiber는 다양한 동기화 객체 지원
     * Acl은 코루틴 및 멀티 스레드 기반의 프로그램에서 동기화 객체를 지원하여, 코루틴/스레드 간 상호 배제와 신호 전달을 안전하게 관리하여 프로그램의 안정성과 성능을 보장

3. **HTTP Module**
   - 고성능 HTTP 서버 및 클라이언트 구현 가능

4. **Redis Client**
   - Redis와의 상호작용을 위한 고성능 클라이언트 모듈 제공

5. **MQTT Module**
   - IoT 장치와의 통신을 위한 MQTT 모듈 제공

6. **Server Framework**
   - 다양한 서버 아키텍처를 위한 템플릿 및 기본 구조 제공

## 이 깃허브에서 다루는 실습 기능 소개

이 깃허브 프로젝트에서는 ACL을 사용하여 TCP 서버, HTTP 서버, 코루틴과 동기화 객체, DB(Redis, Mysql)과 같은 기능을 실습합니다. <br>
또한 채팅 서버 실습을 통해, C++에서 C# WinForms 클라이언트와의 통신도 구현합니다. <br>
   - TCP Echo 서버 (TCP Echo Server + CppConsoleClient) [🖱️click](./TCPServerAcl)
   - Coroutine을 사용한 TCP Echo 서버 (TCP Coroutine Echo Server + CppConsoleClient) [🖱️click](./TCPCoroutineServerAcl)
   - Redis 실습 (Redis Server Acl) [🖱️click](./RedisServerAcl)
   - MySQL 실습 (Mysql Server Acl) [🖱️click](./MySQLServerAcl)
   - 동기화 객체 실습 (Sync Objects Acl) [🖱️click](./SyncObjectsAcl)
   - HTTP Server 실습 (Http Acl) [🖱️click](./HttpAcl)
   - 채팅 서버 실습 (Chat Server Acl) [🖱️click](./ChatServerAcl)
<br>
그리고 C++23 버전을 지원하여, C++23의 다양한 기능을 도입하여 코드의 가독성, 성능, 유지 보수성을 개선했습니다.

<br> <br>

---


# ACL 빌드 및 정적 라이브러리로 추가하는 방법

## 1. ACL 빌드
1. 이 리포지토리의 [📁acl 디렉토리](./acl)로 이동합니다. 또는 [🖱️공식 깃허브](https://github.com/acl-dev/acl)를 clone 합니다.
2. 빌드를 진행하세요. **단, 자신의 프로젝트의 구성(Debug/Release)과 플랫폼(x64/x86)과 일치하도록**
   - windows 기준 : `acl_cpp_vc2022.sln` 솔루션에서 필요한 각 프로젝트의 빌드를 진행한다. <br>
      * lib_acl
      * lib_protocol
      * lib_acl_cpp
      * libfiber
      * libfiber_cpp
   - 빌드 방법에 대한 자세한 내용은 [📄Acl 빌드 후 정적 라이브러리로 추가하기](./Document/Build.md)를 참조하세요.
<br><br>

#### 만약 위의 과정을 생략하고 현재 나의 빌드 버전으로 테스트만 진행하고 싶다면,
현재 내 프로젝트에서 기본적으로 acl 라이브러리 세팅이 되어있기 때문에 테스트가 가능항 상황이다. <br> <br>

만약, 빌드에 문제가 있다면 아래 과정을 따라하자. 
- 이 리포지토리의 [📁acl/Debug_lib_Files 디렉토리](./acl/Debug_lib_Files) 에 빌드 파일이 위치하고 있다. 
- 따라서 추가로 빌드 과정을 거치고 싶지 않다면,
  1. [해당 위치(📁acl/Debug_lib_Files 디렉토리)](./acl/Debug_lib_Files)에 존재하는 .lib 파일들을 그대로 자신의 로컬 [📁acl/Debug 디렉토리](./acl/Debug)에 옮긴 후
  2. 아래 과정을 (경로까지 같게) 똑같이 따라하면 된다. 
  하지만 학습을 위해 직접 빌드부터 라이브러리 추가까지의 과정을 경험하는 것을 추천한다. <br>

## 2. 정적 라이브러리 추가
- 구성관리자 > 구성 & 플랫폼 반드시 확인 (ex. Debug/x64)
- C/C++ > 일반 > **추가 포함 디렉터리**에 `헤더파일 경로` 추가
  + `..\acl\lib_fiber\c\include;..\acl\lib_fiber\cpp\include;..\acl\lib_acl_cpp\include;..\acl\lib_protocol\include;..\acl\lib_acl\include;..\acl\include;%(AdditionalIncludeDirectories)`
- 링커 > 일반 > **추가 라이브러리 디렉터리**에 `lib 파일 경로` 추가
  + Debug : `..\acl\Debug_lib_Files;%(AdditionalLibraryDirectories)`
  + Release : `..\acl\Release_lib_Files;%(AdditionalLibraryDirectories)`
- 링커 > 입력 > **추가 종속성**에 `lib파일 이름` 추가
  + `lib_protocol.lib;lib_acl.lib;lib_acl_cpp.lib;libfiber.lib;libfiber_cpp.lib;$(CoreLibraryDependencies);%(AdditionalDependencies)`

<br>

정적 라이브러리를 Visual C++ 프로젝트에 추가하는 방법은 [📄친절한 정적 라이브러리 추가 방법](./Document/HowToAddCPPStaticLibrary.md)에 사진과 함께 설명되어 있습니다.

<br>

---


# ACL 기능 별 실습

## 1. TCP Echo Server + CppConsoleClient

이 실습은 기본적인 TCP Echo 서버와 C++로 구현된 간단한 콘솔 클라이언트를 다룹니다. 서버는 클라이언트로부터 받은 데이터를 그대로 돌려줍니다.

1. 서버 코드는 `TCPServerAcl/` 디렉토리에 있습니다. [🖱️click](./TCPServerAcl)
2. 클라이언트 코드는 `CppConsoleClient/` 디렉토리에 있습니다. [🖱️click](./CppConsoleClient)

<br>

## 2. TCP Coroutine Echo Server + CppConsoleClient

이 실습은 TCP Echo 서버를 코루틴 기반으로 확장합니다. 이 서버는 경량 코루틴을 사용하여 클라이언트와 비동기적으로 통신합니다.

1. 서버 코드는 `TCPCoroutineServerAcl/` 디렉토리에 있습니다. [🖱️click](./TCPCoroutineServerAcl)
2. 클라이언트 코드는 `CppConsoleClient/` 디렉토리에 있습니다. [🖱️click](./CppConsoleClient)

<br>


## 3. Redis

ACL을 사용하여 Redis와 상호작용하는 예제입니다. 이 예제는 Redis 클라이언트 모듈을 사용하여 데이터베이스와 통신합니다. <br>
Redis 연결 설정, 주요 자료형 실습(String, List, Set, Hash, Sorted Set), Redis JSON 사용 실습을 다루고 있습니다. <br>

1. Redis 코드는 `RedisServerAcl/` 디렉토리에 있습니다. [🖱️click](./RedisServerAcl)

<br>


## 4. MySql

ACL을 사용하여 MySql와 상호작용하는 예제입니다. 이 예제에서는 lib_acl을 통해 mysql 연결 및 쿼리문을 보다 편히 다루고 있습니다. <br>
MySQL 데이터베이스 연결 및 상호작용하여 데이터를 삽입, 조회, 업데이트 및 삭제하는 간단한 CRUD(Create, Read, Update, Delete) 작업을 수행하고 있습니다. <br>

1. MySql 코드는 `MySQLServerAcl/` 디렉토리에 있습니다. [🖱️click](./MySQLServerAcl)

<br>


## 5. Acl Synchronization Object

ACL이 제공하는 동기화 객체를 학습하는 예제입니다. 이 예제에서는 비동기 프로그래밍 및 코루틴 기반의 프로그램에서 사용할 수 있는 다양한 동기화 객체에 대해 학습합니다. <br>
코루틴이나 멀티스레드 환경에서 안정성과 자원 경합을 방지하기 위해 동기화 객체는 항상 필요합니다. <br>
구체적으로 아래와 같은 동기화 객체에 대해 다룹니다.

- fiber_mutex : 코루틴 전용 뮤텍스 객체
- fiber_lock : 일반적인 락 메커니즘을 제공하는 락 객체
- fiber_rwlock : 읽기-쓰기 락 객체
- fiber_sem : 세마포어 객체 (접근 가능한 코루틴 수 제한)
- fiber_event : 코루틴/스레드 간 신호를 전달하여 트리거하는 이벤트 객체
- fiber_cond : 특정 조건을 만족할 때까지 대기하는 코루틴을 깨우기 위해 사용하는 조건 변수 객체
- fiber_event_mutex : 코루틴 기반의 이벤트 뮤텍스 객체

1. Acl Synchronization Object 코드는 `SyncObjectsAcl/` 디렉토리에 있습니다. [🖱️click](./SyncObjectsAcl)

<br>



## 6. HTTP Server

Acl을 사용하여 구현한 HTTP Server입니다. 이 예제는 acl의 http module을 활용해 C++에서도 HTTP 서버 구현을 진행합니다. <br>
구체적으로 Acl의 fiber를 통한 비동기 http server의 구현에 대해 학습합니다.

1. Http Server 코드는 `HttpAcl/` 디렉토리에 있습니다. [🖱️click](./HttpAcl)


<br>


<br>

---


# ACL 채팅 서버 실습 (with C# WinForms client)

Acl이 제공하는 유용한 기능을 활용하여 C++ TCP 채팅 서버를 구현합니다. 이때 C# Winforms client와 통신합니다.

1. 서버 코드는 `ChatServerAcl/` 디렉토리에 있습니다. [🖱️click](./ChatServerAcl)
2. 클라이언트 코드는 `WinFormsClient/` 디렉토리에 있습니다. [🖱️click](./WinFormsClient)

## 실습 목표 소개

이 실습의 목표는 C++ ACL을 사용하여 TCP 채팅 서버를 구축하고, C# WinForms 기반의 클라이언트와 연동하는 것입니다. <br>
이 과정에서 Redis를 사용한 사용자 로그인, 채팅 메시지 처리, 패킷 직렬화 및 역직렬화 등의 개념을 다룹니다.

## 주요 기능

1. **Redis를 사용한 로그인**: 사용자 인증은 Redis 데이터베이스에서 관리됩니다. 사용자가 로그인하면 서버는 Redis에서 해당 사용자의 정보를 확인합니다.

2. **채팅 기능**: 여러 사용자가 채팅방에 입장하여 메시지를 주고받을 수 있습니다. 서버는 각 사용자의 메시지를 해당 방에 있는 모든 사용자에게 브로드캐스트합니다.

3. **C#과 C++ 사이의 패킷 직렬화 기법**
   - **Protocol Buffers**: [Protocol Buffers](https://protobuf.dev/)를 사용하여 C#과 C++ 간의 데이터 직렬화/역직렬화를 보다 쉽게 할 수 있습니다.
   - **현재 구현 방식**: 이 프로젝트에서는 Protocol Buffers 같은 오픈소스 직렬화 데이터 구조를 사용하지 않고, <br>
     직접 메모리 버퍼에 쓰고 읽는 식으로 구현한 직렬화 방식으로 패킷을 주고받습니다. <br>
     이는 사용자 정의된 간단한 직렬화 포맷을 사용하여 데이터를 처리합니다.

4. **동기화 객체**
   - 코루틴 사용 시 동기화 객체의 필요성 [📄동기화 객체란?](./Document/Cpp-SynchronizationObject.md)
   - Acl가 지원하는 동기화 객체 fiber_mutex 를 통해 코루틴/스레드 간 상호 배제와 신호 전달을 안전하게 관리하여 프로그램의 안정성과 성능을 보장
   - [📄lib_fiber가 지원하는 동기화 객체에 대한 설명](./Document/Cpp-SynchronizationObject.md)
   - [📄동기화 객체 fiber_mutex에 대한 설명](./Document/Coroutine-fiber_mutex.md)


## 실습 캡쳐

채팅 프로젝트의 실행 결과를 캡처한 화면입니다:

![실습 캡쳐 이미지](./Document/img/ChatServerExample.PNG) 

<br><br>

---
# C++23 리팩토링 개념 및 수정 사항 정리

C++23로 리팩토링을 진행하면서, 최신 C++ 표준의 기능들을 적용하여 코드의 안정성과 가독성을 개선할 수 있습니다. <br>
아래는 주요 개념들과 이들을 적용한 구체적인 수정 사항을 정리한 내용입니다. <br>

### 1. `std::byte` 사용

#### 개념 설명
`std::byte`는 바이트 단위의 데이터를 명확하게 표현하는 타입으로, 문자 데이터를 저장하는 `char`와는 구분됩니다. 이를 통해 바이트 단위의 연산이나 메모리 조작의 의도를 더욱 명확하게 나타낼 수 있습니다.

#### 적용된 수정 사항
모든 바이트 처리 부분에서 기존의 `char*` 대신 `std::byte*` 혹은 `std::span<std::byte>`를 사용하도록 변경하였습니다. 이는 데이터가 문자 데이터가 아닌 순수 바이트임을 명확히 표현하며, 타입 안정성을 높입니다.

**변경 전:**
```cpp
void PacketHeader::Serialize(char* buffer) const;
PacketHeader PacketHeader::Deserialize(const char* buffer);
```

**변경 후:**
```cpp
void PacketHeader::Serialize(std::span<std::byte> buffer) const;
[[nodiscard]] static PacketHeader Deserialize(std::span<const std::byte> buffer);
```

### 2. `std::span` 사용

#### 개념 설명
`std::span`은 배열이나 연속된 메모리 블록을 안전하고 효율적으로 다룰 수 있는 클래스 템플릿입니다.  <br>
`std::span`을 사용하면 버퍼 크기와 데이터를 함께 처리할 수 있어, 경계를 넘어서는 접근으로 인한 버그를 방지할 수 있습니다. <br>

#### 적용된 수정 사항
함수 인자로 전달되는 모든 버퍼를 `std::span`으로 변경하였습니다. 이를 통해 버퍼 크기를 별도로 전달할 필요가 없어졌으며, 잘못된 크기의 버퍼가 전달되는 경우를 방지할 수 있습니다.

**변경 전:**
```cpp
void LoginRequest::Serialize(char* buffer) const;
LoginRequest LoginRequest::Deserialize(const char* buffer);
```

**변경 후:**
```cpp
void LoginRequest::Serialize(std::span<std::byte> buffer) const;
[[nodiscard]] static LoginRequest Deserialize(std::span<const std::byte> buffer);
```

### 3. `std::array` 사용

#### 개념 설명
`std::array`는 고정 크기 배열을 안전하게 관리할 수 있는 표준 템플릿 클래스입니다. <br>
배열의 크기가 타입에 포함되므로, 함수에 배열을 전달할 때 더욱 안전하게 사용할 수 있습니다. <br>

#### 적용된 수정 사항
고정 크기의 배열(`UserID`, `Message` 등)을 `std::array`로 변경하였습니다.  <br>
이를 통해 배열의 크기를 코드 내에서 명확하게 관리할 수 있으며, 메모리 안전성을 향상시켰습니다. <br>

**변경 전:**
```cpp
struct LoginRequest : public PacketHeader {
    char UserID[32];
    char AuthToken[32];
};
```

**변경 후:**
```cpp
struct LoginRequest : public PacketHeader {
    std::array<char, 32> UserID{};
    std::array<char, 32> AuthToken{};
};
```

### 4. `std::bit_cast` 사용

#### 개념 설명
`std::bit_cast`는 비트 단위의 안전한 타입 변환을 제공하는 C++23의 기능입니다. <br>
메모리 상의 데이터를 그대로 복사하여 다른 타입으로 변환할 때 사용되며, 비트 해석에 대한 안전성을 보장합니다. <br>

#### 적용된 수정 사항
이번 코드 리팩토링에서는 `std::bit_cast`를 적용할 부분이 많지 않았지만, 데이터 직렬화와 역직렬화 작업에서 타입 간 변환이 필요할 경우 유용하게 활용할 수 있습니다.

### 5. `[[nodiscard]]` 속성 사용

#### 개념 설명
`[[nodiscard]]` 속성은 함수의 반환 값을 무시해서는 안 되도록 강제하는 C++ 표준 속성입니다. 이를 통해 반환 값을 무시함으로써 발생할 수 있는 잠재적인 오류를 방지할 수 있습니다.

#### 적용된 수정 사항
모든 역직렬화 함수에 `[[nodiscard]]` 속성을 추가하였습니다. 이를 통해 반환 값을 무시하지 않고 반드시 사용하도록 강제하여, 함수의 결과가 의도대로 사용되지 않는 문제를 예방합니다.

**변경 전:**
```cpp
PacketHeader PacketHeader::Deserialize(const char* buffer);
```

**변경 후:**
```cpp
[[nodiscard]] static PacketHeader Deserialize(std::span<const std::byte> buffer);
```

### 요약
이번 리팩토링에서는 C++23의 최신 기능들을 활용하여 코드의 안정성과 가독성을 크게 향상시켰습니다. 
특히 `std::byte`, `std::span`, `std::array`를 도입하여 메모리 및 버퍼 관리를 더 안전하게 처리할 수 있도록 하였으며, 
`[[nodiscard]]` 속성을 통해 중요한 함수 반환 값을 반드시 처리하도록 하였습니다. 
이러한 리팩토링은 코드의 유지 보수성을 높이는 데 기여할 것입니다.

---



# 문서

## 프로젝트 시작 이전
- [오픈소스 라이브러리(Acl 빌드 후) 정적 라이브러리로 추가 방법](./Document/Build.md)
- [친절한 정적 라이브러리 추가 방법](./Document/HowToAddCPPStaticLibrary.md)

## 개념 및 실습 예제
- [코루틴 설명 및 예제](./Document/Coroutine.md)
- [Acl HTTP 기능](./Document/Acl-HTTP-Server.md)
- [동기화 객체](./Document/Cpp-SynchronizationObject.md)
     + [코루틴 동기화 & fiber_mutex](./Document/Coroutine-fiber_mutex.md)
- [Redis 설명 및 예제](./Document/Redis.md)
    + [Redis 자주 사용하는 자료형](./Document/Acl-Redis-Data.md)
    + [Redis JSON](./Document/Acl-Redis-Json.md)
- [MySQL 설명 및 예제](./Document/MySQL-Acl.md)
- [예제 소스코드 (기본, 코루틴, DB)](./Document/EX.md)

## 원문 번역
- [예제 디렉토리 설명-번역](./Document/SAMPLES-KO.md)


## 추후 진행할 TODO
- [x] 기계 번역했던 문서 링크 걸기
- [ ] httpclient, redis 기능 사용했을 때 사용 안했을 때와 비교해서 메모리 사용량 차이. 프로그램 빌드 시작 때 확인
- [ ] httpclient, redis 기능을 사용했을 때 메모리릭은 없는지(아주 많은 횟수를 5~6시간 사용해보고)
- [ ] 파이버 사용 소켓 서버 성능 및 안정성(긴 시간 운용) 테스트
- [ ] 유닛 테스트 코드 추가하기
- [ ] chat server에 오목 게임 컨텐츠 추가하기
