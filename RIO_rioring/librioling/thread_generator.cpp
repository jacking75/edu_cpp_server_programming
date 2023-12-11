//
// Created by newbiediver on 22. 10. 31.
//

#include "rioring/thread_generator.h"

namespace rioring {

void thread_generator::run_object( thread_object *obj, int cnt ) {
    for ( int i = 0; i < cnt; ++i ) {
        run( [=] { obj->on_thread(); } );
    }
}

void thread_generator::wait_for_terminate() {
    for ( auto & th : threads ) {
        if ( th.joinable() ) {
            th.join();
        }
    }
}

}