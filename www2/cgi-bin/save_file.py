#!/usr/bin/python3
import cgi, sys, os

file_name = os.getenv("UPLOAD_FILENAME")
file_content = os.environb.get(b"UPLOAD_CONTENT")  
save_path = os.path.join(os.getcwd(), file_name)

try:    
    with open(save_path, "wb") as f:
        f.write(file_content)
    
    body = "<h2>Success!</h2>" + f'<p>The file "{file_name}" was uploaded successfully.</p>'
    content_length = len(body.encode("utf-8"))
    
    print("HTTP/1.1 201 Accepted")
    print("Content-Type: text/html")
    print(f"Content-Length: {content_length}")
    print("Connection: close\n")

    print(body)

except:
    body = "<h2>Error: Unable to save file</h2>" + file_name
    content_length = len(body.encode("utf-8"))
    
    print("HTTP/1.1 500 Internal Server Error")
    print("Content-Type: text/html")
    print(f"Content-Length: {content_length}")
    print("Connection: close\n")

    print(body)