#include <klibc/internal/printf.hpp>

namespace {
int putc_nolock(int ch, FILE *stream) {
  if ((static_cast<uint8_t>(ch) != stream->lbf) &&
      (stream->write_pos != stream->write_end)) {
    return *stream->write_pos++ = static_cast<uint8_t>(ch);
  }

  return stdio::overflow(stream, static_cast<uint8_t>(ch));
}
} // namespace

int putc(int ch, FILE *stream) {
  LOCK_STREAM(stream);
  int ret = putc_nolock(ch, stream);
  UNLOCK_STREAM(stream);

  return ret;
}