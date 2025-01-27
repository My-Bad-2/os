#include <klibc/internal/memcopy.hpp>
#include <klibc/string.h>

void *memset(void *dst, int c, size_t len) {
  uintptr_t dstp = reinterpret_cast<uintptr_t>(dst);

  if (len >= 8) {
    size_t length = 0;
    op_t word = 0;

    word = c;
    word |= word << 8;
    word |= word << 16;

    if (OPSIZ > 4) {
      // Do the shift in two dteps to avoid warning if long has 32 bits
      word |= (word << 16) << 16;
    }

    while (dstp % OPSIZ != 0) {
      reinterpret_cast<uint8_t *>(dstp)[0] = c;
      dstp++;
      length--;
    }

    // Write 8 `op_t` per iteration until less than 8 `op_t` remain.
    length = length / (OPSIZ * 8);

    while (length > 0) {
      reinterpret_cast<op_t *>(dstp)[0] = word;
      reinterpret_cast<op_t *>(dstp)[1] = word;
      reinterpret_cast<op_t *>(dstp)[2] = word;
      reinterpret_cast<op_t *>(dstp)[3] = word;
      reinterpret_cast<op_t *>(dstp)[4] = word;
      reinterpret_cast<op_t *>(dstp)[5] = word;
      reinterpret_cast<op_t *>(dstp)[6] = word;
      reinterpret_cast<op_t *>(dstp)[7] = word;

      dstp += 8 * OPSIZ;
      length--;
    }

    length %= OPSIZ * 8;

    // Write 1 `op_t` per iteration
    length = length / OPSIZ;

    while (length > 0) {
      reinterpret_cast<op_t *>(dstp)[0] = word;
      dstp += OPSIZ;
      length--;
    }

    length %= OPSIZ;
  }

  // Write the last few bytes
  while (len > 0) {
    reinterpret_cast<uint8_t *>(dstp)[0] = c;
    dstp++;
    len--;
  }

  return dst;
}