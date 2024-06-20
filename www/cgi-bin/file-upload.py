#!/usr/bin/env python3

import cgi
import cgitb
import os

cgitb.enable()

form = cgi.FieldStorage()

fileitem = form["filename"]

if (fileitem.filename):
	fname = os.path.basename(fileitem.filename)
	location = os.getcwd() + "/cgi-bin/uploads/"
	open(location + fname, "wb").write(fileitem.file.read())
	message = "The file ", fname, " was uploaded successfully."
else:
	message = "The file upload failed."

print("Content-Type: text/html;charset=utf-8")
print("<h1> ", message, " <h1>")


# print()
# print("<html><head><title>CGI Script</title></head><body>")
# print("<h1>Hello from Python CGI script!</h1>")
# print("</body></html>")
