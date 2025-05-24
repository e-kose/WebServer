#!/usr/bin/env python3

# HTTP cevabının başlangıcındaki Content-Type zorunludur
print("Content-Type: text/html\n")

# HTML çıktısı
print("<!DOCTYPE html>")
print("<html lang='en'>")
print("<head><meta charset='UTF-8'><title>Hello CGI</title></head>")
print("<body>")
print("<h1>Hello from Python CGI!</h1>")
print("<p>This is a simple CGI script.</p>")
print("</body>")
print("</html>")
