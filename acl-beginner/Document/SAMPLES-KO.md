# acl 예제

acl에 다양한 기능이 포함되어있기에 예제 목록도 복잡합니다. 이를 정리/번역한 문서입니다.

## 주요 예제

### MYSQL 관련 예제
* **mysql_query** : [sample 소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/db/mysql_query)

* 아래 mysql, mysql2에서는 char 형식으로 raw query로 쿼리문을 처리하고 있습니다. 따라서 이보다 위의 **mysql_query**를 사용하는 것을 추천합니다.
  * 2.52 mysql: mysql 클라이언트 프로그램 [sample 소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/mysql)
  * 2.53 mysql2: mysql 클라이언트 프로그램 [sample 소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/mysql2)

### Redis 관련 예제
* **Redis 관련 파일** [sample 소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/redis)
  - 2.75 redis/redis_connection: redis 연결 클라이언트 프로그램
  - 2.76 redis/redis_hash: redis 해시 테이블과 유사한 클라이언트 애플리케이션
  - 2.77 redis/redis_hyperloglog: redis 하이퍼로그 클라이언트 프로그램
  - 2.78 redis/redis_key: redis 키 클라이언트 응용 프로그램
  - 2.79 redis/redis_list: redis 리스트 클라이언트 프로그램
  - 2.80 redis/redis_manager: redis 연결 풀 클러스터 관리 클라이언트 프로그램
  - 2.81 redis/redis_pool: redis 연결 풀 클라이언트 프로그램
  - 2.82 redis/redis_pubsub: redis 게시-구독 클라이언트 프로그램
  - 2.83 redis/redis_server: redis 서버 관리 클라이언트 애플리케이션
  - 2.84 REDIS/REDIS_SET: REDIS 수집 클라이언트 애플리케이션
  - 2.85 redis/redis_string: redis 문자열 클라이언트 애플리케이션
  - 2.86 redis/redis_trans: redis 트랜잭션 클라이언트
  - 2.87 redis/redis_zset: redis 정렬 집합 클라이언트 프로그램
  - 2.88 redis/redis_zset_pool: redis ordered-set 클라이언트, ordered-set 객체를 사용하여 대용량 데이터 블록을 저장하고 연결 풀링을 지원합니다.

