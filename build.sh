#!/bin/bash

# Build testing with CMake and Make
cd testing
mkdir -p build
cd build
cmake ..
make
cd ../..



