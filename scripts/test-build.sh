#!/bin/bash
# Simple test script for CI/CD

set -e

echo "========================================"
echo "Running Build Tests"
echo "========================================"

# Test CV Worker build
echo "Testing CV Worker build..."
cd services/cv_worker
gcc -o cv_worker src/main.c
if [ -f cv_worker ]; then
    echo "✅ CV Worker built successfully"
    file cv_worker
else
    echo "❌ CV Worker build failed"
    exit 1
fi
cd ../..

# Test API Server build
echo "Testing API Server build..."
cd services/api
gcc -o api_server src/main.c -lpthread
if [ -f api_server ]; then
    echo "✅ API Server built successfully"
    file api_server
else
    echo "❌ API Server build failed"
    exit 1
fi
cd ../..

echo ""
echo "========================================"
echo "✅ All builds successful!"
echo "========================================"
