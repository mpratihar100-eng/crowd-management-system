#!/bin/bash
# Test script for API endpoints

echo "========================================"
echo "Testing Crowd Management API"
echo "========================================"
echo ""

echo "1. Health Check:"
curl -s http://localhost:8080/api/v1/health | python -m json.tool
echo ""

echo "2. Zone Metrics:"
curl -s http://localhost:8080/api/v1/zone/zone-entrance-1/metrics | python -m json.tool
echo ""

echo "3. Heatmap Data:"
curl -s http://localhost:8080/api/v1/heatmap | python -m json.tool
echo ""

echo "4. Recommendations:"
curl -s http://localhost:8080/api/v1/recommendations | python -m json.tool
echo ""

echo "========================================"
echo "Tests complete!"
echo "========================================"
