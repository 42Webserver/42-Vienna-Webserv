#!/usr/bin/env python3

import cgi
import cgitb
import os

cgitb.enable()

#!/usr/bin/env python3
import cgi

# Initialisierung von formdata mit cgi.FieldStorage()
formdata = cgi.FieldStorage()

# Zugriff auf das hochgeladene Dateiobjekt
fileitem = formdata['file']

# Überprüfung, ob es sich um eine hochgeladene Datei handelt
if fileitem.file:
    # Lesen der Datei
    content = fileitem.file.read()
    # Verarbeiten der Datei
    filename = os.path.basename(fileitem.filename)
    
    # Speichern der Datei im Verzeichnis '/tmp'
    with open(f'/sgoinfre/goinfre/Perso/fwechsle/test/{filename}', 'wb') as f:
        f.write(content)
    uploaded = True  # Simulierte Bedingung, um zu zeigen, ob das Hochladen erfolgreich war

print("""
<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="UTF-8">
<title>Datei Upload</title>
</head>
<body>
""")
if (uploaded):
    print("<p>Danke, Ihre Datei wurde erfolgreich hochgeladen.</p>")
else:
    print("<p>Fehler beim Hochladen der Datei. Bitte versuchen Sie es erneut.</p>")
print("</body></html>")

exit()


formData = cgi.FieldStorage()
print(formdata['file'])

print("<h1> ", "hello", " <h1>")

exit()

uploaded_file = formData.getvalue("name")

print(formData)
print(uploaded_file)
print(uploaded_file.value)

exit()

if (uploaded_file.value):

	save_path = os.getcwd() + "www/cgi-bin/uploads/" + uploaded_file.value

	os.makedirs(os.path.dirname(save_path), exist_ok=True)

	with open(save_path, 'wb') as f:
		f.write(uploaded_file.file.read())

	message = "The file ", uploaded_file.value, " was uploaded successfully."
else:
	message = "The file upload failed."

print("<h1> ", message, " <h1>")

exit()

#!/usr/bin/env python3

import cgi
import os

# Parse the form data
form = cgi.FieldStorage()

print(form)

exit()

for field_name in form.keys():
    # Print the field name and its value
    print(f"{field_name}")

# print(form)

exit()

# Check if 'filename' field exists and contains a file
if 'filename' in form and hasattr(form['filename'], 'file'):
    fn = form['filename']
    if (fn):
         print("YES")

    # Attempt to read the file content
    try:
        content = fn.file.read()
        # Process the file content here
        print(content)
    except AttributeError as e:
        print(f"Failed to read file: {e}")
else:
    print("No file was uploaded or 'filename' field is missing.")

exit()

import cgi
import os

# Parse the form data
form = cgi.FieldStorage()

print(form)

# exit()
fn = form['file']

print(fn)

fn.file.read()



exit()

print(form)

if (form.file):
     print("HEUREKA")

if ('filename' in form):
	print("XES")

# Check if a file was uploaded
if 'file' in form:
    # Retrieve the file object
    uploaded_file = form['file']

    # Define where to save the file on the server
    save_path = os.getcwd() + 'www/cgi-bin/files/' + uploaded_file.filename

    # Ensure the directory exists
    os.makedirs(os.path.dirname(save_path), exist_ok=True)

    # Save the file
    with open(save_path, 'wb') as f:
        f.write(uploaded_file.file.read())

    print(f"<p>File '{uploaded_file.filename}' has been successfully uploaded.</p>")
else:
    print("<p>No file was uploaded.</p>")
