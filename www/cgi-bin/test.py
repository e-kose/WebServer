#!/usr/bin/env python3
import cgi
import sys

form = cgi.FieldStorage()
print("Content-Type: text/html\n")

print("<html><body>")
print("<h1>CGI POST Testi</h1>")
if "data" in form:
    print("<p>Gönderilen veri: {}</p>".format(form.getvalue("data")))
else:
    print("<p>Veri gönderilmedi.</p>")
print("</body></html>")
