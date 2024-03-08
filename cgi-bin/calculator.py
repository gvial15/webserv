#! /usr/bin/python3

import cgi
import os
import sys

try:

    path_info = os.environ.get('PATH_INFO', '')
    if path_info:
        parts = path_info.split('/')
        if len(parts) != 4:
            sys.exit(400)
        _, num1, num2, oper = parts

    else:
        form = cgi.FieldStorage()

        num1 = form.getvalue('num1')
        oper = form.getvalue('operation')
        num2 = form.getvalue('num2')
        
    if not oper or not num1 or not num2:
        sys.exit(400)
        # raise ValueError("Parameters are not correct")

    if not num1.isdigit() or not num2.isdigit():
        print("<b>Error: Invalid operation. Only numerical characters are accepted<b>")
        sys.exit(400)

    num1 = int(num1)
    num2 = int(num2)

    if oper == 'add':
        result = num1 + num2
    elif oper == 'subtract':
        result = num1 - num2
    elif oper == 'multiply':
        result = num1 * num2
    elif oper == 'divide' and num2 != 0:
        result = round(float(num1) / num2, 2)
    elif oper == 'divide' and num2 == 0:
        print("<b>Error: Invalid operation. Cannot divide by zero.<b>")
        sys.exit()
    else:
        sys.exit(400)

    print("<b>Result = %s</b>" % str(result))

except Exception as e:
    # print("<b>Error: %s</b>" % str(e))
    sys.exit(400)