#!/bin/bash

#brew install python3.9
python3 -m venv venv
source ./venv/bin/activate
# pip3 install jinja2
diskutil erasevolume HFS+ "RAMDisk" `hdiutil attach -nomount ram://2194304`
