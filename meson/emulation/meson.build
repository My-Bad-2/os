########################################
# QEMU System Invocation with ISO File #
########################################

# Default list of QEMU system architectures
qemu_supported_architectures = [
  'x86_64',
  'arm',
  'aarch64',
  'riscv32',
  'riscv64',
  'ppc64',
  'mips',
]

# Get the current build architecture from Meson
host_arch = host_machine.cpu_family()

# Check if the current architecture is supported
if qemu_supported_architectures.contains(host_arch)
  qemu_binary = 'qemu-system-' + host_arch
else
  error('Unsupported architecture: ' + host_arch)
endif

# Define arguments and flags for QEMU invocation
# User can override these with `meson configure`
qemu_default_args = [
  '-m', '512M',
  '-smp', '1',
  '-rtc', 'base=localtime',
  '-serial', 'stdio',
  '-boot', 'order=d,menu=on,splash-time=100',
  '-cpu', 'max',
]

qemu_extra_args = get_variable('qemu_extra_args', [])
qemu_args = qemu_default_args + qemu_extra_args

qemu_log = meson.project_build_root() / 'qemu.log'
qemu_debug_args = ['-no-reboot', '-no-shutdown', '-d', 'int', '-D', qemu_log, '-s', '-S']
qemu_additional_args = ['-M', 'accel=kvm:hvf:whpx:haxm:tcg']

if host_machine.cpu_family() == 'x86_64'
  qemu_debug_args += ['-M', 'smm=off']
endif

# Specify the ISO file to pass
iso_output = get_variable('iso_output', '')

# Check if an ISO file is provided
if iso_output == ''
  error('An ISO file must be provided with -Dqemu_iso_file=/path/to/file.iso')
endif

run_target(
  'run-qemu-uefi',
  command: [
    qemu_binary,
    qemu_default_args,
    qemu_extra_args,
    qemu_additional_args,
    '-cdrom', iso_output,
    '-bios', ovmf.get_variable('binaries_' + host_machine.cpu_family()),
  ],
  depends: build_iso,
)

run_target(
  'run-qemu-uefi-debug',
  command: [
    qemu_binary,
    qemu_default_args,
    qemu_extra_args,
    qemu_debug_args,
    '-cdrom', iso_output,
    '-bios', ovmf.get_variable('binaries_' + host_machine.cpu_family()),
  ],
  depends: build_iso,
)