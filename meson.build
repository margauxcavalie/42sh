project(
  '42sh',
  'c',
  version: '0.1',
  license: 'MIT',
  default_options: [
    'c_std=c99',
    'warning_level=3',
    'default_library=static',

    # these options would not be suitable in a
    # production configuration, but development easier
    'b_sanitize=address',
    'buildtype=debug',
    'werror=true',
    ],
  )


# print a summary of the build configuration before each build starts
summary({
  'optimization': get_option('optimization'),
  'debug': get_option('debug'),
  'b_sanitize': get_option('b_sanitize'),
  'warning_level': get_option('warning_level'),
  'werror': get_option('werror'),
  })


# add some project-wide flags
cflags = ['-D_POSIX_C_SOURCE=200809L']
add_project_arguments(cflags, language: 'c')


# interpret the meson.build in this directory
all_sources = []
main_sources = []
print_pretoken_main = []
print_token_main = []
pretty_print_main = []
exec_ast_main = []
subdir('src')

all_test_sources = []
subdir('tests/units')

# find readline, which some of the modules below link with
cc = meson.get_compiler('c')
readline_dep = cc.find_library('readline', required: true)

cc = meson.get_compiler('c')
criterion_dep = cc.find_library('criterion', required: false)

bin = executable(
  '42sh',
  all_sources,
  main_sources,
  include_directories: 'src',
  install: true,
  dependencies: [readline_dep],
  )

ast_check = executable(
  'exec_ast',
  all_sources,
  exec_ast_main,
  include_directories: 'src',
  install: true,
  dependencies: [readline_dep],
  )

pretoken_check = executable(
  'pretoken_print',
  all_sources,
  print_pretoken_main,
  include_directories: 'src',
  install: true,
  dependencies: [readline_dep],
  )

token_check = executable(
  'token_print',
  all_sources,
  print_token_main,
  include_directories: 'src',
  install: true,
  dependencies: [readline_dep],
  )

pretty_check = executable(
  'pretty_print',
  all_sources,
  pretty_print_main,
  include_directories: 'src',
  install: true,
  dependencies: [readline_dep],
  )

if criterion_dep.found()

tests_bin = executable(
  'testsuite',
  all_sources,
  all_test_sources,
  include_directories: 'src',
  install: true,
  dependencies: [readline_dep, criterion_dep],
  )

check_unit = custom_target(
  'check_unit',
  depends: tests_bin,
  command: tests_bin,
  console: true,
  output: 'check_unit',
  build_always_stale: true,
)
endif

run_func_tests = meson.source_root() / 'pharaoh'
func_tests_path = meson.source_root() / 'tests/functional'

custom_target(
  'check',
  # depends: [check_unit, bin],
  command: [run_func_tests, func_tests_path],
  # give the testsuite exclusive access to the terminal, which enables
  # pretty colors and live testsuite results (meson buffers it all otherwise)
  console: true,
  # eventhough this rule output no file, we have to the output file of the rule is mandatory, but as
  # but it doesn't matter as the rule is always built
  output: 'check',
  build_always_stale: true,
  # if this option is enabled, tests will run for all builds
  build_by_default: get_option('force_check'),
)

if get_option('doc')
  subdir('doc')
endif
