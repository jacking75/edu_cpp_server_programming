//
// Created by newbiediver on 22. 10. 31.
//

#ifndef RIORING_IO_SERVICE_H
#define RIORING_IO_SERVICE_H

#include "rioring/thread_generator.h"
#include "rioring/thread_object.h"
#include "rioring/thread_lock.h"
#include "rioring/simple_pool.h"

#ifdef _WIN32

#include <WinSock2.h>
#include <MSWSock.h>
#include "rioring/io_context.h"

#pragma comment(lib, "ws2_32")

namespace rioring {

class io_service : private thread_object {
public:
    io_service();
    ~io_service() noexcept override;

#ifdef _WIN32
    static bool initialize_winsock();
    static void deinitialize_winsock();
#endif

    bool run( int concurrency );
    bool submit( io_context *ctx );
    void stop();

    [[nodiscard]] int running_count() const     { return running_io; }

    io_context *allocate_context();

protected:
    void on_thread() override;

private:
    bool load_rio();
    void io( RIO_CQ cq );
    void deallocate_context( io_context *ctx );
    RIO_BUF *allocate_address_context( io_context *ctx );
    void deallocate_address_context( RIO_BUF *buf );
    static sockaddr *current_sockaddr( io_context *ctx );

    RIO_BUFFERID register_buffer( void * buffer, size_t size ) const;
    void unregister_buffer( RIO_BUFFERID id ) const;
    RIO_RQ create_request_queue( SOCKET s );

private:
    friend class socket_object;

    spin_lock                   lock;
    std::vector< RIO_CQ >       io_array;
    simple_pool< io_context >   context_pool;
    simple_pool< RIO_BUF >      address_pool;
    std::atomic_int             running_io{ 0 };
    RIO_EXTENSION_FUNCTION_TABLE rio{};

    thread_generator            tg;
};

}

#else

// liburing-dev 설치
// You have to install 'liburing-dev' using your package manager (like apt, yum)
#include <liburing.h>
#include "rioring/io_context.h"

namespace rioring {

/*
 io_uring 은 기본적으로 async 하지만 각 io_uring 의 스레드 컨텍스트를 구성하여 효율을 높임.
 io_service 는 file descriptor 는 지원하지 않으며 현재는 오직 socket stream 만 지원함.
 io_uring is essentially async, but configures thread context for each io_uring to increase efficiency.
 The io_service does not support file descriptors and currently supports only socket streams.
*/
class io_service : private thread_object {
public:
    io_service();
    ~io_service() override = default;

    bool run( int concurrency );
    bool submit( io_context *ctx );
    void stop();

    [[nodiscard]] int running_count() const     { return running_io; }

    io_context *allocate_context();

protected:
    void on_thread() override;

private:
    void io( io_uring *ring );
    void deallocate_context( io_context *ctx );
    static sockaddr *current_sockaddr( io_context *ctx );

private:
    spin_lock                   lock;
    std::vector< io_uring* >    io_array;
    simple_pool< io_context >   context_pool;
    std::atomic_int             running_io{ 0 };
    thread_generator            tg;

    static thread_local io_uring *thread_ring;
};

}

#endif

#endif //RIORING_IO_SERVICE_H
