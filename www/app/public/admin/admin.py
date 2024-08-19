from session_handler import generateSessionId, checkSessionId, checkCookie, addNewSessionId, deleteSessionId
import os
import cgi

def loginPage():
    print("""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Login Page</title>
        <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
        <link rel="stylesheet" href="./../../assets/styles/style.css">
    </head>
    <body>
        <div class="container">
            <div class="navbar">
                <img src="./../../assets/img/Logo.jpg" width="40px" alt="Logo">
                <a href="/">Home</a>
                <a href="/game">Game</a>
                <a href="/overview">Overview</a>
                <a href="/contact">Contact</a>
            </div>
            <div class="login-container">
                <h2 class="login-header">Login</h2>
                <form action="/admin/" method="POST">
                    <div class="form-group">
                        <label for="username">Username</label>
                        <input type="text" class="form-control" id="username" name="username" required>
                    </div>
                    <div class="form-group">
                        <label for="password">Password</label>
                        <input type="password" class="form-control" id="password" name="password" required>
                    </div>
                    <button type="submit" class="btn btn-primary btn-block">Login</button>
                </form>
            </div>
        </div>

        <script src="https://code.jquery.com/jquery-3.5.1.slim.min.js"></script>
        <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.5.4/dist/umd/popper.min.js"></script>
        <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
    </body>
    </html>
    """)
    exit()

def uploadRsp():
    #Now we response deppending on whats inside of PATHINFO upload or delete!!!
    print("""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>File Upload</title>
        <link href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css" rel="stylesheet">

        <link rel="stylesheet" href="./../../assets/styles/style.css">
    </head>
    <body>
        <div class="container">

            <div class="navbar">
                <img src="./../../assets/img/Logo.jpg" width="40px" alt="Logo">
                <a href="/">Home</a>
                <a href="/game">Game</a>
                <a href="/overview">Overview</a>
                <a href="/contact">Contact</a>
            </div>
            <h1 >Upload Files</h1>
            <div class="card">
                <div class="card-body">
                    <form action="./../cgi-bin/file-upload.py" method="post" enctype="multipart/form-data">
                        <div class="form-group">
                            <h5 class="card-title">File Upload</h5>
                            <input type="file" class="form-control-file" id="file" name="file">
                        </div>
                        <button type="submit" class="btn btn-primary">Upload</button>
                    </form>
                </div>
            </div>
        </div>

        <script src="https://code.jquery.com/jquery-3.5.1.slim.min.js"></script>
        <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.9.2/dist/umd/popper.min.js"></script>
        <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
    </body>
    </html>
    """)

def deleteRsp():
    print("""
    <!DOCTYPE html>
    <html lang="de">
    <head>
        <meta charset="UTF-8">
        <title>Fetch zu CGI</title>
        <link href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css" rel="stylesheet">
        <link rel="stylesheet" href="./../../assets/styles/style.css">
    </head>
    <body>
        <div class="container">
            <div class="navbar">
                <img src="./../../assets/img/Logo.jpg" width="40px" alt="Logo">
                <a href="/">Home</a>
                <a href="/game">Game</a>
                <a href="/overview">Overview</a>
                <a href="/contact">Contact</a>
            </div>
            <h1 class="mb-4">Manage Files</h1>
            <button id="loadFiles" class="btn btn-primary mb-4">Load Files</button>
            <div class="card">
                <div class="card-body">
                    <div id="fileList" class="list-group"></div>
                </div>
            </div>

        </div>

        <div class="modal fade" id="errorModal" tabindex="-1" aria-labelledby="errorModalLabel" aria-hidden="true">
            <div class="modal-dialog">
                <div class="modal-content">
                    <div class="modal-header">
                        <h5 class="modal-title" id="errorModalLabel">Error</h5>
                        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                            <span aria-hidden="true">&times;</span>
                        </button>
                    </div>
                    <div class="modal-body" id="errorModalBody">
                        An error occurred.
                    </div>
                    <div class="modal-footer">
                        <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                    </div>
                </div>
            </div>
        </div>

        <script src="https://code.jquery.com/jquery-3.5.1.slim.min.js"></script>
        <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.9.2/dist/umd/popper.min.js"></script>
        <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
        <script>
            document.getElementById('loadFiles').addEventListener('click', function() {
                fetch('/delete/listfiles.py')
                    .then(response => {
                        if (!response.ok) {
                            throw new Error('Network response was not ok');
                        }
                        return response.json();
                    })
                    .then(data => {
                        const fileList = document.getElementById('fileList');
                        if (!fileList) {
                            console.error('Element with ID "fileList" not found.');
                            return;
                        }

                        // Clear the file list before appending new items
                        fileList.innerHTML = '';

                        data.files.forEach(file => {
                            const listItem = document.createElement('div');
                            listItem.className = 'file-item list-group-item';

                            const fileName = document.createElement('span');
                            fileName.textContent = file;

                            const deleteLink = document.createElement('a');
                            deleteLink.textContent = 'Delete';
                            deleteLink.href = '#';
                            deleteLink.className = 'btn btn-delete btn-sm';
                            deleteLink.addEventListener('click', function(event) {
                                event.preventDefault();
                                fetch(`../delete/remove/${file}`, {
                                    method: 'DELETE'
                                })
                                .then(response => {
                                    if (!response.ok) {
                                        throw new Error('Network response was not ok');
                                    }
                                    // Optionally, remove the file from the list after deletion
                                    listItem.remove();
                                    // Alert the user that the file was successfully deleted
                                    alert('Successfully deleted');
                                })
                                .catch(error => {
                                    console.error('Error:', error);
                                    document.getElementById('errorModalBody').textContent = 'An error occurred: ' + error.message;
                                    $('#errorModal').modal('show');
                                });
                            });

                            listItem.appendChild(fileName);
                            listItem.appendChild(deleteLink);
                            fileList.appendChild(listItem);
                        });
                    })
                    .catch(error => {
                        console.error('Error:', error);
                        document.getElementById('errorModalBody').textContent = 'An error occurred: ' + error.message;
                        $('#errorModal').modal('show');
                    });
            });
        </script>
    </body>
    </html>    
    """)

