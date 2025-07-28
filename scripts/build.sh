#!/bin/bash

echo "🔨 Building Shamir's Secret Sharing Algorithm..."
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

# Create build directory if it doesn't exist
mkdir -p build

# Compile the program
echo "Compiling..."
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude -Isrc -o shamirs_secret src/main.cpp

if [ $? -eq 0 ]; then
    echo "✅ Compilation successful!"
    echo
    echo "🚀 Running with default input..."
    ./shamirs_secret examples/input.json
    echo
    echo "📝 Usage: ./shamirs_secret [input_file.json] [options]"
    echo "Options: --verbose, --time, --help"
else
    echo "❌ Compilation failed!"
    exit 1
fi
