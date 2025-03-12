#!/usr/bin/env python3

body = "<!DOCTYPE html><html><body><h1>HELLO WORLD !</h1></body></html>"
 
content_length = len(body.encode('utf-8'))
 
print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print(f"Content-Length: {content_length}")
print("Connection: close\n")

print(body)