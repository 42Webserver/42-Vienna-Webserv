#!/usr/bin/env python3
import cgi
import os
import sys

sys.path.insert(0, './../cgi-bin')

import session_handler

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

cookie = cookie.replace("id=", "")

if cookie and session_handler.checkSessionId(cookie):
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
            print("IT WORKED!!!")
            id = session_handler.addNewSessionId()
            print("Set-Cookie: id=" + id + "; Max-Age=3600" + "\r\n\r\n")
            responseWelcomePage(id)

    responseLoginPage()

exit()
