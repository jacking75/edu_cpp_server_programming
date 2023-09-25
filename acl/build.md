# acl 라이브러리 컴파일 및 사용 
https://github.com/acl-dev/acl/blob/master/BUILD.md 의 기계 번역이다  
  
acl 라이브러리는 실제로 7개의 라이브러리로 구성되어 있다: lib_acl(기본 라이브러리), lib_protocol(http 및 icmp 프로토콜 라이브러리), lib_acl_cpp(다양한 기능을 추가한 lib_acl 및 lib_protocol 라이브러리의 C++ 버전), lib_fiber(강력한 네트워크 동시성 라이브러리)(개발자가 Go와 같은 동시성을 기반으로 동시 서비스 애플리케이션을 빠르게 작성할 수 있게 해주는 강력한 네트워크 동시성 라이브러리), lib_dict(사전 조회를 위한 라이브러리로 bdb, cdb 및 도쿄 캐비닛 라이브러리를 캡슐화), lib_tls(openssl의 일부 기능을 캡슐화 하여 주로 lib_acl의 ssl 전송을 암호화하는데 사용되는 라이브러리) 및 lib_rpc(구글의 프로토부프 라이브러리를 캡슐화한 라이브러리). 그 중에서 가장 많이 사용하거나 현재 가장 많이 사용하는 라이브러리인 lib_acl, lib_protocol, lib_acl_cpp 세 가지 라이브러리를 주로 사용하므로 이 글에서는 이 세 가지 라이브러리를 컴파일하고 사용하는 방법을 주로 소개한다.  
  
acl 라이브러리는 현재 리눅스, 솔라리스, FreeBSD, 맥OS, IOS, 안드로이드, 윈도우 플랫폼에서 지원되며, 독자들은 다른 플랫폼으로 acl 라이브러리를 포팅할 수 있다.  
  
# I. Linux/UNIX 플랫폼에서 컴파일하기 
gcc/g++ 컴파일러와 gnu make 명령이 필요하다.  
  
## 1. 정적 라이브러리 컴파일 
  
### 1.1 libacl.a 라이브러리 컴파일하기 
gcc로 컴파일: lib_acl 디렉터리에 들어가서 make를 직접 실행하면, 정상적인 상황에서는 lib 디렉터리에 정적 라이브러리가 생성되며, 사용자는 `libacl.a`를 사용하여 자체 프로그램을 작성할 때 Makefile 에 다음 옵션을 추가해야 한다:
  
1.1.1 컴파일 옵션: -I libacl.a 헤더 파일이 있는 디렉터리를 지정합니다(헤더 파일은 lib_acl/include 디렉터리에 있음).   
1.1.2 연결 옵션: -L은 libacl.a가 있는 디렉터리를 지정하고, -lacl은 libacl.a 라이브러리를 연결해야 함을 지정한다.   
1.1.3 사용자는 다음과 같이 자신의 소스 프로그램에 lib_acl의 헤더 파일을 포함시켜야 한다:   
```
#include "lib_acl.h"
```  
  
간단한 Makefile 예시:
```
test: main.o
	gcc -o test main.o -lacl -lz -lpthread -ldl
main.o: main.c
	gcc -O3 -Wall -c main.c -I./lib_acl/include 
```  
  
### 1.2 libprotocol.a 라이브러리 컴파일하기
gcc로 컴파일: lib_protocol 디렉터리로 이동하여 make를 직접 실행하면 정상적인 상황에서는 lib 디렉터리에 `lib_protocol.a`` 정적 라이브러리가 생성되며, 사용자는 `lib_protocol.a`를 사용하여 자체 프로그램을 작성할 때 Makefile에 다음 옵션을 추가해야 한다:  
  
1.2.1 컴파일 옵션: -I 헤더 파일 디렉터리(lib_protocol/include 디렉터리 내)를 lib_protocol.a로 지정한다.   
1.2.2, 링크 옵션: -L은 lib_protocol.a가 있는 디렉터리를 지정하고, -L은 lib_acl.a가 있는 디렉터리를 지정하며, -l_-protocol -l_acl   
1.2.3 사용자는 다음과 같이 애플리케이션에 lib_protocol 헤더 파일을 포함시켜야 한다:   
```
#include "lib_protocol.h"
```  
  
간단한 Makefile 예제:
```
test: main.o
	gcc -o test main.o -lprotocol -lacl -lz -lpthread -ldl
main.o: main.c
	gcc -O3 -Wall -c main.c -I./lib_acl/include -I./lib_protocol/include 
