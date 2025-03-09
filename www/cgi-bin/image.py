#!/usr/bin/env python3
import sys
import requests

image_url = "https://picsum.photos/536/354"

try:
    # Read image as binary
    response = requests.get(image_url, stream=True)
    response.raise_for_status()
    
    body = response.content
    content_length = len(body)
    
    print("HTTP/1.1 200 OK")
    print("Content-Type: image/jpeg")
    print(f"Content-Length: {content_length}")
    print("Connection: close\n", end="")

    sys.stdout.buffer.write(body)

except FileNotFoundError:
    body = "<h2>Error: Unable to fetch image.</h2>"
    content_length = len(body.encode("utf-8"))
    
    print("HTTP/1.1 500 Internal Server Error")
    print("Content-Type: text/html")
    print(f"Content-Length: {content_length}")
    print("Connection: close\n")

    print(body)
