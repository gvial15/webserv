#!/usr/bin/env python3
import cgi
import cgitb
import os
import sys

try:
    cgitb.enable()
    path_info = os.environ.get('PATH_INFO', '')
    if path_info:
        parts = path_info.split('/')
        if len(parts) != 2:
            sys.exit(400)
        _, name = parts
    else:
        form = cgi.FieldStorage()
        name = form.getvalue('name')

    if not name:
        sys.exit(400)

    print("<h1>Bonjour {}!</h1>".format(name))

except Exception as e:
    sys.exit(400)
    