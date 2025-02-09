#ifndef KERNEL_ARCH_X86_64_ARCH_HPP
#define KERNEL_ARCH_X86_64_ARCH_HPP 1

#include <compiler.h>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace kernel::arch {
inline void pause() { asm volatile("pause"); }

inline void enable_interrupts() { asm volatile("sti"); }

inline void disable_interrupts() { asm volatile("cli"); }

inline void halt() { asm volatile("hlt"); }

template <std::unsigned_integral T>
  requires(sizeof(T) <= sizeof(uint32_t))
void outp(uint16_t port, T val) {
  if constexpr (std::same_as<T, uint8_t>) {
    asm volatile("outb %0, %1" ::"a"(val), "Nd"(port));
  }

  if constexpr (std::same_as<T, uint16_t>) {
    asm volatile("outw %0, %1" ::"a"(val), "Nd"(port));
  }

  if constexpr (std::same_as<T, uint32_t>) {
    asm volatile("outl %0, %1" ::"a"(val), "Nd"(port));
  }
}

template <std::unsigned_integral T>
  requires(sizeof(T) <= sizeof(uint32_t))
T inp(uint16_t port) {
  T ret = 0;

  if constexpr (std::same_as<T, uint8_t>) {
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  }

  if constexpr (std::same_as<T, uint16_t>) {
    asm volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
  }

  if constexpr (std::same_as<T, uint32_t>) {
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  }

  return ret;
}

__NO_RETURN void halt_and_spin(bool interrupts = true);
size_t write(std::string_view buf);
void initialize();
} // namespace kernel::arch

#endif // KERNEL_ARCH_X86_64_ARCH_HPP