project('ts-yaml',
  ['c'],
  version : '0.1',
  default_options : ['warning_level=3', 'werror=true'])

tree_sitter = dependency('tree-sitter')
tree_sitter_yaml = dependency('tree-sitter-yaml')
include = include_directories('./include')

ts_yaml = library('ts-yaml', [
   'src/ts-yaml.c'
  ], dependencies: [tree_sitter, tree_sitter_yaml],
  include_directories: include)

executable('main.exe', [
  'src/main.c'
  ], dependencies: [tree_sitter, tree_sitter_yaml],
  link_with: [ts_yaml],
  include_directories: include)
