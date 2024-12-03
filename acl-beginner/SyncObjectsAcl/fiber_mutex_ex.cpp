#include "pch.h"
#include <print>

// 공유 자원
int shared_resource = 0;
acl::fiber_mutex fmutex;

// 코루틴 함수
void test_fiber_mutex(int fiber_id) {
    for (int i = 0; i < 10; ++i) {
        fmutex.lock(); // mutex lock 주석 시 -> 최종 값이 50
        std::print("Mutex locked by [fiber ID: {}] at iteration {}\n", fiber_id, i);

        // 공유 자원에 접근
        int temp = shared_resource;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 경합 유발
        shared_resource = temp + 1;

        std::print("Fiber ID {} accessed resource: {} at iteration {}\n", fiber_id, shared_resource, i);

        fmutex.unlock(); // mutex lock 주석 시 -> 최종 값이 50
        std::print("Mutex unlocked by [fiber ID: {}] at iteration {}\n", fiber_id, i);

        std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
    }
}

// 각 스레드에서 코루틴을 실행하는 함수
void thread_func(int thread_id) {
    for (int i = 1; i <= 5; ++i) {   // 5개의 코루틴 생성
        go[i]() {
            test_fiber_mutex(i);
        };
    }

    acl::fiber::schedule();
}

int main_fmutex() {
    // ACL 초기화
    acl::acl_cpp_init();

    // 5개의 스레드 생성
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(thread_func, i);  // 각 스레드에 코루틴 실행 함수 할당
    }

    // 모든 스레드가 종료될 때까지 대기
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();  // 스레드 종료 대기
        }
    }

    std::print("Final shared resource value: {}", shared_resource); // 정상적인 경우 250

    return 0;
}