# Synchronization Object in ACL
ACL에서 제공하는 다양한 동기화 객체를 실습하고 테스트하는 예제입니다. <br>
이 프로젝트는 ACL의 fiber 모듈에서 제공하는 다양한 동기화 객체를 이용하여 <br>
코루틴 기반의 비동기 프로그래밍 환경에서 자원 관리 및 동기화 문제를 해결하는 방법을 보여줍니다. <br>

-----

# 목차
- ### [프로젝트 개요](#프로젝트-개요)
- ### [코루틴과 동기화 객체의 필요성](#코루틴과-동기화-객체의-필요성-1)
- ### [ACL에서 제공하는 동기화 객체 종류 및 개념](#ACL에서-제공하는-동기화-객체-종류-및-개념-1)
- ### [실습 코드 설명](#실습-코드-설명-1)
  - #### [fiber_mutex_ex.cpp](#fiber_mutex_excpp-1)
  - #### [fiber_lock_ex.cpp](#fiber_lock_excpp-1)
  - #### [fiber_rwlock_ex.cpp](#fiber_rwlock_excpp-1)
  - #### [fiber_sem_ex.cpp](#fiber_sem_excpp-1)
  - #### [fiber_event_ex.cpp](#fiber_event_excpp-1)
  - #### [fiber_cond_ex.cpp](#fiber_cond_excpp-1)
  - #### [fiber_mutex_stat_ex.cpp](#fiber_mutex_stat_excpp-1)


-----

# 프로젝트 개요

이 프로젝트의 주요 목표는 다음과 같습니다:
- 코루틴 기반의 프로그래밍에서 동기화 문제 해결
- ACL에서 제공하는 동기화 객체의 이해

관련 문서:
- acl_fiber란? [📄Acl fiber의 Coroutine](../Document/Coroutine.md)
- 동기화 객체란? [📄동기화 객체](../Document/Cpp-SynchronizationObject.md)


-----

# 코루틴과 동기화 객체의 필요성

코루틴은 비동기 프로그래밍에서 경량화된 스레드 역할을 수행하며, 함수의 중간에서 실행을 멈추고, 나중에 다시 실행을 재개할 수 있는 기능을 제공합니다.  <br>
이러한 코루틴은 매우 효율적이며, 특히 I/O 바운드 작업이나 이벤트 기반 시스템에서 유용합니다. <br> <br>

하지만 코루틴 환경에서 여러 코루틴이 동일한 자원에 접근하는 경우, 데이터 경합(data race)이나 교착 상태(deadlock)와 같은 문제가 발생할 수 있습니다.  <br>
이러한 문제를 해결하기 위해 동기화 객체가 필요합니다. ACL에서 제공하는 동기화 객체는 이러한 문제를 효과적으로 처리할 수 있는 다양한 도구를 제공합니다. <br>

-----

# Acl에서 제공하는 동기화 객체 종류 및 개념

Acl에서 제공하는 동기화 객체는 아래와 같습니다.
- fiber_mutex
- fiber_lock
- fiber_rwlock
- fiber_sem
- fiber_event
- fiber_cond
- fiber_mutex_stat
- event_mutex
- event_timer


### fiber_mutex
- **코루틴 뮤텍스 객체**
- 하나의 코루틴이 자원에 접근하는 동안, 다른 코루틴이 해당 자원에 접근하지 못하도록 상호 배제를 보장합니다.

- **사용 예시**: 코루틴 간 자원 접근 제어

- **구현**
  ```cpp
  bool fiber_mutex::lock(void) { acl_fiber_mutex_lock(mutex_); return true; }
  bool fiber_mutex::unlock(void) { acl_fiber_mutex_unlock(mutex_); return true; }
  ```

- **코드 예시**
  ```cpp
  acl::fiber_mutex mutex;
  mutex.lock();
  // 자원 접근
  mutex.unlock();
  ```

- **관련 파일**: [🖱️Acl 소스코드 : fiber_mutex](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_mutex.cpp)
- [📄더 자세한 fiber_mutex 사용법이 궁금해!](./Coroutine-fiber_mutex.md)

