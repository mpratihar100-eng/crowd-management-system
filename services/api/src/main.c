#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 4096

// Mock database (in-memory for demo)
typedef struct {
    char zone_id[64];
    int people_count;
    float density;
    time_t timestamp;
} ZoneMetric;

typedef struct {
    char id[64];
    char zone_id[64];
    char type[32];
    char priority[16];
    char message[256];
    time_t timestamp;
} Recommendation;

// Global mock database
ZoneMetric zones[10];
int zone_count = 0;

Recommendation recommendations[10];
int rec_count = 0;

pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;

// Initialize mock data
void init_database() {
    pthread_mutex_lock(&db_mutex);
    
    // Add sample zones
    strcpy(zones[0].zone_id, "zone-entrance-1");
    zones[0].people_count = 42;
    zones[0].density = 0.42;
    zones[0].timestamp = time(NULL);
    
    strcpy(zones[1].zone_id, "zone-ride-1");
    zones[1].people_count = 67;
    zones[1].density = 0.67;
    zones[1].timestamp = time(NULL);
    
    zone_count = 2;
    
    // Add sample recommendations
    strcpy(recommendations[0].id, "rec-001");
    strcpy(recommendations[0].zone_id, "zone-entrance-1");
    strcpy(recommendations[0].type, "reroute_staff");
    strcpy(recommendations[0].priority, "high");
    strcpy(recommendations[0].message, "Dispatch 2 staff to entrance - high density");
    recommendations[0].timestamp = time(NULL);
    
    rec_count = 1;
    
    pthread_mutex_unlock(&db_mutex);
    
    printf("Database initialized with %d zones and %d recommendations\n", zone_count, rec_count);
}

// Get zone metrics as JSON
char* get_zone_metrics_json(const char* zone_id) {
    char* json = malloc(2048);
    int offset = 0;
    
    pthread_mutex_lock(&db_mutex);
    
    offset += sprintf(json, "{\"zone_id\":\"%s\",\"metrics\":[", zone_id);
    
    for (int i = 0; i < zone_count; i++) {
        if (strcmp(zones[i].zone_id, zone_id) == 0) {
            offset += sprintf(json + offset,
                "{\"timestamp\":%ld,\"people_count\":%d,\"density\":%.2f}",
                zones[i].timestamp, zones[i].people_count, zones[i].density);
        }
    }
    
    sprintf(json + offset, "]}");
    
    pthread_mutex_unlock(&db_mutex);
    return json;
}

// Get all recommendations as JSON
char* get_recommendations_json() {
    char* json = malloc(4096);
    int offset = 0;
    
    pthread_mutex_lock(&db_mutex);
    
    offset += sprintf(json, "{\"recommendations\":[");
    
    for (int i = 0; i < rec_count; i++) {
        offset += sprintf(json + offset,
            "%s{\"id\":\"%s\",\"zone_id\":\"%s\",\"type\":\"%s\",\"priority\":\"%s\",\"message\":\"%s\",\"timestamp\":%ld}",
            i > 0 ? "," : "",
            recommendations[i].id, recommendations[i].zone_id,
            recommendations[i].type, recommendations[i].priority,
            recommendations[i].message, recommendations[i].timestamp);
    }
    
    sprintf(json + offset, "]}");
    
    pthread_mutex_unlock(&db_mutex);
    return json;
}

// Get heatmap data
char* get_heatmap_json() {
    char* json = malloc(2048);
    
    sprintf(json,
        "{\"tiles\":["
        "{\"x\":100,\"y\":200,\"value\":3.5},"
        "{\"x\":105,\"y\":200,\"value\":2.8},"
        "{\"x\":110,\"y\":200,\"value\":4.2},"
        "{\"x\":100,\"y\":205,\"value\":1.9}"
        "],\"timestamp\":%ld}",
        time(NULL));
    
    return json;
}

void send_response(int client_socket, const char* status, const char* content_type, const char* body) {
    char response[BUFFER_SIZE];
    int content_length = strlen(body);
    
    snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        status, content_type, content_length, body);
    
    send(client_socket, response, strlen(response), 0);
}

void handle_health(int client_socket) {
    char body[256];
    sprintf(body,
        "{\"status\":\"healthy\",\"service\":\"api-server\",\"version\":\"1.0.0\",\"timestamp\":%ld,\"database\":\"connected\"}",
        time(NULL));
    send_response(client_socket, "200 OK", "application/json", body);
}

void handle_zone_metrics(int client_socket, const char* zone_id) {
    char* body = get_zone_metrics_json(zone_id);
    send_response(client_socket, "200 OK", "application/json", body);
    free(body);
}

void handle_heatmap(int client_socket) {
    char* body = get_heatmap_json();
    send_response(client_socket, "200 OK", "application/json", body);
    free(body);
}

void handle_recommendations(int client_socket) {
    char* body = get_recommendations_json();
    send_response(client_socket, "200 OK", "application/json", body);
    free(body);
}

void handle_404(int client_socket) {
    const char* body = "{\"error\":\"Not Found\",\"message\":\"The requested endpoint does not exist\"}";
    send_response(client_socket, "404 Not Found", "application/json", body);
}

void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);
    
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        
        char method[16], path[256];
        sscanf(buffer, "%s %s", method, path);
        
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        char timestamp[64];
        strftime(timestamp, 64, "%Y-%m-%d %H:%M:%S", tm_info);
        
        printf("[%s] %s %s\n", timestamp, method, path);
        
        // Route requests
        if (strcmp(path, "/api/v1/health") == 0) {
            handle_health(client_socket);
        } else if (strncmp(path, "/api/v1/zone/", 14) == 0) {
            char zone_id[64];
            sscanf(path, "/api/v1/zone/%[^/]", zone_id);
            handle_zone_metrics(client_socket, zone_id);
        } else if (strcmp(path, "/api/v1/heatmap") == 0) {
            handle_heatmap(client_socket);
        } else if (strcmp(path, "/api/v1/recommendations") == 0) {
            handle_recommendations(client_socket);
        } else {
            handle_404(client_socket);
        }
    }
    
    close(client_socket);
    return NULL;
}

int main(void) {
    printf("========================================\n");
    printf("API Server v1.0 - FUNCTIONAL\n");
    printf("========================================\n\n");
    
    // Initialize database
    init_database();
    
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }
    
    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        return 1;
    }
    
    printf("Listening on http://0.0.0.0:%d\n\n", PORT);
    printf("Available endpoints:\n");
    printf("  GET  /api/v1/health\n");
    printf("  GET  /api/v1/zone/{zone_id}/metrics\n");
    printf("  GET  /api/v1/heatmap\n");
    printf("  GET  /api/v1/recommendations\n\n");
    printf("Try: curl http://localhost:%d/api/v1/health\n\n", PORT);
    printf("Press Ctrl+C to stop\n");
    printf("========================================\n\n");
    
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) {
            continue;
        }
        
        int* client_sock_ptr = malloc(sizeof(int));
        *client_sock_ptr = client_socket;
        
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, client_sock_ptr);
        pthread_detach(thread);
    }
    
    close(server_socket);
    return 0;
}
