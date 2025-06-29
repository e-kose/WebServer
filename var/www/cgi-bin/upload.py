#!/usr/bin/env python3
import os
import sys
import cgi
import cgitb
import json
from datetime import datetime
import re

# Hata ayıklama
cgitb.enable()

# Yapılandırma
UPLOAD_DIR = os.path.abspath("../uploads")
MAX_SIZE = 100 * 1024 * 1024  # 100MB
LOG_FILE = "/tmp/webserver_upload.log"

print("Content-Type: application/json\n")

def log_event(message):
    with open(LOG_FILE, "a") as log:
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        log.write(f"[{timestamp}] {message}\n")

def sanitize_filename(filename):
    """Güvenli dosya adı oluşturma"""
    # İzin verilen karakterler: harf, rakam, boşluk, -, _, .
    # Diğer karakterleri alt çizgiyle değiştir
    safe_name = re.sub(r'[^\w\s\-_.]', '_', filename)
    # Birden fazla alt çizgiyi tekilleştir
    safe_name = re.sub(r'_{2,}', '_', safe_name)
    # Baştaki ve sondaki boşlukları kaldır
    return safe_name.strip()

try:
    # İstek kontrolü
    if os.environ['REQUEST_METHOD'] != 'POST':
        raise Exception("Only POST requests are accepted")
    
    form = cgi.FieldStorage()
    
    if 'file' not in form:
        raise Exception("File field not found")
    
    file_item = form['file']
    
    if not file_item.file or not file_item.filename:
        raise Exception("Invalid file")
    
    # Orijinal dosya adını al ve sanitize et
    original_filename = os.path.basename(file_item.filename)
    safe_filename = sanitize_filename(original_filename)
    
    # Dizin kontrolü
    os.makedirs(UPLOAD_DIR, exist_ok=True)
    save_path = os.path.join(UPLOAD_DIR, safe_filename)
    
    # Dosya çakışmasını önle (aynı isimde dosya varsa)
    counter = 1
    base_name, ext = os.path.splitext(safe_filename)
    while os.path.exists(save_path):
        safe_filename = f"{base_name}_{counter}{ext}"
        save_path = os.path.join(UPLOAD_DIR, safe_filename)
        counter += 1
    
    # Dosyayı kaydet
    file_size = 0
    with open(save_path, 'wb') as f:
        while True:
            chunk = file_item.file.read(8192)
            if not chunk:
                break
            f.write(chunk)
            file_size += len(chunk)
    
    # Log ve yanıt
    log_event(f"UPLOAD: {original_filename} -> {safe_filename} ({file_size} bytes)")
    
    print(json.dumps({
        "status": "success",
        "message": "File uploaded successfully",
        "original_filename": original_filename,
        "saved_filename": safe_filename,
        "size": file_size,
        "download_url": f"/uploads/{safe_filename}"
    }))

except Exception as e:
    log_event(f"ERROR: {str(e)}")
    print(json.dumps({
        "status": "error",
        "message": str(e)
    }))