# **🌐 Webserv - An HTTP Server Project**

Welcome to **Webserv**! This project was developed by a dedicated team of three developers: **Nikolai**, **Florian**, and **Felix**. Our goal was to create our own HTTP server to gain a deep understanding of how web servers function. Through this project, we explored various aspects of server development, enhanced our programming skills, and gained valuable experience in teamwork and project management.

## **📚 Key Learnings**

During the development of Webserv, we acquired a range of skills and knowledge:

- **💻 C++ Programming**: We deepened our understanding of C++ and learned how to effectively use the language for server development.
- **🔗 Network Programming**: We explored socket programming and the handling of HTTP requests and responses.
- **⚠️ Error Handling**: We implemented robust error-handling mechanisms to ensure server stability.
- **🛠️ Configuration Management**: We developed a flexible configuration file format for easy server management.
- **🚀 CGI Execution**: We enabled dynamic content generation through CGI scripts while maintaining server responsiveness.
- **🤝 Team Collaboration on GitHub**: We utilized GitHub branches, issues, and pull requests for efficient project management.

## **⚙️ Configuration Keywords**

Here is a brief description of the key configuration keywords used in our configuration file:

- **`🔊 listen` (host & port)**: Specifies the host and port for incoming connections. Example: `listen localhost:8080;`
- **`🆔 server_name`**: Defines the server's name, useful for differentiating between multiple server blocks.
- **`📍 location`**: Defines a block for handling requests to a specific URI with unique settings.
- **`📄 index`**: Specifies the default file to serve when a directory is requested. Example: `index index.html;`
- **`🗂️ root`**: Sets the base directory for serving files. Example: `root /your/path/app;`
- **`📏 client_max_body_size`**: Limits the maximum size of the request body, specified in kilobytes.
- **`❗ error_page`**: Specifies custom error pages for specific HTTP status codes. Example: `error_page 404 /path/to/error_page;`
- **`🚫 allowed_methods`**: Defines permitted HTTP methods (e.g., GET, POST, DELETE) for a specific location.
- **`📜 autoindex`**: Enables or disables directory listing for a specific location.
- **`🔄 return`**: Sends a specific HTTP response code and optionally redirects the client to another URL. Example: `return 301 /;`
- **`🔧 extensions`**: Associates file extensions with specific behaviors, such as executing scripts.
- **`⚙️ cgi_methods`**: Lists HTTP methods allowed for CGI scripts in a location. Default is no methods allowed.
- **`🔍 script_path`**: Defines the path to the interpreter for executing CGI scripts. Example: `script_path /usr/bin/python3;`
- **`📂 upload`**: Specifies the directory for storing uploaded files. Example: `upload /your/path/app/public/delete/remove/;`

This configuration structure allows for flexible and powerful setup, enabling Webserv to handle various requests and serve content effectively.

---

## **📝 Example Configuration File: `webserv.conf`**

Below is an example configuration file to help you launch your HTTP server. It is structured to handle various routes and methods, ensuring smooth operation.

```nginx
http {
    server {
        listen localhost:8080;  # The server will listen on localhost at port 8080
        root /your/path/app;    # Base directory for serving files

        location / {
            index index.html;    # Default file to serve for the root location
            allowed_methods GET; # Allowed methods for this location
        }

        location /delete {
            root /your/path/app/public/delete/;  # Directory for delete operations
            cgi_methods POST GET;                 # Allowed methods for CGI
            extension .py;                        # File extension for CGI scripts
            script_path /usr/bin/python3;        # Path to the Python interpreter
            error_page 404 /public/error/404.html; # Custom error page for 404 errors
        }

        location /contact {
            root /your/path/app/public/contact;  # Directory for contact page
            index contact.html;                  # Default file for contact location
        }

        location /game {
            root /your/path/app/public/game;     # Directory for game page
            index game.html;                     # Default file for game location
            allowed_methods GET;                 # Allowed methods for this location
        }

        location /overview {
            root /your/path/app/public/overview; # Directory for overview page
            index overview.html;                 # Default file for overview location
            cgi_methods POST GET;                # Allowed methods for CGI
            extension .py .sh;                   # File extensions for CGI scripts
            script_path /usr/bin/python3 /bin/bash; # Paths to interpreters for CGI
        }

        location /admin {
            root /your/path/app/public/admin;    # Directory for admin page
            allowed_methods GET POST;            # Allowed methods for this location
            cgi_methods POST GET;                # Allowed methods for CGI
            extension .py;                       # File extension for CGI scripts
            script_path /usr/bin/python3;       # Path to the Python interpreter
            index admin.py;                      # Default file for admin location
        }

        location /cgi-bin {
            root /your/path/app/public/cgi-bin/; # Directory for CGI scripts
            cgi_methods POST GET;                 # Allowed methods for CGI
            extension .py .sh;                    # File extensions for CGI scripts
            script_path /usr/bin/python3 /bin/bash; # Paths to interpreters for CGI
            upload /your/path/app/public/delete/remove/; # Directory for file uploads
            return 301 /;                        # Redirect to root on access
        }
    }
}
```

## **🚀 How to Launch the Server**

Follow these steps to get your server up and running:

1. **📥 Pull the GitHub Repository**:
   ```bash
   git clone https://github.com/yourusername/webserv.git
   ```
   Replace `yourusername` with the actual username of the repository owner.

2. **📂 Navigate to the Project Directory**:
   Change into the directory of the cloned repository:
   ```bash
   cd webserv
   ```

3. **⚙️ Compile and Start the Server with Default Configuration**:
   You can compile and start the server with the default configuration file in one step by running:
   ```bash
   make run
   ```

4. **🔐 Ensure the Configuration File Has the Right Permissions**:
   Make sure that the configuration file (`webserv.conf`) has the appropriate permissions to be read by the server. You can set the permissions using:
   ```bash
   chmod 644 webserv.conf
   ```

5. **🌐 Access the Server**:
   Open your web browser and navigate to `http://localhost:8080` to access your application.

6. **🧪 Test the Example Website**:
   To demonstrate all the functions of our web server, sessions and cookies have been integrated into the example website. Log in under "Manage Files" and "Upload Files" using the following credentials:
   - **Username**: admin
   - **Password**: hello123

7. **🛠️ Test Different Routes**:
   You can test various routes such as `/delete`, `/contact`, `/game`, `/overview`, and `/admin` to ensure everything is functioning as expected.

## 📌 Notes

- Ensure that the necessary permissions are set for the directories and files being served.
- Customize the error pages and other settings as needed for your application.
