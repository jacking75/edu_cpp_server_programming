//
// Created by newbiediver on 22. 11. 4.
//

#ifndef RIORING_BUFFER_BASE_H
#define RIORING_BUFFER_BASE_H

#include <cstddef>

namespace rioring {

class buffer_base {
public:
    virtual bool read( unsigned char* buf, size_t size ) = 0;
    virtual void pop( size_t size ) = 0;
    virtual void push( const unsigned char *buf, size_t size ) = 0;
};

}

#endif //RIORING_BUFFER_BASE_H
