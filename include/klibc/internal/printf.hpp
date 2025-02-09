#ifndef KLIBC_INTERNAL_PRINTF_HPP
#define KLIBC_INTERNAL_PRINTF_HPP 1

#include <klibc/stdio.h>
#include <stddef.h>

#define LOCK_STREAM(stream) (static_cast<void>(0))
#define UNLOCK_STREAM(stream) (static_cast<void>(0))

#define NL_ARGMAX 9
#define UNGET 9

struct _IO_FILE {
  uint8_t *read_pos = nullptr;
  uint8_t *read_end = nullptr;
  uint8_t *write_pos = nullptr;
  uint8_t *write_end = nullptr;
  uint8_t *write_base = nullptr;

  size_t (*write)(_IO_FILE *, const uint8_t *, size_t) = nullptr;
  uint8_t *buffer = nullptr;
  void *cookie = nullptr;
  
  size_t buf_size = 0;
  uint32_t flags = 0;
  uint32_t mode = 0;
  
  int offset = 0;
  int lbf = 0;
};

namespace stdio {
enum {
  FILE_PERM = (1 << 0),
  FILE_NO_READ = (1 << 1),
  FILE_NO_WRITE = (1 << 2),
  FILE_EOF = (1 << 3),
  FILE_ERROR = (1 << 4),
  FILE_SVB = (1 << 5),
  FILE_APPEND = (1 << 6),
};

size_t stream_write(const uint8_t *restrict str, size_t length,
                    FILE *restrict stream);
__LOCAL int to_write(FILE *stream);
__LOCAL size_t write(FILE *stream, const uint8_t *buffer, size_t length);
__LOCAL size_t fwrite(const uint8_t *restrict str, size_t length, FILE *stream);
__LOCAL int overflow(FILE *stream, int sym);
} // namespace stdio

#endif // KLIBC_INTERNAL_PRINTF_HPP