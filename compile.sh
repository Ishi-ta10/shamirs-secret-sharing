#!/bin/bash

echo "Compiling Shamir's Secret Sharing Algorithm..."

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ compiler not found. Please install g++."
    exit 1
fi

# Compile the program
g++ -std=c++17 -Wall -Wextra -O2 -o shamirs_secret main.cpp

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "Usage: ./shamirs_secret [input_file.json]"
    echo "Default input file: input.json"
    echo ""
    echo "Running with default input..."
    ./shamirs_secret input.json
else
    echo "Compilation failed!"
    exit 1
fi
