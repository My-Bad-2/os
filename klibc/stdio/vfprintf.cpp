#include <algorithm>
#include <array>
#include <klibc/ctype.h>
#include <klibc/internal/printf.hpp>
#include <klibc/string.h>
#include <limits>

#ifdef USE_FPU
#include <float.h>
#endif

namespace {
enum format_flags : uint32_t {
  ALT_FORM = (1u << ('#' - ' ')),
  ZERO_PAD = (1u << ('0' - ' ')),
  LEFT_ADJ = (1u << ('-' - ' ')),
  PAD_POS = (1u << (' ' - ' ')),
  MARK_POS = (1u << ('+' - ' ')),
  GROUPED = (1u << ('\'' - ' ')),

  FLAG_MASK = (ALT_FORM | ZERO_PAD | LEFT_ADJ | PAD_POS | MARK_POS | GROUPED),
};

enum argument_type {
  BARE,
  LPRE,
  LLPRE,
  HPRE,
  HHPRE,
  BIGLPRE,
  ZTPRE,
  JPRE,
  STOP,
  PTR,
  INT,
  UINT,
  ULLONG,
  LONG,
  ULONG,
  SHORT,
  USHORT,
  CHAR,
  UCHAR,
  LLONG,
  SIZET,
  IMAX,
  UMAX,
  PDIFF,
  UIPTR,
#ifdef USE_FPU
  DBL,
  LDBL,
#endif // USE_FPU
  NOARG,
  MAXSTATE,
};

#define S(x) [(x) - 'A']

constexpr std::array<char, 16> hex_digits = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};

// NOLINTBEGIN
constexpr uint8_t states[]['z' - 'A' + 1] = {
    {
        /* 0: bare types */
        S('d') = INT,   S('i') = INT,     S('o') = UINT,  S('u') = UINT,
        S('x') = UINT,  S('X') = UINT,
#ifdef USE_FPU
        S('e') = DBL,   S('f') = DBL,     S('g') = DBL,   S('a') = DBL,
        S('E') = DBL,   S('F') = DBL,     S('G') = DBL,   S('A') = DBL,
#endif // USE_FPU
        S('c') = INT,   S('C') = UINT,    S('s') = PTR,   S('S') = PTR,
        S('p') = UIPTR, S('n') = PTR,     S('m') = NOARG, S('l') = LPRE,
        S('h') = HPRE,  S('L') = BIGLPRE, S('z') = ZTPRE, S('j') = JPRE,
        S('t') = ZTPRE,
    },
    {
        /* 1: l-prefixed */
        S('d') = LONG,
        S('i') = LONG,
        S('o') = ULONG,
        S('u') = ULONG,
        S('x') = ULONG,
        S('X') = ULONG,
#ifdef USE_FPU
        S('e') = DBL,
        S('f') = DBL,
        S('g') = DBL,
        S('a') = DBL,
        S('E') = DBL,
        S('F') = DBL,
        S('G') = DBL,
        S('A') = DBL,
#endif // USE_FPU
        S('c') = UINT,
        S('s') = PTR,
        S('n') = PTR,
        S('l') = LLPRE,
    },
    {
        /* 2: ll-prefixed */
        S('d') = LLONG,
        S('i') = LLONG,
        S('o') = ULLONG,
        S('u') = ULLONG,
        S('x') = ULLONG,
        S('X') = ULLONG,
        S('n') = PTR,
    },
    {
        /* 3: h-prefixed */
        S('d') = SHORT,
        S('i') = SHORT,
        S('o') = USHORT,
        S('u') = USHORT,
        S('x') = USHORT,
        S('X') = USHORT,
        S('n') = PTR,
        S('h') = HHPRE,
    },
    {
        /* 4: hh-prefixed */
        S('d') = CHAR,
        S('i') = CHAR,
        S('o') = UCHAR,
        S('u') = UCHAR,
        S('x') = UCHAR,
        S('X') = UCHAR,
        S('n') = PTR,
    },
    {
/* 5: L-prefixed */
#ifdef USE_FPU
        S('e') = LDBL,
        S('f') = LDBL,
        S('g') = LDBL,
        S('a') = LDBL,
        S('E') = LDBL,
        S('F') = LDBL,
        S('G') = LDBL,
        S('A') = LDBL,
#endif // USE_FPU
        S('n') = PTR,
    },
    {
        /* 6: z- or t-prefixed (assumed to be same size) */
        S('d') = PDIFF,
        S('i') = PDIFF,
        S('o') = SIZET,
        S('u') = SIZET,
        S('x') = SIZET,
        S('X') = SIZET,
        S('n') = PTR,
    },
    {
        /* 7: j-prefixed */
        S('d') = IMAX,
        S('i') = IMAX,
        S('o') = UMAX,
        S('u') = UMAX,
        S('x') = UMAX,
        S('X') = UMAX,
        S('n') = PTR,
    },
};
// NOLINTEND

