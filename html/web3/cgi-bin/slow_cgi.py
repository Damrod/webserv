#!/usr/bin/env python2

import time
import os
import cgi

form = cgi.FieldStorage()
seconds = form.getvalue('seconds')

print('Content-type: text/html\r\n\r\n')
print('<font size=+1>Environment</font><br>')
for param in os.environ.keys():
    print('<b>%20s</b>: %s<br>' % (param, os.environ[param]))

try:
    number = int(seconds)
    if number < 1:
        number = 1
except ValueError:
    number = 1

for i in range(number):
    print(i)
    time.sleep(1)
