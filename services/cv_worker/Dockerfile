FROM ubuntu:22.04 AS builder

# Prevent interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy source files
COPY src/ ./src/
COPY include/ ./include/

# Build with gcc
RUN gcc -o cv_worker src/main.c -I./include

# Runtime stage
FROM ubuntu:22.04

WORKDIR /app

# Copy binary from builder
COPY --from=builder /app/cv_worker .

# Run
CMD ["./cv_worker", "test.jpg", "cam-001"]