<br>


### fiber_lock
- **일반적인 락 메커니즘 제공**
  - 간단한 상호 배제 락 (코루틴 간 자원 보호를 위한)
  - 단일 스레드 내 코루틴 간의 동기화 관리를 위해 사용하기 때문에, 다중 스레드 환경에서는 문제가 발생할 수 있습니다.

- **사용 예시**: 공유 자원에 대한 단순한 보호와 접근 관리

- **구현**
  ```cpp
  bool fiber_lock::lock(void) { acl_fiber_lock_lock(lock_); return true; }
  bool fiber_lock::unlock(void) { acl_fiber_lock_unlock(lock_); return true; }
  ```

- **코드 예시**
  ```cpp
  acl::fiber_lock lock;
  lock.lock();   // 락 획득
  // 자원 접근
  lock.unlock(); // 락 해제
  ```

- **관련 파일**: [🖱️Acl 소스코드 : fiber_lock](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_lock.cpp)

<br>

#### Q: [fiber_lock]에서 fiber_mutex와 다른 점은 무엇인가?
- fiber_lock은 가벼운 락 메커니즘을 제공하는 반면, fiber_mutex는 더 정교한 상호 배제와 동기화 기능을 제공합니다.
- 간단한 동기화 작업에는 fiber_lock을, 복잡하고 중요한 자원 보호에는 fiber_mutex를 사용하는 것이 일반적입니다.

**차이점**:

- **fiber_lock**:
  - **일반적인 락 메커니즘**을 제공하며, 주로 단순한 상호 배제를 위해 사용됩니다.
  - fiber_lock은 상대적으로 더 가볍고, 일반적인 코루틴 간 자원 보호를 위한 락입니다. 
  - **사용 시나리오**: 단순한 상호 배제와 동기화가 필요한 상황에서 사용됩니다. 일반적으로 보호해야 할 자원에 대한 간단한 락/언락 작업에 적합합니다.

- **fiber_mutex**:
  - **뮤텍스(Mutex)**는 더 강력한 상호 배제 메커니즘을 제공하며, 여러 코루틴이 동일한 자원에 동시 접근하려고 할 때 그 자원을 보호합니다.
  - fiber_mutex는 락 상태를 더 세밀하게 관리하고, 데드락 방지 기능이 추가된 경우가 많습니다.
  - **사용 시나리오**: 더 복잡한 동기화 작업이 필요하거나, 자원의 보호가 필수적인 경우에 적합합니다. 코루틴 사이에서의 데드락을 방지하고자 할 때 유용합니다.




<br>


### fiber_rwlock 
- **읽기-쓰기 락 객체**
- Read의 경우 동시에 여러 코루틴을 허용하고, Write의 경우 하나의 코루틴만 허용

- **사용 예시**: 읽기 작업이 많고, 쓰기 작업은 드물게 발생하는 경우

- **구현**
  ```cpp
  void fiber_rwlock::rlock(void) { acl_fiber_rwlock_rlock(rwlk_); }
  void fiber_rwlock::wlock(void) { acl_fiber_rwlock_wlock(rwlk_); }
  ```

- **코드 예시**
  ```cpp
  acl::fiber_rwlock rwlock;
  rwlock.rlock();
  // 읽기 작업
  rwlock.runlock();

  rwlock.wlock();
  // 쓰기 작업
  rwlock.wunlock();
  ```

- **관련 파일**: [🖱️Acl 소스코드 : fiber_rwlock](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_rwlock.cpp)

<br>


### fiber_sem
- **코루틴 세마포어 객체**
- 특정 자원에 접근할 수 있는 코루틴의 수를 제한하는 객체

- **사용 예시**: 자원에 대한 접근 횟수를 제한하는 상황에서 사용
  
- **구현**
  ```cpp
  int fiber_sem::wait(void) { /* 세마포어 대기 */ }
  void fiber_sem::post(void) { /* 세마포어 해제 */ }
  ```

