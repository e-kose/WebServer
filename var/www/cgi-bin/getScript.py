#!/usr/bin/env python3
# getScript.py - CGI GET Request Test Script

import os
import sys
from urllib.parse import parse_qs

def main():
    # CGI HTTP response header
    print("Content-Type: text/html")
    print()  # Boş satır header'ları bitirir
    
    # HTML başlangıcı
    print("""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>CGI GET Test</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 20px; }
            table { border-collapse: collapse; width: 100%; margin-top: 20px; }
            th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
            th { background-color: #f2f2f2; }
            .success { color: #2ecc71; }
            .error { color: #e74c3c; }
        </style>
    </head>
    <body>
        <h1>WebServer CGI GET Test</h1>
    """)
    
    # Query string'i al
    query_string = os.environ.get('QUERY_STRING', '')
    
    # Query parametrelerini parse et
    params = {}
    if query_string:
        params = parse_qs(query_string)
    
    # Test sonuçları
    test_results = []
    
    # 1. Query String Testi
    test_results.append(("Query String Var", "✓" if query_string else "✗"))
    
    # 2. Parametre Sayısı Testi
    param_count = len(params)
    test_results.append(("Parametre Sayısı", str(param_count)))
    
    # 3. Özel Parametre Testleri
    test_params = [
        ("name", "Ahmet", "string"),
        ("age", "30", "number"),
        ("active", "true", "boolean"),
        ("email", "test@example.com", "email")
    ]
    
    for param, test_value, param_type in test_params:
        if param in params:
            received = params[param][0]
            status = "✓" if received == test_value else f"✗ ({received})"
            test_results.append((f"'{param}' Parametre Testi", status))
        else:
            test_results.append((f"'{param}' Parametre Testi", "✗ (yok)"))
    
    # Sonuçları tablo olarak göster
    print("<h2>Test Sonuçları</h2>")
    print("<table>")
    print("<tr><th>Test</th><th>Sonuç</th></tr>")
    
    for test, result in test_results:
        row_class = "success" if "✓" in result else "error" if "✗" in result else ""
        print(f'<tr><td>{test}</td><td class="{row_class}">{result}</td></tr>')
    
    print("</table>")
    
    # HTML kapanış
    print("</body></html>")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print("Content-Type: text/html")
        print()
        print(f"<html><body><h1>CGI Error</h1><p>{str(e)}</p></body></html>")