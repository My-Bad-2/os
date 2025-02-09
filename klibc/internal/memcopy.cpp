#include <klibc/internal/memcopy.hpp>

void wordcopy_fwd_aligned(unsigned long dstp, unsigned long srcp, size_t len) {
  op_t a0 = 0;
  op_t a1 = 0;

  if (len & 1) {
    reinterpret_cast<op_t *>(dstp)[0] = reinterpret_cast<op_t *>(srcp)[0];

    if (len == 1) {
      return;
    }

    srcp += OPSIZ;
    dstp += OPSIZ;
    len--;
  }

  do {
    a0 = reinterpret_cast<op_t *>(srcp)[0];
    a1 = reinterpret_cast<op_t *>(srcp)[1];

    reinterpret_cast<op_t *>(dstp)[0] = a0;
    reinterpret_cast<op_t *>(dstp)[1] = a1;

    srcp += 2 * OPSIZ;
    dstp += 2 * OPSIZ;
    len -= 2;
  } while (len != 0);
}

void wordcopy_fwd_dest_aligned(unsigned long dstp, unsigned long srcp,
                               size_t len) {
  op_t a0 = 0;
  op_t a1 = 0;
  op_t a2 = 0;

  int sh_1 = 8 * (srcp % OPSIZ);
  int sh_2 = 8 * OPSIZ - sh_1;

  srcp &= -OPSIZ;
  a0 = reinterpret_cast<op_t *>(srcp)[0];

  if (len & 1) {
    a1 = reinterpret_cast<op_t *>(srcp)[1];
    reinterpret_cast<op_t *>(dstp)[0] = MERGE(a0, sh_1, a1, sh_2);

    if (len == 1) {
      return;
    }

    a0 = a1;

    srcp += OPSIZ;
    dstp += OPSIZ;
    len--;
  }

  do {
    a1 = reinterpret_cast<op_t *>(srcp)[1];
    a2 = reinterpret_cast<op_t *>(srcp)[2];

    reinterpret_cast<op_t *>(dstp)[0] = MERGE(a0, sh_1, a1, sh_2);
    reinterpret_cast<op_t *>(dstp)[1] = MERGE(a1, sh_1, a2, sh_2);

    a0 = a2;

    srcp += 2 * OPSIZ;
    dstp += 2 * OPSIZ;
    len -= 2;
  } while (len != 0);
}

void wordcopy_bwd_aligned(unsigned long dstp, unsigned long srcp, size_t len) {
  op_t a0 = 0;
  op_t a1 = 0;

  if (len & 1) {
    srcp -= OPSIZ;
    dstp -= OPSIZ;

    reinterpret_cast<op_t *>(dstp)[0] = reinterpret_cast<op_t *>(srcp)[0];

    if (len == 1) {
      return;
    }

    len -= 1;
  }

  do {
    srcp -= 2 * OPSIZ;
    dstp -= 2 * OPSIZ;

    a1 = reinterpret_cast<op_t *>(srcp)[1];
    a0 = reinterpret_cast<op_t *>(srcp)[0];

    reinterpret_cast<op_t *>(dstp)[1] = a1;
    reinterpret_cast<op_t *>(dstp)[0] = a0;

    len -= 2;
  } while (len != 0);
}

void wordcopy_bwd_dest_aligned(unsigned long dstp, unsigned long srcp,
                               size_t len) {
  op_t a0 = 0;
  op_t a1 = 0;
  op_t a2 = 0;

  int sh_1 = 8 * (srcp % OPSIZ);
  int sh_2 = 8 * OPSIZ - sh_1;

  srcp &= -OPSIZ;
  a2 = reinterpret_cast<op_t *>(srcp)[0];

  if (len & 1) {
    srcp -= OPSIZ;
    dstp -= OPSIZ;
    a1 = reinterpret_cast<op_t *>(srcp)[0];

    reinterpret_cast<op_t *>(dstp)[0] = MERGE(a1, sh_1, a2, sh_2);

    if (len == 1) {
      return;
    }

    a2 = a1;
    len--;
  }

  do {
    srcp -= 2 * OPSIZ;
    dstp -= 2 * OPSIZ;

    a1 = reinterpret_cast<op_t *>(srcp)[1];
    a0 = reinterpret_cast<op_t *>(srcp)[0];

    reinterpret_cast<op_t *>(dstp)[1] = MERGE(a1, sh_1, a2, sh_2);
    reinterpret_cast<op_t *>(dstp)[0] = MERGE(a0, sh_1, a1, sh_2);

    a2 = a0;
    len -= 2;
  } while (len != 0);
}

#if __BYTE_ORDER == __LITTLE_ENDIAN
int memcmp_bytes(op_t byte1, op_t byte2) {
  byte *srcp1 = reinterpret_cast<byte *>(&byte1);
  byte *srcp2 = reinterpret_cast<byte *>(&byte2);

  op_t a0 = 0;
  op_t b0 = 0;

  do {
    a0 = reinterpret_cast<byte *>(srcp1)[0];
    b0 = reinterpret_cast<byte *>(srcp2)[0];

    srcp1++;
    srcp2++;
  } while (a0 == b0);

  return a0 - b0;
}
#endif