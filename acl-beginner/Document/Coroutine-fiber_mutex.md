# 코루틴 동기화와 `fiber_mutex` 사용 가이드

이 문서는 ACL(Advanced C Library)에서 제공하는 `fiber_mutex`를 사용하여 코루틴 기반 환경에서 자원을 안전하게 공유하는 방법을 다룹니다. <br>
코루틴 환경에서 동기화가 필요한 이유와 `fiber_mutex`의 내부 동작 방식, 코드 예제를 통해 코루틴 간 동기화를 설명합니다. <br>

## 목차

1. [코루틴 동기화 소개](#1-코루틴-동기화-소개)
2. [`fiber_mutex`란 무엇인가?](#2-fiber_mutex란-무엇인가)
3. [`fiber_mutex`의 내부 동작 방식](#3-fiber_mutex의-내부-동작-방식)
4. [전통적인 뮤텍스와 `fiber_mutex`의 차이점](#4-전통적인-뮤텍스와-fiber_mutex의-차이점)
5. [왜 코루틴 환경에서 `fiber_mutex`를 사용하는가?](#5-왜-코루틴-환경에서-fiber_mutex를-사용하는가)
6. [코드 예시: `fiber_mutex`를 활용한 코루틴 동기화](#6-코드-예시-fiber_mutex를-활용한-코루틴-동기화)
7. [중요 고려사항](#7-중요-고려사항)
8. [결론](#8-결론)

---

## 1. 코루틴 동기화 소개

코루틴은 협력적 멀티태스킹을 제공하여, 동일한 스레드 내에서 여러 작업을 순차적으로 처리할 수 있습니다. [📄코루틴 문서](./Coroutine.md)  <br><br>

코루틴은 하나의 스레드 내에서 협력적으로 스케줄링되어 실행되기 때문에, 
일반적인 스레드와 달리 명시적인 컨텍스트 전환이 가능합니다.
하지만, 동일한 스레드 내의 여러 코루틴이 동일한 자원에 접근하려 할 때 동기화가 필요할 수 있습니다. <br><br>

예를 들어, 하나의 코루틴이 자원에 쓰기 작업을 하고 있을 때 다른 코루틴이 해당 자원에 접근하려 하면 **데이터 경합**이 발생할 수 있습니다. 
이를 방지하기 위해 코루틴 간의 동기화가 필요합니다. <br><br>

코루틴 환경에서는 전통적인 스레드 동기화 기법을 그대로 사용하는 것이 비효율적일 수 있으며, 이를 위해 ACL은 `fiber_mutex`와 같은 코루틴 전용 동기화 도구를 제공합니다. <br>

---

## 2. `fiber_mutex`란 무엇인가?

fiber_mutex는 **코루틴 간 동기화**를 위해 ACL에서 제공하는 락(mutex)입니다. 
이는 코루틴 환경에 최적화된 동기화 도구로, 코루틴 간에 자원을 안전하게 보호하고 자원 경합을 방지하는 데 사용됩니다. 
fiber_mutex는 코루틴 간 동기화에 특화된 도구이지만, 내부적으로 스레드와 혼합된 환경에서도 사용할 수 있도록 설계되었습니다.


---

## 3. `fiber_mutex`의 내부 동작 방식

`fiber_mutex`는 ACL 라이브러리에서 제공하는 코루틴 동기화 도구로, 코루틴 기반의 멀티태스킹 환경에서 자원 경합을 방지하는 데 사용됩니다. <br>
이 섹션에서는 `fiber_mutex`의 구현 세부 사항을 살펴보고, 어떻게 코루틴 간의 동기화를 관리하는지를 설명합니다.

### `fiber_mutex` 생성자 및 소멸자

[🖱️lib_fiber/cpp/src/fiber_mutex.cpp](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_mutex.cpp)

```cpp
fiber_mutex::fiber_mutex(ACL_FIBER_MUTEX *mutex /* NULL */)
{
	if (mutex) {
		mutex_ = mutex;
		mutex_internal_ = NULL;
	} else {
		mutex_internal_ = mutex_ = acl_fiber_mutex_create(0);
	}
}

fiber_mutex::~fiber_mutex(void)
{
	if (mutex_internal_) {
		acl_fiber_mutex_free(mutex_internal_);
	}
}
```

`fiber_mutex`는 생성 시에 내부적으로 `ACL_FIBER_MUTEX` 구조체를 생성합니다. <br>
만약 외부에서 전달된 `ACL_FIBER_MUTEX`가 있으면 이를 사용하고, 그렇지 않으면 새로운 `ACL_FIBER_MUTEX`를 생성합니다.  <br>
소멸자에서는 내부적으로 생성된 `mutex_internal_`이 있을 경우 이를 해제합니다. <br>

<br>

#### `lock`과 `unlock` 메서드

`fiber_mutex`의 주요 메서드는 `lock()`과 `unlock()`이며, 이를 통해 코루틴 간의 자원 경합을 관리합니다.

```cpp
bool fiber_mutex::lock(void)
{
	acl_fiber_mutex_lock(mutex_);
	return true;
}

bool fiber_mutex::unlock(void)
{
	acl_fiber_mutex_unlock(mutex_);
	return true;
}
```

이 메서드들은 내부적으로 `acl_fiber_mutex_lock` 및 `acl_fiber_mutex_unlock` 함수를 호출합니다. 이 함수들은 `ACL_FIBER_MUTEX` 구조체의 상태를 변경하여 코루틴 간 락을 관리합니다.

#### `acl_fiber_mutex_lock`의 구현

```c
int acl_fiber_mutex_lock(ACL_FIBER_MUTEX *mutex)
{
	int ret;

	if (mutex->flags & FIBER_MUTEX_F_LOCK_TRY) {
		ret = fiber_mutex_lock_try(mutex);
	} else {
		ret = fiber_mutex_lock_once(mutex);
	}

	if (ret == 0) {
		unsigned id = acl_fiber_self();
		long me = id == 0 ? -thread_self() : (long) id;
		mutex->owner = me;

		if (me < 0) {
			thread_waiter_remove(mutex, thread_self());
		}

		mutex->fiber = acl_fiber_running();
	}
	return ret;
}
```

이 함수는 락을 설정하는 핵심 로직입니다. `fiber_mutex_lock_try` 또는 `fiber_mutex_lock_once` 함수를 통해 락을 시도하며, 성공적으로 락이 걸리면 락의 소유자를 설정합니다.

- `mutex->owner`: 락을 소유한 코루틴 또는 스레드를 식별합니다.
- `mutex->fiber`: 현재 락을 소유한 코루틴을 가리킵니다.

`acl_fiber_self()`는 현재 실행 중인 코루틴의 ID를 반환하며, 이 값으로 락의 소유자를 설정합니다. 만약 이 값이 0이라면, 스레드에서 실행 중인 것으로 간주하여 스레드 ID를 사용합니다.

#### `acl_fiber_mutex_unlock`의 구현

```c
int acl_fiber_mutex_unlock(ACL_FIBER_MUTEX *mutex)
{
	ACL_FIBER *fiber;
	int ret;

	pthread_mutex_lock(&mutex->lock);
	fiber = (ACL_FIBER*) array_pop_front(mutex->waiters);

	// 현재 실행 중인 코루틴이 락 소유자인지 확인
	if (acl_fiber_running() != mutex->fiber) {
		msg_error("%s(%d): not the owner fiber=%p, fiber=%p",
			__FUNCTION__, __LINE__, acl_fiber_running(),
			mutex->fiber);
	}

	// 락 해제
	mutex->owner = 0;
	mutex->fiber = NULL;

	ret = pthread_mutex_unlock(&mutex->thread_lock);

	pthread_mutex_unlock(&mutex->lock);

	if (ret != 0) {
		return ret;
	}

	// 대기 중인 코루틴 또는 스레드가 있으면 깨움
	if (fiber) {
		if (thread_self() == fiber->tid) {
			FIBER_READY(fiber);
		} else {
			sync_waiter_wakeup(fiber->sync, fiber);
		}
	}

	return 0;
}
```

락 해제 과정에서, 현재 락을 소유하고 있는 코루틴이 맞는지 확인한 후 락을 해제합니다. 만약 대기 중인 코루틴이나 스레드가 있으면 이를 깨워서 작업을 재개할 수 있도록 합니다.

#### `ACL_FIBER_MUTEX`의 구조

`ACL_FIBER_MUTEX`는 코루틴과 스레드 환경에서 모두 사용할 수 있도록 설계되었습니다.

```c
struct ACL_FIBER_MUTEX {
	RING me;                  // 락을 소유한 코루틴을 관리하는 링 구조체
	long owner;               // 락의 소유자 (코루틴 또는 스레드 ID)
	ACL_FIBER *fiber;         // 락을 소유한 코루틴을 가리키는 포인터
	unsigned flags;           // 락의 상태를 나타내는 플래그
	ARRAY *waiters;           // 대기 중인 코루틴 배열
	ARRAY *waiting_threads;   // 대기 중인 스레드 배열
	pthread_mutex_t lock;     // 내부적인 락을 관리하는 POSIX 뮤텍스
	pthread_mutex_t thread_lock; // 스레드 간 추가 동기화를 위한 POSIX 뮤텍스
};
```


---


## 4. 전통적인 뮤텍스와 `fiber_mutex`의 차이점

전통적인 스레드 기반 뮤텍스는 일반적으로 다중 스레드 환경에서 자원을 보호하는 데 사용됩니다. 하지만, 코루틴 환경에서는 이러한 뮤텍스가 효율적이지 않을 수 있습니다. 

- **전통적인 뮤텍스**: 스레드가 락을 기다리면 스레드는 블록됩니다. 이로 인해 스레드 풀의 크기에 영향을 줄 수 있으며, 전체 시스템 성능에 영향을 미칩니다.
- **fiber_mutex**: 코루틴 전용 락으로, 코루틴이 락을 기다릴 때 스레드가 블록되지 않고 다른 코루틴으로 전환됩니다. 이는 스레드 풀을 유지하면서 코루틴의 동작을 제어할 수 있는 더 효율적인 방법입니다.

---

## 5. 왜 코루틴 환경에서 `fiber_mutex`를 사용하는가?

코루틴은 협력적 멀티태스킹을 제공하므로, 스레드 간 전환보다 가벼운 성능 비용을 가집니다. 
`fiber_mutex`는 이러한 코루틴 환경에 최적화되어 있으며, 스레드와 코루틴이 혼재된 상황에서도 자원 경합을 효과적으로 처리할 수 있습니다.

---

## 6. 코드 예시: `fiber_mutex`를 활용한 코루틴 동기화

아래는 `RoomManager` 클래스에서 `fiber_mutex`를 사용하여 코루틴 간 동기화를 구현한 코드입니다:



```cpp
acl::fiber_mutex room_mutex_;

void RoomManager::EnterRoom(int roomNumber, const std::string& userID, acl::socket_stream* conn) {
    room_mutex_.lock();  // 코루틴 간 자원 보호를 위해 락을 건다

    if (rooms_[roomNumber].size() >= 2) {
        std::string message = "Room is full!";
        conn->write(message.c_str(), message.size());
        room_mutex_.unlock();  // 자원 보호 종료 후 락을 해제한다
        return;
    }

    rooms_[roomNumber][userID] = conn;
    room_mutex_.unlock();  // 자원 보호 종료 후 락을 해제한다
}
```

---

## 7. 중요 고려사항

- **데드락 방지**: 코루틴에서 잘못된 락 사용으로 인해 데드락이 발생할 수 있습니다. 이를 방지하려면 `fiber_mutex`의 사용을 주의 깊게 설계해야 합니다.
- **락의 범위**: 코루틴 간의 자원 경합이 발생할 수 있는 구역에서만 락을 사용하도록 해야 하며, 불필요한 영역에서 락을 사용하면 성능 저하가 발생할 수 있습니다.

---

## 8. 결론

`fiber_mutex`는 코루틴 환경에서 안전한 자원 공유를 가능하게 하는 중요한 도구입니다. 
이 가이드를 통해 `fiber_mutex`의 내부 동작 방식을 이해하고, 이를 사용하여 코루틴 간 자원 경합 문제를 해결하는 방법을 익힐 수 있습니다.

코루틴의 효율성과 동기화를 잘 활용한다면, ACL 라이브러리로 고성능 애플리케이션을 구축할 수 있습니다.

