#!/usr/bin/env python3
import os
import cgi
import cgitb
cgitb.enable()

print("Content-Type: application/json\n")

params = cgi.FieldStorage()
item_id = params.getvalue("id")

if item_id:
    print(f'{{"id": "{item_id}", "data": "Sample data for item {item_id}"}}')
else:
    print('{"error": "Missing id parameter"}')
