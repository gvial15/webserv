#!/usr/bin/env python3
import cgi
import cgitb

cgitb.enable()

form = cgi.FieldStorage()

name = form.getvalue('name')

print("<h1>Bonjour {}!</h1>".format(name))