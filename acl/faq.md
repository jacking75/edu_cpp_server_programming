# FAQ  
https://github.com/acl-dev/acl/blob/master/FAQ.md 의 기계 번역이다  
  
## I. 기본 문제 
  
### 1. acl 라이브러리에 포함된 주요 기능은 무엇인가요? 
acl 프로젝트는 크로스 플랫폼(리눅스, 윈32, 솔라리스, 맥OS, 프리BSD 지원) 네트워크 통신 라이브러리 및 서버 프로그래밍 프레임워크이며, 보다 실용적인 함수 라이브러리를 제공gks다. 라이브러리를 통해 사용자는 다양한 모드(멀티 스레드, 멀티 프로세스, 비 차단, 트리거, UDP 모드, 코 프로세싱 모드)를 매우 쉽게 작성하여 서버 프로그램, 웹 응용 프로그램, 데이터베이스 응용 프로그램을 지원할 수 있다. 또한 라이브러리는 클라이언트 측 통신 라이브러리의 공통 응용 프로그램(예 : HTTP, SMTP, ICMP, redis, 디스크, memcache, 빈 스토크, 핸들러 소켓), 공통 스트리밍 코덱 라이브러리 : XML / JSON / MIME / BASE64 / UUCODE / QPCODE /. RFC2047 등  
  
### 2, acl 라이브러리는 어떤 플랫폼을 지원하나요? 
현재 주요 지원 리눅스/윈도우/마코스/프리브스디/솔라리스(x86).   
   
### 3. acl 라이브러리에는 몇 개의 라이브러리가 포함되어 있나요? 
주로 `lib_acl`(기본 라이브러리로 C로 작성됨), `lib_protocol`(일부 네트워크 애플리케이션을 위한 프로토콜 라이브러리로 C로 작성됨), `lib_acl_cpp`(C++로 작성됨, lib_acl/lib_protocol 두 라이브러리를 캡슐화하면서 더 실용적인 기능을 추가), `lib_fiber`(고성능, 동시 네트워크 코프로세싱 라이브러리를 지원하기 위해 C로 작성됨), `lib_rpc`(고성능, 동시 네트워크 코프로세서 라이브러리를 지원하기 위해 C++로 작성됨), `lib_rpc`(Google protobuf 네트워크 라이브러리를 캡슐화하기 위해 C++로 작성됨) 등이 포함된다. `lib_fiber`(고성능, 고동시 네트워크 동시 라이브러리를 지원하기 위해 C로 작성됨), `lib_rpc`(C++로 작성됨, 구글 프로토부프 네트워크 라이브러리를 캡슐화함).  

### 4. acl 라이브러리의 외부 종속성은 무엇인가요? 
`lib_acl/lib_protocol/lib_fiber`는 기본 시스템 라이브러리에만 의존하고, `lib_acl_cpp`는 mysql 클라이언트 라이브러리, db 모듈은 sqlite 라이브러리, 스트림 모듈은 polarssl 라이브러리(이 라이브러리의 소스 코드는 acl/resource 디렉터리에 있음)에 의존한다. 스트림 모듈은 polarssl 라이브러리에 종속된다(이 라이브러리의 소스 코드는 acl/resource 디렉터리에 있다). 또한 UNIX/LINUX 플랫폼에서는 압축 라이브러리 libz가 필요하며(LINUX는 일반적으로 이 압축 라이브러리와 함께 제공됨), lib_rpc는 protobuf 라이브러리에 종속된다.  
  
### 5. 이러한 타사 라이브러리 없이 acl을 사용할 수 있나요? 
할 수 있다. 기본적으로 이러한 타사 라이브러리가 없어도 acl 라이브러리를 컴파일하고 사용하는 데는 문제가 없으며, 단지 mysql/sqlite/ssl/protobuf 함수를 사용할 수 없을 뿐이다.  
    
  
## II. 편집 및 사용 문제 
  
### 1. ACL 라이브러리 컴파일 프로세스? 
acl 라이브러리의 컴파일 프로세스에 대해서는 acl 컴파일 및 사용법 블로그를 참조한다.  
    
