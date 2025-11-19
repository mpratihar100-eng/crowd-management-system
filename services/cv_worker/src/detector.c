#include "detector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    float confidence;
    int x, y, w, h;
} Detection;

typedef struct {
    Detection* detections;
    int count;
} DetectionResult;

DetectionResult* detect_persons(const char* image_path) {
    DetectionResult* result = malloc(sizeof(DetectionResult));
    result->count = 0;
    result->detections = NULL;
    
    printf("Processing image: %s\n", image_path);
    
    // TODO: Implement ONNX Runtime inference
    
    return result;
}

void free_detection_result(DetectionResult* result) {
    if (result) {
        if (result->detections) free(result->detections);
        free(result);
    }
}
