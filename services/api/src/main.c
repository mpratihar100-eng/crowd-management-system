#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handle_request(const char* method, const char* path) {
    printf("Request: %s %s\n", method, path);
    
    if (strcmp(path, "/api/v1/health") == 0) {
        printf("Response: {\"status\":\"healthy\"}\n");
    }
}

int main(void) {
    printf("API Server v1.0\n");
    printf("Starting on port 8080...\n");
    printf("Endpoints:\n");
    printf("  GET /api/v1/health\n");
    printf("  GET /api/v1/heatmap\n");
    printf("  GET /api/v1/recommendations\n");
    
    // TODO: Implement HTTP server
    handle_request("GET", "/api/v1/health");
    
    return 0;
}
