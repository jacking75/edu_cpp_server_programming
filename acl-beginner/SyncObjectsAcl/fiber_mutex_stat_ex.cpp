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

// 뮤텍스와 사용자 정의 데드락 추적 객체
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

    // 데드락 상태 추적 시작
    mutex_stat.add_mutex(&mutex_ms);

    go[&]() {
        deadlock_fiber1(1);
    };
    go[&]() {
        deadlock_fiber2(2);
    };

    acl::fiber::schedule();

    // 데드락 상태 출력
    mutex_stat.deadlock_check();  // 데드락 여부 확인
    return 0;
}
