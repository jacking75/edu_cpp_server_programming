# 컴투스 C++ 서버 프로그래밍 실습
1. 컴투스 서버 프로그래머만 대상으로 하고 있습니다.  
2. 실습에 참여하기 위해서는 저에게 연락하여 멤버로 참여합니다.  
3. 이 저장소에서 자신만의 브랜치를 새로 만드세요.  
4. 각 단계 별 진행을 할 때마다 매번 프로젝트를 새로 만드세요.  
5. 단계 별로 만들어야 할 것이 잘 이해가 안 되면 꼭 질문 하세요.  
6. 단계를 마치면 저에게 피드백을 받습니다.  
  
  
## 목적:
OS의 Socket API를 사용한 실시간 통신(TCP/UDP)을 하는 게임 서버를 만들 수 있는 기술을 배운다.   
    
- 네트워크 프로그래밍을 위한 핵심 네트워크 이론을 배운다.
- C/C++를 사용한 BSD Socket API 사용하는 방법을 배운다.
- 멀티스레드 프로그래밍을 배운다.
- 패킷을 직렬화 하고 핸들링 하는 방법을 배운다.
- 효율적인 메모리 관리 방법을 배운다.
- 네트워크 라이브러리를 만들어 본다.
- 많은 클라이언트 접속을 처리할 수 있는 서버를 만들어 본다.
- 더미 클라이언트를 이용한 테스트.
- 게임 서버 구조에 대해서 배운다.
- 턴 방식의 단독 게임 서버를 만들어 본다.
- 스케일 아웃을 고려한 게임 서버를 만들어 본다
- 로그 사용에 대해서 배운다
- 서버 크래시에 대처하는 방법을 배운다.
  
  
  
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