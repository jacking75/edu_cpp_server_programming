//
// Created by newbiediver on 22. 11. 13.
//

#ifndef CHATSERVER_RECEIVER_H
#define CHATSERVER_RECEIVER_H

#include <rioring/tcp_socket.h>

using namespace rioring;

struct event_receiver {
    static void on_accept( tcp_socket_ptr &new_socket );
    static void on_error( object_ptr &obj, const std::error_code &ec );
    static void on_receive( socket_ptr &obj, io_buffer *buf, sockaddr *addr );
    static void on_close( socket_ptr &obj );
};

#endif //CHATSERVER_RECEIVER_H
