# #!/usr/bin/env python3

# import cgi
# import cgitb
# import os

# cgitb.enable()

# print("das ist im python script 'file-upload'!")

# formData = cgi.FieldStorage()

# uploaded_file = formData['filename']

# if (uploaded_file.value):

# 	save_path = os.getcwd() + "www/cgi-bin/uploads/" + uploaded_file.value

# 	os.makedirs(os.path.dirname(save_path), exist_ok=True)

# 	with open(save_path, 'wb') as f:
# 		f.write(uploaded_file.file.read())

# 	message = "The file ", uploaded_file.value, " was uploaded successfully."
# else:
# 	message = "The file upload failed."

# print("<h1> ", message, " <h1>")

#!/usr/bin/env python3

import cgi
import os

# Parse the form data
form = cgi.FieldStorage()

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