### 2. Linux 플랫폼 라이브러리 컴파일 순서 문제? 
라이브러리의 종속성 순서는 lib_acl 라이브러리는 acl 라이브러리의 기본 라이브러리이며, 다른 모든 라이브러리는 이 라이브러리에 종속된다. 라이브러리의 종속성 순서는 lib_protocol은 lib_acl에 의존하고, lib_acl_cpp는 lib_acl과 lib_protocol에 의존하고, lib_fiber는 lib_acl에 의존하고, lib_rpc는 lib_acl, lib_protocol, lib_cpp, lib_acl_cpp에 의존하는 순서로 되어 있다. 따라서 애플리케이션이 acl 라이브러리에 연결할 때는 -l_acl_cpp -l_protocol -l_acl 순으로 연결하도록 주의해야 한다.  
    
### 3.유닉스/리눅스 플랫폼에서 acl 라이브러리를 컴파일할 때 어떤 컴파일 옵션이 있나요? 
Unix/Linux 플랫폼에서 acl 라이브러리를 컴파일할 때는 gcc에 대한 컴파일 옵션을 지정해야 한다. acl 라이브러리와 함께 제공되는 Makefile은 운영 체제를 자동으로 인식하고 다른 컴파일 옵션을 선택하며, 다음은 다양한 Unix 플랫폼에 대한 다양한 컴파일 옵션 목록이다(현재 버전은 시스템 유형을 자동으로 인식하므로 다음 컴파일 옵션을 추가할 필요가 없음):  
Linux 플랫폼: -DLINUX2  
MacOS 플랫폼: - DMACOSX  
FreeBSD 플랫폼: -DFREEBSD  
Solaris(x86) 플랫폼: - DSUNOS5  
  

### 4. 가장 간단한 리눅스 플랫폼용 Makefile을 알려주세요. 
다음은 acl 라이브러리를 사용하기 위한 가장 간단한 컴파일 옵션이다(인쇄상의 문제로 인해 메이크파일에 다음을 복사할 때 각 줄 앞의 공백을 수동으로 탭(Tab)으로 입력해야 한다):  
<pre>
fiber: main.o
	g++ -o fiber main.o \
		-L./lib_fiber/lib -lfiber_cpp \
		-L./lib_acl_cpp/lib -l_acl_cpp \
		-L./lib_protocol/lib -l_protocol \
		-L./lib_acl/lib -l_acl \
		-L./lib_fiber/lib -lfiber \
		-lz -lpthread -ldl
main.o: main.cpp
	g++ -O3 -Wall -c main.cpp -DLINUX2 \
		-I./lib_acl/include \
		-I./lib_acl_cpp/include \
		-I./lib_fiber/cpp/include \
		-I./lib_fiber/c/include
</pre> 
  

### 5. Linux 플랫폼에서 libz.a 라이브러리를 찾을 수 없으면 어떻게해야 하나요? 
일반적으로 유닉스/리눅스 플랫폼에는 libz.a 또는 libz.so 압축 라이브러리가 함께 제공되며, 라이브러리를 찾을 수 없는 경우 온라인으로 설치하거나 컴파일하여 zlib 라이브러리를 설치할 수 있다. Centos와 Ubuntu의 경우 각각 아래와 같은 방법으로 온라인으로 설치할 수 있다(루트 ID로 전환해야 함):
  
Centos: yum 설치 zlib-devel  
우분투: apt-get 설치 zlib1g.dev  
  

### 6. 리눅스 플랫폼 acl 라이브러리를 라이브러리에 패키징할 수 있나요? 
이것은 가능하다. 실행: acl 디렉터리에서 build_one을 만들어 lib_acl/lib_protocol/lib_acl_cpp를 완전한 라이브러리인 lib_acl.a/lib_acl.so로 패키징하면 애플리케이션에 연결하는 데 필요한 모든 것이 갖춰진다.  
  

