#!./venv/bin/python3

# Import modules for CGI handling 
import cgi, cgitb 
import os
import urllib


if os.environ["REQUEST_METHOD"] == "POST":
    # Create instance of FieldStorage 
    form = cgi.FieldStorage() 
    # Get data from fields
    login = form.getvalue('login')
    password  = form.getvalue('password')
else:
    dict_params = urllib.parse.parse_qs(os.environ["QUERY_STRING"])
    login = dict_params['login'][0]
    password  = dict_params['password'][0]

print("Content-Type: text/html; charset=UTF-8\r\n\r\n", end="")
print ("<html>")
print ("<head>")
print ("<title>Hello CGI Program</title>")
print ("</head>")
print ("<body>")
print ("<h2>Hello %s!</h2>" % (login))
print ("<h2>your password %s</h2>" % (password))
print ("</body>")
print ("</html>")