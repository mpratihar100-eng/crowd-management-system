#ifndef DETECTOR_H
#define DETECTOR_H

typedef struct {
    float confidence;
    int x, y, w, h;
} Detection;

typedef struct {
    Detection* detections;
    int count;
} DetectionResult;

DetectionResult* detect_persons(const char* image_path);
void free_detection_result(DetectionResult* result);

#endif // DETECTOR_H
