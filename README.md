<div align="center">

# 🌐 WebServ - C++98 HTTP Web Server

![Web Server Architecture](https://upload.wikimedia.org/wikipedia/commons/thumb/1/1e/Web_server_serving_static_content.png/332px-Web_server_serving_static_content.png)

</div>

<div align="center">

![WebServ Logo](https://img.shields.io/badge/WebServ-HTTP%20Server-blue?style=for-the-badge&logo=data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjQiIGhlaWdodD0iMjQiIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTEyIDJMMTMuMDkgOC4yNkwyMCA5TDEzLjA5IDE1Ljc0TDEyIDIyTDEwLjkxIDE1Ljc0TDQgOUwxMC45MSA4LjI2TDEyIDJaIiBzdHJva2U9IndoaXRlIiBzdHJva2Utd2lkdGg9IjIiIHN0cm9rZS1saW5lY2FwPSJyb3VuZCIgc3Ryb2tlLWxpbmVqb2luPSJyb3VuZCIvPgo8L3N2Zz4K)

[![C++](https://img.shields.io/badge/C%2B%2B-98-00599C?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/98)
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](LICENSE)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen?style=flat-square)](Makefile)

*A high-performance, non-blocking HTTP/1.1 web server implementation in C++98*

</div>

---

## 📖 Proje Hakkında

**WebServ**, ekip arkadaşımla birlikte C++98 dilinde sıfırdan geliştirdiğimiz kendi HTTP/1.1 web sunucumuzdur. Nginx benzeri olay tabanlı mimari (event-driven architecture) ile tek bir işlemde birden fazla bağlantıyı eşzamanlı olarak yönetebilen, socket programlama kullanarak tek thread üzerinde çalışan yüksek performanslı bir yapı tasarladık.

Non-blocking I/O mekanizmaları sayesinde çoklu istemci bağlantılarını etkin şekilde yöneten sunucumuz, çoklu port desteği, dinamik içerik işleme ve özelleştirilebilir yönlendirme özellikleriyle modern web sunucusu ihtiyaçlarını karşılamaktadır. Bu proje, ağ programlama, HTTP protokolü ve sistem kaynaklarının optimum kullanımı konularında derinlemesine teknik deneyim sağladı.

### 👥 Takım

Bu proje, **42 İstanbul** öğrencileri tarafından ortak bir çalışma ile hayata geçirilmiştir:

| Developer | GitHub Profile |
|-----------|----------------|
| **menasy** | [![GitHub](https://img.shields.io/badge/GitHub-menasy-blue?style=flat-square&logo=github)](https://github.com/menasy) |
| **ekose** | [![GitHub](https://img.shields.io/badge/GitHub-ekose-blue?style=flat-square&logo=github)](https://github.com/e-kose) |

### 🎯 Proje Hedefleri

- **HTTP/1.1 Protokolü**: RFC 2616 standardına uygun HTTP sunucusu
- **Non-blocking I/O**: Yüksek performans için asenkron soket programlama
- **CGI Desteği**: Python, PHP ve diğer betik dilleri için CGI arayüzü
- **Çoklu Server**: Tek bir yapılandırma dosyasında birden fazla virtual host
- **Modüler Mimari**: Genişletilebilir ve sürdürülebilir kod yapısı

---

## 🌐 Web Server Nedir?

Web sunucusu, HTTP (Hypertext Transfer Protocol) protokolü üzerinden web sayfalarını, dosyaları ve diğer web içeriklerini istemcilere (tarayıcılar, uygulamalar) sunan bir yazılım uygulamasıdır.

### 🔄 Çalışma Prensibi

```
 ┌─────────────┐    HTTP İsteği     ┌─────────────┐
 │   İstemci   │ ──────────────────► │ Web Sunucu  │
 │ (Tarayıcı)  │                    │  (WebServ)  │
 │             │ ◄────────────────── │             │
 └─────────────┘    HTTP Yanıtı     └─────────────┘
```

![Server-Client Architecture](https://github.com/menasy/Project_icons/blob/main/WebServer_Images/ServerClientDiagram.png)
*HTTP Server-Client İletişim Mimarisi*

**Temel İşlevler:**
- 🌍 HTTP isteklerini kabul etme ve işleme
- 📄 Statik dosyaları sunma (HTML, CSS, JS, resimler)
- 🔧 Dinamik içerik üretimi (CGI ile)
- 🛡️ Güvenlik kontrolü ve erişim yönetimi
- 📊 İstek/yanıt loglaması

---

## 🚀 HTTP Protokolü ve İmplementasyon

### 📡 HTTP Nedir?

HTTP (Hypertext Transfer Protocol), World Wide Web'in temelini oluşturan uygulama katmanı protokolüdür. İstemci-sunucu modeline dayalı, durumsuz (stateless) bir protokoldür.

### 🔧 WebServ'de Desteklenen HTTP Özellikleri

#### ✅ HTTP Metodları
- **GET**: Kaynak alma istekleri
- **POST**: Veri gönderme (form, dosya yükleme)
- **DELETE**: Kaynak silme istekleri

#### 📋 HTTP Status Kodları
```cpp
200 OK              // Başarılı istek
301/302 Redirect    // Yönlendirme
400 Bad Request     // Hatalı istek
403 Forbidden       // Erişim reddedildi
404 Not Found       // Kaynak bulunamadı
405 Method Not Allowed  // Metod desteklenmiyor
413 Entity Too Large    // Çok büyük istek
500 Internal Server Error // Sunucu hatası
```

#### 🗂️ HTTP Headers
- **Content-Type**: MIME türü belirleme
- **Content-Length**: İçerik boyutu
- **Connection**: Bağlantı yönetimi (keep-alive)
- **Host**: Virtual host desteği
- **Transfer-Encoding**: Chunked transfer

---

## 🏗️ Mimari ve Tasarım

### 📊 Sistem Mimarisi

```
┌─────────────────────────────────────────────────────┐
│                   WebServ Mimarisi                  │
├─────────────────────────────────────────────────────┤
│  main.cpp (Entry Point)                            │
├─────────────────────────────────────────────────────┤
│  CheckConfig (Yapılandırma Doğrulama)              │
├─────────────────────────────────────────────────────┤
│  WebServer (Ana Sunucu Sınıfı)                     │
│  ├── Poll-based Event Loop                         │
│  ├── Client Connection Management                  │
│  └── Request/Response Handling                     │
├─────────────────────────────────────────────────────┤
│  HttpRequest (HTTP Parser)                         │
│  ├── Header Parsing                                │
│  ├── Body Parsing                                  │
│  └── URL Decoding                                  │
├─────────────────────────────────────────────────────┤
│  FileHandler (Dosya İşlemleri)                     │
│  ├── Static File Serving                           │
│  ├── Directory Listing                             │
│  └── CGI Execution                                 │
├─────────────────────────────────────────────────────┤
│  MethodTransaction (HTTP İşlemleri)                │
│  ├── GET Handler                                   │
│  ├── POST Handler                                  │
│  └── DELETE Handler                                │
├─────────────────────────────────────────────────────┤
│  ServerConf / LocationConf (Yapılandırma)          │
└─────────────────────────────────────────────────────┘
```

### 🔄 Non-blocking I/O ile Event Loop

WebServ, performans optimizasyonu için **poll()** sistem çağrısı ile non-blocking I/O kullanır:

![Event-Driven Architecture](https://github.com/menasy/Project_icons/blob/main/WebServer_Images/eventDriven.jpg)
*Event-Driven Architecture ve Non-blocking I/O Yapısı*

```cpp
// Ana event loop
while (!g_signal) {
    int result = poll(pollVec.data(), pollVec.size(), TIMEOUT);
    
    for (size_t i = 0; i < pollVec.size(); i++) {
        if (pollVec[i].revents & POLLIN) {
            // Yeni veri geldi - read
        }
        if (pollVec[i].revents & POLLOUT) {
            // Yazma hazır - send
        }
    }
}
```

**Avantajları:**
- 🚀 Yüksek eşzamanlılık (binlerce bağlantı)
- 💾 Düşük bellek kullanımı
- ⚡ Hızlı yanıt süreleri
- 🔄 Engellemeyen I/O işlemleri

---

## 🔌 Socket Programlama

### 🌐 TCP Socket Oluşturma

WebServ, Berkeley Sockets API kullanarak TCP bağlantıları yönetir:

```cpp
// Socket oluşturma
int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

// Non-blocking yapma
int flags = fcntl(serverSocket, F_GETFL, 0);
fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK);

// Adres bağlama
struct sockaddr_in serverAddr;
serverAddr.sin_family = AF_INET;
serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
serverAddr.sin_port = htons(8080);

bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
listen(serverSocket, SOMAXCONN);
```

### 🔄 Bağlantı Yaşam Döngüsü

```
1. Socket Creation     → socket()
2. Bind Address        → bind()
3. Listen             → listen()
4. Accept Connection  → accept()
5. Read Request       → recv()
6. Process Request    → HTTP parsing
7. Send Response      → send()
8. Close Connection   → close()
```

---

## 🛠️ CGI (Common Gateway Interface)

### 🔧 CGI Nedir?

CGI, web sunucularının harici programları çalıştırarak dinamik içerik üretmesini sağlayan standart bir arayüzdür.

### 🐍 WebServ'de CGI Desteği

WebServ aşağıdaki CGI özelliklerini destekler:

```bash
# Desteklenen betik türleri
.py  → Python3
.php → PHP
.pl  → Perl
```

#### 🌍 CGI Environment Variables

```cpp
// CGI için set edilen ortam değişkenleri
REQUEST_METHOD=GET
SCRIPT_NAME=/cgi-bin/script.py
PATH_INFO=/extra/path
QUERY_STRING=param=value
CONTENT_TYPE=application/x-www-form-urlencoded
CONTENT_LENGTH=42
SERVER_NAME=localhost
SERVER_PORT=8080
HTTP_HOST=localhost:8080
```

#### 🔄 CGI Execution Flow

```
1. HTTP İsteği gelir
2. CGI betiği tespit edilir
3. Environment variables set edilir
4. fork() ile child process oluşturulur
5. execve() ile betik çalıştırılır
6. Pipe ile output okunur
7. HTTP response olarak gönderilir
```

---

## ⚙️ Kurulum ve Kullanım

### 🔧 Yapılandırma Örneği

```nginx
server {
    listen 127.0.0.1:8080;
    server_name localhost;
    
    client_max_body_size 2M;
    root var/www;
    index index.html;
    
    access_log logs/access.log;
    error_log logs/error.log;
    
    # Ana lokasyon
    location / {
        methods GET POST DELETE;
        autoindex on;
        try_files $uri $uri/ =404;
    }
    
    # CGI lokasyonu
    location /cgi-bin/ {
        methods GET POST;
        cgi_ext .py /usr/bin/python3;
        cgi_ext .php /usr/bin/php;
    }
    
    # Yönlendirme
    location /redirect/ {
        return 302 https://github.com;
    }
    
    # Dosya yükleme
    location /upload/ {
        methods POST;
        client_max_body_size 10M;
    }
    
    # Hata sayfaları
    error_page 404 /errors/404.html;
    error_page 500 /errors/500.html;
}
```

### 🔨 Derleme

```bash
# Projeyi klonlayın
git clone https://github.com/e-kose/WebServer.git
cd WebServer

# Derleyin
make
```

### 🚀 Çalıştırma

```bash
# Temel kullanım
./webserv config/conf.conf
```

Sunucu başlatıldıktan sonra tarayıcınızdan aşağıdaki adrese giderek test edebilirsiniz:
```
http://localhost:8080/
```

Chrome, Firefox, Safari gibi herhangi bir web tarayıcısından bu adrese istek göndererek WebServ sunucusunun çalıştığını doğrulayabilirsiniz.

### 📋 Gereksinimler

- **Derleyici**: g++ veya clang++ (C++98 uyumlu)
- **İşletim Sistemi**: Linux/macOS
- **Python**: CGI desteği için Python 3.x
- **Make**: Build sistemi için

---

## 🧪 Test ve Örnek Kullanım

### 🔍 Temel HTTP Testleri

WebServ sunucusu, kullanıcı dostu web arayüzü üzerinden kolayca test edilebilir. Ana sayfa (`index.html`) üzerinden tüm HTTP metodları ve özellikler interaktif olarak deneyimlenebilir.

![WebServ Ana Sayfa](https://github.com/menasy/Project_icons/blob/main/WebServer_Images/WebServerIndex.png)
*WebServ ana sayfa arayüzü - tüm özellikler web üzerinden test edilebilir*

### 📊 Performans Testi

```bash
# Apache Bench ile yük testi
ab -n 1000 -c 50 http://localhost:8080/

# Siege ile stress test
siege -c 100 -t 60s http://localhost:8080/
```

### 🐛 Debug ve Monitoring

```bash
# Bağlantı durumu
netstat -an | grep :8080

# Process izleme
ps aux | grep webserv

# Memory kullanımı
valgrind --leak-check=full ./webserv config/conf.conf
```

---

## 📁 Proje Yapısı

```
WebServer/
├── 📁 config/
│   └── conf.conf                 # Sunucu yapılandırması
├── 📁 include/                   # Header dosyaları
│   ├── WebServer.hpp            # Ana sunucu sınıfı
│   ├── HttpRequest.hpp          # HTTP parser
│   ├── ServerConf.hpp           # Server yapılandırması
│   ├── LocationConf.hpp         # Location yapılandırması
│   ├── FileHandler.hpp          # Dosya işlemleri
│   ├── MethodTransaction.hpp    # HTTP metodları
│   ├── CheckConfig.hpp          # Config doğrulama
│   ├── Parser.hpp               # Config parser
│   ├── Tokenizer.hpp            # Lexical analyzer
│   └── HelperClass.hpp          # Yardımcı fonksiyonlar
├── 📁 src/                      # Kaynak kodlar
│   ├── WebServer.cpp            # Ana sunucu implementasyonu
│   ├── HttpRequest.cpp          # HTTP parsing logic
│   ├── ServerConf.cpp           # Server config handling
│   ├── LocationConf.cpp         # Location config handling
│   ├── FileHandler.cpp          # File operations
│   ├── MethodTransaction.cpp    # HTTP methods implementation
│   ├── CheckConfig.cpp          # Configuration validation
│   ├── Parser.cpp               # Configuration parsing
│   ├── Tokenizer.cpp            # Lexical analysis
│   └── HelperClass.cpp          # Utility functions
├── 📁 var/www/                  # Web root directory
│   ├── index.html               # Ana sayfa
│   ├── 📁 cgi-bin/              # CGI betikleri
│   │   ├── upload.py            # Dosya yükleme
│   │   ├── getScript.py         # GET test script
│   │   └── sendScript.py        # Response test script
│   ├── 📁 errors/               # Hata sayfaları
│   │   ├── 400.html             # Bad Request
│   │   ├── 403.html             # Forbidden
│   │   ├── 404.html             # Not Found
│   │   └── 500.html             # Internal Server Error
│   └── 📁 static/               # Statik dosyalar
│       ├── 📁 css/              # Stil dosyaları
│       ├── 📁 js/               # JavaScript
│       └── 📁 images/           # Resimler
├── 📁 logs/                     # Log dosyaları
│   ├── access.log               # Erişim logları
│   └── error.log                # Hata logları
├── main.cpp                     # Ana program entry point
├── Makefile                     # Build sistemi
└── README.md                    # Bu dosya
```

---

## 🔧 Özellikler ve Yetenekler

### ✅ HTTP/1.1 Özellikleri

- [x] **GET, POST, DELETE** metodları
- [x] **HTTP/1.1** protokol desteği
- [x] **Keep-Alive** bağlantılar
- [x] **Chunked Transfer Encoding**
- [x] **Virtual Host** (server_name) desteği
- [x] **Custom Error Pages**
- [x] **Request/Response Headers**
- [x] **URL Decoding**
- [x] **Query String** parsing

### 🛡️ Güvenlik Özellikleri

- [x] **Path Traversal** koruması
- [x] **File Permission** kontrolü  
- [x] **Client Body Size** sınırı
- [x] **Input Validation**
- [x] **Safe CGI Execution**
- [x] **Access Control**

### 📊 Performans Özellikleri

- [x] **Non-blocking I/O** (poll)
- [x] **Event-driven** mimari
- [x] **Efficient Memory** kullanımı
- [x] **Connection Pooling**
- [x] **Fast Static File** serving
- [x] **Optimized HTTP** parsing

### 🔧 CGI ve Dinamik İçerik

- [x] **Python CGI** desteği
- [x] **PHP CGI** desteği  
- [x] **Environment Variables**
- [x] **POST Data** handling
- [x] **File Upload** via CGI
- [x] **Safe Process** execution

---

## 📚 Referanslar ve Kaynaklar

### 📖 HTTP ve Web Sunucu Temelleri

- 🌐 [Web Server Tanımı - TechTarget](https://www.techtarget.com/whatis/definition/Web-server)
- 🔗 [HTTP Protokolü Nedir? - DijitalZade](https://www.dijitalzade.com/http-hyper-text-transfer-protocol/)
- 📄 [HTTP/1.1 Specification - RFC 2616](https://tools.ietf.org/html/rfc2616)
- 🌍 [HTTP Overview - Mozilla MDN](https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Overview)
- 💡 [What is HTTP? - FreeCodeCamp](https://www.freecodecamp.org/news/what-is-http/)

### 🛠️ 42 Webserv ve C++ Programlama

- 📝 [Webserv: Building a Non-blocking Web Server - Medium](https://m4nnb3ll.medium.com/webserv-building-a-non-blocking-web-server-in-c-98-a-42-project-04c7365e4ec7)
- 🔧 [C++ Web Programming - TutorialsPoint](https://www.tutorialspoint.com/cplusplus/cpp_web_programming.htm)
- 📚 [42 Webserv Guide - HackMD](https://hackmd.io/@laian/SJZHcOsmT)

### 🔌 Socket Programlama

- 🌐 [Sockets and Network Programming in C - CodeQuoi](https://www.codequoi.com/en/sockets-and-network-programming-in-c/)
- 📖 [Socket Programming in C/C++ - GeeksforGeeks](https://www.geeksforgeeks.org/c/socket-programming-cc/)
- 🚫 [Blocking vs Non-blocking Sockets - Scott Klement](https://www.scottklement.com/rpg/socktut/nonblocking.html)
- 📄 [Linux Sockets Programming - Denison University](https://personal.denison.edu/~bressoud/cs375-s13/supplements/linux_sockets_supplement.pdf)
- 📝 [Introduction to Sockets - Chirag Bhalodia](https://www.chiragbhalodia.com/2022/01/introduction-of-socket.html)

### 🔧 CGI ve Web Geliştirme

- 🐍 [CGI Nedir? - Faprika](https://www.faprika.com/blog/cgi-nedir-cgi-nasil-kullanilir)
- 📺 [Socket Programming Video Tutorial](https://www.youtube.com/watch?v=-utm73RxNo4)

### 🎥 Video Kaynakları

- 🔴 [HTTP Server Implementation](https://www.youtube.com/watch?v=WDn-htpBlnU)
- 🔴 [Socket Programming Tutorial](https://www.youtube.com/watch?v=mStnzIEprH8&list=PLPUbh_UILtZW1spXkwszDBN9YhtB0tFKC&index=6)

---

## 🤝 Katkı ve Geliştirme

---

## 🎯 Sonuç

**WebServ**, modern web teknolojilerinin temellerini anlamak ve sistem programlama becerilerini geliştirmek için kapsamlı bir öğrenme deneyimi sunar. Proje, HTTP protokolünden socket programlamaya, CGI implementasyonundan performans optimizasyonuna kadar web sunucu geliştirmenin önemli konularını kapsar.

### 🎓 Edinilen Kritik Yetenekler

**Sistem Seviyesi Programlama:**
- 🔧 Berkeley Sockets API ile düşük seviyeli ağ programlama mastery'si
- ⚙️ POSIX sistem çağrıları ve kernel interface anlayışı
- 🧠 Memory management ve resource lifecycle yönetimi
- 🔄 Process/thread yönetimi ve IPC (Inter-Process Communication)

**Web Teknolojileri ve Protokoller:**
- 🌐 HTTP/1.1 protokolünün RFC standartlarına uygun implementasyonu
- 📡 TCP/IP stack üzerinde gerçek zamanlı veri iletişimi
- � Web güvenliği, input validation ve attack prevention
- 📋 MIME types, content negotiation ve HTTP headers mastery'si

**Performance Engineering:**
- ⚡ Event-driven architecture ve non-blocking I/O patterns
- 📊 Scalable server design ve concurrent connection handling
- 💾 Memory-efficient data structures ve algorithm optimization
- 🔄 Load balancing concepts ve high-availability principles

**DevOps ve Sistem Yönetimi:**
- �️ Configuration management ve deployment strategies
- 📈 Monitoring, logging ve debugging techniques
- 🔍 Performance profiling ve bottleneck analysis
- 🚀 Production-ready code ve enterprise-level error handling

Bu kapsamlı geliştirme süreci, web teknolojilerinden sistem programlamaya, ağ mimarisinden performans optimizasyonuna kadar geniş bir spektrumda derin teknik bilgi ve deneyim kazandırmıştır. Proje tamamlandığında, modern web altyapısının işleyişini kavrayabilme ve bu tür kompleks sistemleri sıfırdan tasarlayıp uygulayabilme yetkinliği elde edilmiş olmaktadır.

---

<div align="center">

**⭐ Bu projeyi beğendiyseniz star vermeyi unutmayın!**

[🔝 Başa Dön](#-webserv---c98-http-web-server)

</div>
