#!/usr/bin/env python3
"""
Script de prueba mínimo que conecta al servidor TCP en localhost:1234,
envia un request GET_MENU y muestra la respuesta JSON.
"""
import socket
import json
import struct

HOST = '127.0.0.1'
PORT = 1234

req = {
    "cmd": "GET_MENU",
    "payload": {}
}

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.settimeout(5.0)
try:
    s.connect((HOST, PORT))
    payload = json.dumps(req).encode('utf-8')
    header = struct.pack('>I', len(payload))
    s.sendall(header + payload)

    # Leer framing: primero 4 bytes de longitud, luego el body
    try:
        hdr = s.recv(4)
        if len(hdr) < 4:
            print("No se recibió la cabecera completa")
        else:
            msg_len = struct.unpack('>I', hdr)[0]
            data = b""
            while len(data) < msg_len:
                chunk = s.recv(msg_len - len(data))
                if not chunk:
                    break
                data += chunk
            try:
                parsed = json.loads(data.decode('utf-8'))
                print(json.dumps(parsed, indent=2, ensure_ascii=False))
            except Exception as e:
                print("No se pudo parsear la respuesta:", e)
    except socket.timeout:
        print("No se recibió respuesta (timeout)")
finally:
    s.close()
