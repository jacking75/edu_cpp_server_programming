//
// Created by newbiediver on 22. 11. 8.
//

#ifndef RIORING_OBJECT_BASE_H
#define RIORING_OBJECT_BASE_H

#include <memory>
#include <system_error>
#include <functional>

namespace rioring {

class object_base;

using object_ptr = std::shared_ptr< object_base >;

class object_base : public std::enable_shared_from_this< object_base > {
public:
    object_base();
    virtual ~object_base() = default;

    using error_callback = std::function< void( object_ptr &, const std::error_code & ) >;
    void set_error_callback( error_callback callback );

    size_t object_id() const;

protected:
    friend class io_service;
    void io_error( const std::error_code &ec );

protected:
    std::function< void( object_ptr&, const std::error_code& ) >    error_event;
    size_t obj_id{ 0 };
};

}

#endif //RIORING_OBJECT_BASE_H
