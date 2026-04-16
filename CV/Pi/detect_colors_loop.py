from picamera2 import Picamera2
import cv2
import numpy as np
import time

camera = Picamera2()
camera.configure(camera.create_preview_configuration(main={"size": (640, 480)}))
camera.start()

time.sleep(2)

kernel = np.ones((5, 5), np.uint8)

color_ranges = {
    "RED": [
        (np.array([0, 120, 70]), np.array([10, 255, 255])),
        (np.array([170, 120, 70]), np.array([180, 255, 255]))
    ],
    "GREEN": [
        (np.array([35, 80, 50]), np.array([85, 255, 255]))
    ],
    "BLUE": [
        (np.array([90, 80, 50]), np.array([130, 255, 255]))
    ],
    "YELLOW": [
        (np.array([20, 100, 100]), np.array([35, 255, 255]))
    ]
}

while True:
    frame_rgb = camera.capture_array()
    hsv_image = cv2.cvtColor(frame_rgb, cv2.COLOR_RGB2HSV)
    frame_bgr = cv2.cvtColor(frame_rgb, cv2.COLOR_RGB2BGR)

    detections = []

    for color_name, ranges in color_ranges.items():
        mask = None

        for lower_bound, upper_bound in ranges:
            current_mask = cv2.inRange(hsv_image, lower_bound, upper_bound)
            if mask is None:
                mask = current_mask
            else:
                mask = mask | current_mask

        mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
        mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)

        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        for contour in contours:
            area = cv2.contourArea(contour)
            if area < 500:
                continue

            x, y, width, height = cv2.boundingRect(contour)
            center_x = x + width // 2
            center_y = y + height // 2

            detections.append({
                "color": color_name,
                "x": center_x,
                "y": center_y,
                "area": area
            })

            cv2.rectangle(frame_bgr, (x, y), (x + width, y + height), (255, 255, 255), 2)
            cv2.putText(
                frame_bgr,
                f"{color_name} ({center_x},{center_y})",
                (x, y - 10),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.6,
                (255, 255, 255),
                2
            )

    if detections:
        print("Detections:")
        for detection in detections:
            print(f'{detection["color"]},{detection["x"]},{detection["y"]},{int(detection["area"])}')
    else:
        print("No objects detected")

    cv2.imwrite("latest_result.jpg", frame_bgr)
    time.sleep(0.2)