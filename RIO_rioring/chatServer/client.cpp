//
// Created by newbiediver on 22. 11. 13.
//

#include <cstring>
#include <string>
#include "client.h"
#include "scheme.h"
#include "pool.h"

std::atomic_int name_id{ 1 };

client::client( tcp_socket_ptr &s ) : socket{ s } {
    socket->set_user_data( this );
    name = "client" + std::to_string( name_id++ );
}

void client::chat( std::string_view msg ) {
    std::array< char, 1024 > buffer{ 0 };

    auto rc = reinterpret_cast< response_chat* >( std::data( buffer ) );
    new (rc) response_chat();

    std::copy( name.begin(), name.end(), rc->name );
    rc->name[ name.size() ] = 0;

    auto chat_msg = reinterpret_cast< char* >( rc + 1 );
    std::memcpy( chat_msg, msg.data(), msg.size() );
    chat_msg[ msg.size() ] = 0;

    rc->chat_size = static_cast< int >( msg.size() );
    rc->size += rc->chat_size;

    get_pool().chat_all( rc );
}

void client::close() {
    socket->close();
}

void client::send( const void *buf, unsigned short size ) {
    socket->send( buf, size );
}