#!/usr/bin/python2
# -*- coding: utf-8 -*-

"""import cgi, cgitb, os, sys

cgitb.enable(display=0, logdir="./logs")


UPLOAD_DIR = os.environ["UPLOAD_DIR"]
if not UPLOAD_DIR:
    UPLOAD_DIR = './www/root/localhost/uploads/'

# add_header("Content-Type", "text/html; charset=UTF-8") befor body from PIPE
def save_uploaded_file():
    form = cgi.FieldStorage()
    if not form.has_key('file'):
        print '<h1>Not found parameter in form: file</h1>'
        return

    form_file = form['file']
    if not form_file.file:
        print '<h1>Not found parameter in form_file: file</h1>'
        return

    if not form_file.filename:
        print '<h1>Not found parameter in form_file: filename</h1>'
        return

    uploaded_file_path = os.path.join(UPLOAD_DIR, os.path.basename(form_file.filename))
    with open(uploaded_file_path, 'wb') as fout:
        while True:
            chunk = form_file.file.read(100000)
            if not chunk:
                break
            fout.write (chunk)
    print '<h1>Completed file upload</h1>'


print '''
<html>
<head>
  <title>Upload File</title>
</head>
<body>
'''

save_uploaded_file()

print '''
<hr>
<a href="/index.html">Back to upload page</a>
</body>
</html>
'''"""


import cgi, os
import cgitb; cgitb.enable()
import glob, subprocess, shlex, sys, re

UPLOAD_DIR = os.environ["UPLOAD_DIR"]
if not UPLOAD_DIR:
    UPLOAD_DIR = './www/root/localhost/uploads'
UPLOAD_DIR += "/"

try: # Windows needs stdio set for binary mode.
    import msvcrt
    msvcrt.setmode (0, os.O_BINARY) # stdin  = 0
    msvcrt.setmode (1, os.O_BINARY) # stdout = 1
except ImportError:
    pass

def fbuffer(f, chunk_size=10000):
    while True:
        chunk = f.read(chunk_size)
        if not chunk: break
        yield chunk

form = cgi.FieldStorage()
fileitem = form['file']

if fileitem.filename:
    fn = os.path.basename(fileitem.filename)
    f = open(UPLOAD_DIR + fn, 'wb', 10000)
    for chunk in fbuffer(fileitem.file):
        f.write(chunk)
    f.close()
    message = 'The file "' + fn + '" was uploaded successfully'
else:
    message = 'No file was uploaded'

print """\
<html><body>
<p>%s</p>
</body></html>
    """ % (message,)
