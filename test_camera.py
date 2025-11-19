import cv2
import time

print('Testing laptop camera...')
print('Press Q to quit')

# Open laptop camera
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print('ERROR: Cannot open camera!')
    exit()

print('Camera opened successfully!')

while True:
    ret, frame = cap.read()
    
    if not ret:
        print('Failed to grab frame')
        break
    
    # Show the frame
    cv2.imshow('Crowd Management - Camera Feed', frame)
    
    # Press Q to quit
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
print('Camera closed')
