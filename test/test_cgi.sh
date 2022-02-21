#!/bin/bash

while read line; do export "$line"; done < env.tmp
source ../venv/bin/activate
cat body.tmp | /Users/macbook/Desktop/webserver/venv/bin/python3 /Users/macbook/Desktop/webserver/www/root/localhost/cgi-bin/upload3.py #<body.tmp

while read line; do export "$line"; done < env2.tmp
/Users/macbook/Desktop/webserver/venv/bin/python3 /Users/macbook/Desktop/webserver/www/root/localhost/cgi-bin/test-form.py <body2.tmp | cat -e

while read line; do export "$line"; done < env2.tmp
/Users/macbook/Desktop/webserver/www/root/localhost/cgi-bin/users.cgi /Users/macbook/Desktop/webserver/www/root/localhost/cgi-bin/users.cgi <body2.tmp | cat -e