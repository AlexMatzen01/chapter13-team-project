#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

TARGET="colony_simulation"
CXX="g++"
CXXFLAGS="-std=c++17 -Wall -Wextra"

echo "Building $TARGET..."

# Compile all .cpp files in the directory
echo "Running: $CXX $CXXFLAGS *.cpp -o $TARGET"
$CXX $CXXFLAGS *.cpp -o $TARGET

echo ""
echo "Build successful!"
echo "You can run the application using: ./$TARGET"
