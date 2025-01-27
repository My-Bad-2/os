#include <klibc/ctype.h>
#include <klibc/stdlib.h>
#include <limits.h>

#define BASE_HEX 16
#define BASE_DECIMAL 10
#define BASE_OCTAL 8

unsigned long strtoul(const char *restrict nptr, char **restrict endptr,
                      int base) {
  const char *s = nptr;
  unsigned long acc, cutoff;
  int c, neg = 0, any, cutlim;

  do {
    c = *s++;
  } while (isspace(c));

  if (c == '-') {
    neg = 1;
    c = *s++;
  } else if (c == '+') {
    c = *s++;
  }

  if (((base == 0) || (base == BASE_HEX)) &&
      (c == '0' && (*s == 'x' || *s == 'X'))) {
    c = s[1];
    s += 2;
    base = BASE_HEX;
  }

  if (base == 0) {
    base = (c == '0') ? BASE_OCTAL : BASE_DECIMAL;
  }

  cutoff = ULONG_MAX / base;
  cutlim = ULONG_MAX % base;

  for (acc = 0, any = 0;; c = *s++) {
    if (isdigit(c)) {
      c -= '0';
    } else if (isalpha(c)) {
      c -= isupper(c) ? 'A' - BASE_DECIMAL : 'a' - BASE_DECIMAL;
    } else {
      break;
    }

    if (c >= base) {
      break;
    }

    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
      any = -1;
    } else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }

  if (any < 0) {
    acc = ULONG_MAX;
  } else if (neg) {
    acc = -acc;
  }

  if (endptr != nullptr) {
    *endptr = const_cast<char *>(any ? (s - 1) : nptr);
  }

  return acc;
}