#include <klibc/ctype.h>
#include <klibc/stdlib.h>

#define BASE_DECIMAL 10

unsigned long atou(const char *nptr) {
  const char *end = nptr;
  unsigned long val = 0;

  while (isdigit(*end)) {
    val = (val * BASE_DECIMAL) + static_cast<size_t>(*(end++) - '0');
  }

  return val;
}