//
// Created by newbiediver on 22. 11. 5.
//

#ifndef RIORING_SIMPLE_POOL_H
#define RIORING_SIMPLE_POOL_H

#include <list>
#include <iterator>
#include "thread_lock.h"

namespace rioring {

template< typename Obj >
class simple_pool {
public:
    simple_pool() = delete;
    ~simple_pool() {
        std::scoped_lock sc{ lock };
        while ( !pool.empty() ) {
            auto ctx = pool.front();
            delete ctx;

            pool.pop_front();
        }
    }

    explicit simple_pool( size_t init_size ) {
        for ( size_t i = 0; i < init_size; ++i ) {
            pool.push_back( new Obj() );
        }
    }

    Obj *pop() {
        std::scoped_lock sc{ lock };

        if ( pool.empty() ) {
            return new Obj;
        }

        auto ctx = pool.front();
        pool.pop_front();

        return ctx;
    }

    void push( Obj *ctx ) {
        std::scoped_lock sc{ lock };
        pool.push_back( ctx );
    }

    void enum_all( std::vector< Obj* > *all ) {
        std::scoped_lock sc{ lock };
        all->reserve( pool.size() );

        std::copy( pool.begin(), pool.end(), std::back_inserter( *all ) );
    }

private:
    spin_lock            lock;
    std::list< Obj* >    pool;
};

}

#endif //RIORING_SIMPLE_POOL_H
