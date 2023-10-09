cmake_minimum_required(VERSION 3.10)
project(testing)
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
add_executable(main src/example.c)

# Create an executable for your tests
add_executable(example_test tests/example_test.c)

add_test(NAME my_test COMMAND example_test)

target_include_directories(example_test PUBLIC ${CMOCKA_INCLUDE_DIR})
target_link_libraries(example_test ${CMOCKA_LIBRARY})
