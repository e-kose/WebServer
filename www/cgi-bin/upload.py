#!/usr/bin/env python3
import os
import cgi

print("Content-Type: text/html; charset=utf-8\r\n")

form = cgi.FieldStorage()

if "file" not in form or not form["file"].filename:
    print("<h1>🚫 Dosya alınamadı.</h1>")
    exit()

uploaded_file = form["file"]
filename = os.path.basename(uploaded_file.filename)

save_path = f"./www/uploads/{filename}"
try:
    with open(save_path, "wb") as f:
        f.write(uploaded_file.file.read())
    print(f"<h1>✅ Yükleme Başarılı!</h1><p>{filename} kaydedildi.</p>")
except Exception as e:
    print(f"<h1>❌ Yükleme Başarısız</h1><pre>{e}</pre>")
