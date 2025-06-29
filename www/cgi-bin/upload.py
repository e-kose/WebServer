#!/usr/bin/env python3
import os
import sys
import cgi
import cgitb
import uuid

# Hata ayıklama
cgitb.enable(format="text")  # HTML olmayan hata çıktısı

# Yapılandırma - MUTLAK YOL KULLANIN
UPLOAD_DIR = os.path.abspath("../uploads")
MAX_SIZE = 100 * 1024 * 1024  # 100MB

print("Content-Type: text/html; charset=utf-8\n\n")

print("""<!DOCTYPE html>
<html>
<head><title>Dosya Yükleme</title></head>
<body>""")

try:
    # 1. Ortam değişkenlerini al
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    content_type = os.environ.get('CONTENT_TYPE', '')
    
    # 2. Temel kontroller
    if content_length == 0:
        raise Exception("Boş istek gövdesi")
    if content_length > MAX_SIZE:
        raise Exception(f"Dosya boyutu limiti aşıldı (Max: {MAX_SIZE//1024//1024}MB)")
    
    # 3. Boundary'i ayıkla (binary olarak)
    boundary = None
    if 'boundary=' in content_type:
        boundary_str = content_type.split('boundary=')[-1]
        boundary = f"--{boundary_str}".encode('utf-8')
    
    if not boundary:
        raise Exception("Boundary bulunamadı")
    
    # 4. Veriyi BINARY olarak oku
    input_data = sys.stdin.buffer.read(content_length)
    
    # 5. Parçalara ayır (binary olarak)
    parts = input_data.split(boundary)
    
    # 6. Dosya parçasını bul
    file_data = None
    filename = None
    for part in parts:
        if b'filename="' in part:
            # Header ve body'yi ayır
            header_end = part.find(b'\r\n\r\n')
            if header_end == -1:
                continue
                
            header = part[:header_end]
            body = part[header_end+4:]
            
            # Dosya adını çıkar
            filename_start = header.find(b'filename="') + 10
            filename_end = header.find(b'"', filename_start)
            filename = header[filename_start:filename_end].decode('utf-8', errors='replace')
            
            # Son 2 byte'ı at (boundary sonundaki --)
            file_data = body.rstrip(b'\r\n-')
            break
    
    if not file_data or not filename:
        raise Exception("Dosya içeriği bulunamadı")
    
    # 7. Güvenli dosya adı oluştur
    safe_name = f"{uuid.uuid4().hex}_{filename}"
    os.makedirs(UPLOAD_DIR, exist_ok=True)
    save_path = os.path.join(UPLOAD_DIR, safe_name)
    
    # 8. Dosyayı BINARY modda yaz
    with open(save_path, 'wb') as f:
        f.write(file_data)
    
    # 9. Başarılı yanıt
    print(f"<h1>✅ Yükleme Başarılı!</h1>")
    print(f"<p>Dosya: {filename}</p>")
    print(f"<p>Kaydedildi: {safe_name}</p>")
    print(f"<p>Boyut: {len(file_data)} bayt</p>")

except Exception as e:
    print(f"<h1>❌ Hata: {str(e)}</h1>")

print("</body></html>")