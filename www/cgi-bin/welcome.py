#!/usr/bin/env python3

import cgi

# Create instance of FieldStorage
form = cgi.FieldStorage()

print("das ist im python script 'welcome'!")

# Get data from fields
first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')

print("<html>")
print("<head>")
print("<title>CGI Script Output</title>")
print("</head>")
print("<body>")
print("<h2>Hello {} {}</h2>".format(first_name, last_name))
print("</body>")
print("</html>")
