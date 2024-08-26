#!/usr/bin/env python3
import os
import json
from cgi import FieldStorage

# Definiere das Verzeichnis, aus dem die Dateien gelesen werden sollen
directory_path = './remove'

# Lese alle Dateien im Verzeichnis
files = os.listdir(directory_path)

# Erstelle ein JSON-fähiges Objekt
response_data = {'files': files}

# Setze den Content-Type Header auf application/json
print("Content-Type: application/json\r\n\r\n")

# Konvertiere das Objekt in einen JSON-String und schreibe es aus
print(json.dumps(response_data))
