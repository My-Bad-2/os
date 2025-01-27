#include <klibc/stdlib.h>
#include <utility>

void abort() { std::unreachable(); }