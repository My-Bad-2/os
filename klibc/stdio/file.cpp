#include <klibc/internal/printf.hpp>
#include <klibc/stdio.h>

bool feof(FILE *stream) {
  LOCK_STREAM(stream);
  bool ret = !!(stream->flags & stdio::FILE_EOF);
  UNLOCK_STREAM(stream);

  return ret;
}

bool ferror(FILE *stream) {
  LOCK_STREAM(stream);
  bool ret = !!(stream->flags & stdio::FILE_ERROR);
  UNLOCK_STREAM(stream);

  return ret;
}

size_t fwrite(const void *restrict ptr, size_t size, size_t nmemb,
              FILE *restrict stream) {
  size_t length = size * nmemb;

  if (size == 0) {
    nmemb = 0;
  }

  LOCK_STREAM(stream);
  size_t k =
      stdio::stream_write(static_cast<const uint8_t *>(ptr), length, stream);
  UNLOCK_STREAM(stream);

  return (k == length) ? nmemb : (k / size);
}