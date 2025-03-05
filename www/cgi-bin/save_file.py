#!/usr/bin/python3
import cgi, sys, os

file_name = os.getenv("UPLOAD_FILENAME")
file_content = os.getenv("UPLOAD_CONTENT").encode("utf-8")

form = cgi.FieldStorage()

#file_item = form["filename"]							# Get file object
#file_name = "hello.txt" #os.path.basename(file_item.filename)		# Extract filename
#file_content = b"hello" #file_item.file.read()
    
save_path = os.path.join(os.getcwd(), "www", "tmp", file_name)

with open(save_path, "wb") as f:
	f.write(file_content)

print("<h2>Success!</h2>")
print(f'<p>The file "{file_name}" was uploaded successfully.</p>')