### 7. Linux 플랫폼에서 SSL 기능을 사용하는 방법은 무엇인가? 
현재 acl의 lib_acl_cpp C++ 라이브러리는 polarssl을 통합하여 ssl을 지원하며, 지원되는 polarssl 소스 코드는 https://github.com/acl-dev/third_party 에서 다운로드할 수 있으며, 이전 버전의 acl은 libpolarssl.a를 정적으로 연결하여 ssl을 지원했지만, 현재 버전은 libpolarssl.so를 동적으로 로드하여 ssl을 지원한다. 이전 버전의 acl은 libpolarssl.a에 정적으로 연결하여 ssl을 지원했지만, 현재 버전은 특별한 컴파일 조건 없이 더 유연하고 편리한 동적 로딩을 통해 ssl을 지원하며 더 일반적이다.  
  
#### 7.1. 이전 acl 버전은 SSL을 지원한다. 
위의 통합 acl 라이브러리를 사용하는 경우, acl 루트 디렉터리에서 컴파일할 때 make build_one polarssl=on을 실행할 수 있으며, 세 개의 라이브러리(lib_acl.a, lib_protocol.a, lib_acl_cpp.a)를 사용하는 경우 컴파일하기 전에 환경 변수를 지정해야 합니다: export ENV_. FLAGS=HAS_POLARSSL로 설정한 다음 이 세 가지 라이브러리를 컴파일해야 합니다. polarssl-1.2.19-gpl.tgz의 압축을 푼 다음 polarssl-1.2.19 디렉토리로 이동하여 make를 실행합니다. 컴파일이 끝나면 polarssl-1.2.19/library 디렉터리에 libpolarssl.a 라이브러리가 생깁니다; 마지막으로 애플리케이션을 컴파일할 때 libpolarssl.a를 프로젝트에 링크합니다.

#### 7.2. 현재 버전의 acl은 SSL을 지원합니다. 
먼저 polarssl 라이브러리를 다운로드하여 압축을 풀고 polarssl-1.2.19 디렉터리로 이동하여 make lib SHARED=yes를 실행하면 라이브러리 디렉터리에 libpolarssl.so가 생성됩니다.
acl 루트 디렉터리에서 make build_one을 실행하면 세 개의 acl 기본 라이브러리인 libacl.a, libprotocol.a, libacl_cpp.a가 정적 라이브러리인 libacl_all.a로 결합되고, libacl.so, libprotocol.so, lib_acl_cpp.so는 동적 라이브러리인 libacl_all.so로, libacl_all.so는 동적 라이브러리인 libacl_cpp.so는 동적 라이브러리로, libacl_all.so. 를 동적 라이브러리인 libacl_all.so로 변환합니다.
애플리케이션이 시작될 때 다음 코드를 추가합니다. acl::polarssl_conf::set_libpath("libpolarssl.so"); 여기서 경로는 실제 위치에 따라 달라지므로, acl 모듈은 SSL 통신이 필요할 때 자동으로 ssl 모드로 전환합니다.  
  

### 8. Linux에서 mysql 함수를 사용하는 방법은 무엇인가? 
lib_acl_cpp 라이브러리는 mysql 라이브러리를 동적으로 로드하므로 lib_acl_cpp를 컴파일할 때 이미 mysql 함수가 acl 라이브러리에 컴파일되어 있다. 사용자는 acl::db_handle::set_loadpath 함수를 사용하여 mysql 동적 라이브러리를 acl 라이브러리에 등록하기만 하면 되고, mysql 클라이언트 라이브러리의 경우 mysql 공식 다운로드 페이지로 이동하거나 acl/resource 디렉토리에서 mysql-connector-c-6.1.6-src.tar.gz를 컴파일하면 된다다. 
  


## III. 개별 기능 모듈 사용 
  
### 1. acl HTTP 서버가 중단점 다운로드 기능을 지원하나요? 
지원: acl HTTP 모듈은 중단점 다운로드 기능을 지원하며, 중단점 다운로드를 지원하는 서버의 예는 acl\app\wizard_demo\httpd_download를 참조한다.  
  
