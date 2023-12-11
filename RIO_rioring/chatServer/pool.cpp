//
// Created by newbiediver on 22. 11. 13.
//

#include <chrono>
#include <thread>
#include "client.h"
#include "pool.h"

using namespace std::chrono_literals;

pool p;

pool &get_pool() {
    return p;
}

void pool::add( size_t obj_id, client *c ) {
    std::scoped_lock sc{ lock };
    clients.insert( { obj_id, c } );
}

void pool::remove( size_t obj_id ) {
    std::scoped_lock sc{ lock };
    clients.erase( obj_id );
}

void pool::leave_all() {
    std::scoped_lock sc{ lock };
    for ( auto pa : clients ) {
        auto c = pa.second;
        c->close();
    }
}

void pool::wait_for_leave() {
    while ( !clients.empty() ) {
        std::this_thread::sleep_for( 1ms );
    }
}

void pool::chat_all( response_chat *rc ) {
    std::scoped_lock sc{ lock };
    for ( auto pa : clients ) {
        auto c = pa.second;
        c->send( rc, rc->size );
    }
}