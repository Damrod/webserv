#!/usr/bin/env python2

import time
import os

print('Content-type: text/html\r\n\r\n')
print('<font size=+1>Environment</font><br>')
for param in os.environ.keys():
    print('<b>%20s</b>: %s<br>' % (param, os.environ[param]))
for i in range(2):
    print(i)
    time.sleep(1)