union format_args {
  uintmax_t i;
  void *p;
#ifdef USE_FPU
  long double f;
#endif
};

constexpr bool out_of_bound(uint8_t x) { return (x - 'A') > ('z' - 'A'); }

void pop_arg(format_args *arg, int type, va_list *ap) {
  switch (type) {
  case PTR:
    arg->p = va_arg(*ap, void *);
    break;
  case INT:
    arg->i = va_arg(*ap, unsigned int);
    break;
  case UINT:
    arg->i = va_arg(*ap, unsigned int);
    break;
  case LONG:
    arg->i = va_arg(*ap, long);
    break;
  case ULONG:
    arg->i = va_arg(*ap, unsigned long);
    break;
  case ULLONG:
    arg->i = va_arg(*ap, unsigned long long);
    break;
  case SHORT:
    arg->i = (short)va_arg(*ap, int);
    break;
  case USHORT:
    arg->i = (unsigned short)va_arg(*ap, int);
    break;
  case CHAR:
    arg->i = (signed char)va_arg(*ap, int);
    break;
  case UCHAR:
    arg->i = (unsigned char)va_arg(*ap, int);
    break;
  case LLONG:
    arg->i = va_arg(*ap, long long);
    break;
  case SIZET:
    arg->i = va_arg(*ap, size_t);
    break;
  case IMAX:
    arg->i = va_arg(*ap, intmax_t);
    break;
  case UMAX:
    arg->i = va_arg(*ap, uintmax_t);
    break;
  case PDIFF:
    arg->i = va_arg(*ap, ptrdiff_t);
    break;
  case UIPTR:
    arg->i = (uintptr_t)va_arg(*ap, void *);
    break;
#ifdef USE_FPU
  case DBL:
    arg->f = va_arg(*ap, double);
    break;
  case LDBL:
    arg->f = va_arg(*ap, long double);
#endif
  }
}

void print(FILE *stream, const char *str, size_t length) {
  if (ferror(stream)) {
    return;
  }

  stdio::stream_write(reinterpret_cast<const uint8_t *>(str), length, stream);
}

void pad(FILE *stream, char c, int width, int length, int flags) {
  char pad[256] = {};

  if ((flags & (LEFT_ADJ | ZERO_PAD)) || (length >= width)) {
    return;
  }

  length = width - length;

  memset(pad, c, (size_t(length) > sizeof(pad)) ? sizeof(pad) : length);

  for (; size_t(length) >= sizeof(pad); length -= sizeof(pad)) {
    print(stream, pad, sizeof(pad));
  }

  print(stream, pad, length);
}

char *format_hex(uintmax_t x, char *str, bool lower) {
  for (; x; x >>= 4) {
    *--str = hex_digits[(x & 15)] | lower;
  }

  return str;
}

char *format_octal(uintmax_t x, char *str) {
  for (; x; x >>= 3) {
    *--str = '0' + (x & 7);
  }

  return str;
}

char *format_unsigned(uintmax_t x, char *str) {
  size_t y = 0;

  for (; x > std::numeric_limits<unsigned long>::max(); x /= 10) {
    *--str = '0' + (x % 10);
  }

  for (y = x; y >= 10; y /= 10) {
    *--str = '0' + (y % 10);
  }

  if (y != 0) {
    *--str = '0' + y;
  }

  return str;
}

int get_int(char **str) {
  int i = 0;

  for (i = 0; isdigit(**str); (*str)++) {
    if ((i > std::numeric_limits<int>::max() / 10) ||
        ((**str - '0') > (std::numeric_limits<int>::max() - (10 * i)))) {
      i = -1;
    } else {
      i = 10 * i + (**str - '0');
    }
  }

  return i;
}

