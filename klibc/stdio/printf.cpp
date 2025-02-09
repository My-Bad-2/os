#include <klibc/stdio.h>

int sprintf(char *restrict buffer, const char *restrict format, ...) {
  va_list ap;
  va_start(ap, format);
  int ret = vsprintf(buffer, format, ap);
  va_end(ap);

  return ret;
}

int snprintf(char *restrict buffer, size_t maxlen, const char *restrict format,
             ...) {
  va_list ap;
  va_start(ap, format);
  int ret = vsnprintf(buffer, maxlen, format, ap);
  va_end(ap);

  return ret;
}

int fprintf(FILE *restrict stream, const char *restrict format, ...) {
  va_list ap;
  va_start(ap, format);
  int ret = vfprintf(stream, format, ap);
  va_end(ap);

  return ret;
}

int printf(const char *restrict format, ...) {
  va_list ap;
  va_start(ap, format);
  int ret = vfprintf(stdout, format, ap);
  va_end(ap);

  return ret;
}