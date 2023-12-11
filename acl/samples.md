## 샘플 프로그램  

## aio
socket_stream 이나 aio 의 서버는 모두 1개의 스레드에서 동작한다.   
만약 복수개의 스레드에서 동작하고 싶다면 acl 객체를 각 스레드마다 만들어야 한다.  
혹은 filber를 사용해야 한다.  

### socket_stream  
acl 에서 제공하는 샘플 프로그램을 `samples\socket_stream` 디렉토리에 옮긴 것이다.  
  
빌드를 위한 프로젝트 속성
- `C++ 언어 표준` 항목을 `미리 보기 - 최신 C++ 초안의 기능(/std:c++latest)`로 한다.   
- C/C++ - 일반 - 추가 포함 디렉토리 
    - `..\..\acl\include;..\..\acl\lib_acl\include;..\..\acl\lib_acl_cpp\include` 를 추가한다
- 링커 - 일반 - 추가 라이브러리 디렉토리 
    - `..\..\acl\lib\win64\Debug;..\..\..\dist\lib\win64\Debug` 를 추가한다.  
- 링커 - 입력 - 추가 종속성
    - `ws2_32.lib;lib_acl.lib;lib_acl_cpp.lib` 을 추가한다.  
	
위처럼 하였다면 빌드가 잘 될 것이다.   
  
### asio_echo  
`acl::aio` 가 멀티스레드로 동작하는지 테스트 하기 위해 accept나 read 이벤트쪽에 wait를 넣어 보았다.  
1개의 스레드에서 동작하기 때문에 wait 에 의해 대기가 발생한다.  
 