- **코드 예시**
  ```cpp
  acl::fiber_sem sem(1);  // 세마포어 초기화
  sem.wait();   // 세마포어를 대기
  sem.post();   // 세마포어 해제
  ```

- **관련 파일**: [🖱️Acl 소스코드 : fiber_sem](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_sem.cpp)

<br>



### fiber_event
- **코루틴 간 신호를 전달하여, 특정 조건이 발생했을 때 작업을 트리거하는 이벤트 객체**
  - 이벤트 발생 시 대기 중인 코루틴을 깨움

- **사용 예시**: 비동기 작업을 수행하는 코루틴 간 이벤트를 기반으로 동기화하는 경우

- **구현**
  ```cpp
  bool fiber_event::wait(void) { return acl_fiber_event_wait(event_); }
  bool fiber_event::notify(void) { return acl_fiber_event_notify(event_); }
  ```

- **코드 예시**
  ```cpp
  acl::fiber_event event;
  event.wait();   // 다른 코루틴이 이벤트를 발생시킬 때까지 대기
  event.notify(); // 이벤트 발생
  ```

- **관련 파일**: [🖱️Acl 소스코드 : fiber_event](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_event.cpp)

<br>





### fiber_cond
- **조건 만족 시까지 대기하는 코루틴을 깨우는 조건 변수 객체**
- 특정 조건 만족 시 대기 중인 코루틴에게 신호를 보내 작업을 계속하게

- **사용 예시**: 특정 이벤트나 조건이 발생할 때까지 코루틴을 일시 중단시키는 경우
  
- **구현**
  ```cpp
  bool fiber_cond::wait(fiber_mutex& mutex, int timeout /* = -1 */) { return acl_fiber_cond_wait(cond_, mutex.get_mutex()) == 0; }
  bool fiber_cond::notify(void) { return acl_fiber_cond_signal(cond_) == 0; }
  ```

- **코드 예시**
  ```cpp
  acl::fiber_cond cond;
  cond.wait(mutex);   // 조건이 만족될 때까지 대기
  cond.notify(); // 조건이 만족되었음을 신호
  ```

- **관련 파일**: [🖱️Acl 소스코드 : fiber_cond](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_cond.cpp)

<br>

#### Q: [fiber_cond]에서 fiber_event와 다른 점은 무엇인가?
- fiber_cond는 특정 조건이 충족될 때까지 대기시키며, 조건이 만족되면 다수의 코루틴을 깨우는 데 사용됩니다.
- 반면, fiber_event는 특정 이벤트 발생 시 대기 중인 코루틴을 깨우는 데 사용되며, 보다 일반적인 이벤트 기반 동기화에 적합합니다.


**차이점**:

- **fiber_cond** (조건 변수):
  - 특정 조건이 **만족될 때까지** 코루틴을 대기시키고, 조건이 만족되면 대기 중인 코루틴을 깨웁니다.
  - **조건 변수**는 주로 특정한 상태나 값이 만족될 때까지 여러 코루틴을 대기시키고, 조건이 만족되면 하나 이상의 코루틴을 깨우는 데 사용됩니다.
  - **사용 시나리오**: 특정 조건(예: 자원이 사용 가능해짐)을 기다리는 여러 코루틴이 있을 때 유용합니다. 조건이 만족되면 조건 변수에 의해 대기 중인 코루틴들이 작업을 재개합니다.

- **fiber_event** (이벤트):
  - 특정 **이벤트**가 발생하면 대기 중인 코루틴을 깨우는 역할을 합니다. 이벤트는 특정 조건의 충족을 의미할 수도 있지만, 보다 일반적으로 사용됩니다.
  - **이벤트**는 단일한 이벤트 발생 시, 대기 중인 코루틴에게 신호를 보내어 즉시 작업을 재개하게 합니다.
  - **사용 시나리오**: 특정 이벤트(예: I/O 작업 완료, 타이머 만료 등)를 기다리는 코루틴이 있을 때 유용합니다. 이벤트 발생 시 이벤트 객체에 의해 대기 중인 코루틴들이 작업을 재개합니다.



