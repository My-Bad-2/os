#ifndef KLIBC_INTERNAL_MEMCOPY_HPP
#define KLIBC_INTERNAL_MEMCOPY_HPP

#pragma once

#include <compiler.h>
#include <endian.h>
#include <stddef.h>
#include <stdint.h>

#define OP_T_THRES 16

typedef unsigned long __MAY_ALIAS op_t;
typedef unsigned char byte;

#define OPSIZ (sizeof(op_t))

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define MERGE(w0, sh_1, w1, sh_2) (((w0) >> (sh_1)) | ((w1) << (sh_2)))
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
#define MERGE(w0, sh_1, w1, sh_2) (((w0) << (sh_1)) | ((w1) >> (sh_2)))
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
#define CMP_LT_OR_GT(a, b) ((a) > (b) ? 1 : -1)
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
int memcmp_bytes(op_t byte1, op_t byte2);
#define CMP_LT_OR_GT(a, b) memcmp_bytes((a), (b))
#endif

#define BYTE_COPY_FWD(dst_bp, src_bp, nbytes)                                  \
  do {                                                                         \
    size_t __nbytes = (nbytes);                                                \
    while (__nbytes > 0) {                                                     \
      byte __x = reinterpret_cast<byte *>(src_bp)[0];                          \
      (src_bp) += 1;                                                           \
      __nbytes -= 1;                                                           \
      reinterpret_cast<byte *>(dst_bp)[0] = __x;                               \
      (dst_bp) += 1;                                                           \
    }                                                                          \
  } while (0)

#define BYTE_COPY_BWD(dst_bp, src_bp, nbytes)                                  \
  do {                                                                         \
    size_t __nbytes = (nbytes);                                                \
    while (__nbytes > 0) {                                                     \
      byte __x;                                                                \
      (src_bp) -= 1;                                                           \
      __x = reinterpret_cast<byte *>(src_bp)[0];                               \
      (dst_bp) -= 1;                                                           \
      __nbytes -= 1;                                                           \
      reinterpret_cast<byte *>(dst_bp)[0] = __x;                               \
    }                                                                          \
  } while (0)

void wordcopy_fwd_aligned(unsigned long, unsigned long, size_t) __LOCAL;
void wordcopy_fwd_dest_aligned(unsigned long, unsigned long, size_t) __LOCAL;

#define WORD_COPY_FWD(dst_bp, src_bp, nbytes_left, nbytes)                     \
  do {                                                                         \
    if ((src_bp) % OPSIZ == 0) {                                               \
      wordcopy_fwd_aligned(dst_bp, src_bp, (nbytes) / OPSIZ);                  \
    } else {                                                                   \
      wordcopy_fwd_dest_aligned(dst_bp, src_bp, (nbytes) / OPSIZ);             \
    }                                                                          \
    (src_bp) += (nbytes) & -OPSIZ;                                             \
    (dst_bp) += (nbytes) & -OPSIZ;                                             \
    (nbytes_left) = (nbytes) % OPSIZ;                                          \
  } while (0)

void wordcopy_bwd_aligned(unsigned long, unsigned long, size_t) __LOCAL;
void wordcopy_bwd_dest_aligned(unsigned long, unsigned long, size_t) __LOCAL;

#define WORD_COPY_BWD(dst_ep, src_ep, nbytes_left, nbytes)                     \
  do {                                                                         \
    if ((src_ep) % OPSIZ == 0) {                                               \
      wordcopy_bwd_aligned(dst_ep, src_ep, (nbytes) / OPSIZ);                  \
    } else {                                                                   \
      wordcopy_bwd_dest_aligned(dst_ep, src_ep, (nbytes) / OPSIZ);             \
    }                                                                          \
    (src_ep) -= (nbytes) & -OPSIZ;                                             \
    (dst_ep) -= (nbytes) & -OPSIZ;                                             \
    (nbytes_left) = (nbytes) % OPSIZ;                                          \
  } while (0)

#endif // KLIBC_INTERNAL_MEMCOPY_HPP