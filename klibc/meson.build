klibc_sources = files('ctype.cpp')

subdir('internal')
subdir('string')
subdir('stdlib')
subdir('stdio')

clangtidy_files += klibc_sources

klibc = static_library(
  'klibc',
  klibc_sources,
  build_by_default: true,
  include_directories: [
    include_directories('../include/klibc'),
    include_directories('../include'),
  ],
  dependencies: [
    libstdcxx_dep,
  ],
  install: false,
)