int printf_core(FILE *stream, const char *fmt, va_list *ap, format_args *nl_arg,
                int *nl_type) {
  char *a, *z;
  char *str = const_cast<char *>(fmt);
  uint32_t l10n = 0;
  uint32_t flags;

  int width = 0;
  int precision = 0;
  bool xp = false;

  format_args arg = {0};
  int arg_pos = 0;

  uint32_t state = 0;
  uint32_t ps = 0;

  int count = 0;
  int length = 0;

  size_t i = 0;

  char buf[sizeof(uintmax_t) * 3];
  const char *prefix;
  int t = 0;
  int pl = 0;

  while (true) {
    if (length > (std::numeric_limits<int>::max() - count)) {
      return -1;
    }

    count += length;

    if (!*str) {
      break;
    }

    for (a = str; *str && *str != '%'; str++)
      ;

    for (z = str; str[0] == '%' && str[1] == '%'; z++, str += 2)
      ;

    if ((z - a) > (std::numeric_limits<int>::max() - count)) {
      return -1;
    }

    length = z - a;

    if (stream) {
      print(stream, a, length);
    }

    if (length) {
      continue;
    }

    if (isdigit(str[1]) && str[2] == '$') {
      l10n = 1;
      arg_pos = str[1] - '0';
      str += 3;
    } else {
      arg_pos = -1;
      str++;
    }

    for (flags = 0; ((static_cast<uint32_t>(*str) - ' ') < 32) &&
                    (FLAG_MASK & (1u << (*str - ' ')));
         str++) {
      flags |= 1u << (*str - ' ');
    }

    if (*str == '*') {
      if (isdigit(str[1]) && str[2] == '$') {
        l10n = 1;

        if (!stream) {
          nl_type[str[1] - '0'] = INT;
          width = 0;
        } else {
          width = nl_arg[str[1] - '0'].i;
        }

        str += 3;
      } else if (!l10n) {
        width = stream ? va_arg(*ap, int) : 0;
        str++;
      } else {
        return -1;
      }

      if (width < 0) {
        flags |= LEFT_ADJ;
        width = -width;
      }
    } else if ((width = get_int(&str)) < 0) {
      return -1;
    }

    // Read precision
    if (*str == '.' && str[1] == '*') {
      if (isdigit(str[2]) && str[3] == '$') {
        if (!stream) {
          nl_type[str[2] - '0'] = INT;
          precision = 0;
        } else {
          precision = nl_arg[str[2] - '0'].i;
        }

        str += 4;
      } else if (!l10n) {
        precision = stream ? va_arg(*ap, int) : 0;
        str += 2;
      } else {
        return -1;
      }

      xp = (precision >= 0);
    } else if (*str == '.') {
      str++;
      precision = get_int(&str);
      xp = 1;
    } else {
      precision = -1;
      xp = 0;
    }

    // Format specifier state machine
    state = 0;

    do {
      if (out_of_bound(*str)) {
        return -1;
      }

      ps = state;
      state = states[state] S(*str++);
    } while (state - 1 < STOP);

    if (!state) {
      return -1;
    }

    if (state == NOARG) {
      if (arg_pos >= 0) {
        return -1;
      }
    } else {
      if (arg_pos >= 0) {
        if (!stream) {
          nl_type[arg_pos] = state;
        } else {
          arg = nl_arg[arg_pos];
        }
      } else if (stream) {
        pop_arg(&arg, state, ap);
      } else {
        return 0;
      }
    }

    if (!stream) {
      continue;
    }

    // Donot process any new directives once in error state.
    if (ferror(stream)) {
      return -1;
    }

    z = buf + sizeof(buf);
    prefix = "-+   0X0x";
    pl = 0;
    t = str[-1];

    if (ps && ((t & 15) == 3)) {
      t &= ~32;
    }

    if (flags & LEFT_ADJ) {
      flags &= ~ZERO_PAD;
    }

    switch (t) {
    case 'n':
      switch (ps) {
      case BARE:
        *reinterpret_cast<int *>(arg.p) = count;
        break;
      case LPRE:
        *reinterpret_cast<long *>(arg.p) = count;
        break;
      case LLPRE:
        *reinterpret_cast<long long *>(arg.p) = count;
        break;
      case HPRE:
        *reinterpret_cast<unsigned short *>(arg.p) = count;
        break;
      case HHPRE:
        *reinterpret_cast<unsigned char *>(arg.p) = count;
        break;
      case ZTPRE:
        *reinterpret_cast<size_t *>(arg.p) = count;
        break;
      case JPRE:
        *reinterpret_cast<uintmax_t *>(arg.p) = count;
        break;
      }

      continue;
    case 'p':
      precision = std::max(static_cast<size_t>(precision), 2 * sizeof(void *));
      t = 'x';
      flags |= ALT_FORM;
    case 'x':
    case 'X':
      a = format_hex(arg.i, z, t & 32);

      if (arg.i && (flags & ALT_FORM)) {
        prefix += (t >> 4);
        pl = 2;
      }

      goto ifmt_tail;
    case 'o':
      a = format_octal(arg.i, z);

      if ((flags & ALT_FORM) && precision < (z - a + 1)) {
        precision = z - a + 1;
      }

      goto ifmt_tail;
    case 'd':
    case 'i':
      pl = 1;

      if (arg.i > std::numeric_limits<intmax_t>::max()) {
        arg.i = -arg.i;
      } else if (flags & MARK_POS) {
        prefix++;
      } else if (flags & PAD_POS) {
        prefix += 2;
      } else {
        pl = 0;
      }
    case 'u':
      a = format_unsigned(arg.i, z);
    ifmt_tail:
      if (xp && precision < 0) {
        return -1;
      }

      if (xp) {
        flags &= ~ZERO_PAD;
      }

      if (!arg.i && !precision) {
        a = z;
        break;
      }

      precision = std::max(precision, static_cast<int>(z - a + !arg.i));
      break;
    case 'c':
      precision = 1;
      a = z - precision;
      *a = arg.i;
      flags &= ~ZERO_PAD;
      break;
    case 's':
      a = arg.p ? reinterpret_cast<char *>(arg.p)
                : const_cast<char *>("(null)");
      z = a + strnlen(a, precision < 0 ? std::numeric_limits<int>::max()
                                       : precision);

      if (precision < 0 && *z) {
        return -1;
      }

      precision = z - a;
      flags &= ~ZERO_PAD;
      break;
    }

    if (precision < z - a) {
      precision = z - a;
    }

    if (precision > (std::numeric_limits<int>::max() - pl)) {
      return -1;
    }

    if (width < pl + precision) {
      width = pl + precision;
    }

    if (width > (std::numeric_limits<int>::max() - count)) {
      return -1;
    }

    pad(stream, ' ', width, pl + precision, flags);

    print(stream, prefix, pl);

    pad(stream, '0', width, pl + precision, flags ^ ZERO_PAD);
    pad(stream, '0', precision, z - a, 0);

    print(stream, a, z - a);

    pad(stream, ' ', width, pl + precision, flags ^ LEFT_ADJ);

    length = width;
  }

  if (stream) {
    return count;
  }

  if (l10n == 0) {
    return 0;
  }

  for (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
    pop_arg(nl_arg + i, nl_type[i], ap);
  }

  for (; i <= NL_ARGMAX && !nl_type[i]; i++)
    ;

  if (i <= NL_ARGMAX) {
    return -1;
  }

  return 1;
}
} // namespace

