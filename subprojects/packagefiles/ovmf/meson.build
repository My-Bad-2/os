project('ovmf', license: 'BSD-3-Clause')

host_arch = host_machine.cpu_family()

binaries_x86_64 = meson.current_source_dir() / 'bin' / 'RELEASEX64_OVMF.fd'

ovmf_dep = declare_dependency()

if meson.version().version_compare('>=0.54.0')
  meson.override_dependency('ovmf', ovmf_dep)
endif