### 2. acl HTTP 서버가 파일 업로드를 지원하나요? 
지원. 참조 예: acl\app\wizard_demo\httpd_upload.  
  
### 3. acl HTTP 모듈은 서버/클라이언트 모드를 모두 지원하나요? 
지원. 현재 acl의 HTTP 프로토콜 모듈은 클라이언트 측 모드와 서버 측 모드를 모두 지원한다. 즉, 클라이언트 측 프로그램과 서버 측 프로그램을 모두 작성하는 데 acl HTTP를 사용할 수 있다. 클라이언트 측 프로그램을 작성하는 데는 acl의 http_request/http_request_pool/http_request_manager 클래스가 사용된다. 클라이언트 프로그램을 작성하는데는 acl의 http_response/HttpServlet/HttpServletRequest/HttpServeletResponse 클래스가 사용되며, 서버 프로그램을 작성하는 데는 http_response/HttpServletRequest/HttpServeletResponse 클래스가 사용된다.  
  
### 4. acl HTTP 모듈이 웹소켓 통신 프로토콜을 지원하나요? 
지원. 예제를 참조할 수 있다: lib_acl_cpp\samples\websocket  
  
### 5. acl HTTP 모듈이 세션을 지원하나요? 
acl HTTP 모듈은 서버 프로그래밍에 사용될 때 세션 저장을 지원하며, 현재 세션 데이터를 저장하기 위해 memcached 또는 redis 사용을 지원한다.  
  
### 6. HttpServletRequest가 json 또는 xml 데이터 본문을 읽을 수 없는 이유는 무엇인가요? 
HTTP 클라이언트 요청 데이터 본문이 json 또는 xml인 경우 기본적으로 json/xml 데이터는 acl::HttpServletRequest 객체에서 읽을 수 없는데, 그 주된 이유는 HttpServletRequest에 내장된 자동 읽기 및 구문 분석 json/xml/x-www-form- urlencoded 유형 데이터 함수가 내장되어 있기 때문이다. urlencoded 타입 데이터의 경우, 사용자는 json 타입 데이터 본문과 같이 구문 분석된 객체를 직접 가져오기만 하면 된다:  
```
void get_json(acl::HttpServletRequest& req)
{
	acl::json* json = req.getJson();
	...
}
```  
애플리케이션이 자체적으로 json 데이터를 읽고 구문 분석하려면 acl::HttpServlet::setParseBody(false)를 호출하여 acl::HttpServletRequest 클래스 객체 내에서 데이터의 자동 읽기를 비활성화해야 한다.  
  
### 7. 설정되지 않은 호스트 필드로 인한 http_request 오류 
표준 웹 서버(예: nginx)에 액세스하기 위해 acl::http_request 클래스 객체를 사용할 때 HTTP 요청 헤더에 Host 필드를 설정하지 않으면 nginx가 400 오류를 반환한다. 주된 이유는 HTTP/1.1 프로토콜에서 HTTP 클라이언트가 다음과 같은 방식으로 Host 필드를 설정해야 하기 때문이다:  
```
bool http_client(void)
{
	acl::http_request req("www.sina.com.cn:80");
	acl::http_header& hdr = req.request_header();
	hdr.set_url("/").set_host("www.sina.com.cn");
	if (!req.request(NULL, 0)) {
		return false;
	}
	acl::string body;
	if (req.get_body(body)) {
		printf("%s\r\n", body.c_str());
	}
	
	... 
}
```  
   

### Redis 모듈 
  
#### 1. acl redis 라이브러리가 클러스터링을 지원하나요? 
답변: 예, acl redis 클라이언트 라이브러리는 클러스터형 및 독립 실행형 redis 서버를 모두 지원한다.  
  
#### 2. acl redis 라이브러리는 어떻게 분할되나요? 
acl redis 클라이언트 라이브러리는 명령 클래스와 연결 클래스라는 두 가지 주요 범주로 나누어진다:  
  
