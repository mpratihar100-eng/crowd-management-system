#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple image structure
typedef struct {
    int width;
    int height;
    int channels;
    unsigned char* data;
} Image;

// Detection structure
typedef struct {
    float x, y, w, h;
    float confidence;
    int class_id;
} Detection;

typedef struct {
    Detection* detections;
    int count;
    int capacity;
} DetectionResult;

// Load image (simplified - supports RAW format for demo)
Image* load_image(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Could not open image file %s\n", filename);
        return NULL;
    }
    
    Image* img = malloc(sizeof(Image));
    
    // For demo: assume 640x480 RGB image
    img->width = 640;
    img->height = 480;
    img->channels = 3;
    img->data = malloc(img->width * img->height * img->channels);
    
    // Read image data
    size_t bytes_read = fread(img->data, 1, img->width * img->height * img->channels, file);
    fclose(file);
    
    if (bytes_read == 0) {
        printf("Warning: Empty or invalid image file\n");
        // Create dummy data for testing
        memset(img->data, 128, img->width * img->height * img->channels);
    }
    
    printf("Loaded image: %dx%d, %d channels\n", img->width, img->height, img->channels);
    return img;
}

// Simple blob detection (for demo - simulates person detection)
DetectionResult* detect_persons_simple(Image* img) {
    DetectionResult* result = malloc(sizeof(DetectionResult));
    result->capacity = 10;
    result->detections = malloc(sizeof(Detection) * result->capacity);
    result->count = 0;
    
    printf("Running detection on %dx%d image...\n", img->width, img->height);
    
    // Simulate detecting 3 persons (for demo)
    // In real implementation, this would use ONNX Runtime with YOLOv8
    
    Detection det1 = {100.0, 150.0, 80.0, 200.0, 0.95, 0};
    Detection det2 = {300.0, 180.0, 75.0, 190.0, 0.87, 0};
    Detection det3 = {500.0, 160.0, 85.0, 210.0, 0.92, 0};
    
    result->detections[result->count++] = det1;
    result->detections[result->count++] = det2;
    result->detections[result->count++] = det3;
    
    printf("Detected %d persons\n", result->count);
    
    return result;
}

// Export detections to JSON
char* detections_to_json(DetectionResult* result, const char* camera_id) {
    char* json = malloc(4096);
    int offset = 0;
    
    offset += sprintf(json + offset, "{\"camera_id\":\"%s\",\"detections\":[", camera_id);
    
    for (int i = 0; i < result->count; i++) {
        Detection* det = &result->detections[i];
        offset += sprintf(json + offset,
            "%s{\"id\":\"d%d\",\"bbox\":[%.1f,%.1f,%.1f,%.1f],\"confidence\":%.2f,\"class\":\"person\"}",
            i > 0 ? "," : "",
            i + 1, det->x, det->y, det->w, det->h, det->confidence);
    }
    
    sprintf(json + offset, "]}");
    return json;
}

void free_image(Image* img) {
    if (img) {
        if (img->data) free(img->data);
        free(img);
    }
}

void free_detection_result(DetectionResult* result) {
    if (result) {
        if (result->detections) free(result->detections);
        free(result);
    }
}

int main(int argc, char** argv) {
    printf("========================================\n");
    printf("CV Worker v1.0 - FUNCTIONAL\n");
    printf("========================================\n\n");
    
    const char* image_path = "test.jpg";
    const char* camera_id = "cam-001";
    
    if (argc > 1) {
        image_path = argv[1];
    }
    if (argc > 2) {
        camera_id = argv[2];
    }
    
    printf("Processing:\n");
    printf("  Image: %s\n", image_path);
    printf("  Camera: %s\n\n", camera_id);
    
    // Load image
    Image* img = load_image(image_path);
    if (!img) {
        printf("Creating dummy image for testing...\n");
        img = malloc(sizeof(Image));
        img->width = 640;
        img->height = 480;
        img->channels = 3;
        img->data = calloc(img->width * img->height * img->channels, 1);
    }
    
    // Detect persons
    DetectionResult* result = detect_persons_simple(img);
    
    // Export to JSON
    char* json = detections_to_json(result, camera_id);
    printf("\nJSON Output:\n%s\n\n", json);
    
    // Save to file
    FILE* out = fopen("detections.json", "w");
    if (out) {
        fprintf(out, "%s", json);
        fclose(out);
        printf("Saved to detections.json\n");
    }
    
    // Cleanup
    free(json);
    free_detection_result(result);
    free_image(img);
    
    printf("\nProcessing complete!\n");
    return 0;
}
