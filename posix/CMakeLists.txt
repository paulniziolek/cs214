# All commented code in this CMake file is reserved for unit testing.
cmake_minimum_required(VERSION 3.10)
project(posix)
# enable_testing()

# Find cmocka headers and libraries
# find_path(CMOCKA_INCLUDE_DIR cmocka.h)
# find_library(CMOCKA_LIBRARY NAMES cmocka)

# Ensure they are found
# if(NOT CMOCKA_INCLUDE_DIR)
#     message(FATAL_ERROR "cmocka.h not found!")
# endif()
# 
# if(NOT CMOCKA_LIBRARY)
#     message(FATAL_ERROR "cmocka library not found!")
# endif()
# 
# Include the src directory
include_directories(src)

# Create an executable for your main project (if any)
add_compile_options(-pg)
add_executable(posix src/word.c src/hashmap.c src/error.c)
add_executable(hashmap src/hashmap.c src/hashmap_test.c)


# Create an executable for your tests
# add_executable(mymalloc_test tests/mymalloc_test.c)

# add testing files
# add_test(NAME mymalloc_test COMMAND mymalloc_test)
#
# target_include_directories(mymalloc_test PUBLIC ${CMOCKA_INCLUDE_DIR})
# target_link_libraries(mymalloc_test ${CMOCKA_LIBRARY})
