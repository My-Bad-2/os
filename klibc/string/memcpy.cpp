#include <klibc/internal/memcopy.hpp>
#include <klibc/string.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t len) {
  uintptr_t destp = reinterpret_cast<uintptr_t>(dest);
  uintptr_t srcp = reinterpret_cast<uintptr_t>(src);

  if (len >= OP_T_THRES) {
    len -= (-destp) % OPSIZ;

    BYTE_COPY_FWD(destp, srcp, (-destp) % OPSIZ);
    WORD_COPY_FWD(destp, srcp, len, len);
  }

  BYTE_COPY_FWD(destp, srcp, len);

  return dest;
}