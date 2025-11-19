#include <stdio.h>
#include "detector.h"

int main(int argc, char** argv) {
    printf("CV Worker v1.0 - Starting...\n");
    
    if (argc < 2) {
        printf("Usage: %s <image_path>\n", argv[0]);
        return 1;
    }
    
    DetectionResult* result = detect_persons(argv[1]);
    printf("Detected %d persons\n", result->count);
    
    free_detection_result(result);
    return 0;
}
