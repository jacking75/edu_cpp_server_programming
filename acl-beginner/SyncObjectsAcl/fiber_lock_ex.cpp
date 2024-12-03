#include "pch.h"
#include <random>
#include <print>

// 공유 자원
int shared_resource_flock = 0;
acl::fiber_lock lock;  // fiber_lock 사용

// 자원에 접근하는 코루틴
void lock_fiber(int fiber_id) {
    for (int i = 0; i < 10; ++i) {
        lock.lock();  // fiber_lock 활성화
        std::print("Fiber {} locked the resource at iteration {}\n", fiber_id, i);

        shared_resource_flock++;
        std::print("Fiber {} accessed resource: {}\n", fiber_id, shared_resource_flock);

        lock.unlock();  // fiber_lock 해제
        std::print("Fiber {} unlocked the resource at iteration {}\n", fiber_id, i);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


// 각 스레드에서 코루틴을 실행하는 함수
static void thread_func(int thread_id) {
    for (int i = 1; i <= 100; ++i) { 
        go[i]() {
            lock_fiber(i);
        };
    }

    acl::fiber::schedule();
}

int main_flock() {
    // ACL 초기화
    acl::acl_cpp_init();

    std::vector<std::thread> threads; // 단일 스레드로 진행!
    for (int i = 0; i < 1; ++i) { 
        threads.emplace_back(thread_func, i);  // 각 스레드에 코루틴 실행 함수 할당
    }

    // 모든 스레드가 종료될 때까지 대기
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();  // 스레드 종료 대기
        }
    }
    std::print("Final shared resource value: {}\n", shared_resource_flock); // 정상 50

    return 0;
}