### Coroutine 관련 예제
* 전체 Coroutine 관련 lib_fiber 예제 : [samples 소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c++)
* lib_fiber 설명 문서(EN) : [lib_fiber 설명 문서](https://github.com/acl-dev/acl/blob/master/lib_fiber/README_en.md)

* 동기화 객체 사용 예제
  + event_mutex [sample 소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/event_mutex)
  + thread_cond [sample 소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_cond)
  + thread_event [sample 소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_event)
  + thread_lock [sample 소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_lock)
  + thread_mutex [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_mutex)
  + thread_tbox [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_tbox)
    
* 위 Samples은 [acl project](https://github.com/acl-dev/acl/) library의 APIs 사용


---
# 아래 SAMPLES.md 파일의 기계 번역

원문 출처 : [SAMPLES.md 파일](https://github.com/acl-dev/acl/blob/master/SAMPLES.md)


## acl 프로젝트의 테스트 및 애플리케이션 예제는 주로 다음과 같이 세 가지 예제 모음으로 구성되어 있습니다:

### 1. acl/samples [소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl/samples)
#### : 이 디렉터리에 있는 예제는 주로 lib_acl 및 lib_protocol 라이브러리를 기반으로 합니다.
- 1.1 acl: 현재 acl 라이브러리 버전 번호를 출력합니다.
- 1.2 aio/client: 비차단 io 클라이언트.
- 1.3 aio/server: 비차단형 io 서버.
- 1.4 base64: base64 인코딩/디코딩 프로그램
- 1.5 btree: 바이너리 트리 프로그램
- 1.6 cache: 객체 캐싱 프로그램
- 1.7 cache2: 객체 캐싱 프로그램
- 1.8 cgi_env: CGI 환경 변수를 가져오는 CGI 프로그램.
- 1.9 chunk_chain: 바이너리 청크 데이터 프로그램
- 1.10 configure: 구성 파일 핸들러
- 1.11 connect: 네트워크 클라이언트 연결 프로그램
- 1.12 dbpool: 데이터베이스 연결 풀 프로그램
- 1.13 dlink: 청크 조회 알고리즘 프로그램
- 1.14 dns: 도메인 이름 조회 프로그램
- 1.15 dns_req: 도메인 이름 조회 프로그램
- 1.16 event: 이벤트 엔진 프로그램
- 1.17 fifo: FIFO 알고리즘 프로그램
- 1.18 file: 파일 스트림 핸들러
- 1.19 file_fmt: UNIX \n을 WIN32 \r\n으로 변환하거나 역변환하는 프로그램
- 1.20 FileDir: win32에서 디렉터리 조작 프로그램
- 1.21 flock: 파일 잠금 핸들러
- 1.22 gc: 자동 메모리 회수 프로그램
- 1.23 htable: 해시 테이블 핸들러
- 1.24 http/header: HTTP 클라이언트 프로그램
- 1.25 http/url_get1: 웹 페이지 다운로드 클라이언트 프로그램
- 1.26 http/url_get2: 웹 다운로드 클라이언트 프로그램
- 1.27 http/url_get3: 웹 페이지 다운로드 클라이언트 프로그램
- 1.28 http_aio: 간단한 HTTP 비동기 다운로더
- 1.29 http_client: WIN32용 HTTP 클라이언트
- 1.30 http_probe: WIN32용 HTTP 클라이언트 프로그램
- 1.31 ifconf: 로컬 NIC를 가져오는 프로그램
- 1.32 iplink: IP 주소 세그먼트 관리 프로그램
- 1.33 iterator: C 스타일의 트래버스 프로그램.
- 1.34 json: json 객체 핸들러
- 1.35 json2: json 객체 핸들러
- 1.36 json3: JSON 객체 핸들러
- 1.37 jt2ft: 중국어 간체에서 중국어 번체로 변환기
- 1.38 log: 로그 핸들러
- 1.39 master/aio_echo: 비차단 에코 서버 프로그램
- 1.40 master/aio_proxy: 논-블로킹 TCP 프록시 프로그램
- 1.41 master/ioctl_echo2: 멀티스레드 에코 서버 프로그램
- 1.42 master/ioctl_echo3: 멀티 스레드 에코 서버 프로그램
- 1.43 master/master_notify: 멀티 스레드 서버 프로그램
- 1.44 master/master_threads: 멀티 스레드 서버 프로그램
- 1.45 master/single_echo: 멀티스레드 에코 서버 프로그램
- 1.46 master/trigger: 트리거 서버 프로그램
- 1.47 master/udp_echo: UDP 에코 서버 프로그램
- 1.48 memdb: 간단한 인메모리 데이터베이스 프로그램
- 1.49 mempool: 메모리 풀 프로그램
- 1.50 mkdir: 다단계 디렉토리 프로그램 만들기
- 1.51 net: 간단한 네트워크 프로그램
- 1.52 ping: 차단/비차단 핑 프로그램
- 1.53 pipe: 파이프 핸들러
- 1.54 proctl: WIN32 부모/자식 프로그램
- 1.55 resolve: 도메인 이름 확인 프로그램
- 1.56 server: 단순 서버 프로그램
- 1.57 server1: 단순 서버 프로그램
- 1.58 slice: 메모리 풀 슬라이싱 프로그램
- 1.59 slice_mem: 메모리 풀 슬라이싱 프로그램
- 1.60 smtp_client: SMTP 클라이언트 전송 프로그램
- 1.61 string: acl 문자열 핸들러
- 1.62 thread: 스레드 프로그램
- 1.63 token_tree: 256-포크 트리 프로그램
- 1.64 udp_clinet: UDP 클라이언트 프로그램
- 1.65 udp_server: UDP 서버 프로그램
- 1.66 urlcode: URL 인코딩 핸들러
- 1.67 vstream: IO 네트워크 스트림 핸들러
- 1.68 vstream_client: 네트워크 클라이언트 스트리밍 프로시저
- 1.69 vstream_fseek: 파일 스트림 핸들러
- 1.70 vstream_fseek2: 파일 스트림 핸들러
- 1.71 vstream_popen: 파이프 스트림 핸들러
- 1.71 vstream_popen2: 파이프라인 스트림 핸들러
- 1.71 vstream_popen3: 파이프라인 스트림 핸들러
- 1.72 vstream_server: 웹 서버 애플리케이션
- 1.73 xml: XML 파서
- 1.74 xml2: XML 파서
- 1.75 zdb: 숫자 키/값 파일 저장 엔진 프로그램

### 2. acl/lib_acl_cpp/samples [소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples)
#### : 이 디렉터리에 있는 예제는 기본적으로 lib_acl_cpp 라이브러리에 기반한 C++ 예제입니다.
- 2.1 aio/aio_client: 비차단 네트워크 클라이언트 프로그램
- 2.2 aio/aio_dns: 비차단 도메인 이름 확인 클라이언트 프로그램
- 2.3 aio/aio_echo: 논 블로킹 에코 서버 프로그램
- 2.4 aio/aio_ipc: 차단/비차단 통합 네트워크 프로그램
- 2.5 aio/aio_server: 비차단 서버 프로그램
- 2.6 beanstalk: 큐잉 애플리케이션 빈스톡드용 클라이언트 애플리케이션
- 2.7 benchmark: 성능 테스트와 관련된 프로그램 모음
- 2.8 cgi: 간단한 웹 CGI 프로그램
- 2.9 cgi_upload: 업로드된 파일을 받기 위한 CGI 프로그램
- 2.10 charset: 문자 집합 변환 프로그램
- 2.11 check_trigger: 원격 HTTP 서버의 상태를 확인하는 트리거 프로그램.
- 2.12 connect_manager: 클라이언트 연결 풀링 클러스터 관리자.
- 2.13 db_service: 데이터베이스 관련 애플리케이션
- 2.14 dbpool: 데이터베이스 클라이언트 연결을 풀링하는 애플리케이션.
- 2.15 dircopy: 디렉터리 파일 복사 프로그램
- 2.16 final_class: 클래스 상속을 비활성화합니다.
- 2.17 flock: 파일 잠금 핸들러
- 2.18 fs_benchmark: 파일 시스템 스트레스 테스트 프로그램
- 2.19 fstream: 파일 스트리밍 프로그램
- 2.20 gui_rpc: WIN32 인터페이스의 메시지와 통합된 WIN32의 차단 프로시저 예제
- 2.21 hsclient: 핸들 소켓 클라이언트 프로그램
- 2.22 http_client: HTTP 클라이언트 애플리케이션
- 2.23 http_client2: HTTP 클라이언트 프로그램
- 2.24 http_mime: HTTP 프로토콜 MIME 포맷 핸들러
- 2.25 http/http_request: http_request 클래스를 사용하는 HTTP 클라이언트.
- 2.26 http_request_manager: HTTP 클라이언트 연결 풀링 클러스터 애플리케이션
- 2.27 http_request_pool: HTTP 클라이언트 연결 풀링 절차
- 2.28 http_request2: http_request 클래스를 사용하는 HTTP 클라이언트 프로그램
- 2.29 http/http_response: http_reponse 클래스를 사용하여 HTTP 클라이언트 요청에 응답하는 HTTP 클라이언트 애플리케이션.
- 2.30 http_server: 간단한 HTTP 서버 애플리케이션
- 2.31 http_servlet: JAVA HttpServlet과 유사한 애플리케이션
- 2.32 http_servlet2: 자바 서블릿과 유사한 프로그램
- 2.33 HttpClient: 간단한 HTTP 클라이언트 애플리케이션
- 2.34 json/json0 - json7: json 문자열 객체 파서
- 2.35 logger: 로깅 애플리케이션
- 2.36 master/master_aio: 비차단 서버 프로그램
- 2.37 master_aio_proxy: 비차단 TCP 프록시 서버 프로그램
- 2.38 master/master_http_aio: 간단한 논-블로킹 HTTP 서버 프로그램
- 2.39 master/master_http_rpc: 차단/비차단 통합 HTTP 서버 프로그램
- 2.40 master/master_http_threads: 멀티 스레드 HTTP 서버 프로그램
- 2.40 master/master_http_threads2: 멀티 스레드 HTTP 서버 프로그램
- 2.41 master/maser_proc: 프로세스 풀 서버 프로그램
- 2.42 master/master_threads: 멀티스레드 서버 프로그램
- 2.43 master/master_trigger: 트리거 서버 프로그램
- 2.44 master/master_trigger: UDP 통신 서버 프로그램
- 2.45 master/master_udp_threads: 멀티 스레드 UDP 통신 서버 프로그램
- 2.46 md5: MD5 핸들러
- 2.47 mem_cache: 멤캐시 클라이언트 프로그램
- 2.47 memcache_pool: 연결 풀링을 지원하는 멤캐시드 클라이언트 애플리케이션
- 2.48 mime: 메일 MIME 파싱 핸들러
- 2.49 mime_base64: MIME BASE64 포맷 핸들러
- 2.50 mime_qp: MIME QP 형식 처리기
- 2.51 mime_xxcode: MIME XXCODE 포맷 핸들러
- **2.52 mysql: mysql 클라이언트 프로그램** [소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/mysql)
- **2.53 mysql2: mysql 클라이언트 프로그램** [소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/mysql2)
- 2.54 rfc822: 메일용 RFC822 프로토콜 핸들러
- 2.55 rfc2047: 메일용 RFC2047 프로토콜 핸들러
- 2.56 rpc_download: 차단/비차단 통합 HTTP 다운로드 프로그램
- 2.57 scan_dir: 디렉토리에 대한 재귀적 스캐너
- 2.58 singleton: 단일 인스턴스 애플리케이션
- 2.59 session: 세션 프로그램
- 2.60 socket_client: 네트워크 클라이언트 프로그램
- 2.61 socket_stream: 네트워크 스트림 핸들러
- 2.62 SQLITE: SQLITE 데이터베이스 프로그램
- 2.63 ssl_aio_client: SSL 비차단 네트워크 클라이언트 프로그램
- 2.64 ssl_clinet: SSL 차단 네트워크 클라이언트 프로시저
- 2.65 string: 동적 버퍼 핸들러
- 2.66 string2: 동적 버퍼 핸들러
- 2.67 thread: 멀티 스레드 프로그램
- 2.68 thread_client: 멀티 스레드 클라이언트 프로그램
- 2.69 thread_pool: 스레드 풀 프로그램
- 2.70 udp_client: UDP 통신 클라이언트 프로그램
- 2.71 url_coder: URL 코더 및 디코더 프로그램
- 2.72 win_dbservice: WIN32 GUI 기반 데이터베이스 핸들러
- 2.73 winaio: WIN32 GUI 기반 비차단 클라이언트 프로그램
- 2.74 xml: XML 객체 파싱 핸들러
- 2.75 zlib: 압축 포맷 핸들러
- **Redis 관련 파일** [소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/redis)
- 2.75 redis/redis_connection: redis 연결 클라이언트 프로그램
- 2.76 redis/redis_hash: redis 해시 테이블과 유사한 클라이언트 애플리케이션
- 2.77 redis/redis_hyperloglog: redis 하이퍼로그 클라이언트 프로그램
- 2.78 redis/redis_key: redis 키 클라이언트 응용 프로그램
- 2.79 redis/redis_list: redis 리스트 클라이언트 프로그램
- 2.80 redis/redis_manager: redis 연결 풀 클러스터 관리 클라이언트 프로그램
- 2.81 redis/redis_pool: redis 연결 풀 클라이언트 프로그램
- 2.82 redis/redis_pubsub: redis 게시-구독 클라이언트 프로그램
- 2.83 redis/redis_server: redis 서버 관리 클라이언트 애플리케이션
- 2.84 REDIS/REDIS_SET: REDIS 수집 클라이언트 애플리케이션
- 2.85 redis/redis_string: redis 문자열 클라이언트 애플리케이션
- 2.86 redis/redis_trans: redis 트랜잭션 클라이언트
- 2.87 redis/redis_zset: redis 정렬 집합 클라이언트 프로그램
- 2.88 redis/redis_zset_pool: redis ordered-set 클라이언트, ordered-set 객체를 사용하여 대용량 데이터 블록을 저장하고 연결 풀링을 지원합니다.

### 3. acl/app 
#### : 이 디렉터리에 있는 예제는 주로 실용적인 예제입니다.
- 3.1 wizard: acl 서버 프레임워크 ** 프로그램을 기반으로 프로그램 템플릿을 생성하는 데 사용됩니다.
- 3.2 gid: 전 세계적으로 고유한 ID 번호를 생성하는 데 사용되는 서비스 프로그램(클라이언트 라이브러리 포함).
- 3.3 net_tools: 네트워크 상태를 테스트하는 프로그램.
- 3.4 master_dispatch: 백엔드 서버에 TCP 연결을 분산하는 연결 밸런서.
- 3.5 jaws(현재 사용할 수 없음): acl의 논 블로킹 통신 모듈과 HTTP 모듈을 기반으로 하는 간단한 HTTP 고 동시성 서버 프로그램.

Translated with DeepL.com (free version)
