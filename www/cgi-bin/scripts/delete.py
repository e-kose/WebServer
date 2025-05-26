#!/usr/bin/env python3
import os
import cgi
import cgitb
cgitb.enable()

print("Content-Type: text/plain\n")

params = cgi.FieldStorage()
item_id = params.getvalue("id")

if item_id:
    print(f"Item with ID {item_id} deleted.")
else:
    print("Missing 'id' parameter.")
