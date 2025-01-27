project(
  'os',
  ['c', 'cpp'],
  default_options: [
    'warning_level=3',
    'werror=false',
    # This project defaults to a release build
    'debug=false',
    'optimization=2',
    'b_lto=true',
    'c_std=gnu23',
    'buildtype=debugoptimized',
  ],
  meson_version: '>=0.58.0',
  version: '0.0.1',
)

################################
# Project Options and Settings #
################################

# Pick up our common compiler variables + desired_*_flags variables
subdir('meson/compiler')
subdir('meson/compiler/c')
subdir('meson/compiler/cpp')

if get_option('debug')
  add_project_arguments('-DDEBUG', language: ['c', 'cpp'])
  add_project_arguments('-DDEBUG', language: ['c', 'cpp'], native: true)
endif

if get_option('disable-builtins')
  desired_common_compile_flags += '-fno-builtin'
endif

if get_option('disable-stack-protection')
  desired_common_compile_flags += '-fno-stack-protector'
endif

if get_option('disable-rtti')
  desired_cpp_compile_flags += '-fno-rtti'
endif

if get_option('disable-exceptions')
  desired_cpp_compile_flags += ['-fno-exceptions', '-fno-unwind-tables']
endif

if get_option('enable-pedantic')
  desired_common_compile_flags += '-pedantic'
else
  desired_common_compile_flags += '-Wno-pedantic'
endif

if get_option('enable-pedantic-error')
  desired_common_compile_flags += '-pedantic-error'
endif

compile_settings_list = [
  {
    'lang': 'c',
    'compiler': host_c_compiler,
    'flags': desired_c_compile_flags,
    'isnative': false,
  },
  {
    'lang': 'c',
    'compiler': native_c_compiler,
    'flags': desired_native_c_compile_flags,
    'isnative': true,
  },
  {
    'lang': 'cpp',
    'compiler': host_cpp_compiler,
    'flags': desired_cpp_compile_flags,
    'isnative': false,
  },
  {
    'lang': 'cpp',
    'compiler': native_cpp_compiler,
    'flags': desired_native_cpp_compile_flags,
    'isnative': true,
  },
]

link_settings_list = [
  {
    'lang': 'c',
    'compiler': host_c_compiler,
    'flags': [],
    'isnative': false,
  },
  {
    'lang': 'c',
    'compiler': native_c_compiler,
    'flags': [],
    'isnative': true,
  },
  {
    'lang': 'cpp',
    'compiler': host_cpp_compiler,
    'flags': [],
    'isnative': false,
  },
  {
    'lang': 'cpp',
    'compiler': native_cpp_compiler,
    'flags': [],
    'isnative': true,
  },
]

if host_machine.cpu_family() == 'x86_64'
  desired_common_compile_flags += [
    '-m64',
    '-march=x86-64',
    '-mno-80387',
    '-mno-mmx',
    '-mno-sse',
    '-mno-sse2',
    '-mno-red-zone',
    '-mcmodel=kernel',
  ]

  if host_c_compiler.get_id() == 'clang'
    add_project_arguments(['-target', 'x86_64-pc-none-elf'], language: ['c', 'cpp'])
  endif

  desired_common_link_flags += ['-Wl,-m,elf_x86_64']
  linker_scripts = ['linker-amd64.ld']
endif

linker_paths = ['meson/linker-scripts/']
libgcc_path = meson.current_source_dir() / get_option('libgcc-location')

if desired_common_compile_flags.contains('-mno-red-zone')
  libgcc_path += '/no-red-zone'
endif

# Process the compilation flags
subdir('meson/linker/linker-script-as-property')
subdir('meson/compiler/check-and-apply-flags')

#################
# Build Modules #
#################

# Include reusable build modules here via subdir() calls
subdir('meson/linker/linker-map')
subdir('meson/objcopy')

###############
# Subprojects #
###############

limine = subproject('limine')
ovmf = subproject('ovmf')
awesome_css = subproject('doxygen-awesome-css')

###############
# Dependencies #
###############

limine_dep = dependency('limine')
libstdcxx_dep = dependency('freestnd_cxx_hdrs')

#######################
# Process Source Tree #
#######################

# Add files to this variable if you want them analyzed by clang-tidy
clangtidy_files = []

subdir('klibc')
subdir('kernel')

###################
# Tooling Modules #
###################

subdir('meson/standard')
subdir('meson/analysis/clang-tidy')
subdir('meson/format')
subdir('meson/analysis/cppcheck')
subdir('meson/docs/doxygen')

#############
# Packaging #
#############

# Invoke the package module
subdir('meson/package')
subdir('meson/emulation')