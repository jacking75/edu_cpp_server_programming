# C++ 서버 프로그래밍 실습
      
## TODO
- 처음부터 직접 네트워크 라이브러리 만들기  
    - 네트워크 프로그래밍을 위한 핵심 네트워크 이론을 배운다.
	- 멀티스레드 프로그래밍을 배운다.
	- Socket API 에 대해서 학습한다 
	- 동기 I/O 기반의 채팅 서버, 게임 서버를 만든다
	- 비동기 I/O 기반의 채팅 서버, 게임 서버를 만든다
- 오픈 소스 네트워크 라이브러리 리팩토링
- 오픈 소스 네트워크 라이브러리를 사용하여 게임 서버 만들기  
    - 로그 사용에 대해서 배운다
	- 실용적인 게임 서버 개발 방법을 배운다
	
	
### 공통
- 패킷을 직렬화 하고 핸들링 하는 방법을 배운다.
- 효율적인 메모리 관리 방법을 배운다.
- 많은 클라이언트 접속을 처리할 수 있는 서버를 만들어 본다.
- 더미 클라이언트를 이용한 테스트.
- 게임 서버 구조에 대해서 배운다.
- 턴 방식의 단독 게임 서버를 만들어 본다.
- 스케일 아웃을 고려한 게임 서버를 만들어 본다
- 서버 크래시에 대처하는 방법을 배운다.
- DB 프로그래밍
- Redis 프로그래밍
- 오목 게임 만들기
- mmo 게임 만들기
    - 클라는 콘솔이나 winform 
	- 2d 게임
	- 맵관리
- 패킷 포맷
    - 바이너리
	- 구글 프로토버퍼
- 링버퍼 만들기
- 로그 라이브러리 만들기
- 메모리 풀
    - 객체 풀
	- 크기별 메모리 풀 만들기 
	- tcmalloc 사용하기
  
    
    
## Sync I/O(select 사용) 방식으로 서버 만들기 
`SyncIOChatServer` 디렉토리에 있는 코드를 토대로 채팅 서버와 오목게임서버를 만든다.  

- 채팅 서버 만들기
    - [참고 코드: SelectChatServer](https://github.com/jacking75/com2us_cppNetStudy_work/tree/master/SelectChatServer ) 를 참고한다  
- 오목 게임 서버 만들기
    - 채팅 서버를 다 만들었다면 여기에 오목 게임 콘텐츠를 구현하여 온라인 오목 게임을 만든다.
	- 오목 게임 클라이언트는 `CSharp_OmokClient` 디렉토리에 있는 것을 사용한다.  
    
    
<br/>
	

## Async I/O(IOCP 사용) 방식으로 서버 만들기
  
### IOCP 단계별 과제 구현하기
[edu_cpp_IOCP](https://github.com/jacking75/edu_cpp_IOCP)   
IOCP 프로그래밍에 대해서 학습한다  
  
### (도서)"온라인 게임 서버" 분석 및 리팩토링
[YES24](http://www.yes24.com/Product/Goods/1776627 )  
이 책을 통해서 게임 서버를 만들 때 IOCP를 어떻게 사용하는지 배운다.  
이 책의 예제 코드는 `book_codes` 디렉토리에 있는 `codes_book_onlinegameserver.zip` 이다.   
책의 코드를 이해 했다면 이 코드를 Modern C++ 코드로 리팩토링한다.    
- MFC는 사용하지 않는다.
- C++ 다운 코드를 만들고 네임컨벤셜을 따른다.  
    
    
### IOCP를 사용한 오목 게임 서버 
IOCP API를 사용하는 네트워크 라이브러리를 만든 후 이것을 사용하는 오목 게임 서버를 개발한다.
클라이언트는 `CSharp_OmokClient` 에 있는 C# WinForm 프로그램을 사용한다.    
클라이언트 개발 편의를 위해 C++ 이나 Unity를 사용하지 않는다
   
   
### (도서)"게임 서버 프로그래밍 입문" 분석
[YES24](http://www.yes24.com/Product/Goods/18497117 )  이 책에는 IOCP를 사용하여 게임 서버를 만드는 방법을 설명하고 있다.  
책의 내용을 응용해서 게임 서버를 만들어본다.  
    
	
### 참고 코드
- `IOCP` 디렉토리에 있는 `IocpNetLib` 라이브러리와 이것을 사용하여 만든 `IocpNetLib_Echo`, `IocpNetLib_IocpChatServer`을 참고한다.
- `IOCP` 디렉토리에 있는 `nhn_next_IocpServer` 를 참고한다.
  
  
### 코드 분석 및 리팩토링 하기
`IOCP_ReWork` 디렉토리에 있는 코드들을 분석한 후 기능 개선 및 최신의 C++ 코드로 리팩토링한다.  

     
<br/>     
    
	
## 오픈 소스 코드 분석
C++ 학습 및 게임 서버 개발에 사용할 목적으로 아래 오픈소스 코드를 분석한다.  
  
- [CppLogger2. Lightweight logger for C++](https://github.com/emilienlemaire/CppLogger2 )
- [pretty_log](https://github.com/myyrakle/pretty_log  )
- [BlackMATov/curly.hpp  Simple cURL C++17 wrapper](https://github.com/BlackMATov/curly.hpp )  
- `opensource` 디렉토리의 Dispatcher
- `opensource` 디렉토리의 Ring-Log
  
  
## 오픈 소스 네트워크 라이브러리 및 프레임워크 활용  
- acl
    - Acl(고급 C/C++ 라이브러리) 프로젝트는 강력한 멀티 플랫폼 네트워크 통신 라이브러리 및 서비스 프레임워크로, Linux, WIN32, Solaris, FreeBSD, MacOS, AndroidOS, iOS를 지원한다.
	- Acl 프로젝트에는 네트워크 통신, 서버 프레임워크, 애플리케이션 프로토콜, 다중 코더 등 몇 가지 중요한 모듈이 있다.
	- HTTP/SMTP/ICMP//MQTT/Redis/Memcached/Beanstalk/Handler Socket 과 같은 일반적인 프로토콜이 Acl에서 구현되며, XML/JSON/MIME/BASE64/UUCODE/QPCODE/RFC2047/RFC1035 등과 같은 코덱 라이브러리도 Acl에 포함되어 있다. 
	- 또한 Acl은 Mysql, Postgresql, Sqlite와 같이 널리 사용되는 데이터베이스를 위한 통합 추상 인터페이스도 제공한다.
	- Acl 라이브러리를 사용하면 데이터베이스 애플리케이션을 더 쉽고, 빠르고, 안전하게 작성할 수 있다.
- RIO_rioring
    -  Widows의 `Registered I/O` 와 Linux의 `io_uring`을 지원하는 라이브러리
- `opensource` 디렉토리의 Easy-Reactor
- `opensource` 디렉토리의 edu_CQNetLib
- `opensource` 디렉토리의 zsummerX
- `opensource` 디렉토리의 GameProject3