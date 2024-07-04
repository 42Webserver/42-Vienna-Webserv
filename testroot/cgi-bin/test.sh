#/bin/bash

echo "<!DOCTYPE html>
<html>
	<head>
		<meta charset="UTF-8">
		<title> CGI </title>
	</head>
	<body>
		<h1>Argv1: $0</h1>
		<h1>Query: $QUERY_STRING</h1>
		<p>" $(env) "</p>
		<p>" $(ls -la) "<p>
		</body>
<html>"
