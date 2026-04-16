from picamera2 import Picamera2
import cv2
import time

camera = Picamera2()
camera.configure(camera.create_preview_configuration(main={"size": (640, 480)}))
camera.start()

time.sleep(2)

frame = camera.capture_array()
print("Captured frame shape:", frame.shape)

cv2.imwrite("test_frame.jpg", frame)
print("Saved test_frame.jpg")