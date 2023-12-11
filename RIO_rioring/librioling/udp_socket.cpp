//
// Created by newbiediver on 22. 11. 19.
//

#include <cstring>
#include "rioring/io_service.h"
#include "rioring/udp_socket.h"

namespace rioring {

udp_socket::udp_socket( io_service *io ) : socket_object{ io, socket_object::type::udp } {

}

udp_socket_ptr udp_socket::create( io_service *io ) {
    auto socket = new udp_socket{ io };
    return udp_socket_ptr{ socket };
}

bool udp_socket::activate( family_type type ) {
    int af = AF_INET;
    if ( type == family_type::ipv6 ) {
        af = AF_INET6;
    }

#ifdef _WIN32
    socket_handler = WSASocketW( af, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_REGISTERED_IO );
    if ( socket_handler == INVALID_SOCKET ) {
        return false;
    }

    if ( af == AF_INET ) {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;

        if ( bind( socket_handler, (sockaddr*)&addr, sizeof( addr ) ) == SOCKET_ERROR ) {
            return false;
        }
    } else {
        sockaddr_in6 addr{};
        addr.sin6_family = AF_INET;

        if ( bind( socket_handler, (sockaddr*)&addr, sizeof( addr ) ) == SOCKET_ERROR ) {
            return false;
        }
    }

#else
    socket_handler = socket( af, SOCK_DGRAM, IPPROTO_UDP );
    if ( socket_handler < 0 ) {
        return false;
    }

    if ( af == AF_INET ) {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;

        if ( bind( socket_handler, (sockaddr*)&addr, sizeof( addr ) ) < 0 ) {
            return false;
        }
    } else {
        sockaddr_in6 addr{};
        addr.sin6_family = AF_INET;

        if ( bind( socket_handler, (sockaddr*)&addr, sizeof( addr ) ) < 0 ) {
            return false;
        }
    }
#endif
    on_active();

    return true;
}

void udp_socket::on_active() {
    recv_buffer.assign( RIORING_DATA_BUFFER_SIZE );
    send_buffer.assign( RIORING_DATA_BUFFER_SIZE );

    /*constexpr linger lg{ 1, 0 };
    constexpr int reuse = RIORING_REUSE_ADDR;
    setsockopt( socket_handler, SOL_SOCKET, SO_LINGER, reinterpret_cast< const char* >( &lg ), sizeof( lg ) );
    setsockopt( socket_handler, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast< const char* >( &reuse ), sizeof( int ) );*/

    socket_object::on_active();
    submit_receiving();
}

void udp_socket::on_shutdown() {
    recv_buffer.destroy();
    send_buffer.destroy();
}

void udp_socket::submit_receiving() {
    auto ctx = current_io->allocate_context();

    ctx->handler = shared_from_this();
    ctx->type = io_context::io_type::read;
    ctx->ctype = io_context::context_type::udp_context;
#ifdef _WIN32
    ctx->rq = request_queue;
    ctx->Offset = 0;
    ctx->BufferId = recv_buffer_id;
    ctx->Length = RIORING_DATA_BUFFER_SIZE;
#else
    ctx->iov.iov_base = std::data( recv_bind_buffer );
    ctx->iov.iov_len = RIORING_DATA_BUFFER_SIZE;

    ctx->msg.msg_iov = &ctx->iov;
    ctx->msg.msg_iovlen = 1;
#endif

    current_io->submit( ctx );
}

void udp_socket::submit_sending( sockaddr *addr ) {
    auto ctx = current_io->allocate_context();

    ctx->handler = shared_from_this();
    ctx->type = io_context::io_type::write;
    ctx->ctype = io_context::context_type::udp_context;

#ifdef _WIN32
    auto size = std::min< size_t >( send_buffer.size(), RIORING_DATA_BUFFER_SIZE );
    std::memcpy( std::data( send_bind_buffer ), *send_buffer, size );

    ctx->rq = request_queue;
    ctx->Offset = 0;
    ctx->BufferId = send_buffer_id;
    ctx->Length = static_cast< ULONG >( size );

    ctx->addr.si_family = addr->sa_family;
    switch ( addr->sa_family ) {
    case AF_INET6:
        std::memcpy( &ctx->addr.Ipv6, addr, sizeof( sockaddr_in6 ) );
        break;
    case AF_INET:
    default:
        std::memcpy( &ctx->addr.Ipv4, addr, sizeof( sockaddr_in ) );
        break;
    }

    current_io->submit( ctx );
#else
    ctx->iov.iov_base = const_cast< unsigned char* >( *send_buffer );
    ctx->iov.iov_len = send_buffer.size();
    ctx->msg.msg_iov = &ctx->iov;
    ctx->msg.msg_iovlen = 1;

    size_t addr_len = 0;
    switch ( addr->sa_family ) {
    case AF_INET:
        addr_len = sizeof( sockaddr_in );
        break;
    case AF_INET6:
        addr_len = sizeof( sockaddr_in6 );
        break;
    default:
        break;
    }

    if ( addr_len ) {
        std::memcpy( &ctx->addr, addr, addr_len );
        current_io->submit( ctx );
    }
#endif
}

bool udp_socket::send_to( const void *bytes, size_t size, sockaddr *addr ) {
    std::scoped_lock sc{ lock };
    if ( send_buffer.empty() ) {
        send_buffer.push( static_cast< const unsigned char* >( bytes ), size );
        submit_sending( addr );
    } else {
        send_buffer.push( static_cast< const unsigned char* >( bytes ), size );
    }
    return true;
}

}