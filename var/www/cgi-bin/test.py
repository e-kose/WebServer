#!/usr/bin/env python3
import json
import os

print("Content-Type: application/json\n")

print(json.dumps({
    "status": "success",
    "message": "CGI çalışıyor!",
    "method": os.environ.get('REQUEST_METHOD', 'GET'),
    "timestamp": "2023-06-28T12:00:00Z"
}))