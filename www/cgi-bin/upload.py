#!/usr/bin/env python3
import os
import sys
import tempfile

# Hata ayıklama için log

def log_debug(message):
    with open("/tmp/upload_debug.log", "a") as debug_file:
        debug_file.write(message + "\n")

print("Content-Type: text/html; charset=utf-8\r\n\r\n")

try:
    # Input boyutunu al
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    if content_length == 0:
        print("<h1>🚫 Hata: Boş istek</h1>")
        sys.exit(0)
    
    # Ham input oku
    raw_data = sys.stdin.read(content_length)
    
    # Debug için ham veriyi logla
    log_debug(f"Alınan ham veri:\n{raw_data[:200]}...")
    
    # Boundary belirle
    boundary = None
    if 'boundary=' in os.environ.get('CONTENT_TYPE', ''):
        boundary = '--' + os.environ['CONTENT_TYPE'].split('boundary=')[-1]
        log_debug(f"Bulunan boundary: {boundary}")
    
    if not boundary:
        print("<h1>🚫 Hata: Boundary bulunamadı</h1>")
        sys.exit(0)
    
    # Dosya içeriğini çıkar
    file_data = None
    parts = raw_data.split(boundary)
    for part in parts:
        if 'filename="' in part:
            header, _, content = part.partition('\r\n\r\n')
            filename_start = header.find('filename="') + 10
            filename_end = header.find('"', filename_start)
            filename = header[filename_start:filename_end]
            
            # Dosya içeriğini son \r\n'den temizle
            file_data = content.rstrip('\r\n-')
            break
    
    if not file_data:
        print("<h1>🚫 Dosya içeriği bulunamadı</h1>")
        sys.exit(0)
    
    # Dosyayı kaydet
    upload_dir = "../uploads"
    os.makedirs(upload_dir, exist_ok=True)
    save_path = os.path.join(upload_dir, filename)
    
    with open(save_path, "wb") as f:
        f.write(file_data.encode('latin1'))
    
    print(f"<h1>✅ Yükleme Başarılı!</h1>")
    print(f"<p>Dosya: {filename}</p>")
    print(f"<p>Boyut: {len(file_data)} bayt</p>")
    print(f"<p>Kaydedildi: {save_path}</p>")

except Exception as e:
    print(f"<h1>❌ Kritik Hata</h1>")
    print(f"<pre>{str(e)}</pre>")
    log_debug(f"Hata: {str(e)}")