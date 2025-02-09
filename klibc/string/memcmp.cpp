#include <klibc/internal/memcopy.hpp>
#include <klibc/string.h>

namespace {
int memcmp_common_alignment(unsigned long srcp1, unsigned long srcp2,
                            size_t len) {
  op_t a0, a1;
  op_t b0, b1;

  switch (len % 4) {
  default:
  case 2:
    a0 = reinterpret_cast<op_t *>(srcp1)[0];
    b0 = reinterpret_cast<op_t *>(srcp2)[0];

    srcp1 -= 2 * OPSIZ;
    srcp2 -= 2 * OPSIZ;
    len += 2;

    goto do1;
  case 3:
    a1 = reinterpret_cast<op_t *>(srcp1)[0];
    b1 = reinterpret_cast<op_t *>(srcp2)[0];

    srcp1 -= OPSIZ;
    srcp2 -= OPSIZ;
    len++;

    goto do2;
  case 0:
    if ((OP_T_THRES <= (3 * OPSIZ)) && (len == 0)) {
      return 0;
    }

    a0 = reinterpret_cast<op_t *>(srcp1)[0];
    b0 = reinterpret_cast<op_t *>(srcp2)[0];

    goto do3;
  case 1:
    a1 = reinterpret_cast<op_t *>(srcp1)[0];
    b1 = reinterpret_cast<op_t *>(srcp2)[0];

    srcp1 += OPSIZ;
    srcp2 += OPSIZ;
    len--;

    if ((OP_T_THRES <= (3 * OPSIZ)) && (len == 0)) {
      goto do0;
    }
  }

  do {
    a0 = reinterpret_cast<op_t *>(srcp1)[0];
    b0 = reinterpret_cast<op_t *>(srcp2)[0];

    if (a1 != b1) {
      return CMP_LT_OR_GT(a1, b1);
    }

  do3:
    a1 = reinterpret_cast<op_t *>(srcp1)[1];
    b1 = reinterpret_cast<op_t *>(srcp2)[1];

    if (a0 != b0) {
      return CMP_LT_OR_GT(a0, b0);
    }

  do2:
    a0 = reinterpret_cast<op_t *>(srcp1)[2];
    b0 = reinterpret_cast<op_t *>(srcp2)[2];

    if (a1 != b1) {
      return CMP_LT_OR_GT(a1, b1);
    }

  do1:
    a1 = reinterpret_cast<op_t *>(srcp1)[3];
    b1 = reinterpret_cast<op_t *>(srcp2)[3];

    if (a0 != b0) {
      return CMP_LT_OR_GT(a0, b0);
    }

    srcp1 += 4 * OPSIZ;
    srcp2 += 4 * OPSIZ;
    len -= 4;
  } while (len != 0);

do0:
  if (a1 != b1) {
    return CMP_LT_OR_GT(a1, b1);
  }

  return 0;
}

int memcmp_not_common_alignment(unsigned long srcp1, unsigned long srcp2,
                                size_t len) {
  op_t a0, a1, a2, a3;
  op_t b0, b1, b2, b3;
  op_t x;
  int shl, shr;

  shl = 8 * (srcp1 % OPSIZ);
  shr = 8 * OPSIZ - shl;

  srcp1 &= -OPSIZ;

  switch (len % 4) {
  default:
  case 2:
    a1 = ((op_t *)srcp1)[0];
    a2 = ((op_t *)srcp1)[1];
    b2 = ((op_t *)srcp2)[0];

    srcp1 -= 1 * OPSIZ;
    srcp2 -= 2 * OPSIZ;
    len += 2;

    goto do1;
  case 3:
    a0 = ((op_t *)srcp1)[0];
    a1 = ((op_t *)srcp1)[1];
    b1 = ((op_t *)srcp2)[0];

    srcp2 -= 1 * OPSIZ;
    len++;

    goto do2;
  case 0:
    if (OP_T_THRES <= 3 * OPSIZ && len == 0) {
      return 0;
    }

    a3 = ((op_t *)srcp1)[0];
    a0 = ((op_t *)srcp1)[1];
    b0 = ((op_t *)srcp2)[0];

    srcp1 += 1 * OPSIZ;
    goto do3;
  case 1:
    a2 = ((op_t *)srcp1)[0];
    a3 = ((op_t *)srcp1)[1];
    b3 = ((op_t *)srcp2)[0];

    srcp1 += 2 * OPSIZ;
    srcp2 += 1 * OPSIZ;
    len--;

    if (OP_T_THRES <= 3 * OPSIZ && len == 0) {
      goto do0;
    }
  }

  do {
    a0 = ((op_t *)srcp1)[0];
    b0 = ((op_t *)srcp2)[0];
    x = MERGE(a2, shl, a3, shr);

    if (x != b3) {
      return CMP_LT_OR_GT(x, b3);
    }
  do3:
    a1 = ((op_t *)srcp1)[1];
    b1 = ((op_t *)srcp2)[1];
    x = MERGE(a3, shl, a0, shr);

    if (x != b0) {
      return CMP_LT_OR_GT(x, b0);
    }

  do2:
    a2 = ((op_t *)srcp1)[2];
    b2 = ((op_t *)srcp2)[2];
    x = MERGE(a0, shl, a1, shr);

    if (x != b1) {
      return CMP_LT_OR_GT(x, b1);
    }

  do1:
    a3 = ((op_t *)srcp1)[3];
    b3 = ((op_t *)srcp2)[3];
    x = MERGE(a1, shl, a2, shr);

    if (x != b2) {
      return CMP_LT_OR_GT(x, b2);
    }

    srcp1 += 4 * OPSIZ;
    srcp2 += 4 * OPSIZ;
    len -= 4;
  } while (len != 0);

do0:
  x = MERGE(a2, shl, a3, shr);

  if (x != b3) {
    return CMP_LT_OR_GT(x, b3);
  }

  return 0;
}
} // namespace

int memcmp(const void *src1, const void *src2, size_t len) {
  op_t a0;
  op_t b0;
  unsigned long int srcp1 = (unsigned long int)src1;
  unsigned long int srcp2 = (unsigned long int)src2;
  op_t res;

  if (len >= OP_T_THRES) {
    while (srcp2 % OPSIZ != 0) {
      a0 = ((byte *)srcp1)[0];
      b0 = ((byte *)srcp2)[0];
      srcp1 += 1;
      srcp2 += 1;
      res = a0 - b0;

      if (res != 0) {
        return res;
      }

      len -= 1;
    }

    if (srcp1 % OPSIZ == 0) {
      res = memcmp_common_alignment(srcp1, srcp2, len / OPSIZ);
    } else {
      res = memcmp_not_common_alignment(srcp1, srcp2, len / OPSIZ);
    }

    if (res != 0) {
      return res;
    }

    // Number of bytes remaining in the interval [0..OPSIZ-1].
    srcp1 += len & -OPSIZ;
    srcp2 += len & -OPSIZ;
    len %= OPSIZ;
  }

  // There are just a few bytes to compare. Use byte memory operations.
  while (len != 0) {
    a0 = ((byte *)srcp1)[0];
    b0 = ((byte *)srcp2)[0];

    srcp1++;
    srcp2++;

    res = a0 - b0;

    if (res != 0) {
      return (int)res;
    }

    len--;
  }

  return 0;
}