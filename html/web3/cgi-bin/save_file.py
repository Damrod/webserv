#!/usr/bin/python

import cgi, os, cgitb

cgitb.enable()

form = cgi.FieldStorage()

# Get filename here.
fileitem = form['filename']

# Test if the file was uploaded
if fileitem.filename:
    # strip leading path from file name to avoid
    # directory traversal attacks
    fn = os.path.basename(fileitem.filename)
    directory = '/tmp/webserv/'
    if not os.path.exists(directory):
        os.mkdir(directory)
    if not os.path.exists(directory + fn):
        open('/tmp/webserv/' + fn, 'wb').write(fileitem.file.read())
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
