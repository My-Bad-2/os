/**
 * @file
 * @brief This file provides the declarations for kernel-level API calls related
 * to I/O operations.
 *
 * The file defines:
 * - The `iovec` structure, which is used for vectorized I/O operations,
 * representing a buffer and its length.
 * - The `kernel_writev` function, which allows writing data from multiple
 * buffers to an output device.
 *
 * These API calls facilitate the efficient handling of I/O by supporting
 * multiple buffer writes in a single operation.
 */
#ifndef KERNEL_API_CALLS_H
#define KERNEL_API_CALLS_H 1

#include <compiler.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @struct iovec
 * @brief Represents a buffer and its length for vectorized I/O operations.
 */
struct iovec {
  void *buffer; ///< Starting address of the buffer.
  size_t len;   ///< Length of the buffer in bytes.
};

__CDECLS_BEGIN

/**
 * @brief Writes data from multiple iov to the output device.
 * @param iov Array of `iovec` structures containing the iovs to write.
 * @param iovcnt Number of iovs in the array.
 * @return The total number of bytes written, or -1 on error.
 */
int kernel_writev(struct iovec *iov, int iovcnt);

__CDECLS_END

#endif // KERNEL_API_CALLS_H