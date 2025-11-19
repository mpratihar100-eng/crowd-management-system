@echo off
REM Startup script for Windows

echo ========================================
echo Crowd Management System - Startup
echo ========================================
echo.

echo Starting services...
docker-compose up -d

echo.
echo Waiting for services to be healthy...
timeout /t 10 /nobreak > nul

echo.
echo ✅ System started successfully!
echo.
echo Access points:
echo   - API Server: http://localhost:8080
echo   - Dashboard: http://localhost:3000
echo   - Database: localhost:5432
echo.
echo API Endpoints:
echo   - GET  http://localhost:8080/api/v1/health
echo   - GET  http://localhost:8080/api/v1/zone/zone-entrance-1/metrics
echo   - GET  http://localhost:8080/api/v1/heatmap
echo   - GET  http://localhost:8080/api/v1/recommendations
echo.
echo To view logs: docker-compose logs -f
echo To stop: docker-compose down
echo ========================================

pause
