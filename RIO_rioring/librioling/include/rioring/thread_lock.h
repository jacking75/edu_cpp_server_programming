//
// Created by newbiediver on 22. 10. 31.
//

#ifndef RIORING_THREAD_LOCK_H
#define RIORING_THREAD_LOCK_H

#include <atomic>
#include <mutex>

namespace rioring {

class lock_base {
public:
    virtual void lock() = 0;
    virtual bool try_lock() = 0;
    virtual void unlock() = 0;
};

class spin_lock : public lock_base{
public:
    spin_lock();

    void lock() override;
    bool try_lock() override;
    void unlock() override;

private:
    std::atomic_flag       af{};
};

class critical_section : public lock_base {
public:
    void lock() override;
    bool try_lock() override;
    void unlock() override;

private:
    std::mutex      mx;

};

}

#endif //RIORING_THREAD_LOCK_H
