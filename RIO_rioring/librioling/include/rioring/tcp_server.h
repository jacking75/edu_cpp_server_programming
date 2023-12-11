//
// Created by newbiediver on 22. 11. 5.
//

#ifndef RIORING_TCP_SERVER_H
#define RIORING_TCP_SERVER_H

#include "tcp_socket.h"

#ifdef WIN32

#include "rioring/thread_object.h"
#include "rioring/thread_generator.h"

namespace rioring {

class tcp_server;
class io_service;

using tcp_server_ptr = std::shared_ptr< tcp_server >;

// Transform to downstream
inline tcp_server_ptr to_tcp_server_ptr( const object_ptr &s ) {
    return std::dynamic_pointer_cast< tcp_server, object_base >( s );
}

class tcp_server : private thread_object, public object_base {
public:
    using accept_event = std::function< void( tcp_socket_ptr& ) >;

    tcp_server() = delete;
    ~tcp_server() override = default;

    static tcp_server_ptr create( io_service *io );
    void set_accept_event( accept_event event );

    bool run( unsigned short port );
    void stop();

protected:
    void on_thread() override;

protected:
    explicit tcp_server( io_service *io );
    void error_occur( std::errc err );

private:
    io_service  *current_io;
    SOCKET      server_socket{ INVALID_SOCKET };

    std::function< void( tcp_socket_ptr &new_socket ) >                 accept_event_callback;

    thread_generator    tg;
};

}

#else

namespace rioring {

class io_service;
class tcp_server;

using tcp_server_ptr = std::shared_ptr< tcp_server >;

// Transform to downstream
inline tcp_server_ptr to_tcp_server_ptr( const object_ptr &s ) {
    return std::dynamic_pointer_cast< tcp_server, object_base >( s );
}

class tcp_server : public object_base {
public:
    using accept_event = std::function< void( tcp_socket_ptr& ) >;

    tcp_server() = delete;
    ~tcp_server() override = default;

    static tcp_server_ptr create( io_service *io );
    void set_accept_event( accept_event event );

    bool run( unsigned short port );
    void stop();

protected:
    explicit tcp_server( io_service *io );
    void io_accepting( int new_socket, struct ::sockaddr *addr );
    void submit_accept();

    virtual void on_accept( tcp_socket_ptr &new_socket );

private:
    friend class io_service;

    io_service  *current_io;
    int         server_socket{ 0 };

    sockaddr_in6    addr6{};
    socklen_t       addr_len{ sizeof( sockaddr_in6 ) };

    std::function< void( tcp_socket_ptr& ) >    new_connect_event;
};

}

#endif

#endif //RIORING_TCP_SERVER_H
