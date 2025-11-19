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
    float trend;
    time_t timestamp;
} ZoneMetric;

typedef struct {
    char id[64];
    char zone_id[64];
    char type[32];
    char priority[16];
    char message[256];
    int people_count;
    float threshold;
    float trend;
    float density;
    int staff_count;
    time_t timestamp;
} Recommendation;

ZoneMetric zones[10];
int zone_count = 0;
Recommendation recommendations[10];
int rec_count = 0;
pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_database() {
    pthread_mutex_lock(&db_mutex);
    
    strcpy(zones[0].zone_id, "zone-entrance-1");
    zones[0].people_count = 42;
    zones[0].density = 0.028;
    zones[0].trend = 0.15;
    zones[0].timestamp = time(NULL);
    
    strcpy(zones[1].zone_id, "zone-ride-1");
    zones[1].people_count = 67;
    zones[1].density = 0.034;
    zones[1].trend = 0.25;
    zones[1].timestamp = time(NULL);
    
    strcpy(zones[2].zone_id, "zone-food-1");
    zones[2].people_count = 55;
    zones[2].density = 0.022;
    zones[2].trend = 0.10;
    zones[2].timestamp = time(NULL);
    
    zone_count = 3;
    
    strcpy(recommendations[0].id, "rec-001");
    strcpy(recommendations[0].zone_id, "zone-entrance-1");
    strcpy(recommendations[0].type, "reroute_staff");
    strcpy(recommendations[0].priority, "high");
    strcpy(recommendations[0].message, "Dispatch 2 staff to entrance - high density");
    recommendations[0].people_count = 42;
    recommendations[0].threshold = 40;
    recommendations[0].trend = 0.15;
    recommendations[0].density = 0.028;
    recommendations[0].staff_count = 2;
    recommendations[0].timestamp = time(NULL);
    
    rec_count = 1;
    
    pthread_mutex_unlock(&db_mutex);
    printf("Database initialized: %d zones, %d recommendations\n", zone_count, rec_count);
}

void send_response(int sock, const char* status, const char* body) {
    char response[BUFFER_SIZE];
    snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n"
        "\r\n%s",
        status, strlen(body), body);
    send(sock, response, strlen(response), 0);
}

void handle_health(int sock) {
    char body[256];
    snprintf(body, sizeof(body),
        "{\"status\":\"healthy\",\"service\":\"api-server\",\"version\":\"1.0.0\",\"timestamp\":%ld}",
        time(NULL));
    send_response(sock, "200 OK", body);
}

void handle_zone_metrics(int sock, const char* zone_id) {
    char body[2048];
    int offset = 0;
    
    pthread_mutex_lock(&db_mutex);
    
    offset += snprintf(body + offset, sizeof(body) - offset,
        "{\"zone_id\":\"%s\",\"metrics\":[", zone_id);
    
    int found = 0;
    for (int i = 0; i < zone_count; i++) {
        if (strcmp(zones[i].zone_id, zone_id) == 0) {
            offset += snprintf(body + offset, sizeof(body) - offset,
                "%s{\"timestamp\":%ld,\"people_count\":%d,\"density\":%.3f,\"trend\":%.2f}",
                found ? "," : "", zones[i].timestamp, zones[i].people_count,
                zones[i].density, zones[i].trend);
            found = 1;
        }
    }
    
    snprintf(body + offset, sizeof(body) - offset, "]}");
    pthread_mutex_unlock(&db_mutex);
    
    send_response(sock, "200 OK", body);
}

void handle_heatmap(int sock) {
    char body[2048];
    snprintf(body, sizeof(body),
        "{\"tiles\":["
        "{\"x\":100,\"y\":200,\"value\":3.5},"
        "{\"x\":105,\"y\":200,\"value\":2.8},"
        "{\"x\":110,\"y\":200,\"value\":4.2},"
        "{\"x\":100,\"y\":205,\"value\":1.9}"
        "],\"timestamp\":%ld}", time(NULL));
    send_response(sock, "200 OK", body);
}

void handle_recommendations(int sock) {
    char body[4096];
    int offset = 0;
    
    pthread_mutex_lock(&db_mutex);
    offset += snprintf(body, sizeof(body), "{\"recommendations\":[");
    
    for (int i = 0; i < rec_count; i++) {
        offset += snprintf(body + offset, sizeof(body) - offset,
            "%s{\"id\":\"%s\",\"zone_id\":\"%s\",\"type\":\"%s\",\"priority\":\"%s\","
            "\"message\":\"%s\",\"timestamp\":%ld,\"people_count\":%d,"
            "\"threshold\":%.1f,\"trend\":%.2f,\"density\":%.3f,\"staff_count\":%d}",
            i > 0 ? "," : "", recommendations[i].id, recommendations[i].zone_id,
            recommendations[i].type, recommendations[i].priority, recommendations[i].message,
            recommendations[i].timestamp, recommendations[i].people_count,
            recommendations[i].threshold, recommendations[i].trend,
            recommendations[i].density, recommendations[i].staff_count);
    }
    
    snprintf(body + offset, sizeof(body) - offset, "]}");
    pthread_mutex_unlock(&db_mutex);
    
    send_response(sock, "200 OK", body);
}

void handle_404(int sock) {
    const char* body = "{\"error\":\"Not Found\",\"message\":\"The requested endpoint does not exist\"}";
    send_response(sock, "404 Not Found", body);
}

void* handle_client(void* arg) {
    int sock = *(int*)arg;
    free(arg);
    
    char buffer[BUFFER_SIZE];
    int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) {
        close(sock);
        return NULL;
    }
    
    buffer[bytes] = '\0';
    
    char method[16], path[256];
    sscanf(buffer, "%s %s", method, path);
    
    printf("[%s] %s\n", method, path);
    
    if (strcmp(path, "/api/v1/health") == 0) {
        handle_health(sock);
    } else if (strncmp(path, "/api/v1/zone/", 13) == 0) {
        char zone_id[64];
        char* start = path + 13;
        char* end = strstr(start, "/metrics");
        if (end) {
            int len = end - start;
            strncpy(zone_id, start, len);
            zone_id[len] = '\0';
            handle_zone_metrics(sock, zone_id);
        } else {
            handle_404(sock);
        }
    } else if (strcmp(path, "/api/v1/heatmap") == 0) {
        handle_heatmap(sock);
    } else if (strcmp(path, "/api/v1/recommendations") == 0) {
        handle_recommendations(sock);
    } else {
        handle_404(sock);
    }
    
    close(sock);
    return NULL;
}

int main(void) {
    printf("========================================\n");
    printf("API Server v1.0 - FUNCTIONAL\n");
    printf("========================================\n\n");
    
    init_database();
    
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    
    bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_sock, 10);
    
    printf("Listening on http://0.0.0.0:%d\n\n", PORT);
    printf("Endpoints:\n");
    printf("  GET /api/v1/health\n");
    printf("  GET /api/v1/zone/{zone_id}/metrics\n");
    printf("  GET /api/v1/heatmap\n");
    printf("  GET /api/v1/recommendations\n\n");
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &len);
        
        int* sock_ptr = malloc(sizeof(int));
        *sock_ptr = client_sock;
        
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, sock_ptr);
        pthread_detach(thread);
    }
    
    return 0;
}
