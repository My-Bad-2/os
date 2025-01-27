#include <klibc/ctype.h>
#include <klibc/stdlib.h>
#include <limits.h>

#define BASE_HEX 16
#define BASE_DECIMAL 10
#define BASE_OCTAL 8

long int strtol(const char *restrict nptr, char **restrict endptr, int base) {
  const char *str = nptr;

  size_t acc = 0;
  size_t cutoff = 0;

  int cutlim = 0;
  int sym = 0;
  int neg = 0;
  int any = 0;

  do {
    sym = *str++;
  } while (isspace(sym));

  if (sym == '-') {
    neg = 1;
    sym = *str++;
  } else if (sym == '+') {
    sym = *str++;
  }

  if (((base == 0) || (base == BASE_HEX)) &&
      (sym == '0' && (*str == 'x' || *str == 'X'))) {
    sym = str[1];
    str += 2;
    base = BASE_HEX;
  }

  if (base == 0) {
    base = (sym == '0') ? BASE_OCTAL : BASE_DECIMAL;
  }

  cutoff = neg ? -static_cast<size_t>(LONG_MIN) : LONG_MAX;
  cutlim = cutoff % base;
  cutoff /= base;

  for (acc = 0, any = 0;; sym = *str++) {
    if (isdigit(sym)) {
      sym -= '0';
    } else if (isalpha(sym)) {
      sym -= isupper(sym) ? ('A' - BASE_DECIMAL) : ('a' - BASE_DECIMAL);
    } else {
      break;
    }

    if (sym >= base) {
      break;
    }

    if (any < 0 || acc > cutoff || (acc == cutoff && sym > cutlim)) {
      any = -1;
    } else {
      any = 1;
      acc *= base;
      acc += sym;
    }
  }

  if (any < 0) {
    acc = neg ? LONG_MIN : LONG_MAX;
  } else if (neg) {
    acc = -acc;
  }

  if (endptr != nullptr) {
    *endptr = const_cast<char *>(any ? (str - 1) : nptr);
  }

  return acc;
}

int atoi(const char *nptr) { return strtol(nptr, nullptr, BASE_DECIMAL); }

long atol(const char *nptr) { return strtol(nptr, nullptr, BASE_DECIMAL); }