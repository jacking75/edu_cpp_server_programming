# Acl에서 제공하는 동기화 객체

## 개요
- acl_fiber란? [📄Acl fiber의 Coroutine](./Coroutine.md)
- 동기화 객체란? [📄동기화 객체](./Cpp-SynchronizationObject.md)

acl_fiber에서 제공하는 경량 스레드의 일종인 coroutine 사용 시, 동기화를 위해 지원되는 동기화 객체에 대한 문서

------------------------------------------------

## fiber_mutex
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



------------------------------------------------

## fiber_lock
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

### Q: [fiber_lock]에서 fiber_mutex와 다른 점은 무엇인가?

**차이점**:

- **fiber_lock**:
  - **일반적인 락 메커니즘**을 제공하며, 주로 단순한 상호 배제를 위해 사용됩니다.
  - fiber_lock은 상대적으로 더 가볍고, 일반적인 코루틴 간 자원 보호를 위한 락입니다. 
  - **사용 시나리오**: 단순한 상호 배제와 동기화가 필요한 상황에서 사용됩니다. 일반적으로 보호해야 할 자원에 대한 간단한 락/언락 작업에 적합합니다.

- **fiber_mutex**:
  - **뮤텍스(Mutex)**는 더 강력한 상호 배제 메커니즘을 제공하며, 여러 코루틴이 동일한 자원에 동시 접근하려고 할 때 그 자원을 보호합니다.
  - fiber_mutex는 락 상태를 더 세밀하게 관리하고, 데드락 방지 기능이 추가된 경우가 많습니다.
  - **사용 시나리오**: 더 복잡한 동기화 작업이 필요하거나, 자원의 보호가 필수적인 경우에 적합합니다. 코루틴 사이에서의 데드락을 방지하고자 할 때 유용합니다.

**요약**
- fiber_lock은 가벼운 락 메커니즘을 제공하는 반면, fiber_mutex는 더 정교한 상호 배제와 동기화 기능을 제공합니다.
- 간단한 동기화 작업에는 fiber_lock을, 복잡하고 중요한 자원 보호에는 fiber_mutex를 사용하는 것이 일반적입니다.



<br>

------------------------------------------------

## fiber_rwlock 
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

------------------------------------------------

## fiber_sem
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

------------------------------------------------

## fiber_event
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


------------------------------------------------


## fiber_cond
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

### Q: [fiber_cond]에서 fiber_event와 다른 점은 무엇인가?

**차이점**:

- **fiber_cond** (조건 변수):
  - 특정 조건이 **만족될 때까지** 코루틴을 대기시키고, 조건이 만족되면 대기 중인 코루틴을 깨웁니다.
  - **조건 변수**는 주로 특정한 상태나 값이 만족될 때까지 여러 코루틴을 대기시키고, 조건이 만족되면 하나 이상의 코루틴을 깨우는 데 사용됩니다.
  - **사용 시나리오**: 특정 조건(예: 자원이 사용 가능해짐)을 기다리는 여러 코루틴이 있을 때 유용합니다. 조건이 만족되면 조건 변수에 의해 대기 중인 코루틴들이 작업을 재개합니다.

- **fiber_event** (이벤트):
  - 특정 **이벤트**가 발생하면 대기 중인 코루틴을 깨우는 역할을 합니다. 이벤트는 특정 조건의 충족을 의미할 수도 있지만, 보다 일반적으로 사용됩니다.
  - **이벤트**는 단일한 이벤트 발생 시, 대기 중인 코루틴에게 신호를 보내어 즉시 작업을 재개하게 합니다.
  - **사용 시나리오**: 특정 이벤트(예: I/O 작업 완료, 타이머 만료 등)를 기다리는 코루틴이 있을 때 유용합니다. 이벤트 발생 시 이벤트 객체에 의해 대기 중인 코루틴들이 작업을 재개합니다.

**요약**: fiber_cond는 특정 조건이 충족될 때까지 대기시키며, 조건이 만족되면 다수의 코루틴을 깨우는 데 사용됩니다. 반면, fiber_event는 특정 이벤트 발생 시 대기 중인 코루틴을 깨우는 데 사용되며, 보다 일반적인 이벤트 기반 동기화에 적합합니다.


<br>

------------------------------------------------

## fiber_mutex_stat
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

------------------------------------------------

## event_mutex
- **이벤트 뮤텍스 객체**
- 코루틴 및 스레드 간의 상호 배제를 보장하며 이벤트를 동기화하는 메커니즘
  
- **코드 예시**

  ```cpp
  acl::event_mutex mutex;
  mutex.lock();   // 자원을 잠금
  mutex.unlock(); // 자원을 해제
  ```

<br>

------------------------------------------------

## event_timer
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

---

이 문서는 ACL에서 제공하는 다양한 동기화 객체를 설명하고, 각 객체가 어떻게 사용되는지를 코드 예시와 함께 보여줍니다. <br>
이를 통해 코루틴 기반의 비동기 프로그래밍에서 동기화를 구현할 때 필요한 정보를 얻을 수 있습니다.
