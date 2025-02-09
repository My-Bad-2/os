#include <algorithm>
#include <klibc/internal/printf.hpp>
#include <klibc/string.h>
#include <limits>

namespace {
struct cookie {
  char *src;
  size_t len;
};

size_t sn_write(FILE *stream, const uint8_t *str, size_t length) {
  cookie *ck = static_cast<cookie *>(stream->cookie);
  size_t len = std::min(
      ck->len, static_cast<size_t>(stream->write_pos - stream->write_base));

  if (len != 0) {
    memcpy(ck->src, stream->write_base, len);
    ck->src += len;
    ck->len -= len;
  }

  len = std::min(ck->len, 1ul);

  if (len != 0) {
    memcpy(ck->src, str, len);
    ck->src += len;
    ck->len -= len;
  }

  *ck->src = 0;
  stream->write_pos = stream->write_base = stream->buffer;

  return length;
}
} // namespace

int vsnprintf(char *restrict buffer, size_t maxlen, const char *restrict format,
              va_list arg) {
  uint8_t buf[1];
  char dummy[1];

  cookie ck = {
      .src = maxlen ? buffer : dummy,
      .len = maxlen ? (maxlen - 1) : 0,
  };

  FILE stream = {
      .write = sn_write,
      .buffer = buf,
      .cookie = &ck,
      .lbf = EOF,
  };

  *ck.src = 0;

  return vfprintf(&stream, format, arg);
}

int vsprintf(char *restrict buffer, const char *restrict format, va_list arg) {
  return vsnprintf(buffer, std::numeric_limits<int>::max(), format, arg);
}