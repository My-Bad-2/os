#include <klibc/string.h>

size_t strlen(const char *str) {
  const char *end = str;

  while (*end != '\0') {
    end++;
  }

  return (size_t)(end - str);
}

size_t strnlen(const char *str, size_t maxlen) {
  const char *end = str;
  size_t length = 0;

  while ((*end != '\0') && (length <= maxlen)) {
    end++;
    length++;
  }

  return length;
}