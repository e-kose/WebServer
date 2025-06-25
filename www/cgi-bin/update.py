#!/usr/bin/env python3
import os
import cgi
import cgitb
cgitb.enable()

print("Content-Type: text/plain\n")

params = cgi.FieldStorage()
item_id = params.getvalue("id")
new_value = params.getvalue("value")

if item_id and new_value:
    print(f"Item {item_id} updated to '{new_value}'.")
else:
    print("Missing 'id' or 'value' parameter.")
