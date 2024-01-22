cmake_minimum_required(VERSION 3.10)
project(ttts)

include_directories(src)

# Server Executable
add_executable(ttts src/ttts.c src/sbuf.c)

# Client Executable
add_executable(tttc src/tttc.c)
