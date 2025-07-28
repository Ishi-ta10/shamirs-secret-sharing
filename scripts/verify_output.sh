#!/bin/bash

echo "🧪 Verifying Assignment Output..."
echo "================================"

# Build the project
if [ ! -f "catalog_assignment" ]; then
    echo "Building project..."
    make
    if [ $? -ne 0 ]; then
        echo "❌ Build failed!"
        exit 1
    fi
fi

echo
echo "📋 Running assignment and capturing output..."
./catalog_assignment > output.txt 2>&1

echo
echo "📄 Assignment Output:"
echo "===================="
cat output.txt

echo
echo "🔍 Extracting secrets..."
echo "========================"

# Extract the final results
grep "Test Case.*Secret:" output.txt

echo
echo "✅ Verification complete!"
echo "📝 Output saved to output.txt"
