#!/usr/bin/env python
import sys

sys.stdout.write('Location: https://www.google.com\r\n')
sys.stdout.write('Status: 301\r\n\r\n')

while True:
    sys.stdout.write('No Content-Type header\r\n')
