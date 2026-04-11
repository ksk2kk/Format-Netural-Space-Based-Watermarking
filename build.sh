#!/bin/bash
set -e

echo "[INFO] Starting Linux build process..."

# Ensure the bin directory exists
mkdir -p bin

echo "[INFO] Compiling src/advanced_main.cpp with g++..."
g++ -std=c++17 -O3 -Wall -Wextra -pthread src/advanced_main.cpp -o bin/advanced_main

echo "[SUCCESS] Build completed successfully. Executable is in bin/advanced_main"
