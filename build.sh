#!/bin/bash

# Build testing with CMake and Make
mkdir -p testing/build
cd testing/build
cmake ..
make
cd ../..

# Build malloc with CMake and Make
mkdir -p malloc/build
cd malloc/build
cmake ..
make
cd ../..

# Build posix with CMake and Make
mkdir -p posix/build
cd posix/build
cmake ..
make
cd ../..

# Build mysh with CMake and Make
mkdir -p mysh/build
cd mysh/build
cmake ..
make
cd ../..

# Build ttts with CMake and Make
mkdir -p ttts/build
cd mysh/build
cmake ..
make
cd ../..
