//
// Created by newbiediver on 22. 11. 13.
//

#ifndef CHATSERVER_CLIENT_H
#define CHATSERVER_CLIENT_H

#include <rioring/tcp_socket.h>

using namespace rioring;

class client {
public:
    client() = delete;
    ~client() = default;

    explicit client( tcp_socket_ptr &s );

    void chat( std::string_view msg );

    void close();
    void send( const void* buf, unsigned short size );

private:
    friend class receiver;

    tcp_socket_ptr      socket;
    std::string         name;
};

#endif //CHATSERVER_CLIENT_H
