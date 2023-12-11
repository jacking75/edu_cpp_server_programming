//
// Created by newbiediver on 22. 11. 13.
//

#ifndef CHATSERVER_POOL_H
#define CHATSERVER_POOL_H

#include <unordered_map>
#include <cstdlib>
#include <rioring/thread_lock.h>
#include "scheme.h"

class client;

class pool {
public:
    pool() = default;
    ~pool() = default;

    void add( size_t obj_id, client *c );
    void remove( size_t obj_id );
    void leave_all();
    void wait_for_leave();

    void chat_all( response_chat *rc );

private:
    rioring::critical_section               lock;
    std::unordered_map< size_t, client * >  clients;
};

pool &get_pool();

#endif //CHATSERVER_POOL_H
