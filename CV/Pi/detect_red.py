from picamera2 import Picamera2
import cv2
import numpy as np
import time

camera = Picamera2()
camera.configure(camera.create_preview_configuration(main={"size": (640, 480)}))
camera.start()

time.sleep(2)

frame_rgb = camera.capture_array()

# Convert correctly from RGB to HSV
hsv_image = cv2.cvtColor(frame_rgb, cv2.COLOR_RGB2HSV)

lower_red_1 = np.array([0, 120, 70])
upper_red_1 = np.array([10, 255, 255])

lower_red_2 = np.array([170, 120, 70])
upper_red_2 = np.array([180, 255, 255])

mask_1 = cv2.inRange(hsv_image, lower_red_1, upper_red_1)
mask_2 = cv2.inRange(hsv_image, lower_red_2, upper_red_2)
red_mask = mask_1 | mask_2

kernel = np.ones((5, 5), np.uint8)
red_mask = cv2.morphologyEx(red_mask, cv2.MORPH_OPEN, kernel)
red_mask = cv2.morphologyEx(red_mask, cv2.MORPH_CLOSE, kernel)

contours, _ = cv2.findContours(red_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

detected = False

# Convert to BGR only for OpenCV drawing/saving consistency
frame_bgr = cv2.cvtColor(frame_rgb, cv2.COLOR_RGB2BGR)

for contour in contours:
    area = cv2.contourArea(contour)
    if area < 500:
        continue

    x, y, width, height = cv2.boundingRect(contour)
    center_x = x + width // 2
    center_y = y + height // 2

    cv2.rectangle(frame_bgr, (x, y), (x + width, y + height), (255, 255, 255), 2)
    cv2.putText(
        frame_bgr,
        f"RED ({center_x},{center_y})",
        (x, y - 10),
        cv2.FONT_HERSHEY_SIMPLEX,
        0.6,
        (255, 255, 255),
        2
    )

    print(f"Detected red object at x={center_x}, y={center_y}, area={area}")
    detected = True

cv2.imwrite("red_result.jpg", frame_bgr)
cv2.imwrite("red_mask.jpg", red_mask)

if not detected:
    print("No red object detected")

print("Saved red_result.jpg and red_mask.jpg")