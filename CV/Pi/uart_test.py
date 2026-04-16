import serial
import time

messages = [
    "GREEN,0,1\n",
    "ORANGE,2,4\n",
    "BROWN,5,6\n"
]

ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=1)
time.sleep(2)

while True:
    for message in messages:
        ser.write(message.encode("utf-8"))
        print("Sent:", message.strip())

        response = ser.readline()
        if response:
            print("Received:", response.decode(errors="ignore").strip())
        time.sleep(1)