주요 명령 클래스는 redis_key, redis_string, redis_hash, redis_list, redis_set, redis_zset, redis_cluster, redis_geo, redis_hyperloglog, redis_pubsub, redis_transaction, redis_server, redis_script 이다. redis_transaction, redis_server, redis_script, 이 모든 클래스는 기본 클래스 redis_command에서 상속하고, 서브클래스 redis는 이 모든 명령 클래스를 상속하므로 사용자가 acl::redis를 사용하여 모든 redis 클라이언트 명령을 직접 조작할 수 있다  
  
주요 연결 클래스는 redis_client, redis_client_pool, redis_client_cluster 이다. 명령 클래스 오브젝트는 이러한 연결 클래스 오브젝트를 통해 redis-server와 상호 작용한다. redis_client는 단일 연결 클래스이고, redis_client_pool은 연결 풀 클래스이다. 클래스로, 이 두 클래스는 클러스터링 되지 않은 redis-server 환경에서만 사용할 수 있으며, **redis-server의 클러스터 모드를 지원하지 않으며**, **redis-server의 클러스터 모드에 연결하려면 redis_client_cluster를 사용해야 하며**, 동시에 redis_client_cluster는 클러스터링되지 않은 모드와도 호환된다. redis_client_cluster는 클러스터링 되지 않은 연결과도 호환된다.  
  
#### 3. acl redis 라이브러리에서 어떤 객체 연산 클래스가 스레드에 안전하나? 
acl redis 라이브러리의 모든 명령 클래스 오브젝트와 redis_client 단일 연결 클래스 오브젝트는 동시에 둘 이상의 스레드에서 사용할 수 없으며(std::string을 여러 스레드에서 사용할 수 없는 것처럼), **redis_client_pool 및 redis_client_cluster 두 연결 클래스 오브젝트는 스레드에 안전하며 동일한 오브젝트를 동시에 둘 이상의 스레드에서 사용할 수 있다**. 객체는 여러 스레드에서 동시에 사용할 수 있다.  
  
#### 4. 연결이 끊어지면 다시 연결되나요? 
acl redis 라이브러리의 연결 클래스인 redis_client, redis_client_pool, redis_client_cluster는 비정상적인 연결 해제를 감지하면 자동으로 재연결을 시도하므로 상위 레벨 사용자가 연결 해제를 다시 시도할 필요가 없다.
  
  

### 공동 프로그램 모듈 
  
#### 1. 프로토콜에서 비차단 원칙이란 무엇인가요? 
예를 들어, 지금 데이터베이스에 액세스해야 하는데, 이 작업은 차단 작업이어야 하며, 데이터베이스 액세스를 위한 동시 스레드가 10개라면, 이 비차단을 어떻게 이해해야 하나요? 시스템의 IO API (읽기 / 쓰기 등) 하단의 동시 접근 방식은 이러한 API를 호출하는 데이터베이스 작업에 연결되므로 IO의 동시 하단에있는 차단 DB 작업의 표면도 비 차단 모드로 변환된다.  
  
#### 2. acl 라이브러리가 멀티스레딩을 지원하나요? 
acl 라이브러리는 멀티스레딩을 지원하지만, go 언어와는 다른 방식으로 지원한다. 사용자는 다중 프로세스와 다중 스레드(각 프로세스는 둘 이상의 스레드를 시작할 수 있지만 프로세스 간 스레드 스케줄링은 분리되어 있음)의 관계처럼 각 스레드에서 다수의 acl 동시 스레드를 생성할 수 있으며, 각 스레드는 자신이 속한 스레드의 동시 스케줄러에 의해 스케줄링된다. 각 연결은 해당 연결이 속한 스레드 내의 연결 스케줄러에 의해 스케줄링된다.  
  
#### 3. acl 라이브러리는 멀티 코어 CPU를 어떻게 지원하나요? 
2)에서 설명한 것처럼 사용자는 각각 acl 공동 스케줄러를 사용하여 여러 스레드를 시작할 수 있으며, 각 공동 스케줄러는 자신과 동일한 스레드에 속한 공동 스케줄러의 상태를 스케줄링할 책임이 있다.  
  
