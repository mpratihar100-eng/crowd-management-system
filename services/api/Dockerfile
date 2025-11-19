FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    gcc \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy source files
COPY src/ ./src/
COPY include/ ./include/

# Build with gcc and link pthread
RUN gcc -o api_server src/main.c -lpthread -I./include

# Runtime stage
FROM ubuntu:22.04

WORKDIR /app

# Copy binary from builder
COPY --from=builder /app/api_server .

# Expose port
EXPOSE 8080

# Run
CMD ["./api_server"]
