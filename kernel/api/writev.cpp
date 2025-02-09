#include <kernel/api/calls.hpp>
#include <kernel/arch/arch.hpp>

namespace kernel::api {
size_t writev(struct iovec *iov, int iov_count) {
  size_t total_written = 0;

  for (int i = 0; i < iov_count; i++) {
    size_t bytes_written = arch::write(
        std::string_view(static_cast<const char *>(iov[i].buffer), iov[i].len));

    if (bytes_written == static_cast<size_t>(-1)) {
      return -1;
    }

    total_written += bytes_written;

    if (bytes_written != iov[i].len) {
      break;
    }
  }

  return total_written;
}
} // namespace kernel::api