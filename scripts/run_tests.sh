#!/bin/bash

echo "🧪 Running Shamir's Secret Sharing Tests..."
echo "=========================================="

# Build the project first
if [ ! -f "shamirs_secret" ]; then
    echo "Building project..."
    make
    if [ $? -ne 0 ]; then
        echo "❌ Build failed!"
        exit 1
    fi
fi

echo
echo "📋 Test 1: Basic functionality"
echo "------------------------------"
./shamirs_secret examples/test1.json

echo
echo "📋 Test 2: Verbose output"
echo "-------------------------"
./shamirs_secret examples/test2.json --verbose

echo
echo "📋 Test 3: Nuclear scenario (large numbers)"
echo "-------------------------------------------"
./shamirs_secret examples/nuclear_scenario.json --time

echo
echo "📋 Test 4: Default input"
echo "------------------------"
./shamirs_secret examples/input.json

echo
echo "✅ All tests completed!"
