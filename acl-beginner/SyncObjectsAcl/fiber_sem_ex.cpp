#include "pch.h"
#include <print>

// 세마포어 객체
acl::fiber_sem sem(2);  // 최대 두 개의 코루틴만 접근 가능
int shared_resource_sem = 0;

void sem_fiber(int fiber_id) {
    for (int i = 0; i < 5; ++i) {
        sem.wait();  // 세마포어 대기
        std::print("Fiber {} accessed the resource at iteration {}\n", fiber_id, i);

        shared_resource_sem++;
        std::print("Fiber {} updated resource to: {}\n", fiber_id, shared_resource_sem);

        sem.post();  // 세마포어 해제
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main_sem() {
    acl::acl_cpp_init();

    // 3개의 코루틴 생성
    for (int i = 0; i < 3; i++) {
        go[&]() {
            sem_fiber(i+1);
        };
    }
    
    acl::fiber::schedule();
    std::print("Final shared resource value: {}\n", shared_resource_sem);
    return 0;
}
