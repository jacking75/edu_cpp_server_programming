//
// Created by newbiediver on 22. 10. 31.
//

#include "rioring/thread_lock.h"

namespace rioring {

spin_lock::spin_lock() {
    af.clear( std::memory_order_release );
}

void spin_lock::lock() {
    while ( !try_lock() );
}

bool spin_lock::try_lock() {
    return !af.test_and_set( std::memory_order_acquire );
}

void spin_lock::unlock() {
    af.clear( std::memory_order_release );
}

void critical_section::lock() {
    mx.lock();
}

bool critical_section::try_lock() {
    return mx.try_lock();
}

void critical_section::unlock() {
    mx.unlock();
}

}