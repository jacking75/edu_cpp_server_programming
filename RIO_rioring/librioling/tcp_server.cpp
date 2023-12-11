//
// Created by newbiediver on 22. 11. 5.
//

#include "rioring/io_service.h"
#include "rioring/tcp_server.h"

#ifdef _WIN32

namespace rioring {

tcp_server::tcp_server( io_service *io ) : current_io{ io } {

}

tcp_server_ptr tcp_server::create( io_service *io ) {
    auto ptr = new tcp_server{ io };
    return tcp_server_ptr{ ptr };
}

void tcp_server::set_accept_event( accept_event event ) {
    accept_event_callback = std::move( event );
}

void tcp_server::error_occur( std::errc err ) {
    if ( error_event ) {
        auto ptr = shared_from_this();
        error_event( ptr, std::make_error_code( err ) );
    }
}

bool tcp_server::run( unsigned short port ) {
    server_socket = WSASocketW( AF_INET6, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_REGISTERED_IO );
    if ( server_socket == INVALID_SOCKET ) {
        error_occur( std::errc( errno ) );
        return false;
    }

    constexpr int reuse = RIORING_REUSE_ADDR, v6only = RIORING_ONLY_IPV6;
    setsockopt( server_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast< const char* >( &reuse ), sizeof( int ) );
    setsockopt( server_socket, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast< const char* >( &v6only ), sizeof( int ) );

    struct ::sockaddr_in6 in{};
    memset( &in, 0, sizeof( in ) );
    in.sin6_family = AF_INET6;
    in.sin6_port = htons( port );
    in.sin6_addr = in6addr_any;

    if ( bind( server_socket, reinterpret_cast< const struct sockaddr * >( &in ), sizeof( in ) ) == SOCKET_ERROR ) {
        error_occur( std::errc( errno ) );
        return false;
    }

    if ( listen( server_socket, SOMAXCONN ) == SOCKET_ERROR ) {
        error_occur( std::errc( errno ) );
        return false;
    }

    tg.run_object( this, 1 );
    return true;
}

void tcp_server::stop() {
    closesocket( server_socket );
    server_socket = INVALID_SOCKET;

    tg.wait_for_terminate();
}

void tcp_server::on_thread() {
    sockaddr_in6 in{};
    int size = sizeof( in );

    while ( true ) {
        SOCKET new_socket = WSAAccept( server_socket, reinterpret_cast< ::sockaddr* >( &in ), &size, nullptr, 0 );
        if ( new_socket == INVALID_SOCKET ) {
            break;
        }

        auto new_connector = tcp_socket::create( current_io, new_socket );
        new_connector->set_remote_info( reinterpret_cast< ::sockaddr* >( &in ) );

        if ( accept_event_callback ) {
            accept_event_callback( new_connector );
        }

        new_connector->on_active();
    }
}

}

#else

#include <utility>

namespace rioring {

tcp_server::tcp_server( io_service *io ) : current_io{ io } {

}

// 실제 tcp_server 를 생성하는 api
 tcp_server_ptr tcp_server::create( io_service *io ) {
    auto pointer = new tcp_server{ io };
    return tcp_server_ptr{ pointer };
}

void tcp_server::set_accept_event( tcp_server::accept_event event ) {
    new_connect_event = std::move( event );
}

bool tcp_server::run( unsigned short port ) {
    auto error_occur = [&] {
        io_error( std::make_error_code( std::errc( errno ) ) );
    };

    server_socket = socket( AF_INET6, SOCK_STREAM, 0 );
    if ( server_socket == -1 ) {
        error_occur();
        return false;
    }

    constexpr int reuse = RIORING_REUSE_ADDR, v6only = RIORING_ONLY_IPV6;
    setsockopt( server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( int ) );
    setsockopt( server_socket, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof( int ) );

    sockaddr_in6 in{};
    in.sin6_family = AF_INET6;
    in.sin6_port = htons( port );
    in.sin6_addr = in6addr_any;

    if ( bind( server_socket, reinterpret_cast< ::sockaddr* >( &in ), sizeof( in ) ) < 0 ) {
        error_occur();
        return false;
    }

    if ( listen( server_socket, 5 ) < 0 ) {
        error_occur();
        return false;
    }

    submit_accept();

    return true;
}

void tcp_server::stop() {
    auto ctx = current_io->allocate_context();
    ctx->handler = shared_from_this();
    ctx->type = io_context::io_type::shutdown;

    current_io->submit( ctx );
}

void tcp_server::submit_accept() {
    auto ctx = current_io->allocate_context();
    ctx->handler = shared_from_this();
    ctx->type = io_context::io_type::accept;

    current_io->submit( ctx );
}

void tcp_server::io_accepting( int new_socket, struct ::sockaddr *addr ) {
    tcp_socket_ptr socket = tcp_socket::create( current_io, new_socket );
    socket->set_remote_info( addr );

    on_accept( socket );
    socket->on_active();
}

void tcp_server::on_accept( tcp_socket_ptr &new_socket ) {
    if ( new_connect_event ) {
        new_connect_event( new_socket );
    }

    submit_accept();
}

}

#endif