#!/usr/bin/python3
import cgi, sys, os

filename_env = os.getenv("UPLOAD_FILENAME")

form = cgi.FieldStorage()

if "filename" not in form or not form["filename"].filename:
    print("<h2>Error: No file uploaded</h2>")
    sys.exit(0)

fileitem = form["filename"] 
fn = os.path.basename(fileitem.filename)
if filename_env:
    fn = filename_env
    
cwd = os.getcwd()
save_path = os.path.join(cwd, fn)

try:
	with open(save_path, "wb") as f:
		f.write(fileitem.file.read())

	print("<h2>Success!</h2>")
	print(f'<p>The file "{fn}" was uploaded successfully.</p>')

except Exception as e:
    print(f'<h2>Error</h2><p>Could not save file: {e}</p>')