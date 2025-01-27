if host_machine.cpu_family() == 'x86_64'
  qemu_extra_args = [
    '-M', 'q35',
  ]

  iso_extra_args = [
    '-b', 'boot/limine/limine-bios-cd.bin',
    '-no-emul-boot',
    '-boot-load-size', '4',
    '-boot-info-table',
    '-hfsplus',
    '-apm-block-size', '2048',
    '--efi-boot', 'boot/limine/limine-uefi-cd.bin',
    '-efi-boot-part',
    '--efi-boot-image',
    '--protective-msdos-label',
  ]
endif

iso_files = []
iso_files += limine.get_variable('limine_files')
iso_files += meson.project_source_root() / 'meson' / 'scripts' / 'limine.conf'

iso_inputs = iso_files
iso_efi = limine.get_variable('efi_files')

iso_output = meson.project_name() + '.iso'