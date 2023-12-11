//
// Created by newbiediver on 22. 11. 3.
//

#ifndef RIORING_TCP_SOCKET_H
#define RIORING_TCP_SOCKET_H

#include "socket_object.h"

namespace rioring {

class io_service;
class tcp_socket;

using tcp_socket_ptr = std::shared_ptr< tcp_socket >;

// Transform to downstream
inline tcp_socket_ptr to_tcp_socket_ptr( const object_ptr &s ) {
    return std::dynamic_pointer_cast< tcp_socket, object_base >( s );
}

// Transform to downstream
inline tcp_socket_ptr to_tcp_socket_ptr( const socket_ptr &s ) {
    return std::dynamic_pointer_cast< tcp_socket, socket_object >( s );
}

class tcp_socket : public socket_object {
public:
    tcp_socket() = delete;
    ~tcp_socket() override = default;
    explicit tcp_socket( io_service *io );
#ifdef WIN32
    explicit tcp_socket( io_service *io, SOCKET sock );
#else
    explicit tcp_socket( io_service *io, int sock );
#endif

    static tcp_socket_ptr create( io_service *io );
#ifdef WIN32
    static tcp_socket_ptr create( io_service *io, SOCKET sock );
#else
    static tcp_socket_ptr create( io_service *io, int sock );
#endif

    bool connect( std::string_view address, unsigned short port );
    [[nodiscard]] bool connected() const { return connected_flag; }

    bool send( const void *bytes, size_t size );

protected:
    void submit_receiving() override;
    void submit_sending( sockaddr *addr ) override;

    void on_shutdown() override;
    void on_active() override;

private:
    void set_remote_info( struct ::sockaddr *addr );

private:
    friend class tcp_server;

    bool            connected_flag{ false };
};

}

#endif //RIORING_TCP_SOCKET_H
