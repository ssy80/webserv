#!/usr/bin/env python3
import sys

image_path = "./www/images/42.png"

with open(image_path, "rb") as f:
    img_data = f.read()

sys.stdout.buffer.write(img_data)
