//
// Created by newbiediver on 22. 11. 5.
//

#ifndef RIORING_ADDRESS_RESOLVER_H
#define RIORING_ADDRESS_RESOLVER_H

#ifdef __linux__
#include <netdb.h>
#endif
#include <string>
#include <cstring>

namespace rioring {

inline struct ::addrinfo *resolve( std::string_view address, unsigned short port ) {
    ::addrinfo *result, hint = {};
    auto port_string = std::to_string( port );

    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;

    if ( getaddrinfo( address.data(), port_string.c_str(), &hint, &result ) != 0 ) {
        return nullptr;
    }

    return result;
}

inline bool convert_socket_address( std::string_view address, unsigned short port, sockaddr_in6 *out ) {
    auto info = resolve( address, port );
    for ( auto a = info; a != nullptr; a = a->ai_next ) {
        if ( a->ai_family == AF_INET6 ) {
            std::memcpy( out, a->ai_addr, sizeof( sockaddr_in6 ) );
            freeaddrinfo( info );
            return true;
        }
    }

    freeaddrinfo( info );

    return false;
}

inline bool convert_socket_address( std::string_view address, unsigned short port, sockaddr_in *out ) {
    auto info = resolve( address, port );
    for ( auto a = info; a != nullptr; a = a->ai_next ) {
        if ( a->ai_family == AF_INET ) {
            std::memcpy( out, a->ai_addr, sizeof( sockaddr_in ) );
            freeaddrinfo( info );
            return true;
        }
    }

    freeaddrinfo( info );

    return false;
}

}

#endif //RIORING_ADDRESS_RESOLVER_H
