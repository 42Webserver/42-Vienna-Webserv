#!/usr/bin/env python3

import cgi
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

# Get form data
form = cgi.FieldStorage()
name = form.getvalue('name')
email = form.getvalue('email')
message = form.getvalue('message')

# Email configuration
sender_email = "" #Email for admin user!!!
sender_password = "" #Password gmail=>secrets app-password create!
receiver_email = email

# Create the email content
subject = "Thank you for contacting us"
body = f"Dear {name},\n\nThank you for reaching out. We have received your message:\n\n{message}\n\nBest regards,\nYour Company"

# Create the email message
msg = MIMEMultipart()
msg['From'] = sender_email
msg['To'] = receiver_email
msg['Subject'] = subject
msg.attach(MIMEText(body, 'plain'))

# Send the email
try:
    server = smtplib.SMTP('smtp.gmail.com', 587) 
    server.starttls()
    server.login(sender_email, sender_password)
    text = msg.as_string()
    server.sendmail(sender_email, receiver_email, text)
    server.quit()
    print("Content-type: text/html\r\n\r\n")
    print("<html><body><h2>Email sent successfully!</h2></body></html>")
except Exception as e:
    print("Content-type: text/html\r\n\r\n")
    print(f"<html><body><h2>Failed to send email: {e}</h2></body></html>")