import cv2
import numpy as np
import time
import json
from datetime import datetime

class PrivacyFirstDetector:
    '''
    Privacy-compliant person detection that:
    - Detects motion/presence without identifying individuals
    - Uses background subtraction for crowd counting
    - Never stores identifiable information
    - Immediately anonymizes all data
    '''
    
    def __init__(self, camera_id='cam-001'):
        self.camera_id = camera_id
        
        # Background subtractor for motion detection
        self.bg_subtractor = cv2.createBackgroundSubtractorMOG2(
            history=500,
            varThreshold=16,
            detectShadows=False
        )
        
        # Minimum blob size (in pixels) to count as a person
        self.min_blob_area = 1000  # Adjust based on camera distance
        self.max_blob_area = 50000
        
        print(f'Initialized Privacy-First Detector for {camera_id}')
        print('Privacy features:')
        print('  ✓ No facial recognition')
        print('  ✓ No individual tracking')
        print('  ✓ No frame storage')
        print('  ✓ Count-only output')
    
    def detect_people_count(self, frame):
        '''
        Detect number of people using background subtraction
        Returns only COUNT - no identifying information
        '''
        
        # Apply background subtraction
        fg_mask = self.bg_subtractor.apply(frame)
        
        # Remove noise
        kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))
        fg_mask = cv2.morphologyEx(fg_mask, cv2.MORPH_OPEN, kernel)
        fg_mask = cv2.morphologyEx(fg_mask, cv2.MORPH_CLOSE, kernel)
        
        # Find contours (blobs representing people)
        contours, _ = cv2.findContours(
            fg_mask, 
            cv2.RETR_EXTERNAL, 
            cv2.APPROX_SIMPLE
        )
        
        # Count valid blobs (people)
        people_count = 0
        detections = []
        
        for contour in contours:
            area = cv2.contourArea(contour)
            
            # Filter by size (remove noise and very large objects)
            if self.min_blob_area < area < self.max_blob_area:
                people_count += 1
                
                # Get bounding box (for counting only, not identification)
                x, y, w, h = cv2.boundingRect(contour)
                
                # Store only anonymous position data
                detections.append({
                    'id': f'person_{people_count}',  # Generic ID, changes each frame
                    'bbox': [x, y, w, h],
                    'centroid': [x + w//2, y + h//2],
                    'area': int(area),
                    'confidence': 0.85  # High confidence for motion-based
                })
        
        return people_count, detections
    
    def create_privacy_mask(self, frame, detections):
        '''
        Create anonymized visualization (optional for debugging)
        Shows boxes/circles without showing actual people
        '''
        # Create blank canvas (don't show actual frame)
        height, width = frame.shape[:2]
        anonymous_frame = np.zeros((height, width, 3), dtype=np.uint8)
        
        # Draw only anonymized representations
        for det in detections:
            x, y, w, h = det['bbox']
            cx, cy = det['centroid']
            
            # Draw circle at centroid (no actual person visible)
            cv2.circle(anonymous_frame, (cx, cy), 20, (0, 255, 0), -1)
            
            # Draw bounding box
            cv2.rectangle(anonymous_frame, (x, y), (x+w, y+h), (255, 255, 255), 2)
            
            # Add count label
            cv2.putText(anonymous_frame, det['id'], (x, y-10),
                       cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
        
        return anonymous_frame
    
    def process_frame(self, frame):
        '''
        Process a single frame and return ONLY count data
        No identifiable information is retained
        '''
        
        # Detect people count
        people_count, detections = self.detect_people_count(frame)
        
        # Create anonymous result (NO identifiable data)
        result = {
            'camera_id': self.camera_id,
            'timestamp': datetime.utcnow().isoformat() + 'Z',
            'people_count': people_count,
            'detections': [
                {
                    'id': det['id'],
                    'centroid': det['centroid'],
                    'confidence': det['confidence']
                    # NOTE: No facial features, no tracking IDs, no PII
                }
                for det in detections
            ],
            'privacy_compliant': True,
            'data_retention': 'none'  # No frames stored
        }
        
        return result, people_count

def process_camera_stream(camera_source=0, camera_id='cam-001'):
    '''
    Process camera stream with privacy-first detection
    '''
    
    print('='*60)
    print('PRIVACY-FIRST CROWD DETECTION')
    print('='*60)
    print(f'Camera: {camera_id}')
    print(f'Source: {camera_source}')
    print()
    
    # Initialize detector
    detector = PrivacyFirstDetector(camera_id)
    
    # Open camera
    if isinstance(camera_source, str) and camera_source.startswith('rtsp'):
        print(f'Connecting to RTSP: {camera_source}')
        cap = cv2.VideoCapture(camera_source)
    else:
        camera_idx = int(camera_source) if str(camera_source).isdigit() else 0
        print(f'Opening camera {camera_idx}')
        cap = cv2.VideoCapture(camera_idx)
    
    if not cap.isOpened():
        print('ERROR: Cannot open camera')
        return
    
    print('Camera opened successfully!')
    print('Press Q to quit\n')
    
    frame_count = 0
    
    while True:
        ret, frame = cap.read()
        
        if not ret:
            print('Failed to grab frame')
            break
        
        frame_count += 1
        
        # Process frame (privacy-compliant)
        result, people_count = detector.process_frame(frame)
        
        # Log results (no PII)
        if frame_count % 30 == 0:  # Every second at 30fps
            print(f'Frame {frame_count}: {people_count} people detected')
            print(f'  Privacy: ✓ No identification')
            print(f'  Data: {json.dumps(result, indent=2)}')
            print()
        
        # Create anonymized visualization (optional)
        anonymous_view = detector.create_privacy_mask(frame, result['detections'])
        
        # Show ONLY anonymous visualization (not actual camera feed)
        cv2.imshow('Privacy-First Detection (Anonymized)', anonymous_view)
        
        # Press Q to quit
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
    cap.release()
    cv2.destroyAllWindows()
    print('\nCamera processing stopped')

if __name__ == '__main__':
    import sys
    
    # Get camera source from command line or use default
    camera = sys.argv[1] if len(sys.argv) > 1 else '0'
    cam_id = sys.argv[2] if len(sys.argv) > 2 else 'privacy-cam-001'
    
    process_camera_stream(camera, cam_id)
