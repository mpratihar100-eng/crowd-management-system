# setup-files.ps1 - Creates all essential project files

Write-Host "Creating project files..." -ForegroundColor Cyan

# Create .gitignore
@"
# Build artifacts
build/
*.o
*.a
*.so
*.dll
*.exe

# CMake
CMakeCache.txt
CMakeFiles/

# IDE
.vscode/
.idea/

# Python
__pycache__/
venv/

# Node
node_modules/
.env.local

# Secrets
*.key
secrets.yaml

# Models
*.onnx
*.pt

# Logs
*.log

# Terraform
.terraform/
*.tfstate
"@ | Out-File -FilePath .gitignore -Encoding UTF8

Write-Host "Created .gitignore" -ForegroundColor Green

# Create docker-compose.yml
@"
version: '3.8'

services:
  postgres:
    image: postgres:15-alpine
    environment:
      POSTGRES_DB: crowd_mgmt
      POSTGRES_USER: postgres
      POSTGRES_PASSWORD: password
    ports:
      - "5432:5432"
    volumes:
      - postgres_data:/var/lib/postgresql/data

  api:
    build:
      context: ./services/api
    ports:
      - "8080:8080"
    environment:
      DATABASE_URL: postgresql://postgres:password@postgres:5432/crowd_mgmt
    depends_on:
      - postgres

volumes:
  postgres_data:
"@ | Out-File -FilePath docker-compose.yml -Encoding UTF8

Write-Host "Created docker-compose.yml" -ForegroundColor Green

# Create root CMakeLists.txt
@"
cmake_minimum_required(VERSION 3.20)
project(CrowdManagementSystem C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(services/cv_worker)
add_subdirectory(services/api)
"@ | Out-File -FilePath CMakeLists.txt -Encoding UTF8

Write-Host "Created CMakeLists.txt" -ForegroundColor Green

# Create CV Worker files
@"
# Create the setup script
@'
# setup-files.ps1 - Creates all essential project files

Write-Host "Creating project files..." -ForegroundColor Cyan

# Create .gitignore
@"
# Build artifacts
build/
*.o
*.a
*.so
*.dll
*.exe

# CMake
CMakeCache.txt
CMakeFiles/

# IDE
.vscode/
.idea/

# Python
__pycache__/
venv/

# Node
node_modules/
.env.local

# Secrets
*.key
secrets.yaml

# Models
*.onnx
*.pt

# Logs
*.log

# Terraform
.terraform/
*.tfstate
"@ | Out-File -FilePath .gitignore -Encoding UTF8

Write-Host "Created .gitignore" -ForegroundColor Green

# Create docker-compose.yml
@"
version: '3.8'

services:
  postgres:
    image: postgres:15-alpine
    environment:
      POSTGRES_DB: crowd_mgmt
      POSTGRES_USER: postgres
      POSTGRES_PASSWORD: password
    ports:
      - "5432:5432"
    volumes:
      - postgres_data:/var/lib/postgresql/data

  api:
    build:
      context: ./services/api
    ports:
      - "8080:8080"
    environment:
      DATABASE_URL: postgresql://postgres:password@postgres:5432/crowd_mgmt
    depends_on:
      - postgres

volumes:
  postgres_data:
"@ | Out-File -FilePath docker-compose.yml -Encoding UTF8

Write-Host "Created docker-compose.yml" -ForegroundColor Green

# Create root CMakeLists.txt
@"
cmake_minimum_required(VERSION 3.20)
project(CrowdManagementSystem C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(services/cv_worker)
add_subdirectory(services/api)
"@ | Out-File -FilePath CMakeLists.txt -Encoding UTF8

Write-Host "Created CMakeLists.txt" -ForegroundColor Green

# Create CV Worker files
@"
project(cv_worker C)

set(SOURCES src/main.c src/detector.c)
add_executable(cv_worker `${SOURCES})
target_include_directories(cv_worker PRIVATE include)
"@ | Out-File -FilePath services/cv_worker/CMakeLists.txt -Encoding UTF8

@"
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y build-essential cmake
WORKDIR /app
COPY . .
RUN cmake . && make
CMD ["./cv_worker"]
"@ | Out-File -FilePath services/cv_worker/Dockerfile -Encoding UTF8

@"
#include <stdio.h>

int main() {
    printf("CV Worker starting...\n");
    return 0;
}
"@ | Out-File -FilePath services/cv_worker/src/main.c -Encoding UTF8

@"
// Detector implementation
"@ | Out-File -FilePath services/cv_worker/src/detector.c -Encoding UTF8

@"
#ifndef DETECTOR_H
#define DETECTOR_H
// Detector header
#endif
"@ | Out-File -FilePath services/cv_worker/include/detector.h -Encoding UTF8

Write-Host "Created CV Worker files" -ForegroundColor Green

# Create API files
@"
project(api_server C)

set(SOURCES src/main.c)
add_executable(api_server `${SOURCES})
"@ | Out-File -FilePath services/api/CMakeLists.txt -Encoding UTF8

@"
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y build-essential cmake
WORKDIR /app
COPY . .
RUN cmake . && make
CMD ["./api_server"]
"@ | Out-File -FilePath services/api/Dockerfile -Encoding UTF8

@"
#include <stdio.h>

int main() {
    printf("API Server starting on port 8080...\n");
    return 0;
}
"@ | Out-File -FilePath services/api/src/main.c -Encoding UTF8

Write-Host "Created API files" -ForegroundColor Green

# Create GitHub Actions
@"
name: CI/CD Pipeline

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    - name: Build
      run: |
        mkdir build && cd build
        cmake ..
        make
"@ | Out-File -FilePath .github/workflows/ci.yml -Encoding UTF8

Write-Host "Created GitHub Actions workflow" -ForegroundColor Green

Write-Host "`nAll files created successfully!" -ForegroundColor Cyan
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. git add ." -ForegroundColor White
Write-Host "2. git commit -m 'Add project structure'" -ForegroundColor White
Write-Host "3. git push" -ForegroundColor White
