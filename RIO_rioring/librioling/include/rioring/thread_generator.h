//
// Created by newbiediver on 22. 10. 31.
//

#ifndef RIORING_THREAD_GENERATOR_H
#define RIORING_THREAD_GENERATOR_H

#include "thread_lock.h"
#include "thread_object.h"
#include <vector>

namespace rioring {

class thread_generator {
public:
    template< typename... Args >
    void run( Args &&...args ) {
        run_thread( std::forward< Args >( args )... );
    }

    void run_object( thread_object *obj, int cnt = 1 );
    void wait_for_terminate();

private:
    template< typename Func, typename... Args >
    void run_thread( Func &&fn, Args &&...args ) {
        std::scoped_lock sc{ lock };
        threads.emplace_back( std::thread{ std::forward< Func >( fn ), std::forward< Args >( args )... } );
    }

    template< typename Func >
    void run_thread( Func &&fn ) {
        std::scoped_lock sc{ lock };
        threads.emplace_back( std::thread{ std::forward< Func >( fn ) } );
    }

private:
    spin_lock                   lock;
    std::vector< std::thread >  threads;
};

}

#endif //RIORING_THREAD_GENERATOR_H