<br>



### fiber_mutex_stat
- **뮤텍스의 상태를 추적하여 데드락(교착상태)를 감지하는 객체**

- **사용 예시**: 데드락 발생 시 진단 및 상태 출력

- **구현**
```cpp
void fiber_mutex_stat_tracker::add_mutex(acl::fiber_mutex* mtx) {
    // 추적 중인 뮤텍스 추가
}

void fiber_mutex_stat_tracker::lock_mutex(acl::fiber_mutex* mtx, int fiber_id) {
    // 뮤텍스 잠금 기록
}

void fiber_mutex_stat_tracker::unlock_mutex(acl::fiber_mutex* mtx, int fiber_id) {
    // 뮤텍스 해제 기록
}

void fiber_mutex_stat_tracker::deadlock_check() {
    // 데드락 발생 여부 확인
}
```

- **코드 예시**
```cpp
acl::fiber_mutex mutex;
fiber_mutex_stat_tracker tracker;
tracker.add_mutex(&mutex);

tracker.lock_mutex(&mutex, 1);
// 잠금 작업
tracker.unlock_mutex(&mutex, 1);

tracker.deadlock_check();  // 데드락 여부 확인
```

- **관련 파일**: [🖱️Acl 소스코드 : fiber_mutex_stat](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_mutex_stat.cpp)

<br>



### event_mutex
- **이벤트 뮤텍스 객체**
- 코루틴 및 스레드 간의 상호 배제를 보장하며 이벤트를 동기화하는 메커니즘
  
- **코드 예시**

  ```cpp
  acl::event_mutex mutex;
  mutex.lock();   // 자원을 잠금
  mutex.unlock(); // 자원을 해제
  ```

<br>



### event_timer
- **이벤트 타이머 객체**
- 특정 시간 이후에 트리거되는 타이머 이벤트를 처리하기 위해 사용됩니다.
  
- **구현**: 일반적으로 `timer` 클래스와 함께 사용되며, 이벤트 루프와 통합되어 실행됩니다.

- **코드 예시**
  ```cpp
  acl::event_timer timer;
  timer.schedule(1000, [] {
      // 1초 후 실행할 코드
  });
  ```

- **관련 파일**: [🖱️Acl 소스코드 : event_timer](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/event_timer.cpp) 

<br>





------

# 실습 코드 설명


### fiber_mutex_ex.cpp

```cpp
#include "pch.h"

// 공유 자원
int shared_resource = 0;
acl::fiber_mutex fmutex;

// 코루틴 함수
void test_fiber_mutex(int fiber_id) {
    for (int i = 0; i < 10; ++i) {
        fmutex.lock(); // mutex lock 주석 시 -> 최종 값이 50
        std::cout << "Mutex locked by [fiber ID: " << fiber_id << "] at iteration " << i << std::endl;
        int temp = shared_resource;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 경합 유발
        shared_resource = temp + 1;
        std::cout << "Fiber ID " << fiber_id << " accessed resource: " << shared_resource << " at iteration " << i << std::endl;
        fmutex.unlock(); // mutex lock 주석 시 -> 최종 값이 50
        std::cout << "Mutex unlocked by [fiber ID: " << fiber_id << "] at iteration " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
    }
}

int main_fmutex() {
    acl::acl_cpp_init();
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(thread_func, i);  // 각 스레드에 코루틴 실행 함수 할당
    }
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();  // 스레드 종료 대기
        }
    }
    std::cout << "Final shared resource value: " << shared_resource << std::endl; // 정상적인 경우 250
    return 0;
}
```

**설명**
- 이 코드는 `fiber_mutex`를 이용하여 상호 배제 기능을 구현합니다. 
- 코루틴이 자원을 안전하게 접근하도록 보장하며, 경합을 방지합니다.

<br>


### fiber_lock_ex.cpp

