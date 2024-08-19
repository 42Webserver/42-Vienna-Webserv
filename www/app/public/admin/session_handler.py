#!/usr/bin/env python3

import os
import uuid
import datetime
import glob
import re

def generateSessionId():
    return str(uuid.uuid4())

def checkSessionId(id):

    file_path = os.path.join("/tmp/webserv_sessions", id)

    if not os.path.exists(file_path):
        return (False)
    
    creation_time = os.path.getctime(file_path)
    creation_datetime = datetime.datetime.fromtimestamp(creation_time)
    now = datetime.datetime.now()
    time_difference = now - creation_datetime
    
    if time_difference > datetime.timedelta(hours=1):
        os.remove(file_path)
        return (False)
    else:
        return (True)

def checkCookie(cookie_str):

    pattern = r'id=[\w-]+?'

    match = re.search(pattern, cookie_str)

    if match:
        id = match.group().replace("id=", "")
        return (checkSessionId(id))
    else:
        return (False)

def addNewSessionId():

    if not os.path.exists("/tmp/webserv_sessions"):
        os.makedirs("/tmp/webserv_sessions")

    files = glob.glob(os.path.join("/tmp/webserv_sessions", '*'))
    num_files = len(files)

    if num_files > 9:
        sorted_files = sorted(files, key=os.path.getmtime)
        while num_files > 9:
            os.remove(sorted_files.pop(0))
            num_files -= 1

    id = generateSessionId()

    file_path = os.path.join("/tmp/webserv_sessions", id)
    open(file_path, 'w').close()
    print("Successfully created new file for session-id: ", id)
    return (id)

def deleteSessionId(id):
    id = id.replace("id=", "")
    file_path = os.path.join("/tmp/webserv_sessions", id)
    os.remove(file_path)

