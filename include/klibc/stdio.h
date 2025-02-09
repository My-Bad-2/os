#ifndef KLIBC_STDIO_H
#define KLIBC_STDIO_H 1

#include <compiler.h>

__CDECLS_BEGIN

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#define EOF (-1)

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define BUFSIZ 1024

typedef struct _IO_FILE FILE;

extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;

bool ferror(FILE *stream) __WARN_UNUSED_RESULT __NONNULL((1));
bool feof(FILE *stream) __WARN_UNUSED_RESULT __NONNULL((1));
size_t fwrite(const void *restrict ptr, size_t size, size_t nmemb,
              FILE *restrict stream) __NONNULL((4));

int putchar(int ch);
int putc(int ch, FILE *stream) __NONNULL((2));

int fputs(const char *restrict str, FILE *restrict stream) __NONNULL((2));
int puts(const char *str);

int fprintf(FILE *restrict stream, const char *restrict format, ...)
    __NONNULL((1));
int printf(const char *restrict format, ...);
int sprintf(char *restrict buffer, const char *restrict format, ...);
int snprintf(char *restrict buffer, size_t maxlen, const char *restrict format,
             ...) __PRINTFLIKE(3, 4);

int vfprintf(FILE *restrict stream, const char *restrict format, va_list arg)
    __NONNULL((1));
int vprintf(const char *restrict format, va_list arg);
int vsprintf(char *restrict buffer, const char *restrict format, va_list arg);
int vsnprintf(char *restrict buffer, size_t maxlen, const char *restrict format,
              va_list arg) __PRINTFLIKE(3, 0);

__CDECLS_END

#endif // KLIBC_STDIO_H