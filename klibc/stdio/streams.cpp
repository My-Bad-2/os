#include <klibc/internal/printf.hpp>

namespace {
uint8_t stderr_buffer[UNGET] = {};
uint8_t stdin_buffer[BUFSIZ + UNGET] = {};
uint8_t stdout_buffer[BUFSIZ + UNGET] = {};

// NOLINTBEGIN
__LOCAL FILE stderr_file = {
    .write = stdio::write,
    .buffer = stderr_buffer + UNGET,
    .buf_size = 0,
    .flags = stdio::FILE_PERM | stdio::FILE_NO_READ | stdio::FILE_SVB,
    .lbf = -1,
};

__LOCAL FILE stdin_file = {
    .buffer = stdin_buffer + UNGET,
    .buf_size = sizeof(stdin_buffer) - UNGET,
    .flags = stdio::FILE_PERM | stdio::FILE_NO_WRITE,
};

__LOCAL FILE stdout_file = {
    .write = stdio::write,
    .buffer = stdout_buffer + UNGET,
    .buf_size = sizeof(stdout_buffer) - UNGET,
    .flags = stdio::FILE_PERM | stdio::FILE_NO_READ,
    .lbf = '\n',
};
// NOLINTEND
} // namespace

FILE *const stdout = &stdout_file;
FILE *const stdin = &stdin_file;
FILE *const stderr = &stderr_file;