```cpp
#include "pch.h"
#include <random>

// 공유

 자원
int shared_resource_flock = 0;
acl::fiber_lock lock;  // fiber_lock 사용

// 자원에 접근하는 코루틴
void lock_fiber(int fiber_id) {
    for (int i = 0; i < 10; ++i) {
        lock.lock();  // fiber_lock 활성화
        std::cout << "Fiber " << fiber_id << " locked the resource at iteration " << i << std::endl;
        shared_resource_flock++;
        std::cout << "Fiber " << fiber_id << " accessed resource: " << shared_resource_flock << std::endl;
        lock.unlock();  // fiber_lock 해제
        std::cout << "Fiber " << fiber_id << " unlocked the resource at iteration " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main_flock() {
    acl::acl_cpp_init();
    std::vector<std::thread> threads; // 단일 스레드로 진행!
    for (int i = 0; i < 1; ++i) { 
        threads.emplace_back(thread_func, i);  // 각 스레드에 코루틴 실행 함수 할당
    }
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();  // 스레드 종료 대기
        }
    }
    std::cout << "Final shared resource value: " << shared_resource_flock << std::endl; // 정상 50
    return 0;
}
```

**설명**
- 이 코드는 `fiber_lock` 객체를 이용하여 간단한 상호 배제를 구현합니다.
- 여러 코루틴이 동일한 자원에 접근할 때, `fiber_lock`을 사용하여 자원의 동시 접근을 방지합니다.


<br>


### fiber_rwlock_ex.cpp

```cpp
#include "pch.h"

// 읽기-쓰기 락 객체
acl::fiber_rwlock rwlock;
int shared_resource_rwlock = 0;

void reader_fiber(int fiber_id) {
    for (int i = 0; i < 5; ++i) {
        rwlock.rlock();  // 읽기 락
        std::cout << "Reader Fiber " << fiber_id << " read the resource: " << shared_resource_rwlock << " at iteration " << i << std::endl;
        rwlock.runlock(); // 읽기 락 해제
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void writer_fiber(int fiber_id) {
    for (int i = 0; i < 5; ++i) {
        rwlock.wlock();  // 쓰기 락
        shared_resource_rwlock++;
        std::cout << "Writer Fiber " << fiber_id << " updated resource to: " << shared_resource_rwlock << " at iteration " << i << std::endl;
        rwlock.wunlock(); // 쓰기 락 해제
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main_rw() {
    acl::acl_cpp_init();
    go[&]() { reader_fiber(1); };
    go[&]() { reader_fiber(2); };
    go[&]() { writer_fiber(3); };
    acl::fiber::schedule();
    std::cout << "Final shared resource value: " << shared_resource_rwlock << std::endl;
    return 0;
}
```

**설명**
- 이 코드는 `fiber_rwlock`을 사용하여 읽기-쓰기 락을 구현합니다.
- 읽기 작업은 동시에 여러 코루틴에서 수행할 수 있으며, 쓰기 작업은 하나의 코루틴만 수행할 수 있습니다.

<br>

### fiber_sem_ex.cpp

```cpp
#include "pch.h"

// 세마포어 객체
acl::fiber_sem sem(2);  // 최대 두 개의 코루틴만 접근 가능
int shared_resource_sem = 0;

void sem_fiber(int fiber_id) {
    for (int i = 0; i < 5; ++i) {
        sem.wait();  // 세마포어 대기
        std::cout << "Fiber " << fiber_id << " accessed the resource at iteration " << i << std::endl;
        shared_resource_sem++;
        std::cout << "Fiber " << fiber_id << " updated resource to: " << shared_resource_sem << std::endl;
        sem.post();  // 세마포어

 해제
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main_sem() {
    acl::acl_cpp_init();
    for (int i = 0; i < 3; i++) {
        go[&]() { sem_fiber(i+1); };
    }
    acl::fiber::schedule();
    std::cout << "Final shared resource value: " << shared_resource_sem << std::endl;
    return 0;
}
```

**설명**
- 이 코드는 `fiber_sem`을 사용하여 세마포어 기반의 동기화를 구현합니다.
- 제한된 자원에 대해 두 개의 코루틴만 접근할 수 있도록 관리합니다.

