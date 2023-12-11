//
// Created by newbiediver on 22. 11. 13.
//

#include <iostream>
#include "client.h"
#include "pool.h"
#include "receiver.h"
#include "scheme.h"

void event_receiver::on_accept( tcp_socket_ptr &new_socket ) {
    auto cl = new client{ new_socket };

    // bind all callback at this time
    new_socket->set_error_callback( []( auto &&obj, auto &&ec ) {
        event_receiver::on_error( std::forward< decltype( obj ) >( obj ), std::forward< decltype( ec ) >( ec ) );
    } );

    new_socket->set_receive_event( []( auto &&obj, auto &&buf, auto &&addr ) {
        event_receiver::on_receive( std::forward< decltype( obj ) >( obj ), std::forward< decltype( buf ) >( buf ), std::forward< decltype( addr ) >( addr ) );
    } );

    new_socket->set_close_event( []( auto &&obj ) {
        event_receiver::on_close( std::forward< decltype( obj ) >( obj ) );
    } );

    get_pool().add( new_socket->object_id(), cl );
}

void event_receiver::on_error( object_ptr &obj, const std::error_code &ec ) {
    std::cerr << ec.message() << std::endl;
}

void event_receiver::on_receive( socket_ptr &obj, io_buffer *buf, sockaddr *addr [[maybe_unused]] ) {
    tcp_socket_ptr socket = to_tcp_socket_ptr( obj );
    auto cl = reinterpret_cast< client* >( socket->get_user_data() );

    std::array< unsigned char, 1024 > picked{ 0 };
    auto sc = reinterpret_cast< scheme* >( std::data( picked ) );
    while ( true ) {
        if ( !buf->read( &picked[0], sizeof( scheme ) ) ) break;    // Receive all not yet
        if ( !buf->read( &picked[0], sc->size ) ) break;            // Receive all not yet
        buf->pop( sc->size );

        if ( sc->msg != req_chat ) {
            std::cerr << "invalid message received" << std::endl;
            socket->close( tcp_socket::close_type::force );
            break;
        }

        auto request = reinterpret_cast< request_chat* >( sc );
        auto chat_message = reinterpret_cast< char* >( request + 1 );

        if ( request->chat_size > 0 ) {
            cl->chat( chat_message );
        }
    }
}

void event_receiver::on_close( socket_ptr &obj ) {
    get_pool().remove( obj->object_id() );

    std::cout << "closed socket -> " << obj->remote_ipv4() << ":" << obj->remote_port() << std::endl;
}