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

uploaded = False
# Überprüfung, ob es sich um eine hochgeladene Datei handelt
if 'file' in formdata:
	fileitem = formdata['file']
	if fileitem.file:
		# Lesen der Datei
		content = fileitem.file.read()
		# Verarbeiten der Datei
		if not fileitem.filename:
			print("<p>You have to submit a file!!!</p>")
			exit()
		filename = os.path.basename(fileitem.filename)

		# Speichern der Datei im Verzeichnis '/tmp'
		upload_location = os.environ.get('UPLOAD')
		if not upload_location:
			upload_location = '/sgoinfre/goinfre/Perso/fheid/test/'
		with open(upload_location + filename, 'wb') as f:
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