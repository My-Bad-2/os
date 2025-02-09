#include <array>
#include <kernel/api/calls.hpp>
#include <klibc/internal/printf.hpp>
#include <klibc/string.h>

namespace stdio {
int overflow(FILE *stream, int sym) {
  uint8_t symbol = sym;

  if (!stream->write_end && to_write(stream)) {
    return EOF;
  }

  if ((stream->write_pos != stream->write_end) && (symbol == stream->lbf)) {
    return *stream->write_pos++ = symbol;
  }

  if (stream->write(stream, &symbol, 1) != 1) {
    return EOF;
  }

  return symbol;
}

int to_write(FILE *stream) {
  stream->mode |= stream->mode - 1;

  if (stream->flags & FILE_NO_WRITE) {
    stream->flags |= FILE_ERROR;
    return EOF;
  }

  stream->read_pos = stream->read_end = nullptr;

  stream->write_pos = stream->write_base = stream->buffer;
  stream->write_end = stream->buffer + stream->buf_size;

  return 0;
}

size_t stream_write(const uint8_t *restrict str, size_t length,
                    FILE *restrict stream) {
  size_t i = 0;

  if (!stream->write_end && stdio::to_write(stream)) {
    return stream->write(stream, str, length);
  }

  if (stream->lbf >= 0) {
    for (i = length; i && str[i - 1] != '\n'; i--)
      ;

    if (i) {
      size_t n = stream->write(stream, str, i);

      if (n < i) {
        return n;
      }

      str += i;
      length -= i;
    }
  }

  memcpy(stream->write_pos, str, length);
  stream->write_pos += length;

  return length + i;
}

size_t write(FILE *stream, const uint8_t *buffer, size_t length) {
  std::array<kernel::api::iovec, 2> iovs = {};

  iovs[0].buffer = stream->write_base;
  iovs[0].len = stream->write_pos - stream->write_base;

  iovs[1].buffer = static_cast<void *>(const_cast<uint8_t *>(buffer));
  iovs[1].len = length;

  kernel::api::iovec *iov = iovs.begin();

  size_t rem = iovs[0].len + iovs[1].len;
  size_t iov_count = iovs.size();
  size_t count = 0;

  while (true) {
    count = kernel::api::writev(iov, iov_count);

    if (count == rem) {
      stream->write_end = stream->buffer + stream->buf_size;
      stream->write_pos = stream->write_base = stream->buffer;

      return length;
    }

    if (count == static_cast<size_t>(-1)) {
      stream->write_pos = stream->write_base = stream->write_end = nullptr;
      stream->flags |= FILE_ERROR;

      return (iov_count == 2) ? 0 : length - iov->len;
    }

    rem -= count;

    if (count > iov->len) {
      count -= iov->len;
      iov++;
      iov_count--;
    }

    iov->buffer = reinterpret_cast<char *>(iov[0].buffer) + count;
    iov->len -= count;
  }

  return 0;
}
} // namespace stdio