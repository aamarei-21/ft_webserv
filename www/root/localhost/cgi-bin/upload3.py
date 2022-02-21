#!./venv/bin/python3
# Import Basic OS functions
import os, sys
# Import modules for CGI handling
import cgi, cgitb
# import urllib.request

# enable debugging
# cgitb.enable()

inFileData = None

form = cgi.FieldStorage()

UPLOAD_DIR = os.environ["UPLOAD_DIR"]
if not UPLOAD_DIR:
    UPLOAD_DIR = './www/root/localhost/uploads'
print("Content-Type: text/html; charset=UTF-8\r\n\r\n", end="")
# IF A FILE WAS UPLOADED (name=file) we can find it here.
if "file" in form:
    form_file = form['file']
    # form_file is now a file object in python
    if form_file.filename:

        uploaded_file_path = os.path.join(UPLOAD_DIR, os.path.basename(form_file.filename))
        with open(uploaded_file_path, 'wb') as fout:
            # read the file in chunks as long as there is data
            while True:
                chunk = form_file.file.read(100000)
                if not chunk:
                    break
                # write the file content on a file on the hdd
                fout.write(chunk)
        print("""
<html>
<head>
<title></title>
</head>
<body>
Succsess!!!

</body>
</html>
""")
        sys.exit()
        # load the written file to display it
        # with open(uploaded_file_path, 'r') as fin:
        #     inFileData = ""
        #     for line in fin:
        #         inFileData += line

print("""
<html>
<head>
<title></title>
</head>
<body>
Fail!!!

</body>
</html>
""")
# headers = {'User-Agent':'Mozilla/5.0 (Windows NT 6.1; WOW64; rv:23.0) Gecko/20100101 Firefox/23.0'}

