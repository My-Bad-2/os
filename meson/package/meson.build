##################################################
# ISO File Creation with xorriso and Meson Build #
##################################################
# By including this module in your build, you can automate ISO file creation
# using xorriso. This script defines targets for generating ISO files and
# includes customizable arguments for input files, output location, and
# additional options.
#
# By default, the script expects input files in the project directory and
# generates an ISO named `output.iso`. These defaults can be overridden using
# variables defined below.
#
# Overrideable variables:
#   iso_output:      Path to the output ISO file (default: `output.iso`)
#   iso_inputs:      Array of files or directories to include in the ISO
#   iso_extra_args:  Array of additional arguments to pass to xorriso

# Define variables for inputs, output, and additional arguments
iso_output = get_variable('iso_output', 'output.iso')
iso_inputs = get_variable('iso_inputs', [])
iso_efi = get_variable('iso_efi', [])

iso_extra_args = get_variable('iso_extra_args', [])
iso_root_dir = get_variable('iso_root_dir', meson.project_build_root() / 'iso_root')

# Validate inputs
if iso_inputs.length() == 0
  error(
    'No input files provided. Set the variable `iso_inputs` to specify files and directories to include in the ISO.',
  )
endif

# Add a run target for ISO creation
build_iso = custom_target(
  'build-iso',
  output: iso_output,
  command: [
    'rm',
    '-rf', iso_root_dir,
    '&&',
    'mkdir',
    '-p', iso_root_dir / 'boot/EFI',
    '&&',
    'cp',
    iso_efi,
    iso_root_dir / 'boot/EFI',
    '&&',
    'mkdir',
    '-p', iso_root_dir / 'boot/limine',
    '&&',
    'cp',
    iso_files,
    iso_root_dir / 'boot/limine',
    '&&',
    'cp',
    kernel.full_path(),
    iso_root_dir / 'boot',
    '&&',
    'xorriso',
    '-as', 'mkisofs',
    '-R',
    '-r',
    '-J', iso_extra_args,
    iso_root_dir,
    '-o', meson.project_build_root() / iso_output,
    '&&',
    limine.get_variable('limine_exec'),
    'bios-install',
    iso_output,
  ],
  depends: [kernel],
)