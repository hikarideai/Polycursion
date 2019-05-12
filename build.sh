#!/bin/bash
# Quick-and-dirty build script for the whole project.
# Build directory is used by cmake and make.
# Bin directory is where the project just works.

# Extract binary executable name from the cmake
# This might not take into account different targets.
TARGET_NAME=$(cat CMakeLists.txt | grep "set(TARGET" | awk '{print substr($0, index($0, " ") + 1, index($0, ")") - index($0, " ") - 1);}')

# Create auxillary directories if they are not yet created
if [ ! -d "build" ]; then
    mkdir build
fi

if [ ! -d "bin" ]; then
    mkdir bin
fi

# Generate project and compile
cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
make

# Move executable to the bin folder
mv $TARGET_NAME ../bin/

# Move all necessary resources that the app needs to the bin/ directory
cd ..
if [ -d "resources" ]; then
    cp -a resources/. bin/
fi
