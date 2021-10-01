#!/usr/bin/python

# Import modules for CGI handling
import cgi

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data crom fields
fname = form.getvalue('fname')
lname = form.getvalue('lname')

print 'Content-type:text/html\r\n\r\n'
print '<html>'
print '<head>'
print '<title>Hello GET - CGI Program</title>'
print '</head>'
print '<body>'
print '<h2>Hello %s %s</h2>' % (fname, lname)
print '</body>'
print '</html>'
