//
// Created by newbiediver on 22. 11. 15.
//

#ifndef RIORING_UDP_SERVER_H
#define RIORING_UDP_SERVER_H

#include "udp_socket.h"

namespace rioring {

class udp_server;
using udp_server_ptr = std::shared_ptr< udp_server >;

inline udp_server_ptr to_udp_server_ptr( socket_ptr &socket ) {
    return std::dynamic_pointer_cast< udp_server, socket_object >( socket );
}

inline udp_server_ptr to_udp_server_ptr( object_ptr &obj ) {
    return std::dynamic_pointer_cast< udp_server, object_base >( obj );
}

class udp_server : public udp_socket {
public:
    udp_server() = delete;
    ~udp_server() override = default;

    explicit udp_server( io_service *io );
    static udp_server_ptr create( io_service *io );

    bool run( unsigned short port );
    void stop();

};

}

#endif //RIORING_UDP_SERVER_H