#### 4. acl 스레드에서 mysql 클라이언트 라이브러리를 연결하면 mysql 라이브러리의 소스 코드가 수정되나요? 
아니요, mysql 클라이언트 라이브러리가 사용하는 시스템 IO API는 읽기/쓰기/폴링이며, acl 라이브러리는 기본 시스템 IO 프로세스를 후킹하므로, 사용자 프로그램이 mysql 라이브러리와 acl 라이브러리로 컴파일될 때, mysql 라이브러리의 IO 프로세스는 acl 라이브러리가 후킹한 API가 직접 인수하게 되어, mysql 라이브러리 코드 한 줄 수정 없이 mysql 클라이언트 라이브러리를 스레딩할 수 있게 된다. 클라이언트 라이브러리를 한 줄도 수정하지 않고도 스레딩할 수 있다.  
  
#### 5. acl 라이브러리가 도메인 이름 확인을 지원하나요? 
지원. 많은 C/C++로 구현된 동시 라이브러리는 gethostbyname(_r) 함수를 구현하지 않기 때문에 동시 프로그래밍 도메인 이름 확인을 사용하는 사용자는 도메인 이름 확인 프로세스 구현의 DNS 프로토콜 수준에서 별도의 스레드를 사용하여 완료해야 하며, 이 함수 모듈을 기반으로 하는 동시 라이브러리는 타사 라이브러리를 사용하거나 별도의 스레드를 시작하여 도메인 이름 확인을 완료할 필요 없이 HOOK 시스템 gethostbyname(_r) API를 사용하여 도메인 이름 확인을 완료해야 한다. 도메인 이름 확인을 완료하기 위해 타사 라이브러리나 별도의 스레드를 시작할 필요 없이) API를 사용할 수 있다.  
  
#### 6. acl 라이브러리에서 시스템 오류 번호는 어떻게 처리되나요? 
acl 라이브러리는 공동 안전 오류 번호를 구현한다. 멀티스레딩으로 프로그래밍할 때 오류 번호를 각 스레드에 바인딩할 수 있는 것처럼, 오류 번호는 acl 라이브러리에서 각 acl 스레드에 바인딩된다. 따라서 strerror(errno)를 호출할 때에도 공동 안전하다.
   
#### 7. 너무 많은 동시 프로세스가 시작되면 mysql과 같은 백엔드 서비스에 동시성 압력이 발생하나요? 이를 어떻게 피할 수 있나요? 
시작 스레드가 많고 모두 mysql 연산을 필요로 하는 경우, 이로 인해 mysql 서버에 동시성 스트레스를 유발할 수 있는 것은 사실이다. 이러한 동시성 스트레스를 피하기 위해 acl 라이브러리는 백엔드에서 높은 동시성을 지원하지 않는 서비스에 대한 연결 보호를 제공하는 데 사용되는 동시 세마포어를 제공한다.  
  


### 서버 모듈 
  
#### 1. 서버 프로그래밍 모델에는 몇 가지가 있나요? 각각의 특징은 무엇인가요? 
현재 acl 라이브러리는 프로세스 풀 모델, 스레드 풀 모델, 논 블로킹 모델, 동시 모델, UDP 통신 모델, 트리거 모델을 제공합니다. 각 프로그래밍 모델의 특징은 다음과 같습니다:

