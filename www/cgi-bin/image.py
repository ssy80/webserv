#!/usr/bin/env python3
import sys

image_path = "./www/images/rainbow.png"

try:
    with open(image_path, "rb") as f:
        img_data = f.read()

    sys.stdout.buffer.write(b"Content-Type: image/png\r\n\r\n")
    sys.stdout.buffer.write(img_data)

except Exception as e:
    sys.stdout.buffer.write(b"Content-Type: text/plain\r\n\r\n")
    sys.stdout.buffer.write(f"Error: {str(e)}".encode())
