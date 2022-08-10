#!/usr/bin/env python3

import socket

def udp_server(host='127.0.0.1', port=5555):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    print(f"Listening on udp {host}:{port}")
    s.bind((host, port))
    while True:
        (data, addr) = s.recvfrom(128*1024)
        yield data.decode()


for data in udp_server():
    print(f"> {data}")