<br>


### fiber_event_ex.cpp

```cpp
#include "pch.h"

// 공유 자원
int shared_resource_for_fevent = 0;
acl::fiber_mutex mutex;
acl::fiber_event event;  // fiber_event 사용

// 이벤트를 기다리는 코루틴
void event_waiting_fiber(int fiber_id) {
    std::cout << "Fiber " << fiber_id << " is waiting for an event...\n";
    event.wait();  // 이벤트 발생을 기다림
    mutex.lock();
    shared_resource_for_fevent++;
    std::cout << "Fiber " << fiber_id << " processed event, shared resource: " << shared_resource_for_fevent << std::endl;
    mutex.unlock();
}

// 이벤트를 발생시키는 코루틴
void event_notifier_fiber(int fiber_id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 기다림
    std::cout << "Fiber " << fiber_id << " is notifying the event...\n";
    event.notify();  // 이벤트 발생
}

int main_fevent() {
    acl::acl_cpp_init();
    go[&]() { event_waiting_fiber(1); };
    go[&]() { event_notifier_fiber(2); };
    acl::fiber::schedule();
    std::cout << "Final shared resource value: " << shared_resource_for_fevent << std::endl;
    return 0;
}
```

**설명**
- 이 코드는 `fiber_event` 객체를 이용하여 코루틴 간 이벤트 기반 동기화를 구현합니다.
- 하나의 코루틴이 이벤트를 발생시키면, 대기 중이던 다른 코루틴이 실행됩니다.

<br>

### fiber_cond_ex.cpp

```cpp
#include "pch.h"
#include <signal.h>

// 공유 자원
int shared_resource_for_fcond = 0;
acl::fiber_mutex mutex_cond;
acl::fiber_cond cond;  // fiber_cond 사용

// 조건을 기다리는 코루틴
void condition_waiting_fiber(int fiber_id) {
    std::cout << "Fiber " << fiber_id << " is waiting for condition...\n";
    mutex_cond.lock();
    cond.wait(mutex_cond);  // 조건 충족을 기다림
    std::cout << "Fiber " << fiber_id << " condition met, shared resource: " << shared_resource_for_fcond << std::endl;
    mutex_cond.unlock();
}

// 조건을 만족시키는 코루틴
void condition_notifier_fiber(int fiber_id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 기다림
    std::cout << "Fiber " << fiber_id << " is signaling the condition...\n";
    mutex_cond.lock();
    shared_resource_for_fcond++;
    cond.notify();  // 조건 충족 시그널 전송
    mutex_cond.unlock();
}

int main_fcond() {
    acl::acl_cpp_init();
    go[&]() { condition_waiting_fiber(1); };
    go[&]() { condition_notifier_fiber(2); };
    acl::fiber::schedule();
    std::cout << "Final shared resource value: " << shared_resource_for_fcond << std::endl;
    return 0;
}
```

**설명**
- 이 코드는 `fiber_cond` 객체를 이용하여 코루틴 간 조건 변수를 사용해 동기화를 구현합니다.
- 하나의 코루틴이 조건을 만족할 때까지 대기하고, 다른 코루틴이 조건을 만족시키면 대기 중이던 코루틴이 실행됩니다.


<br>


### fiber_mutex_stat_ex.cpp

