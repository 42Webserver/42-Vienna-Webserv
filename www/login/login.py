#!/usr/bin/env python3
import cgi
import os
import uuid
import datetime

def generateSessionId():
    return str(uuid.uuid4())

def addNewSessionId():

    if not os.path.exists("/tmp/webserv_sessions"):
        os.makedirs("/tmp/webserv_sessions")

    id = generateSessionId()

    file_path = os.path.join("/tmp/webserv_sessions", id)
    open(file_path, 'w').close()
    print("Successfully created new file for session-id: ", id)
    return (id)

def checkSessionId(id):

    file_path = os.path.join("/tmp/webserv_sessions", id)

    if not os.path.exists(file_path):
        return (False)
    
    creation_time = os.path.getctime(file_path)
    creation_datetime = datetime.datetime.fromtimestamp(creation_time)
    now = datetime.datetime.now()
    time_difference = now - creation_datetime
    
    if time_difference > datetime.timedelta(hours=3):
        os.remove(file_path)
        return (False)
    else:
        return (True)

def responseLoginPage():
    print("It seems like you're a new visitor, please login...<br></br>")
    print("""
    <form action="./../login/login.py" method="post" enctype="multipart/form-data">
    <div class="container">
      <label for="username"><b>Username</b></label>
      <input type="text" placeholder="Enter Username" name="username" required>
  
        <br></br>

      <label for="password"><b>Password</b></label>
      <input type="password" placeholder="Enter Password" name="password" required>
  
      <br></br>

      <button type="submit">Login</button>
    </div>
    </form> 
    """)
    exit()

def responseWelcomePage(id):

    print("""
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="UTF-8">
        <title> CGI </title>
    </head>

    <body>
            <p>Successfully logged in!</p>
            <form action="./../cgi-bin/welcome.py" method="post" enctype="application/x-www-form-urlencoded">
                Firstname: <input type="text" name="first_name" />

                <br />

                Lastname: <input type="text" name="last_name" />

                <input type="submit" value="Submit" />
            </form>
    </body>
    </html>
    """)
    exit()

cookie = os.environ.get('HTTP_COOKIE')

# cookie = "37d01189-a40e-4b49-be64-6e5b6fe44a2e"

if cookie and checkSessionId(cookie):
    print("VALID SESSION-ID IN DIRECTORY!!!")
    responseWelcomePage(cookie)

else:

    formdata = cgi.FieldStorage()

    if 'username' and 'password' in formdata:

        username = formdata.getvalue('username')
        password = formdata.getvalue('password')

        if username != 'admin' or password != 'hello123':
            responseLoginPage()
            exit()
        else:
            addNewSessionId()
            print("Set-Cookie: id=", id, "\r\n\r\n")
            responseWelcomePage(addNewSessionId())

    responseLoginPage()

exit()