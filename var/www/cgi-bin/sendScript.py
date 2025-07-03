#!/usr/bin/env python3
import sys
import os
import time

def main():
    # HTTP başlıklarını yaz
    sys.stdout.write(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
    )
    sys.stdout.flush()

    # 10 MB veri boyutu
    total_size = 10 * 1024 * 1024  # 10 MB
    chunk_size = 64 * 1024  # 64 KB
    chunks = total_size // chunk_size
    remainder = total_size % chunk_size

    # İlerleme bilgisi
    sys.stderr.write(f"Starting to send {total_size/(1024*1024):.2f} MB data\n")
    sys.stderr.flush()

    # Ana veri gönderimi
    chunk_data = b'X' * chunk_size
    for i in range(chunks):
        sys.stdout.buffer.write(chunk_data)
        sys.stdout.flush()
        
        # Her 1MB'da bir ilerleme raporu
        if i % (1024*1024 // chunk_size) == 0:
            mb_sent = (i * chunk_size) / (1024 * 1024)
            sys.stderr.write(f"Sent: {mb_sent:.2f} MB\n")
            sys.stderr.flush()
        
        # Test için küçük gecikme (isteğe bağlı)
        # time.sleep(0.001)

    # Kalan veriyi gönder
    if remainder > 0:
        sys.stdout.buffer.write(b'X' * remainder)
        sys.stdout.flush()

    sys.stderr.write(f"TOTAL SENT: {total_size/(1024*1024):.2f} MB\n")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        sys.stderr.write(f"CGI ERROR: {str(e)}\n")
        sys.stdout.write(
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            f"CGI Error: {str(e)}"
        )