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

