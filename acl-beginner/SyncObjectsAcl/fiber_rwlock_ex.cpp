#include "pch.h"
#include <print>

// 읽기-쓰기 락 객체
acl::fiber_rwlock rwlock;
int shared_resource_rwlock = 0;

void reader_fiber(int fiber_id) {
    for (int i = 0; i < 5; ++i) {
        rwlock.rlock();  // 읽기 락
        std::print("Reader Fiber {} read the resource: {} at iteration {}\n", fiber_id, shared_resource_rwlock, i);
        rwlock.runlock(); // 읽기 락 해제
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void writer_fiber(int fiber_id) {
    for (int i = 0; i < 5; ++i) {
        rwlock.wlock();  // 쓰기 락
        shared_resource_rwlock++;
        std::print("Writer Fiber {} updated resource to: {} at iteration {}\n", fiber_id, shared_resource_rwlock, i);
        rwlock.wunlock(); // 쓰기 락 해제
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main_rw() {
    acl::acl_cpp_init();

    // 2개의 읽기 코루틴 생성
    go[&]() {
        reader_fiber(1);
    };
    go[&]() {
        reader_fiber(2);
    };

    // 1개의 쓰기 코루틴 생성
    go[&]() {
        writer_fiber(3);
    };

    acl::fiber::schedule();
    std::print("Final shared resource value: {}\n", shared_resource_rwlock);
    return 0;
}
