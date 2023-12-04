cmake_minimum_required(VERSION 3.10)
project(malloc)

include_directories(src)

add_executable(mysh src/mysh.c src/builtins.c)


