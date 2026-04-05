project('tp_othello', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++23'])

tp_inc = include_directories('include')
tp_src = files('main.cpp')

tp_othello = executable(
        'tp_othello', tp_src, install : true,
        include_directories : tp_inc)

test('test', tp_othello)