#!/usr/bin/python3
import cgi, sys, os

try:
	file_name = os.getenv("UPLOAD_FILENAME")
	file_content = os.getenv("UPLOAD_CONTENT").encode("utf-8")

	form = cgi.FieldStorage()

	save_path = os.path.join(os.getcwd(), "www", "tmp", file_name)

	with open(save_path, "wb") as f:
		f.write(file_content)

	print("<h2>Success!</h2>")
	print(f'<p>The file "{file_name}" was uploaded successfully.</p>')

except:
    print("<h2>Failure</h2>")