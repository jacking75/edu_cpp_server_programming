# C++ 서버 프로그래밍 실습
      
## TODO
- 처음부터 직접 네트워크 라이브러리 만들기  
    - 네트워크 프로그래밍을 위한 핵심 네트워크 이론을 배운다.
	- 멀티스레드 프로그래밍을 배운다.
	- Socket API 사용하는 방법을 배운다.
	- IOCP
- 기존 네트워크 라이브러리 리팩토링
- 기존 네트워크 라이브러리를 사용하여 게임 서버 만들기  
    - 로그 사용에 대해서 배운다
	
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
  
    
    
## Sync I/O: select 사용 
    
### 채팅 서버
[참고 코드: SelectChatServer](https://github.com/jacking75/com2us_cppNetStudy_work/tree/master/SelectChatServer )   
  
  
### 오목 게임 서버
    
    
<br/>
	

## Async I/O: IOCP 사용  
  
### IOCP 단계별 과제 구현하기
[edu_cpp_IOCP](https://github.com/jacking75/edu_cpp_IOCP)  
  
### (도서)"온라인 게임 서버" 분석 및 리팩토링
[YES24](http://www.yes24.com/Product/Goods/1776627 )  
이 책은 팀에 있다.  예제 코드는 `onlinegameserver.zip` 이다.  
이 책의 예제 코드 중 게임서버 코드를 Modern C++ 코드로 리팩토링한다.  
- MFC는 사용하지 않는다.
- C++스러운 네임컨벤셜을 따른다.  
- C가 아닌 C++ 코드스럽게 한다.  
  
  
### IOCP를 사용한 오목 게임 서버 
  
  
### (도서)"게임 서버 프로그래밍 입문" 분석
[YES24](http://www.yes24.com/Product/Goods/18497117 )  
    
	
### 참고 코드
[IocpChatServer](https://github.com/jacking75/com2us_cppNetStudy_work/tree/master/IocpChatServer)  
[nhn_next_IocpServer](https://github.com/jacking75/com2us_cppNetStudy_work/tree/master/nhn_next_IocpServer)  
     
<br/>     
    
	
## 오픈 소스 코드 분석
- [CppLogger2. Lightweight logger for C++](https://github.com/emilienlemaire/CppLogger2 )
- [pretty_log](https://github.com/myyrakle/pretty_log  )
- [BlackMATov/curly.hpp  Simple cURL C++17 wrapper](https://github.com/BlackMATov/curly.hpp )  