# 동기화 객체(Synchronization Object) 개념

## 목차

1. [**동기화의 개요**](#1-동기화의-개요)
    - 동기화란 무엇인가?
    - 동기화가 필요한 이유
    - 동기화의 주요 개념

2. [**유저모드 동기화 vs 커널모드 동기화**](#2-유저모드-동기화-vs-커널모드-동기화)
    - 유저모드 동기화
    - 커널모드 동기화
    - 성능 차이 및 선택 기준

3. [**동기화 객체의 종류**](#3-동기화-객체의-종류)
    - Critical Section(크리티컬 섹션)
    - Spinlock(스핀락)
    - Read-Write Lock(읽기-쓰기 잠금)
      
    - Mutex(뮤텍스)
        - 기본 뮤텍스
        - 이름 있는 뮤텍스
    - Semaphore(세마포어)
    - Event(이벤트)
    - Condition Variable(조건 변수)

4. [**각 동기화 객체의 C++ 예제**](#4-각-동기화-객체의-상세-설명-및-예제)
    - Critical Section
    - Mutex
    - Semaphore
    - Spinlock
    - Read-Write Lock
    - Event
    - Condition Variable

5. [**ACL에서 제공하는 동기화 객체**](#5-Acl에서-제공하는-동기화-객체)


6. [**동기화 객체 선택 기준**](#5-동기화-객체-선택-기준)
    - 성능 요구 사항
    - 코드 복잡도
    - 교착 상태와의 관계
    - 실시간 시스템에서의 사용

7. [**교착 상태(Deadlock)와 동기화**](#6-교착-상태(Deadlock)와-동기화)
    - 교착 상태란 무엇인가?
    - 교착 상태가 발생하는 이유
    - 교착 상태를 피하는 방법
    - 교착 상태 방지와 해제 기법

8. [**효율적인 동기화 전략**](#7-효율적인-동기화-전략)
    - 최소한의 잠금 전략
    - 동기화 비용 줄이기
    - 컨텍스트 스위칭 최소화
    - 비동기 프로그래밍 기법과의 비교

---

### 1. **동기화의 개요**

#### 동기화란 무엇인가?
동기화는 여러 스레드 또는 프로세스가 **동시**에 같은 자원에 접근하는 상황에서 그 **자원의 일관성을 보장**하는 기술입니다. <br>
동기화는 스레드나 프로세스 간의 **작업 순서를 조율**하고, **데이터 경합을 피하면서** <br> 
작업을 안전하게 수행할 수 있도록 하는 메커니즘입니다. <br>

#### 동기화가 필요한 이유
동기화가 필요한 이유는 **데이터 경합**을 방지하기 위해서입니다. <br>
여러 스레드가 동시에 공유 자원에 접근할 때, 서로의 작업이 겹치면 의도하지 않은 결과가 발생할 수 있습니다. <br>
동기화는 이러한 상황을 방지하기 위해 존재합니다. <br>

#### 동기화의 주요 개념
- **상호 배제(Mutual Exclusion)** : 동시에 하나의 스레드만 자원에 접근할 수 있도록 보장.
- **교착 상태(Deadlock)** : 두 개 이상의 스레드가 서로의 작업이 끝나기를 기다리면서 발생하는 상태.
- **경쟁 상태(Race Condition)** : 여러 스레드가 동시에 자원에 접근하여 비일관성 있는 결과를 초래하는 상태.

---

### 2. **유저모드 동기화 vs 커널모드 동기화**

#### 유저모드 동기화
유저모드 동기화는 커널로 전환하지 않고 유저 영역에서 동기화를 처리하는 방식입니다.  <br>
주로 빠른 성능을 제공하지만 기능이 제한적입니다. <br>
**크리티컬 섹션(Critical Section)**, **스핀락(Spinlock)**, **SRWLock(Read-Write Lock)** 과 같은 기법이 이에 해당합니다. <br>

- **장점**: 컨텍스트 스위칭이 발생하지 않으므로 빠름.
- **단점**: 동일한 프로세스 내에서만 사용 가능. 다중 프로세스 간 동기화는 불가능.

<br>

#### 커널모드 동기화
커널모드 동기화는 커널로 전환하여 동기화를 처리하는 방식으로, <br>
유저모드 동기화보다 속도는 느리지만 더 다양한 기능을 사용할 수 있습니다. <br>
**뮤텍스(Mutex)**, **세마포어(Semaphore)**, **이벤트(Event)** 와 같은 객체들이 이에 해당합니다.  <br>
프로세스 간 자원 공유 시 주로 사용됩니다. <br>

- **장점**: 다중 프로세스 간 자원 공유가 가능.
- **단점**: 컨텍스트 스위칭이 발생하므로 성능 저하 가능성.

<br>

#### 성능 차이 및 선택 기준
- **성능 차이**: 유저모드 동기화는 커널모드 동기화보다 빠릅니다. 하지만, 프로세스 간 동기화가 필요할 때는 커널모드 동기화를 사용해야 합니다.
- **선택 기준**: 성능이 중요한 경우 유저모드 동기화 사용. 다중 프로세스 동기화가 필요하면 커널모드 동기화를 선택.

---

### 3. **동기화 객체의 종류**

#### Critical Section(크리티컬 섹션)
- **정의**: 유저모드에서 제공되는 상호 배제 기법으로, 하나의 스레드만 특정 코드 블록을 실행할 수 있도록 합니다. <br>
  + 이때! 임계구역의 의미인 "크리티컬 섹션"과 헷갈리면 안됩니다 <br>
                         : 한 순간에 하나의 스레드만 접근이 요구되는 공유 자원에 접근하는 코드 영역을 뜻합니다.   <br>
- 뮤텍스처럼 락을 걸고 임계 영역을 진입하며 공유 자원을 사용한 후 다시 반납하는 형식입니다.
- 즉, 특정 임계영역에 대한 키(Critical Section Object)를 가져야만 접근 가능하도록 하는 것으로 <br>
  먼저 접근한 스레드는 락을 획득하고 그 이후 접근하는 스레드는 대기시키는 방식입니다. <br> (이때 스레드 교환 시 컨텍스트 스위칭 비용 발생)
- **특징**: 동일 프로세스 내에서만 사용 가능, 빠른 성능.

<br>

#### Spinlock(스핀락)
- **정의**: 임계 구역에 진입이 불가능할 때, 진입이 가능할 때까지 루프를 돌면서 재시도하는 방식으로 구현된 락
- 잠금을 대기할 때 스레드를 블록하지(재우지) 않고 계속 확인하는 방식입니다. **빠르게 잠금을 해제**할 수 있는 경우에 **유용**합니다.
- OS의 스케줄링 지원을 받지 않기 때문에 스레드에 대한 컨텍스트 스위칭 비용이 발생하지 않습니다.
- 따라서 임계 구역에 빠르게 진입할 수 있는 경우 컨텍스트 스위칭을 제거할 수 있어 효율적이지만, <br>
  만약 스핀락이 오랜 시간을 소요한다면 다른 스레드를 실행하지 못하고 대기하게되어 비효율적인 결과를 가져올 수 있습니다.


<br>


#### Read-Write Lock(읽기-쓰기 잠금)
- **정의**: 읽기 작업과 쓰기 작업을 구분하여, 다수의 스레드가 동시에 읽기를 수행할 수 있지만 쓰기는 하나의 스레드만 가능합니다.
- 만약, Write가 거의 일어나지 않고 Read가 대부분이라면 멀티 쓰레드가 공유 자원을 읽고 쓴다면 <br>
  excusive lock을 사용하는 Critical Section이나 Mutex는 병목이 될 수 있기 때문에..

<br><br>


#### Mutex(뮤텍스)
- 하나의 스레드/프로세스에 의해 소유될 수 있는 Key를 기반으로 한 상호배제 기법
- 동기화 대상이 1개
- **기본 뮤텍스**: 커널모드에서 제공되는 동기화 객체로, 하나의 스레드만 자원에 접근할 수 있도록 보장합니다.
- **이름 있는 뮤텍스(Named Mutex)**: 프로세스 간 공유가 가능하며, 여러 프로세스에서 같은 이름을 가진 뮤텍스를 사용해 자원을 보호할 수 있습니다.

<br>


#### Semaphore(세마포어)
- 현재 공유자원에 접근할 수 있는 스레드/프로세스의 수를 나타내는 값을 두어 상호배제를 달성하는 기법
- 동기화 대상이 여러개(1개 이상)
- **정의**: 일정 수의 스레드만 자원에 접근할 수 있도록 제어하는 동기화 객체입니다.

<br>



#### Event(이벤트)
- **정의**: 특정 사건이 발생할 때 여러 스레드에게 알리는 방식으로 동작합니다. 주로 스레드 간의 비동기적 작업을 조율할 때 사용됩니다.
- 즉 이벤트 객체는 실행 순서를 동기화할 때 주로 사용합니다.
- 스레드를 함수 내부에서 이벤트 객체로 블락하고 signaled 상태가 되었을 때 실행 시작할 수 있도록 합니다.
  

<br>


#### Condition Variable(조건 변수)
- **정의**: 하나 이상의 스레드가 특정 조건이 충족될 때까지 대기하고, 조건이 충족되면 대기 중인 스레드를 깨우는 방식으로 동작합니다.
- 따라서 condition variable를 사용하면, 멀티 스레드 간 동기화를 구현할 수 있습니다.
- 그러나 멀티 스레드 간 안정적인 동기화 구현을 위해서는 condition variable을 사용 외에도 <br>
  packaged task나 async와 같이 c++에서 제공하는 task를 이용하는 것도 좋다고 합니다.


<br>

---

### 4. **각 동기화 객체의 상세 설명 및 예제**

#### Critical Section
- **정의**: 동일한 프로세스 내에서 하나의 스레드만 자원에 접근할 수 있도록 보호하는 유저모드 동기화 객체.
- **예제**:
```cpp
#include <windows.h>
#include <iostream>

CRITICAL_SECTION cs;

void thread_func() {
    EnterCriticalSection(&cs);
    // 자원 접근 코드
    std::cout << "Thread is accessing the resource" << std::endl;
    LeaveCriticalSection(&cs);
}

int main() {
    InitializeCriticalSection(&cs);
    // 스레드 생성 및 자원 접근
    DeleteCriticalSection(&cs);
    return 0;
}
```
- **장점**: 빠르고 효율적이지만, 동일 프로세스 내에서만 사용 가능.

#### Mutex
- **정의**: 커널모드 동기화 객체로, 프로세스 간 자원 공유에 사용됩니다.
- **이름 있는 뮤텍스 예제**:
```cpp
#include <windows.h>
#include <iostream>

HANDLE mutex;

void thread_func() {
    WaitForSingleObject(mutex, INFINITE);
    // 자원 접근 코드
    std::cout << "Thread is accessing the resource" << std::endl;
    ReleaseMutex(mutex);
}

int main() {
    mutex = CreateMutex(NULL, FALSE, "MyMutex");
    // 스레드 생성 및 자원 접근
    CloseHandle(mutex);
    return 0;
}
```
- **장점**: 프로세스 간 자원 보호 가능.

#### Semaphore
- **예제**:
```cpp
#include <windows.h>
#include <iostream>

HANDLE sem;

void thread_func() {
    WaitForSingleObject(sem, INFINITE);
    // 자원 접근
    std::cout << "Thread is accessing the resource" << std::endl;
    ReleaseSemaphore(sem, 1, NULL);
}

int main() {
    sem = CreateSemaphore(NULL, 2, 2, NULL);
    // 스레드 생성
    CloseHandle(sem);
    return 0;
}
```

---


### 5. **ACL에서 제공하는 동기화 객체**


ACL은 **비동기 프로그래밍** 및 **코루틴 기반**의 프로그램에서 사용할 수 있는 다양한 동기화 객체들을 제공합니다. 
이 동기화 객체들은 코루틴 간 자원 경합을 방지하고, 안전하고 효율적인 동기화를 구현하는 데 사용됩니다. 
아래는 ACL에서 제공하는 주요 동기화 객체들입니다.

#### 5.1 **`fiber_mutex`**
- **설명**: 코루틴 전용 뮤텍스 객체로, 하나의 코루틴이 자원에 접근하는 동안 다른 코루틴이 해당 자원에 접근하지 못하도록 상호 배제를 보장합니다.
- **사용 시나리오**: 코루틴 간 자원 경합을 방지하기 위한 상호 배제가 필요할 때 사용됩니다.
- **코드 예시**:
  ```cpp
  acl::fiber_mutex mutex;
  mutex.lock();
  // 자원 접근
  mutex.unlock();
  ```

#### 5.2 **`fiber_event`**
- **설명**: 코루틴 간 또는 스레드 간에 신호를 전달하여 특정 조건이 발생했을 때 작업을 트리거하는 이벤트 객체입니다.
- **사용 시나리오**: 한 코루틴이 특정 조건을 만족할 때 다른 코루틴에게 신호를 보내어 작업을 트리거하거나 대기 중인 코루틴을 깨울 때 사용됩니다.
- **코드 예시**:
  ```cpp
  acl::fiber_event event;
  event.wait();   // 다른 코루틴이 이벤트를 발생시킬 때까지 대기
  event.notify(); // 이벤트 발생
  ```

#### 5.3 **`fiber_sem` (세마포어)**
- **설명**: 특정 자원에 대해 접근 가능한 코루틴의 수를 제한하는 세마포어 객체입니다. 다수의 코루틴이 자원에 접근하는 것을 제어합니다.
- **사용 시나리오**: 특정 자원에 대한 동시 접근을 제한하고자 할 때 사용됩니다.
- **코드 예시**:
  ```cpp
  acl::fiber_sem sem(1);  // 세마포어 초기화
  sem.wait();   // 세마포어를 대기
  sem.post();   // 세마포어 해제
  ```

#### 5.4 **`fiber_lock`**
- **설명**: 코루틴 및 스레드 간 상호 배제를 보장하는 잠금 객체로, 일반적인 락 메커니즘을 제공하며, 코루틴 간 자원 접근을 제어합니다.
- **사용 시나리오**: 다수의 코루틴이 동시에 자원을 접근하는 것을 방지하기 위한 상호 배제 메커니즘입니다.
- **코드 예시**:
  ```cpp
  acl::fiber_lock lock;
  lock.lock();   // 락 획득
  // 자원 접근
  lock.unlock(); // 락 해제
  ```

#### 5.5 **`fiber_cond` (조건 변수)**
- **설명**: 특정 조건을 만족할 때까지 대기하는 코루틴을 깨우기 위해 사용되는 조건 변수 객체입니다.
- **사용 시나리오**: 특정 조건이 만족되었을 때 대기 중인 코루틴에게 신호를 보내 작업을 계속하게 하는데 사용됩니다.
- **코드 예시**:
  ```cpp
  acl::fiber_cond cond;
  cond.wait();   // 조건이 만족될 때까지 대기
  cond.signal(); // 조건이 만족되었음을 신호
  ```

#### 5.6 **`event_mutex`**
- **설명**: 코루틴 기반의 이벤트 뮤텍스 객체로, 코루틴 및 스레드 간의 상호 배제를 보장하면서 이벤트를 동기화하는 메커니즘을 제공합니다.
- **사용 시나리오**: 자원의 상호 배제와 함께 이벤트 신호를 사용하여 작업을 동기화할 때 사용됩니다.
- **코드 예시**:
  ```cpp
  acl::event_mutex mutex;
  mutex.lock();   // 자원을 잠금
  mutex.unlock(); // 자원을 해제
  ```

---

### 6. **각 ACL 동기화 객체의 상세 설명 및 예제**

#### **1. Mutex 계열 (뮤텍스)**

#### **1.1. fiber_mutex**
- **설명**: `fiber_mutex`는 하나의 코루틴만 자원에 접근할 수 있도록 보장하는 상호 배제 객체입니다. 일반적인 뮤텍스처럼 사용되며, 자원에 대한 독점적 접근을 관리합니다.
- **기능**: 자원에 대한 락/언락, 시도한 락 획득.
- **사용 예시**: 코루틴 간 공유 자원 접근 제어.
- **코드 예시**:
  
  ```cpp
  bool fiber_mutex::lock(void) { acl_fiber_mutex_lock(mutex_); return true; }
  bool fiber_mutex::unlock(void) { acl_fiber_mutex_unlock(mutex_); return true; }
  ```

- **관련 파일**: [🖱️Acl 소스코드 : fiber_mutex](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_mutex.cpp)
- [📄더 자세한 fiber_mutex 사용법이 궁금해!](./Coroutine-fiber_mutex.md)

---

#### **2. Condition Variable 계열 (조건 변수)**

#### **2.1. fiber_cond**
- **설명**: `fiber_cond`는 조건이 만족될 때까지 코루틴을 대기시키고, 조건이 만족되면 해당 코루틴을 깨우는 동기화 객체입니다.
- **기능**: 특정 조건이 충족될 때까지 코루틴 대기, 조건 만족 시 대기 코루틴 깨움.
- **사용 예시**: 특정 이벤트나 조건이 발생할 때까지 코루틴을 일시적으로 중단시킴.
- **코드 예시**:
  ```cpp
  bool fiber_cond::wait(fiber_mutex& mutex, int timeout /* = -1 */) { return acl_fiber_cond_wait(cond_, mutex.get_mutex()) == 0; }
  bool fiber_cond::notify(void) { return acl_fiber_cond_signal(cond_) == 0; }
  ```
- **관련 파일**: [🖱️Acl 소스코드 : fiber_cond](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_cond.cpp)

---

#### **3. Lock 계열 (락)**

#### **3.1. fiber_lock**
- **설명**: `fiber_lock`은 간단한 상호 배제 락으로, 코루틴 간 자원을 보호하기 위한 락입니다.
- **기능**: 자원에 대한 독점적 접근 제어.
- **사용 예시**: 공유 자원에 대한 단순한 보호와 접근 관리.
- **코드 예시**:
  ```cpp
  bool fiber_lock::lock(void) { acl_fiber_lock_lock(lock_); return true; }
  bool fiber_lock::unlock(void) { acl_fiber_lock_unlock(lock_); return true; }
  ```
- **관련 파일**: [🖱️Acl 소스코드 : fiber_lock](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_lock.cpp)

#### **3.2. fiber_rwlock**
- **설명**: `fiber_rwlock`은 읽기-쓰기 락으로, 여러 코루틴이 동시 읽기를 허용하고, 쓰기는 하나의 코루틴만 허용하는 동기화 객체입니다.
- **기능**: 읽기 작업 동시 허용, 쓰기 작업은 단일 코루틴만 수행.
- **사용 예시**: 읽기 작업이 많고, 쓰기 작업이 드물게 발생하는 경우.
- **코드 예시**:
  ```cpp
  void fiber_rwlock::rlock(void) { acl_fiber_rwlock_rlock(rwlk_); }
  void fiber_rwlock::wlock(void) { acl_fiber_rwlock_wlock(rwlk_); }
  ```
- **관련 파일**: [🖱️Acl 소스코드 : fiber_rwlock](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_rwlock.cpp)

---

#### **4. Semaphore 계열 (세마포어)**

#### **4.1. fiber_sem**
- **설명**: `fiber_sem`은 일정 수의 코루틴만 자원에 접근할 수 있도록 제한하는 동기화 객체입니다.
- **기능**: 일정 수의 코루틴만 자원에 접근 가능.
- **사용 예시**: 자원에 대한 접근 횟수를 제한하는 상황에서 사용.
- **코드 예시**:
  ```cpp
  int fiber_sem::wait(void) { /* 세마포어 대기 */ }
  void fiber_sem::post(void) { /* 세마포어 해제 */ }
  ```
- **관련 파일**: [🖱️Acl 소스코드 : fiber_sem](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_sem.cpp)

---

#### **5. Event 계열 (이벤트)**

#### **5.1. fiber_event**
- **설명**: `fiber_event`는 특정 이벤트가 발생했을 때 대기 중인 코루틴을 깨우는 역할을 하는 동기화 객체입니다.
- **기능**: 이벤트 발생 시 대기 중인 코루틴을 깨움.
- **사용 예시**: 비동기 작업을 수행하는 코루틴 간 이벤트를 기반으로 동기화하는 경우.
- **코드 예시**:
```cpp
bool fiber_event::wait(void) { return acl_fiber_event_wait(event_); }
bool fiber_event::notify(void) { return acl_fiber_event_notify(event_); }
```
- **관련 파일**: [🖱️Acl 소스코드 : fiber_event](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_event.cpp)

---

#### **6. 기타 (Stat 관련)**

#### **6.1. fiber_mutex_stat**
- **설명**: `fiber_mutex_stat`는 뮤텍스의 상태를 추적하여 교착 상태를 감지하고 분석하는 도구입니다.
- **기능**: 뮤텍스의 상태를 추적하여 교착 상태 진단.
- **사용 예시**: 교착 상태 발생 시 진단 및 상태 출력.
- **코드 예시**:
```cpp
bool fiber_mutex::deadlock(fiber_mutex_stats& out) { /* 교착 상태 추적 */ }
void fiber_mutex::deadlock_show(void) { /* 교착 상태 출력 */ }
```
- **관련 파일**: [🖱️Acl 소스코드 : fiber_mutex_stat](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_mutex_stat.cpp)




---

### 7. **동기화 객체 선택 기준**

<br>


#### 성능 요구 사항
성능이 중요하면 유저모드 동기화 객체(크리티컬 섹션 등)를 우선적으로 고려합니다.  <br>
성능보다 안정성이 더 중요하면 커널모드 동기화 객체(뮤텍스, 세마포어 등)를 사용합니다. <br>

#### 교착 상태와의 관계
교착 상태는 여러 동기화 객체를 사용할 때 발생할 수 있으므로,  <br>
사용 순서를 잘 정의하거나 교착 상태 예방 기법을 사용해야 합니다.  <br>

#### Acl에서의 선택 기준
- **성능**: 경량 작업에서는 `fiber_mutex`와 `fiber_lock` 같은 유저모드 동기화를 사용하여 성능을 극대화할 수 있습니다. 다중 프로세스 간 동기화가 필요하면 커널모드 동기화 객체인 `Mutex`나 `Semaphore`를 사용합니다.
- **코루틴 기반 시스템**: ACL의 `fiber_*` 동기화 객체들은 경량 코루틴 기반 시스템에서 효율적으로 작동하므로, 코루틴 환경에서는 이를 우선적으로 사용하는 것이 좋습니다.
- **읽기-쓰기 비율**: 읽기 작업이 많고 쓰기 작업이 적다면 `fiber_rwlock` 같은 읽기-쓰기 잠금을 고려합니다.


---

### 8. **교착 상태(Deadlock)와 동기화**

<br>

#### 교착 상태란 무엇인가?
교착 상태는 두 개 이상의 스레드가 서로 자원을 기다리면서 발생하는 상황으로, 프로그램이 멈추게 됩니다.

#### 교착 상태 방지 기법
- **순서 보장**: 모든 스레드가 자원에 접근할 때 동일한 순서로 자원에 접근하도록 설계합니다.
- **타임아웃**: 일정 시간이 지나면 스레드가 대기를 포기하도록 설계합니다.

---



