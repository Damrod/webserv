#!/usr/bin/env python
import sys

sys.stdout.write('Content-type:text/html\r\n')
sys.stdout.write('Location: https://www.google.com\r\n')
sys.stdout.write('Status: 301 Moved Permanently\r\n\r\n')
