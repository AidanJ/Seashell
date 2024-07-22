project('seashell', 'cpp', 'c',
   default_options: [
    'default_library=static',
    'warning_level=3',
    'buildtype=debug',
    'cpp_std=c++23'
   ])

src_files = [
  'src/main.cpp',
  'src/Token.hpp',
  'src/Token.cpp',
  'src/Lexer.hpp',
  'src/Lexer.cpp',
  'src/Log.hpp',
  'src/Expr.hpp',
  'src/Parser.hpp',
  'src/Parser.cpp',
]

executable(
  'sshl.bin',
  files(src_files),
  dependencies: [
    dependency('fmt'),
  ]
)
