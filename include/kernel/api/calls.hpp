#ifndef KERNEL_API_CALLS_HPP
#define KERNEL_API_CALLS_HPP 1

#include <compiler.h>
#include <stddef.h>
#include <stdint.h>

namespace kernel::api {
struct iovec {
  void *buffer = nullptr;
  size_t len = 0;
};

size_t writev(struct iovec *iov, int iovcnt);
} // namespace kernel::api

#endif // KERNEL_API_CALLS_HPP