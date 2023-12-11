//
// Created by newbi on 2022-11-07.
//

#include <algorithm>
#include "rioring/io_double_buffer.h"

namespace rioring {

/*
 io_double_buffer: 두개의 io_buffer 를 운용하는 버퍼
 io_double_buffer: Has two io_buffer to operate.
*/
io_double_buffer::io_double_buffer( size_t size ) : main_buffer( size ) {

}

void io_double_buffer::assign( size_t size ) {
    main_buffer.assign( size );
}

void io_double_buffer::destroy() {
    main_buffer.destroy();
    sub_buffer.destroy();
}

bool io_double_buffer::read( unsigned char *buf, size_t size ) {
    return main_buffer.read( buf, size );
}

void io_double_buffer::pop( size_t size ) {
    return main_buffer.pop( size );
}

//
void io_double_buffer::push( const unsigned char *buf, size_t size ) {
    if ( !sub_buffer.empty() ) {
        sub_buffer.push( buf, size );
    } else if ( main_buffer.empty() ) {
        main_buffer.push( buf, size );
    } else if ( main_buffer.offset() + size > main_buffer.capacity() ) {
        if ( sub_buffer.capacity() == 0 ) {
            sub_buffer.assign( std::max( size * 2, main_buffer.capacity() ) );
        }
        sub_buffer.push( buf, size );
    } else {
        main_buffer.push( buf, size );
    }
}

// main_buffer 가 비워지면 sub_buffer 를 main 으로 끌어올림
// Raise sub_buffer to main when the main_buffer is empty
void io_double_buffer::elevate() {
    if ( main_buffer.empty() && !sub_buffer.empty() ) {
        main_buffer.destroy();
        main_buffer = std::move( sub_buffer );
    }
}

}