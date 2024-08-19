#!/bin/bash

# Set the content type to JSON
echo -e "Content-type: application/json\r\n\r\n"

# Directory to list
directory="$PWD/../../../../src"

# List the directory contents and format as JSON
json_output="{\"files\": ["
first=true
for file in "$directory"/*; do
    if [ "$first" = true ]; then
        first=false
    else
        json_output+=", "
    fi
    json_output+="\"$(basename "$file")\""
done
json_output+="]}"

# Output the JSON object
echo "$json_output"