- 프로세스 풀링 모델: 각 프로세스는 하나의 클라이언트 연결을 처리하고 여러 연결을 처리해야하는 경우 여러 프로세스를 시작해야하며 이 모델의 가장 큰 단점은 낮은 수준의 동시성이며 장점은 간단한 프로그래밍이다  
- 스레드 풀 모델: 많은 수의 클라이언트 연결은 여러 스레드로 구성된 스레드 풀에 의해 처리되며, 연결에 연결을 읽을 데이터가 있는 경우에만 연결이 스레드에 바인딩되고 처리 후 스레드가 스레드 풀로 반환되며 이 모델의 장점은 적은 수의 스레드가 큰 동시성을 처리하기 시작한다는 것이며 단점은 스레드 프로그래밍 스레드 보안 문제에 주의를 기울여야한다는 것이다
- 비 차단 모델: 프로세스에 하나의 작업자 스레드만 있으며 비 차단 통신을 사용하여 매우 많은 수의 동시 클라이언트 연결을 지원할 수 있으며 리소스 소비가 적고 대규모 동시성 지원, 고성능의 장점, 단점은 프로그래밍 복잡성이 높다는 것이다
- 동시 모델: 각 클라이언트 연결은 각 프로세스의 작업자 스레드 인 동시 스레드에 바인딩되고 각 스레드에 많은 수의 동시 스레드를 생성 할 수 있으며, 장점은 대규모 동시성, 고성능, 간단한 프로그래밍 및 광범위한 응용 시나리오를 지원한다는 점이며 단점은 비 차단 모델보다 더 많은 메모리를 차지한다는 점이다
- UDP 통신 모델 : 간단한 UDP 통신 지원 
- 트리거 모델 : 일반적으로 시간 제한 작업 적용에 사용된다.
   
위의 서비스 모델은 구성 파일 구성 항목을 통해 시작할 프로세스 수를 결정하여 여러 프로세스를 시작할 수 있다.
  
#### 2. acl_master의 역할은 무엇인가요? 어떤 플랫폼이 지원되나요? 
acl_master는 위의 각 서버에서 작성된 서비스 프로세스 제어 및 관리 프로그램으로, 각 서비스 하위 프로세스를 시작 및 중지하고, 하위 프로세스 수와 사전 시작 정책을 제어하며, 하위 프로세스의 이상 징후를 모니터링한다. acl_master는 LINUX의 xinetd 서비스 프로세스와 다소 유사하지만, acl_master가 더 완전하고 강력하여 상주 및 반 상주 서비스 하위 프로세스를 지원하는 반면, xinetd는 연결을 위한 프로세스만 생성하고 동시성이 높은 서비스 시나리오에는 사용할 수 없다는 차이점이 있다. 마스터는 리눅스의 xinetd 서비스 프로세스와 약간 유사하지만, 차이점은 acl_master가 더 완전하고 강력하며 상주 및 준 상주 서비스 하위 프로세스를 지원하는 반면, xinetd는 연결에 대한 프로세스만 생성하므로 동시 접속이 많은 서비스 애플리케이션 시나리오에는 사용할 수 없다는 것이다.  
현재 acl_master 프로세스는 리눅스/MacOS/프리BSD/솔라리스(X86)와 같은 유닉스 플랫폼에서만 지원되며, **윈도우 플랫폼에서는 지원되지 않는다**.  
  
#### 3. 서비스 하위 프로세스가 acl_master 제어 관리 없이 독립적으로 실행될 수 있나요? 
가능하다. acl_master가 없는 경우 위의 각 서비스 모델에서 작성한 서비스 프로그램을 수동으로 시작할 수 있다. 시작 방법은 일반적으로 . /xxxx 단독 xxxx.cf로 서비스 프로그램이 단독 모드로 시작되며, 자세한 내용은 main.cpp의 시작 모드를 참고하시기 바라며, WINDOWS 플랫폼에서는 단독 모드에서만 수동으로 시작할 수 있다.  
  
