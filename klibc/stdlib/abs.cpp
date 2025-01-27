#include <klibc/stdlib.h>

int abs(int num) { return num < 0 ? -num : num; }

long labs(long num) { return num < 0 ? -num : num; }

long long llabs(long long num) { return num < 0 ? -num : num; }