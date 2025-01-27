project('limine', 'c', license: 'BSD-2-Clause')

arch = host_machine.cpu_family()

this_dir = meson.current_source_dir()

limine_files = []
efi_files = []

if arch == 'x86_64'
  limine_files += [
    this_dir / 'limine-bios.sys',
    this_dir / 'limine-bios-cd.bin',
    this_dir / 'limine-uefi-cd.bin',
  ]

  efi_files += [
    this_dir / 'BOOTX64.EFI',
    this_dir / 'BOOTIA32.EFI',
  ]
endif

run_command(
  meson.get_compiler('c', native: true).get_id(),
  this_dir / 'limine.c',
  '-Wall',
  '-Wextra',
  '-std=c99',
  '-O2',
  '-o', this_dir / 'limine',
  check: true,
)

limine_exec = this_dir / 'limine'

limine_dep = declare_dependency(include_directories: include_directories('.'))

if meson.version().version_compare('>=0.54.0')
  meson.override_dependency('limine', limine_dep)
endif