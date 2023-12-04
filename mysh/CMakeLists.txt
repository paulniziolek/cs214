cmake_minimum_required(VERSION 3.10)
project(malloc)

include_directories(src)

add_executable(mysh src/mysh.c src/builtins.c)
add_executable(builtins_test src/builtins.c src/builtins_test.c)