def welcomeRsp():
    print("""
    
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>Welcome</title>
        <link href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css" rel="stylesheet">
        <link rel="stylesheet" href="./../../assets/styles/style.css">
    </head>
    <body>
     <div class="container">
        <div class="navbar navbar-expand-lg navbar-light">
                <div class="d-flex justify-content-between align-items-center w-100">
                    <div class="d-flex align-items-center">
                        <img src="./../../assets/img/Logo.jpg" width="40px" alt="Logo" class="mr-3">
                        <a href="/" class="nav-link">Home</a>
                        <a href="/game" class="nav-link">Game</a>
                        <a href="/overview" class="nav-link">Overview</a>
                        <a href="/contact" class="nav-link">Contact</a>
                    </div>
                    <div class="d-flex">
                        <button id="loginButton" class="btn btn-primary mr-2">Login</button>
                        <button id="logoutButton" class="btn btn-secondary">Logout</button>
                    </div>
                </div>
        </div>
            <h1>Welcome to Demo WebSurfer</h1>
            <div class="center-container">
                <div class="container">
                    <div class="row d-flex justify-content-center">
                        <div class="col-sm-12 col-lg-4 mb-4">
                            <div class="circle-img-container">
                                <img src="./../../assets/img/nsassenb.jpg" alt="Profile 1" class="img-fluid circle-img">
                                <div class="hover-text"><h2>Nikolai Sassenbach</h2><br>
                                        <h3>Responsible</h3> 
                                        <ul>
                                            <li>Poll-Master</li>
                                            <li>Response</li>
                                        </ul>
                                </div>
                            </div>
                        </div>
                        <div class="col-sm-12 col-lg-4 mb-4">
                            <div class="circle-img-container">
                                <img src="./../../assets/img/fheid.jpg" alt="Profile 2" class="img-fluid circle-img">
                                <div class="hover-text"><h2>Florian Heid</h2><br>
                                    <h3>Responsible</h3>
                                    <ul>
                                        <li>CGI-Master</li>
                                        <li>Configuration</li>
                                    </ul>
                                </div>
                            </div>
                        </div>
                        <div class="col-sm-12 col-lg-4 mb-4">
                            <div class="circle-img-container">
                                <img src="./../../assets/img/fwechsle.jpg" alt="Profile 3" class="img-fluid circle-img">
                                <div class="hover-text"><h2>Felix Wechsler</h2><br>
                                    <h3>Responsible</h3>
                                    <ul>
                                        <li>Response-Master</li>
                                        <li>Configuration </li>
                                    </ul>
                                    
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
            
            <div class="row">
                <div class="col-md-6">
                    <div class="card">
                        <div class="card-body">
                            <h5 class="card-title">Manage Files</h5>
                            <p class="card-text">View and manage your files.</p>
                            <a href="/admin/delete" class="btn btn-primary">Go to Manage Files</a>
                        </div>
                    </div>
                </div>
                <div class="col-md-6">
                    <div class="card">
                        <div class="card-body">
                            <h5 class="card-title">Upload Files</h5>
                            <p class="card-text">Upload new files to the system.</p>
                            <a href="/admin/upload" class="btn btn-primary">Go to Upload Files</a>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        <script src="https://code.jquery.com/jquery-3.5.1.slim.min.js"></script>
        <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.9.2/dist/umd/popper.min.js"></script>
        <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
        <script>
            document.getElementById('loginButton').addEventListener('click', function() {
                // Redirect to login page or show login modal
                window.location.href = '/admin/';
            });

            document.getElementById('logoutButton').addEventListener('click', function() {
                // Perform logout action, e.g., clear session and redirect to home page
                // This is a simple example, you might need to call an API to handle logout
                alert('You have been logged out.');
                window.location.href = '/admin/logout';
            });
        </script>    
    </body>
    </html>    
    """)

cookie = os.environ.get('HTTP_COOKIE')
query = os.environ.get('PATH_INFO')

if query == '/logout':
    deleteSessionId(cookie)
    welcomeRsp()
    exit()

if cookie:
    cookie = cookie.replace("id=", "")

if cookie and checkSessionId(cookie):
    if query == '/upload':
        uploadRsp()
    elif query == '/delete':
        deleteRsp()
    else:
        welcomeRsp()

else:

    formdata = cgi.FieldStorage()

    if 'username' and 'password' in formdata:

        username = formdata.getvalue('username')
        password = formdata.getvalue('password')

        if username != 'admin' or password != 'hello123':
            loginPage()
            exit()
        else:
            id = addNewSessionId()
            print("Set-Cookie: id=" + id + "; Max-Age=3600" + "\r\n\r\n")
            if query == 'upload':
                uploadRsp()
                exit()
            elif query == 'delete':
                deleteRsp()
                exit()
            else:
                welcomeRsp()
                exit()
    loginPage()

exit()
