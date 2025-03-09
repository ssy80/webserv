#!/usr/bin/python3
import datetime

body = f"<h2>Current Server Time</h2><p>{datetime.datetime.now()}</p>"

content_length = len(body.encode('utf-8'))

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print(f"Content-Length: {content_length}")
print("Connection: close\n")

print(body)
