//
// Created by newbiediver on 22. 11. 19.
//

#ifndef RIORING_UDP_SOCKET_H
#define RIORING_UDP_SOCKET_H

#include "socket_object.h"

namespace rioring {

class io_service;
class udp_socket;

using udp_socket_ptr = std::shared_ptr< udp_socket >;

// Transform to downstream
inline udp_socket_ptr to_udp_socket_ptr( const object_ptr &s ) {
    return std::dynamic_pointer_cast< udp_socket, object_base >( s );
}

// Transform to downstream
inline udp_socket_ptr to_udp_socket_ptr( const socket_ptr &s ) {
    return std::dynamic_pointer_cast< udp_socket, socket_object >( s );
}

// udp 소켓은 타겟의 sockaddr 정보를 알고 있어야 전송이 가능합니다.
// udp socket can send packet but you have to know peer sockaddr.
class udp_socket : public socket_object {
public:
    enum class family_type {
        ipv4,
        ipv6
    };
    udp_socket() = delete;
    ~udp_socket() override = default;
    explicit udp_socket( io_service *io );

    // 소켓 object 는 create 함수를 통하여 shared_ptr 로 사용하는 것을 권장드립니다.
    // Recommend to create api to use as shared_ptr.
    static udp_socket_ptr create( io_service *io );

    bool activate( family_type type );
    bool send_to( const void *bytes, size_t size, sockaddr *addr );

protected:
    void submit_receiving() override;
    void submit_sending( sockaddr *addr ) override;

    void on_active() override;
    void on_shutdown() override;
};

}

#endif //RIORING_UDP_SOCKET_H
