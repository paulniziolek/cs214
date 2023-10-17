cmake_minimum_required(VERSION 3.10)
project(malloc)
enable_testing()

# Find cmocka headers and libraries
find_path(CMOCKA_INCLUDE_DIR cmocka.h)
find_library(CMOCKA_LIBRARY NAMES cmocka)

# Ensure they are found
if(NOT CMOCKA_INCLUDE_DIR)
    message(FATAL_ERROR "cmocka.h not found!")
endif()

if(NOT CMOCKA_LIBRARY)
    message(FATAL_ERROR "cmocka library not found!")
endif()

# Include the src directory
include_directories(src)

# Create an executable for your main project (if any)
add_executable(main src/memgrind.c src/mymalloc.c)

# Create an executable for your tests
add_executable(mymalloc_test tests/mymalloc_test.c)

# add testing files
add_test(NAME mymalloc_test COMMAND mymalloc_test)

target_include_directories(mymalloc_test PUBLIC ${CMOCKA_INCLUDE_DIR})
target_link_libraries(mymalloc_test ${CMOCKA_LIBRARY})
