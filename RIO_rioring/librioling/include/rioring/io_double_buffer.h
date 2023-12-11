//
// Created by newbi on 2022-11-07.
//

#ifndef RIORING_IO_DOUBLE_BUFFER_H
#define RIORING_IO_DOUBLE_BUFFER_H

#include "io_buffer.h"

namespace rioring {

class io_double_buffer : public buffer_base {
public:
    io_double_buffer() = default;

    explicit io_double_buffer(size_t size);

    void assign(size_t size);
    void destroy();
    bool read(unsigned char *buf, size_t size) override;
    void pop(size_t size) override;
    void push(const unsigned char *buf, size_t size) override;
    void elevate();

    [[nodiscard]] bool empty() const { return main_buffer.empty() && sub_buffer.empty(); }
    [[nodiscard]] size_t size() const { return main_buffer.size(); }

    const unsigned char *operator*() const { return *main_buffer; }

private:
    io_buffer main_buffer, sub_buffer;
};

}
#endif //RIORING_IO_DOUBLE_BUFFER_H
