#!/usr/bin/env python3
import sys

image_path = "./www/images/42.png"

try:
    # Read image as binary
    with open(image_path, "rb") as f:
        img_data = f.read()

    print("Content-Type: image/png")
    print("Content-Length:", len(img_data))
    print("Status: 200 OK")
    print()

    sys.stdout.buffer.write(img_data)

except FileNotFoundError:
    print("Status: 404 Not Found")
    print("Content-Type: text/plain")
    print()
    print("Error: Image not found.")
