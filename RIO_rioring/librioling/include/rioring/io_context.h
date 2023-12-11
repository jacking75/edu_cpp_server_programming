//
// Created by newbiediver on 22. 11. 2.
//

#ifndef RIORING_CONTEXT_H
#define RIORING_CONTEXT_H

#include <memory>

#ifdef WIN32

#include <mswsockdef.h>
#include <ws2def.h>
#include <ws2ipdef.h>

namespace rioring {

class object_base;

struct io_context : public RIO_BUF {
    enum class io_type {
        unknown,
        read,
        write,
    };

    enum class context_type {
        tcp_context,
        udp_context
    };

    io_context() : RIO_BUF{} {}
    virtual ~io_context() = default;

    io_type         type{ io_type::unknown };
    RIO_RQ          rq{};
    std::shared_ptr< object_base >     handler;

    context_type    ctype{ context_type::tcp_context };
    SOCKADDR_INET   addr{};
    RIO_BUFFERID    addr_buffer_id{};
    RIO_BUF *       binded_address_context{};

};

}

#else

#include <bits/types/struct_iovec.h>
#include <netinet/in.h>

namespace rioring {

class object_base;

struct io_context {
    enum class io_type {
        unknown,
        accept,
        read,
        write,
        shutdown,
    };

    enum class context_type {
        tcp_context,
        udp_context
    };

    io_context() {
        msg.msg_name = &addr;
        msg.msg_namelen = sizeof( sockaddr_storage );
    }
    virtual ~io_context() = default;

    io_type         type{ io_type::unknown };
    context_type    ctype{ context_type::tcp_context };
    iovec           iov{};
    std::shared_ptr< object_base >     handler;

    msghdr              msg{};
    sockaddr_storage    addr{};
};

}

#endif

#endif //RIORING_CONTEXT_H
