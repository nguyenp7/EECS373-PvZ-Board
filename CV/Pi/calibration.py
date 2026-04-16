from picamera2 import Picamera2
import cv2
import time

camera = Picamera2()
camera.configure(camera.create_preview_configuration(main={"size": (640, 480)}))
camera.start()
time.sleep(2)

frame = camera.capture_array()
frame_bgr = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
cv2.imwrite("calibration_board.jpg", frame_bgr)

print("Saved calibration_board.jpg")