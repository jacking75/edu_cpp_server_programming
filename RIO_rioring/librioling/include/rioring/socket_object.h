//
// Created by newbiediver on 22. 11. 2.
//

#ifndef RIORING_EVENT_HANDLER_H
#define RIORING_EVENT_HANDLER_H

#ifdef _WIN32
#include <WS2tcpip.h>
#else
#include <netinet/in.h>
#endif
#include <array>
#include "predefined.h"
#include "object_base.h"
#include "thread_lock.h"
#include "io_buffer.h"
#include "io_double_buffer.h"
#ifdef _WIN32
#include <mswsockdef.h>
#endif

namespace rioring {

class io_service;
class socket_object;

using socket_ptr = std::shared_ptr< socket_object >;

inline socket_ptr to_socket_ptr( const object_ptr &s ) {
    return std::dynamic_pointer_cast< socket_object, object_base >( s );
}

class socket_object : public object_base {
public:
    using receive_event = std::function< void( socket_ptr&, io_buffer*, sockaddr * ) >;
    using send_event = std::function< void( socket_ptr& ) >;
    using close_event = std::function< void( socket_ptr& ) >;

    enum class type {
        tcp,
        udp
    };

    enum class close_type {
        graceful,
        force
    };

    socket_object() = delete;
    ~socket_object() override = default;

    explicit socket_object( io_service *io, type t ) : current_io{ io }, socket_type{ t } {}

#ifdef WIN32
    explicit socket_object( io_service *io, type t, SOCKET sock ) : current_io{ io }, socket_handler{ sock }, socket_type{ t } {}
#else
    explicit socket_object( io_service *io, type t, int sock ) : current_io{ io }, socket_handler{ sock }, socket_type{ t } {}
#endif

    void close( close_type type = close_type::graceful );

    void set_user_data( void *data )                { user_data = data; }

    // 사용자의 데이터를 socket과 바인딩 시킬 때 사용.
    // Used to bind the user's data to socket.
    [[nodiscard]] void* get_user_data() const       { return user_data; }
    [[nodiscard]] const char* remote_ipv4() const   { return std::data( remote_v4_addr_string ); }
    [[nodiscard]] const char* remote_ipv6() const   { return std::data( remote_v6_addr_string ); }
    [[nodiscard]] unsigned short remote_port() const { return remote_port_number; }
    [[nodiscard]] sockaddr *socket_address() const;

    [[nodiscard]] type protocol() const             { return socket_type; }

    void set_receive_event( receive_event event );
    void set_send_complete_event( send_event event );
    void set_close_event( close_event event );

protected:
    void io_received( size_t bytes_transferred, sockaddr *addr );
    void io_sent( size_t bytes_transferred, sockaddr *addr );
    void io_shutdown();

protected:
#ifdef WIN32
    virtual void on_active();
#else
    virtual void on_active() {}
#endif
    virtual void submit_receiving() {}
    virtual void submit_sending( sockaddr *addr ) {}
    virtual void submit_shutdown();

protected:
    virtual void on_shutdown() {}
    virtual void on_send_complete();

private:
    socket_ptr cast_socket_ptr();

protected:
    friend class io_service;
    friend class tcp_server;

    io_service      *current_io;

#ifdef WIN32
    SOCKET          socket_handler{ INVALID_SOCKET };
    RIO_RQ          request_queue{ nullptr };
    RIO_BUFFERID    recv_buffer_id{ nullptr }, send_buffer_id{ nullptr };
    SOCKADDR_INET   addr_storage{};
    std::array< unsigned char, RIORING_DATA_BUFFER_SIZE > recv_bind_buffer{ 0 }, send_bind_buffer{ 0 };
#else
    int                     socket_handler{ 0 };
    sockaddr_storage        addr_storage{};
    std::array< unsigned char, RIORING_DATA_BUFFER_SIZE > recv_bind_buffer{ 0 };
#endif

    type            socket_type;
    bool            shutdown_gracefully{ false };

    unsigned short  remote_port_number{ 0 };

    std::array< char, 16 >  remote_v4_addr_string{ 0 };
    std::array< char, 40 >  remote_v6_addr_string{ 0 };

    void*                   user_data{ nullptr };

    critical_section        lock;
    io_buffer               recv_buffer;
    io_double_buffer        send_buffer;

    receive_event   recv_event;
    send_event      send_complete_event;
    close_event     socket_close_event;
};

}

//#endif

#endif //RIORING_EVENT_HANDLER_H
