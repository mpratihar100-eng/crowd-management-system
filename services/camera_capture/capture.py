import cv2
import requests
import time
import json
import base64
import os

API_URL = os.getenv('API_URL', 'http://api:8080/api/v1')
CAMERA_ID = os.getenv('CAMERA_ID', 'laptop-cam-001')
CAMERA_SOURCE = os.getenv('CAMERA_SOURCE', '0')  # 0 = laptop webcam
RTSP_URL = os.getenv('RTSP_URL', '')  # For CCTV cameras

def capture_from_camera():
    print(f'Starting camera capture...')
    print(f'Camera ID: {CAMERA_ID}')
    print(f'Source: {CAMERA_SOURCE}')
    
    # Open camera (0 = laptop webcam, or RTSP URL for CCTV)
    if RTSP_URL:
        print(f'Connecting to RTSP: {RTSP_URL}')
        cap = cv2.VideoCapture(RTSP_URL)
    else:
        camera_index = int(CAMERA_SOURCE) if CAMERA_SOURCE.isdigit() else 0
        print(f'Opening laptop camera {camera_index}')
        cap = cv2.VideoCapture(camera_index)
    
    if not cap.isOpened():
        print('ERROR: Could not open camera!')
        return
    
    print('Camera opened successfully!')
    print(f'Resolution: {int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))}x{int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))}')
    
    frame_count = 0
    
    while True:
        ret, frame = cap.read()
        
        if not ret:
            print('Failed to grab frame, retrying...')
            time.sleep(1)
            continue
        
        frame_count += 1
        
        # Process every 15 frames (1 fps at 15fps camera)
        if frame_count % 15 == 0:
            # Resize for processing
            resized = cv2.resize(frame, (640, 480))
            
            # Encode as JPEG
            _, buffer = cv2.imencode('.jpg', resized)
            img_base64 = base64.b64encode(buffer).decode('utf-8')
            
            # Send to API for processing
            try:
                payload = {
                    'camera_id': CAMERA_ID,
                    'frame_id': f'frame-{frame_count}',
                    'timestamp': int(time.time()),
                    'image': img_base64
                }
                
                # In real system, this would go to CV worker
                # For now, just log it
                print(f'Captured frame {frame_count} from {CAMERA_ID}')
                
                # Save frame locally for testing
                cv2.imwrite(f'/tmp/frame_{frame_count}.jpg', resized)
                
            except Exception as e:
                print(f'Error sending frame: {e}')
        
        # Limit to ~15 FPS
        time.sleep(0.066)
    
    cap.release()

if __name__ == '__main__':
    print('='*50)
    print('Camera Capture Service v1.0')
    print('='*50)
    capture_from_camera()
