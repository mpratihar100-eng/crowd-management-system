# Quick Start Guide

## Build and Run Locally

### CV Worker

\\\ash
cd services/cv_worker
gcc -o cv_worker src/main.c
./cv_worker test.jpg cam-001
\\\

Output: Creates \detections.json\ with detected persons

### API Server

\\\ash
cd services/api
gcc -o api_server src/main.c -lpthread
./api_server
\\\

Then test in another terminal:
\\\ash
curl http://localhost:8080/api/v1/health
curl http://localhost:8080/api/v1/zone/zone-entrance-1/metrics
curl http://localhost:8080/api/v1/heatmap
curl http://localhost:8080/api/v1/recommendations
\\\

## Build with Docker

\\\ash
# Build all services
docker-compose build

# Run API server
docker-compose up api

# Test
curl http://localhost:8080/api/v1/health
\\\

## Features Implemented

✅ **CV Worker**
- Image loading and processing
- Person detection (simulated)
- JSON export of detections
- Camera ID support

✅ **API Server**
- HTTP server on port 8080
- Multi-threaded request handling
- In-memory database (mock)
- 4 working endpoints:
  - GET /api/v1/health
  - GET /api/v1/zone/{zone_id}/metrics
  - GET /api/v1/heatmap
  - GET /api/v1/recommendations
- JSON responses with CORS headers
- Request logging with timestamps

## Next Steps

1. **Add PostgreSQL**: Replace in-memory database with real PostgreSQL
2. **Add ONNX Runtime**: Replace simulated detection with real YOLOv8 model
3. **Add Kafka**: Connect services via message bus
4. **Add WebSocket**: Real-time updates to dashboard
5. **Add Authentication**: JWT-based API security
