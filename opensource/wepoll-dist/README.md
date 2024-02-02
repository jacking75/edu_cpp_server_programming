# wepoll - epoll for windows
[원 소스](https://github.com/piscisaureus/wepoll )  
  
이 라이브러리는 Windows 애플리케이션용 epoll API를 구현한다. 이 라이브러리는 빠르고 확장성이 뛰어나며 Linux의 epoll API 및 동작과 매우 유사하다.  
  
## 근거
Linux, OS X 및 기타 여러 운영 체제와 달리 Windows에는 소켓 상태 알림을 수신하기 위한 마땅한 API가 없다. select 및 WSAPoll API만 지원하지만 확장성이 떨어지고 다른 문제가 있다.  
  
소프트웨어가 크로스 플랫폼으로 설계된 경우 I/O 완료 포트를 사용하는 것이 항상 실용적인 것은 아니다. Wepoll은 Linux에서 실행되도록 설계된 소프트웨어의 드롭인 대체에 훨씬 더 가까운 대안을 제공한다.    
   
## 특징
- 100,000개의 소켓을 효율적으로 폴링할 수 있다.
- 완벽한 스레드 안전.
- 여러 스레드가 동일한 epoll 포트를 폴링할 수 있다.
- 여러 개의 epoll 세트에 소켓을 추가할 수 있다.
- 모든 epoll 이벤트(EPOLLIN, EPOLLOUT, EPOLLPRI, EPOLLRDHUP)가 지원된다.  
- 레벨 트리거 및 원샷(EPOLLONESTHOT) 모드가 지원된다.
- 간편한 임베드: 두 개의 파일만 있으면 된다.
  

## 제한 사항
- 소켓에서만 작동한다.
- 엣지 트리거(EPOLLET) 모드는 지원되지 않는다.
  
  
## 사용 방법
- 라이브러리는 단일 소스 파일(wepoll.c)과 단일 헤더 파일(wepoll.h)로 배포된다.
- `.c` 파일을 프로젝트의 일부로 컴파일하고 필요한 곳에 헤더를 포함하세요.

  
## 호환성
- Windows Vista 이상이 필요하다.
- 최신 버전의 MSVC, Clang 및 GCC로 컴파일할 수 있다.
  
   
## API
  
### 일반 참고 사항
- epoll 포트는 파일 설명자가 아닌 핸들이다.
- 모든 함수는 실패 시 errno와 GetLastError()를 모두 설정한다.
- 더 자세한 설명서는 epoll(7) 매뉴얼 페이지와 아래에 링크된 함수별 매뉴얼 페이지를 참조하세요.





<br>    
   
# 아래 글이 원본 README.md 이다
[![][ci status badge]][ci status link]

This library implements the [epoll][man epoll] API for Windows
applications. It is fast and scalable, and it closely resembles the API
and behavior of Linux' epoll.

## Rationale

Unlike Linux, OS X, and many other operating systems, Windows doesn't
have a good API for receiving socket state notifications. It only
supports the `select` and `WSAPoll` APIs, but they
[don't scale][select scale] and suffer from
[other issues][wsapoll broken].

Using I/O completion ports isn't always practical when software is
designed to be cross-platform. Wepoll offers an alternative that is
much closer to a drop-in replacement for software that was designed
to run on Linux.

## Features

* Can poll 100000s of sockets efficiently.
* Fully thread-safe.
* Multiple threads can poll the same epoll port.
* Sockets can be added to multiple epoll sets.
* All epoll events (`EPOLLIN`, `EPOLLOUT`, `EPOLLPRI`, `EPOLLRDHUP`)
  are supported.
* Level-triggered and one-shot (`EPOLLONESTHOT`) modes are supported
* Trivial to embed: you need [only two files][dist].

## Limitations

* Only works with sockets.
* Edge-triggered (`EPOLLET`) mode isn't supported.

## How to use

The library is [distributed][dist] as a single source file
([wepoll.c][wepoll.c]) and a single header file ([wepoll.h][wepoll.h]).<br>
Compile the .c file as part of your project, and include the header wherever
needed.

## Compatibility

* Requires Windows Vista or higher.
* Can be compiled with recent versions of MSVC, Clang, and GCC.

## API

### General remarks

* The epoll port is a `HANDLE`, not a file descriptor.
* All functions set both `errno` and `GetLastError()` on failure.
* For more extensive documentation, see the [epoll(7) man page][man epoll],
  and the per-function man pages that are linked below.

### epoll_create/epoll_create1

```c
HANDLE epoll_create(int size);
HANDLE epoll_create1(int flags);
```

* Create a new epoll instance (port).
* `size` is ignored but most be greater than zero.
* `flags` must be zero as there are no supported flags.
* Returns `NULL` on failure.
* [Linux man page][man epoll_create]

### epoll_close

```c
int epoll_close(HANDLE ephnd);
```

* Close an epoll port.
* Do not attempt to close the epoll port with `close()`,
  `CloseHandle()` or `closesocket()`.

### epoll_ctl

```c
int epoll_ctl(HANDLE ephnd,
              int op,
              SOCKET sock,
              struct epoll_event* event);
```

* Control which socket events are monitored by an epoll port.
* `ephnd` must be a HANDLE created by
  [`epoll_create()`](#epoll_createepoll_create1) or
  [`epoll_create1()`](#epoll_createepoll_create1).
* `op` must be one of `EPOLL_CTL_ADD`, `EPOLL_CTL_MOD`, `EPOLL_CTL_DEL`.
* `sock` must be a valid socket created by [`socket()`][msdn socket],
  [`WSASocket()`][msdn wsasocket], or [`accept()`][msdn accept].
* `event` should be a pointer to a [`struct epoll_event`](#struct-epoll_event).<br>
  If `op` is `EPOLL_CTL_DEL` then the `event` parameter is ignored, and it
  may be `NULL`.
* Returns 0 on success, -1 on failure.
* It is recommended to always explicitly remove a socket from its epoll
  set using `EPOLL_CTL_DEL` *before* closing it.<br>
  As on Linux, closed sockets are automatically removed from the epoll set, but
  wepoll may not be able to detect that a socket was closed until the next call
  to [`epoll_wait()`](#epoll_wait).
* [Linux man page][man epoll_ctl]

### epoll_wait

```c
int epoll_wait(HANDLE ephnd,
               struct epoll_event* events,
               int maxevents,
               int timeout);
```

* Receive socket events from an epoll port.
* `events` should point to a caller-allocated array of
  [`epoll_event`](#struct-epoll_event) structs, which will receive the
  reported events.
* `maxevents` is the maximum number of events that will be written to the
  `events` array, and must be greater than zero.
* `timeout` specifies whether to block when no events are immediately available.
  - `<0` block indefinitely
  - `0`  report any events that are already waiting, but don't block
  - `≥1` block for at most N milliseconds
* Return value:
  - `-1` an error occurred
  - `0`  timed out without any events to report
  - `≥1` the number of events stored in the `events` buffer
* [Linux man page][man epoll_wait]

### struct epoll_event

```c
typedef union epoll_data {
  void* ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
  SOCKET sock;        /* Windows specific */
  HANDLE hnd;         /* Windows specific */
} epoll_data_t;
```

```c
struct epoll_event {
  uint32_t events;    /* Epoll events and flags */
  epoll_data_t data;  /* User data variable */
};
```

* The `events` field is a bit mask containing the events being
  monitored/reported, and optional flags.<br>
  Flags are accepted by [`epoll_ctl()`](#epoll_ctl), but they are not reported
  back by [`epoll_wait()`](#epoll_wait).
* The `data` field can be used to associate application-specific information
  with a socket; its value will be returned unmodified by
  [`epoll_wait()`](#epoll_wait).
* [Linux man page][man epoll_ctl]

| Event         | Description                                                          |
|---------------|----------------------------------------------------------------------|
| `EPOLLIN`     | incoming data available, or incoming connection ready to be accepted |
| `EPOLLOUT`    | ready to send data, or outgoing connection successfully established  |
| `EPOLLRDHUP`  | remote peer initiated graceful socket shutdown                       |
| `EPOLLPRI`    | out-of-band data available for reading                               |
| `EPOLLERR`    | socket error<sup>1</sup>                                             |
| `EPOLLHUP`    | socket hang-up<sup>1</sup>                                           |
| `EPOLLRDNORM` | same as `EPOLLIN`                                                    |
| `EPOLLRDBAND` | same as `EPOLLPRI`                                                   |
| `EPOLLWRNORM` | same as `EPOLLOUT`                                                   |
| `EPOLLWRBAND` | same as `EPOLLOUT`                                                   |
| `EPOLLMSG`    | never reported                                                       |

| Flag             | Description               |
|------------------|---------------------------|
| `EPOLLONESHOT`   | report event(s) only once |
| `EPOLLET`        | not supported by wepoll   |
| `EPOLLEXCLUSIVE` | not supported by wepoll   |
| `EPOLLWAKEUP`    | not supported by wepoll   |

<sup>1</sup>: the `EPOLLERR` and `EPOLLHUP` events may always be reported by
[`epoll_wait()`](#epoll_wait), regardless of the event mask that was passed to
[`epoll_ctl()`](#epoll_ctl).


[ci status badge]:  https://ci.appveyor.com/api/projects/status/github/piscisaureus/wepoll?branch=master&svg=true
[ci status link]:   https://ci.appveyor.com/project/piscisaureus/wepoll/branch/master
[dist]:             https://github.com/piscisaureus/wepoll/tree/dist
[man epoll]:        http://man7.org/linux/man-pages/man7/epoll.7.html
[man epoll_create]: http://man7.org/linux/man-pages/man2/epoll_create.2.html
[man epoll_ctl]:    http://man7.org/linux/man-pages/man2/epoll_ctl.2.html
[man epoll_wait]:   http://man7.org/linux/man-pages/man2/epoll_wait.2.html
[msdn accept]:      https://msdn.microsoft.com/en-us/library/windows/desktop/ms737526(v=vs.85).aspx
[msdn socket]:      https://msdn.microsoft.com/en-us/library/windows/desktop/ms740506(v=vs.85).aspx
[msdn wsasocket]:   https://msdn.microsoft.com/en-us/library/windows/desktop/ms742212(v=vs.85).aspx
[select scale]:     https://daniel.haxx.se/docs/poll-vs-select.html
[wsapoll broken]:   https://daniel.haxx.se/blog/2012/10/10/wsapoll-is-broken/
[wepoll.c]:         https://github.com/piscisaureus/wepoll/blob/dist/wepoll.c
[wepoll.h]:         https://github.com/piscisaureus/wepoll/blob/dist/wepoll.h
