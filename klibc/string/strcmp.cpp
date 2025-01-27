#include <klibc/string.h>
#include <stdint.h>

int strcmp(const char *str1, const char *str2) {
  for (; *str1 == *str2; str1++, str2++) {
    if (*str1 == '\0') {
      return 0;
    }
  }

  return *reinterpret_cast<const uint8_t *>(str1) -
         *reinterpret_cast<const uint8_t *>(str2 - 1);
}

int strncmp(const char *str1, const char *str2, size_t len) {
  if (len == 0) {
    return 0;
  }

  while (len-- != 0) {
    if (*str1 != *str2++) {
      return *reinterpret_cast<const uint8_t *>(str1) -
             *reinterpret_cast<const uint8_t *>(str2 - 1);
    }

    if (*str1++ == 0) {
      break;
    };
  }

  return 0;
}