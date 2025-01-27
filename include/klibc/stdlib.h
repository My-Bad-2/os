#ifndef KLIBC_STDLIB_H
#define KLIBC_STDLIB_H 1

#include <compiler.h>
#include <stddef.h>

__CDECLS_BEGIN

typedef struct {
  int quot;
  int rem;
} div_t;

typedef struct {
  long int quot;
  long int rem;
} ldiv_t;

typedef struct {
  long long int quot;
  long long int rem;
} lldiv_t;

long int strtol(const char *restrict nptr, char **restrict endptr, int base)
    __NONNULL((1));
unsigned long int strtoul(const char *restrict nptr, char **restrict endptr,
                          int base) __NONNULL((1));

long long int strtoll(const char *restrict nptr, char **restrict endptr,
                      int base) __NONNULL((1));
unsigned long long int strtoull(const char *restrict nptr,
                                char **restrict endptr, int base)
    __NONNULL((1));

int atoi(const char *nptr) __PURE __NONNULL((1));
long atol(const char *nptr) __PURE __NONNULL((1));
long atoll(const char *nptr) __PURE __NONNULL((1));
unsigned long atou(const char *nptr) __PURE __NONNULL((1));

int abs(int num) __CONST;
long int labs(long int num) __CONST;
long long int llabs(long long int num) __CONST;

div_t div(int numer, int denom) __CONST;
ldiv_t ldiv(long int numer, long int denom) __CONST;
lldiv_t lldiv(long long int numer, long long int denom) __CONST;

extern void abort(void) __NO_RETURN;

__CDECLS_END

#endif // KLIBC_STDLIB_H