```  


### 1.3 libacl_cpp.a 라이브러리 컴파일하기 
g++ 컴파일러 사용: lib_acl_cpp 디렉터리에 들어가서 make static을 실행하여 `libacl_cpp.a`` 정적 라이브러리를 컴파일하면 lib 디렉터리 아래에 `libacl_cpp.a`가 생성되며, 사용자는 `libacl_cpp.a`로 프로그램을 작성할 때 Makefile에 다음 옵션을 추가해야 한다:  
  
1.3.1 컴파일 옵션: -I 헤더 파일 디렉터리(lib_acl_cpp/include 디렉터리 내)를 libacl_cpp.a로 지정한다. 
1.3.2. 링크 옵션: -L   
  
libacl_cpp.a가 있는 디렉터리를 지정하고, -L은 libprotocol.a가 있는 디렉터리를 지정하고, -L은 libacl.a가 있는 디렉터리를 지정한다.  예: -L./lib_acl_cpp/lib -lacl_cpp -L./lib_protocol/lib -lprotocol -L./lib_acl/lib -lacl, 이 세 라이브러리의 종속성에 주의해야 한다: libprotocol.a는 libacl.a와 libacl_ cpp.a는 libprotocol.a와 lib_acl.a에 종속된다. g++로 컴파일할 때 라이브러리의 순서가 올바른지, 종속 라이브러리가 항상 종속 라이브러리 뒤에 배치되어 있는지 확인해야 하며, 그렇지 않으면 프로그램 연결 시 존재하지 않는 함수로 보고된다.  
  
1.3.3 사용자는 다음과 같이 애플리케이션에 lib_acl.hpp 헤더 파일을 포함시켜야 한다: 
```
#include "acl_cpp/lib_acl.hpp"
```  
  
간단한 Makefile 예제:
```
test: main.o
	g++ -o test main.o -lacl_cpp -lprotocol -lacl -lz -lpthread -ldl
main.o: main.cpp
	g++ -O3 -Wall -c main.cpp -I./lib_acl_cpp/include -I./lib_acl/include -I./lib_protocol/include 
```  
  
애플리케이션이 libacl_cpp.a 라이브러리의 클래스 및 함수만 사용하는 경우 libacl_cpp.a의 헤더 파일만 포함하면 된다. libacl_cpp.a 라이브러리에 종속되는 libprotocol.a 및 libacl.a 라이브러리의 헤더 파일은 암시적으로 처리되므로 명시적으로 포함할 필요가 없다.
  
  

## 2. 동적 라이브러리 컴파일 
libacl.so, libprotocol.so, libacl_cpp.so를 컴파일하는 방법은 정적 라이브러리를 컴파일하는 방법과 다르므로 세 개의 디렉터리로 개별적으로 이동해야 한다. 여기서 공유는 동적 라이브러리를 컴파일해야 함을 의미하며 `${lib_path}`를 실제 대상 경로로 대체해야 한다(예: make shared rpath=/opt/acl/lib, 동적 라이브러리는 /opt/acl/lib 디렉토리에서 컴파일되어 저장된다). 실제 대상 경로를 실제 경로로 바꿔야 한다(예: make shared rpath=/opt/acl/lib). 그러면 동적 라이브러리가 컴파일되어 `/opt/acl/lib` 디렉터리에 저장되며, 이 세 라이브러리를 컴파일하는 순서는 다음과 같다:   
libacl.so --> libprotocol.so --> libacl_cpp.so   
  
또한, libacl_cpp.so를 컴파일할 때는 미리 리소스 디렉토리에 polarssl 라이브러리를 컴파일한 후, libacl_cpp.so를 컴파일해야 하며, 이 때 polarssl.lib 라이브러리가 위치한 경로를 지정해야 하고, SSL 통신 방식이 필요하지 않은 경우 lib_acl_cpp/. Makefile, 마지막 컴파일 옵션: -DHAS_POLARSSL.  
  
애플리케이션은 헤더 파일이 포함될 때 정적 라이브러리와 동일한 방식으로 이 세 가지 동적 라이브러리를 사용하며, 정적 라이브러리와 유사한 방식으로 동적 라이브러리에 연결하고 .a를 .so로 바꾸기만 하며 위의 정적 라이브러리와 동일한 포함 순서를 요구한다. 예를 들어  
a. 헤더 파일 포함 모드 컴파일: -I/opt/acl/include/acl -I/opt/acl/include/protocol -II/opt/acl/include  
b. 연결에 포함된 라이브러리 파일: -L/opt/acl/lib -lacl_cpp -lprotocol -lacl  
c. 런타임 라이브러리 파일 로드 모드: -Wl,-rpath,/opt/acl/lib, 이 매개변수는 프로그램이 실행될 때 로드해야 하는 세 개의 동적 라이브러리 위치를 /opt/acl/lib 디렉터리에서 지정한다.   
  

