#!/bin/bash

echo "🔨 Building Catalog Placements Assignment..."
echo

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo "❌ Error: g++ compiler not found!"
    echo "Please install g++ using your package manager:"
    echo "  Ubuntu/Debian: sudo apt install g++"
    echo "  macOS: xcode-select --install"
    echo "  CentOS/RHEL: sudo yum install gcc-c++"
    exit 1
fi

# Compile the program
echo "Compiling..."
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude -Isrc -o catalog_assignment src/main.cpp

if [ $? -eq 0 ]; then
    echo "✅ Compilation successful!"
    echo
    echo "🚀 Running assignment..."
    ./catalog_assignment
else
    echo "❌ Compilation failed!"
    exit 1
fi
