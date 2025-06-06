#!/usr/bin/env python3
import os

print("Content-Type: text/plain\n")

path_info = os.environ.get('PATH_INFO', '')
if path_info:
    print(f"User ID from PATH_INFO: {path_info[1:]}")
else:
    print("No PATH_INFO given")