## 3. 단일 라이브러리로 컴파일 
더 쉽게 사용하기 위해 위의 세 가지 라이브러리를 하나의 라이브러리로 결합하는 방법도 있다. acl 라이브러리 디렉터리에서 `make build_one`을 실행하면 통합 라이브러리인 libacl_all.a 와 libacl_all.so가 생성되며 여기에는 libacl, lib_protocol, lib_acl_cpp 라이브러리가 포함되어 있다. Makefile은 훨씬 더 간단하게 작성할 수 있다:  
```
test: main.o
	g++ -o test main.o -lacl_all -lz -lpthread -ldl
main.o: main.cpp
	g++ -O3 -Wall -c main.cpp -I./lib_acl_cpp/include -I./lib_acl/include -I./lib_protocol/include 
```  
  

## 4. cmake로 컴파일 
make 유틸리티로 acl 라이브러리를 컴파일하는 것 외에도, 매우 간단한 프로세스인 cmake 유틸리티를 사용하여 acl 라이브러리를 컴파일할 수도 있다:  
<pre>
$mkdir build
$cd build
$cmake ..
$make
</pre>  
  
<br>  
  
  
# II. Windows 플랫폼 
  
## 1. 편집 
acl 프로젝트의 루트 디렉터리에는 여러 VC 프로젝트 파일이 있으며, 사용자는 필요에 따라 프로젝트 파일을 열 수 있다: acl_cpp_vc2003.sln, acl_cpp_vc2008.sln, acl_cpp_vc2010.sln, acl_cpp_vc2012.sln(가장 먼저 지원되는 VC6). 사용자는 정적 라이브러리 디버그 버전, 정적 라이브러리 릴리스 버전, 동적 라이브러리 디버그 버전, 동적 라이브러리 릴리스 버전인 lib_acl, lib_protocol, lib_acl_cpp를 컴파일하도록 선택할 수 있으며, 컴파일이 완료된 후 `acl\dist\lib\win32` 디렉터리에 생성된 정적 라이브러리에는 다음이 있다:  
lib_acl_vc20xxd.lib, lib_acl_vc20xx.lib, lib_protocol_vc20xxd.lib 및 lib_protocol_vc20xx.lib;  
  
동적 라이브러리와 관련된 생성된 파일은 lib_acl_d.dll/lib_acl_d.lib, lib_acl.dll/lib_acl.lib, lib_protocol_d.dll/lib_protocol_d.lib, lib_protocol.dll/lib_. protocol.lib, lib_acl_cpp_d.dll/lib_acl_cpp_d.lib, lib_acl_cpp.dll/lib_acl_cpp.lib     
  

## 2. 사용 
a) win32 플랫폼에서 lib_acl 및 lib_protocol 정적 라이브러리를 사용할 경우, include 디렉터리에 lib_acl/include, lib_protocol/include, lib_acl_cpp/include의 경로만 추가하고 링크할 때 정적 라이브러리의 경로와 이름을 지정하기만 하면 된다.
b) win32 플랫폼에서 lib_acl의 동적 라이브러리를 사용하는 경우, a)에 명시된 작업을 수행할 뿐만 아니라 전처리기 정의에 ACL_DLL을 추가해야 하며, lib_protocol의 동적 라이브러리를 사용하는 경우, 전처리기 정의에 HTTP_DLL 및 ICMP_DLL을 추가해야 하고, lib_acl_cpp의 동적 라이브러리를 사용하는 경우, 전처리기 정의에 ACL_CPP_DLL을 추가해야 한다. 동적 라이브러리를 사용하는 경우 전처리기 정의에 ACL_CPP_DLL을 추가해야 한다.  
  
## 주의 사항   
동적 라이브러리를 생성할 때 가장 기본이 되는 라이브러리는 lib_acl이고, lib_protocol은 lib_acl에 의존하며, lib_acl_cpp는 lib_protocol과 lib_acl에 의존하기 때문에 생성 순서에 주의해야 하며, 컴파일 순서는 다음과 같다: lib_acl, lib_protocol, lib_acl_cpp. cpp

