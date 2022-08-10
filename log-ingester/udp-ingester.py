#!/usr/bin/env python3

import socket
from rich import print as pprint
import json

ignored_fcts = []
ignored_fcts.append("preg_replace")

def udp_server(host='127.0.0.1', port=5555):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    print(f"Listening on udp {host}:{port}")
    s.bind((host, port))
    while True:
        (data, addr) = s.recvfrom(128*1024)
        yield data.decode()


for data in udp_server():
    if "sulf" in data.lower():
        log_blob = json.loads(data)
        should_print = True
        for ignored_fct in ignored_fcts:
            if ignored_fct == log_blob.get("name"):
                should_print = False
                continue
        if should_print:
            pprint(log_blob)