int vfprintf(FILE *restrict stream, const char *restrict format, va_list arg) {
  va_list ap;
  std::array<int, NL_ARGMAX + 1> nl_type = {0};
  std::array<format_args, NL_ARGMAX + 1> nl_arg = {};
  std::array<uint8_t, 80> internal_buf = {};
  uint8_t *saved_buf = 0;

  int old_err = 0;
  int ret = 0;

  va_copy(ap, arg);

  if (printf_core(nullptr, format, &ap, nl_arg.begin(), nl_type.begin()) < 0) {
    va_end(ap);
    return -1;
  }

  LOCK_STREAM(stream);

  old_err = stream->flags & stdio::FILE_ERROR;
  stream->flags &= ~stdio::FILE_ERROR;

  if (!stream->buf_size) {
    saved_buf = stream->buffer;
    stream->buffer = internal_buf.begin();
    stream->buf_size = internal_buf.size();
    stream->write_pos = stream->write_base = stream->write_end = 0;
  }

  if (!stream->write_end && stdio::to_write(stream)) {
    ret = -1;
  } else {
    ret = printf_core(stream, format, &ap, nl_arg.begin(), nl_type.begin());
  }

  if (saved_buf) {
    stream->write(stream, 0, 0);

    if (stream->write_pos == 0) {
      ret = -1;
    }

    stream->buffer = saved_buf;
    stream->buf_size = 0;
    stream->write_pos = stream->write_base = stream->write_end = 0;
  }

  if (ferror(stream)) {
    ret = -1;
  }

  stream->flags |= old_err;

  UNLOCK_STREAM(stream);

  va_end(ap);
  return ret;
}

int vprintf(const char *restrict format, va_list arg) {
  return vfprintf(stdout, format, arg);
}