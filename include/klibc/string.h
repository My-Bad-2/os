#ifndef KLIBC_STRING_H
#define KLIBC_STRING_H 1

#include <compiler.h>
#include <stddef.h>

__CDECLS_BEGIN

void *memcpy(void *restrict dest, const void *restrict src, size_t len)
    __NONNULL((1, 2));
void *memmove(void *dest, const void *src, size_t len) __NONNULL((1, 2));
void *memset(void *dest, int symbol, size_t len) __NONNULL((1));
int memcmp(const void *src1, const void *src2, size_t len) __PURE
    __NONNULL((1, 2));

char *strcpy(char *restrict dest, const char *restrict src) __NONNULL((1, 2));
char *strncpy(char *restrict dest, const char *restrict src, size_t len)
    __NONNULL((1, 2));

char *strcat(char *restrict dest, const char *restrict src) __NONNULL((1, 2));
char *strncat(char *restrict dest, const char *restrict src, size_t len)
    __NONNULL((1, 2));

int strcmp(const char *str1, const char *str2) __PURE __NONNULL((1, 2));
int strncmp(const char *str1, const char *str2, size_t len) __PURE
    __NONNULL((1, 2));

size_t strnlen(const char *str, size_t maxlen) __PURE __NONNULL((1));
size_t strlen(const char *str) __PURE __NONNULL((1));

__CDECLS_END

#endif // KLIBC_STRING_H