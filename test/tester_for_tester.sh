#!/bin/bash

while read line; do export "$line"; done < env_tester
/Users/macbook/Desktop/webserver/www/root/localhost/cgi-bin/cgi_tester /Users/macbook/Desktop/webserver/www/root/localhost/YoupiBanane/youpi.bla < /Users/macbook/Desktop/webserver/test/file_should_exist_after > result