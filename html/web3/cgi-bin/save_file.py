#!/usr/bin/env python2

import os, cgi, cgitb
from ConfigParser import SafeConfigParser

cgitb.enable()

form = cgi.FieldStorage()

# Get filename here.
fileitem = form['filename']

# Test if the file was uploaded
if fileitem.filename:
    # strip leading path from file name to avoid
    # directory traversal attacks
    fn = os.path.basename(fileitem.filename)
    parser = SafeConfigParser()
    this_dir = os.path.dirname(os.path.abspath(__file__))
    ini_file = os.path.join(this_dir, 'config/save_file.ini')
    parser.read(ini_file)
    directory = parser.get('paths', 'upload_path')
    if not os.path.exists(directory):
        os.mkdir(directory)
    if not os.path.exists(directory + fn):
        open(directory + fn, 'wb').write(fileitem.file.read())
        message = 'The file "' + fn + '" was uploaded successfully'
    else:
        message = 'The file already exists'

else:
    message = 'No file was uploaded'

print """\
Content-Type: text/html\r\n\r\n
<html>
<body>
  <p>%s</p>
</body>
</html>
""" % (message)