#### 4. 수동 모드로 실행할 때 "유휴 시간 초과 - 종료, 유휴"가 발생하면 어떻게 해야 하나요? 
acl의 서버 프로그래밍 모델은 세미 레지던시(일정 시간 동안 유휴 상태로 실행되거나 처리한 연결 수가 설정값에 도달하면 자동으로 종료되는 기능으로, 주기적으로 프로세스 종료를 통해 리소스 누수를 방지할 수 있는 장점이 있고, 다른 한편으로는 개발 중 밸그라인드를 통해 사용자의 메모리 확인이 용이함)를 지원하므로, 프로세스가 종료되지 않게 하려면 단독 모드에서 시작 구성 파일을 서비스 프로그램에 전달할 수 있다. 프로세스가 종료되지 않도록 하려면, 시작 구성 파일을 단독 모드에서 서비스 프로그램에 전달할 수 있습니다(예: startup as . /xxxx 단독 xxxx.cf(main.cpp 및 관련 헤더 파일 참조), 서버 모델에 따라 다음 구성 항목을 채택한다:

- 스레드 풀링 모델: 단일 사용 제한 및 단일 유휴 제한 구성 항목을 0으로 설정한다
- 스레드 풀링 모델: 구성 항목 ioctl_use_limit 및 ioctl_idle_limit을 0으로 설정한다
- 비차단 모델: 구성 항목 aio_use_limit 및 aio_idle_limit을 0으로 설정한다
- 동시 모델: 구성 항목 fiber_use_limit 및 fiber_idle_limit을 0으로 설정한다
- UDP 통신 모델: 구성 항목 udp_use_limit 및 udp_idle_limit을 0으로 설정한다
- 트리거 모델: 구성 항목 trigger_use_limit을 0으로 설정한다
    
#### 5. acl_master 제어 모드에서 서비스 하위 프로세스가 여러 프로세스를 사전 시작하는 방법은 무엇인가요? 
각 서비스 서브 프로세스의 설정 파일을 수정하고, 설정 항목인 마스터_맥스프로크와 마스터_프리포크를 시작할 프로세스 수로 설정하고(동일한 값으로), 서브 프로세스가 유휴 종료되지 않도록 XXX_USE_LIMIT와 XXX_IDLE_LIMIT를 0으로 설정해야 한다. XXX_USE_LIMIT와 XXX_IDLE_LIMIT는 서버 모델마다 다르므로 위 (4)의 설명을 참조하시기 바랍니다. LIMIT와 XXX_IDLE_LIMIT는 서버 모델마다 다르며, 자세한 내용은 위 (4)를 참고하시기 바런다.

#### 6. acl_master 제어 모드, 인트라넷 주소만 수신하는 방법은 무엇인가요? 
acl_master 모드에서는 마스터 서비스 구성 항목을 퍼지 매칭할 수 있다. 즉, 수신 주소를 192.168.*로 작성할 수 있다. *:8192 또는 10.0.*. *:8192로 작성하면 acl_master가 서버의 모든 NIC 주소를 자동으로 검색하지만 서비스 일치 조건의 인트라넷 주소만 수신하여 통합 배포에 편의를 제공한다.  
  



### 데이터베이스 모듈  
  
#### 1. acl 데이터베이스 클라이언트에서 지원하는 데이터베이스는 무엇인가요? 
현재 acl 데이터베이스 클라이언트 라이브러리는 다음과 같은 데이터베이스를 지원한다: mysql, postgresql, sqlite  
  
#### 2. acl 데이터베이스 모듈은 어떻게 작동하나요? 
acl 데이터베이스 모듈은 공식 데이터베이스 드라이버(mysql, postgresql, sqlite 포함)를 캡슐화하므로 사용자는 버전 일관성을 고려하여 해당 공식 데이터베이스 드라이버를 다운로드해야 하며, https://github.com/acl-dev/third_party 에서 다운로드하는 것이 좋습니다. 또한, acl 데이터베이스 모듈은 동적 로딩 방법을 사용하여 데이터베이스 드라이버를 로드하므로 사용자는 컴파일 된 데이터베이스 동적 라이브러리를 적절한 위치에 배치하고 acl::db_handle::set_loadpath()를 호출해야 합니다. 또한, acl 데이터베이스 모듈은 동적 로딩 방식을 사용하여 데이터베이스 드라이버를 로드하므로 사용자는 컴파일된 데이터베이스 동적 라이브러리를 적절한 위치에 배치하고 acl::db_handle::set_loadpath( )를 호출하여 데이터베이스 드라이버 동적 라이브러리의 전체 경로를 설정해야 acl 데이터베이스 모듈이 이 경로를 사용하여 동적으로 로드할 수 있습니다.  
  