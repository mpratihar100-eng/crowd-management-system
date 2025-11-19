import cv2
import numpy as np

print('Testing Privacy-First Detection')
print('='*60)
print()
print('This test will:')
print('1. Open your camera')
print('2. Detect people using background subtraction')
print('3. Show ONLY anonymous circles (not actual video)')
print('4. Count people without identifying them')
print()
print('Press Q to quit')
print('='*60)

# Import detector
import sys
sys.path.append('services/privacy_cv')
from detector import process_camera_stream

# Run with laptop camera
process_camera_stream(0, 'test-camera')
