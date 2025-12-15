#!/bin/bash

# Educational Run Script for 7 Wonders Duel
# This script compiles the project using clang++ and runs the executable.

echo "=========================================="
echo "   7 Wonders Duel - Educational Build"
echo "=========================================="

# 1. Compile
echo "[1/2] Compiling C++ Source Code..."
clang++ -std=c++17 *.cpp -o 7wonders

# Check if compilation succeeded
if [ $? -eq 0 ]; then
    echo "      > Compilation Successful!"
else
    echo "      > Compilation Failed. Please check errors above."
    exit 1
fi

# 2. Run
echo "[2/2] Launching Game..."
echo "------------------------------------------"
./7wonders
