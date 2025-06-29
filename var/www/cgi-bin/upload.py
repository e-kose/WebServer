#!/usr/bin/env python3
import os
import sys
import cgi
import cgitb
import uuid
import json
from datetime import datetime

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

try:
    # İstek kontrolü
    if os.environ['REQUEST_METHOD'] != 'POST':
        raise Exception("Sadece POST istekleri kabul edilir")
    
    form = cgi.FieldStorage()
    
    if 'file' not in form:
        raise Exception("Dosya alanı bulunamadı")
    
    file_item = form['file']
    
    if not file_item.file or not file_item.filename:
        raise Exception("Geçersiz dosya")
    
    # Dosya adı ve uzantı
    filename = os.path.basename(file_item.filename)
    file_ext = os.path.splitext(filename)[1]
    safe_name = f"{uuid.uuid4().hex}{file_ext}"
    
    # Dizin kontrolü
    os.makedirs(UPLOAD_DIR, exist_ok=True)
    save_path = os.path.join(UPLOAD_DIR, safe_name)
    
    # Dosyayı kaydet
    with open(save_path, 'wb') as f:
        while True:
            chunk = file_item.file.read(8192)
            if not chunk:
                break
            f.write(chunk)
    
    # Log ve yanıt
    file_size = os.path.getsize(save_path)
    log_event(f"UPLOAD: {filename} -> {safe_name} ({file_size} bytes)")
    
    print(json.dumps({
        "status": "success",
        "message": "Dosya başarıyla yüklendi",
        "filename": filename,
        "saved_as": safe_name,
        "size": file_size,
        "download_url": f"/uploads/{safe_name}"
    }))

except Exception as e:
    log_event(f"ERROR: {str(e)}")
    print(json.dumps({
        "status": "error",
        "message": str(e)
    }))