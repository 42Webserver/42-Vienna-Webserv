#!/usr/bin/env python3

import os
import cgi
from urllib.parse import parse_qs

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')

query_string = os.environ.get('QUERY_STRING')
params = parse_qs(query_string)

print("<html>")
print("<head>")
print("<title>CGI Script Output</title>")
print("</head>")
print("<body>")
print("<h2>Hello {} {}</h2>".format(first_name, last_name))
for key, values in params.items():
    print("<p>" + key + ": ")
    for value in values:
        print(value)
    print("</p>")
print("</body>")
print("</html>")
