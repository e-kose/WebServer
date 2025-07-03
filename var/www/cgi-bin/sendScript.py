#!/usr/bin/env python3
# sendScript.py - CGI Test Script

import os
import sys

def main():
    # CGI HTTP response header
    print("Content-Type: text/html")
    print()  # Boş satır header'ları bitirir
    
    # HTML başlangıcı
    print("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>WebServer Send Test</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background-color: #f5f5f5; }
        .container { max-width: 800px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; }
        h1 { color: #2c3e50; }
        .info { background: #e8f4f8; padding: 15px; border-radius: 5px; margin: 10px 0; }
        table { width: 100%; border-collapse: collapse; margin: 20px 0; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #3498db; color: white; }
        .success { color: #27ae60; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🚀 WebServer Send Handler Test</h1>
        <div class="info">
            <strong>Test Durumu:</strong> <span class="success">✅ Başarılı</span><br>
            <strong>Amaç:</strong> Büyük yanıtların gönderilmesini test etmek
        </div>
        
        <h2>Çevre Değişkenleri</h2>
        <table>
            <tr><th>Değişken</th><th>Değer</th></tr>""")
    
    # Çevre değişkenleri
    env_vars = ['REQUEST_METHOD', 'SCRIPT_NAME', 'QUERY_STRING', 'SERVER_NAME', 'SERVER_PORT']
    for var in env_vars:
        value = os.environ.get(var, 'Tanımsız')
        print(f'            <tr><td>{var}</td><td>{value}</td></tr>')
    
    print("""        </table>
        
        <h2>Test Verisi</h2>
        <table>
            <tr><th>Sıra</th><th>Veri</th><th>Zaman</th><th>Durum</th></tr>""")
    
    # Test verisi (2000 satır - ~400KB)
    for i in range(1, 2001):
        data = f"TEST_DATA_{i:04d}_LARGE_CONTENT_FOR_TESTING_SENDHANDLER_PERFORMANCE_AND_CHUNKED_TRANSFER"
        status = "✅ Aktif" if i % 3 == 0 else "⚠️ Beklemede" if i % 3 == 1 else "🔄 İşleniyor"
        timestamp = f"2025-07-04 {(i % 24):02d}:{(i % 60):02d}:{(i % 60):02d}"
        
        print(f'            <tr><td>{i}</td><td>{data}</td><td>{timestamp}</td><td class="success">{status}</td></tr>')
    
    print("""        </table>
        
        <h2>Büyük Metin Bloğu (1MB+ İçerik)</h2>
        <div style="background: #f8f9fa; padding: 15px; border-radius: 5px; font-family: monospace;">""")
    
    # Büyük metin bloğu (1000+ iterasyon - ~1MB)
    lorem_text = """WebServer Performance Test Block: Bu metin bloğu sendHandler fonksiyonunun 
    büyük veri aktarımı sırasında performansını test etmek için tasarlanmıştır. HTTP protokolü 
    üzerinden chunked transfer encoding kullanarak büyük yanıtların gönderilmesi durumunda 
    sunucunun nasıl davrandığını analiz etmek amacıyla kullanılmaktadır. Bu test, özellikle 
    büyük dosyaların veya uzun yanıtların gönderilmesi gerektiğinde sunucunun stabilitesini 
    ve performansını ölçmek için kritik öneme sahiptir."""
    
    for i in range(1000):
        print(f"[{i+1:04d}] {lorem_text} - Iteration {i+1} of 1000 - Testing chunked transfer encoding performance.")
    
    print("""        </div>
        
        <h2>Ek Veri Blokları</h2>
        <div style="background: #fff3cd; padding: 15px; border-radius: 5px;">""")
    
    # Ek veri blokları (200KB+ ek içerik)
    additional_text = "A" * 1000  # 1KB'lık string
    for i in range(200):
        print(f"<p>Ek Veri Bloku {i+1:03d}: {additional_text}</p>")
    
    print("""        </div>
        
        <h2>Test Sonuçları</h2>
        <table>
            <tr><th>Metrik</th><th>Değer</th></tr>
            <tr><td>Toplam Veri Satırı</td><td class="success">2,000</td></tr>
            <tr><td>Metin Bloku Sayısı</td><td class="success">1,000</td></tr>
            <tr><td>Ek Veri Blokları</td><td class="success">200</td></tr>
            <tr><td>Tahmini Boyut</td><td class="success">~1.5MB</td></tr>
            <tr><td>Test Durumu</td><td class="success">✅ Başarılı</td></tr>
        </table>
        
        <div class="info">
            <strong>Not:</strong> Bu test ~1.5MB büyüklüğünde bir yanıt oluşturarak 
            sendHandler fonksiyonunun büyük veri aktarımı performansını test etmektedir. 
            Eğer bu mesajı okuyorsanız, WebServer başarıyla büyük yanıtları gönderebilmektedir.
        </div>
    </div>
</body>
</html>""")

if __name__ == "__main__":
    main()
