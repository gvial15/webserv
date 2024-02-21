#! /usr/bin/python3

import cgi
import os
import sys

try:
    form = cgi.FieldStorage()

    num1 = form.getvalue('num1')
    oper = form.getvalue('operation')
    num2 = form.getvalue('num2')

    if not oper or not num1 or not num2:
        raise ValueError("Parameters are not correct")

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
    else:
        raise ValueError("Invalid operation")

    print("<b>Result = %s</b>" % str(result))

except Exception as e:
    print("<b>Error: %s</b>" % str(e))