```cpp
#include "pch.h"
#include <unordered_map>
#include <mutex>
#include <chrono>

// 데드락 상태 추적을 위한 사용자 정의 클래스
class fiber_mutex_stat_tracker {
public:
    void add_mutex(acl::fiber_mutex* mtx) {
        std::lock_guard<std::mutex> lock(mutex_);
        tracked_mutexes_[mtx] = false;  // 뮤텍스가 잠금 상태인지 추적
    }
    void lock_mutex(acl::fiber_mutex* mtx, int fiber_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        tracked_mutexes_[mtx] = true;  // 뮤텍스가 잠금되었음을 기록
        std::cout << "Fiber " << fiber_id << " locked the mutex.\n";
    }
    void unlock_mutex(acl::fiber_mutex* mtx, int fiber_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        tracked_mutexes_[mtx] = false;  // 뮤텍스가 해제되었음을 기록
        std::cout << "Fiber " << fiber_id << " unlocked the mutex.\n";
    }
    void deadlock_check() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& pair : tracked_mutexes_) {
            if (pair.second) {
                std::cout << "Deadlock detected! A mutex is still locked.\n";
            }
        }
    }
private:
    std::mutex mutex_;
    std::unordered_map<acl::fiber_mutex*, bool> tracked_mutexes_;  // 뮤텍스 잠금 상태 추적
};

acl::fiber_mutex mutex_ms;
fiber_mutex_stat_tracker mutex_stat;

void deadlock_fiber1(int fiber_id) {
    mutex_ms.lock();
    mutex_stat.lock_mutex(&mutex_ms, fiber_id);
    std::this_thread::sleep_for(std::chrono::seconds(1));  // 지연으로 데드락 유발 가능성
    mutex_stat.unlock_mutex(&mutex_ms, fiber_id);
    mutex_ms.unlock();
}

void deadlock_fiber2(int fiber_id) {
    mutex_ms.lock();
    mutex_stat.lock_mutex(&mutex_ms, fiber_id);
    std::this_thread::sleep_for(std::chrono::seconds(1));  // 지연으로 데드락 유발 가능성
    mutex_stat.unlock_mutex(&mutex_ms, fiber_id);
    mutex_ms.unlock();
}

int main_ms() {
    acl::acl_cpp_init();
    mutex_stat.add_mutex(&mutex_ms);
    go[&]() { deadlock_fiber1(1); };
    go[&]() { deadlock_fiber2(2); };
    acl::fiber::schedule();
    mutex_stat.deadlock_check();  // 데드락 여부 확인
    return 0;
}
```

**설명**
- 이 코드는 `fiber_mutex_stat`를 이용하여 데드락을 감지합니다.
- 특정 뮤텍스가 잠금 상태로 유지될 경우 데드락이 발생할 수 있음을 경고합니다.


<br>


### main.cpp

```cpp
#include <iostream>
#include <string>

// 각 동기화 객체의 main 함수 선언
int main_fcond();   // fiber_cond 예제 함수
int main_fevent();  // fiber_event 예제 함수
int main_flock();   // fiber_lock 예제 함수
int main_fmutex();  // fiber_mutex 예제 함수
int main_sem();     // fiber_sem 예제 함수
int main_rw();      // fiber_rwlock 예제 함수
int main_ms();      // fiber_mutex_stat 예제 함수

int main() {
    std::string input;

    while (true) {
        std::cout << "Select the synchronization object to test:\n";
        std::cout << "1. fiber_cond\n";
        std::cout << "2. fiber_event\n";
        std::cout << "3. fiber_lock\n";
        std::cout << "4. fiber_mutex\n";
        std::cout << "5. fiber_sem\n";
        std::cout << "6. fiber_rwlock\n";
        std::cout << "7. fiber_mutex_stat (deadlock check)\n";
        std::cout << "Enter your choice (or 'q' to quit):  ";
        std::cin >> input;

        if (input == "1") {
            main_fcond();
        }
        else if (input == "2") {
            main_fevent();
        }
        else if (input == "3") {
            main_flock();
        }
        else if (input == "4") {
            main_fmutex();
        }
        else if (input == "5") {
            main_sem();
        }
        else if (input == "6") {
            main_rw();
        }
        else if (input == "7") {
            main_ms();
        }
        else if (input == "q") {
            std::cout << "Exiting..." << std::endl;
            break;
        }
        else {
            std::cout << "Invalid choice! Please try again." << std::endl;
        }
    }
}
```

**설명**
- 이 코드는 사용자가 콘솔에서 입력한 값에 따라 각 동기화 객체의 테스트 코드를 실행하도록 구성되어 있습니다.
- 선택된 동기화 객체의 동작을 이해하고 실습할 수 있도록 도와줍니다.

---



