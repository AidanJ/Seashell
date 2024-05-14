project('seashell', 'cpp', 'c',
   default_options: [
    'default_library=static',
    'warning_level=3',
    'buildtype=debug',
    'cpp_std=c++23'
   ])

src_files = [
  'src/main.cpp',
]

executable(
  'sshl.bin',
  files(src_files),
  dependencies: [
    dependency('fmt'),
  ]
)
