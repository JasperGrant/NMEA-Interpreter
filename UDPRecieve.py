
import socket

localIP = "255.255.255.255"
localPort = 50040

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.bind((localIP, localPort))

while True:
    receivedBytes = sock.recvfrom(1024)
    print(receivedBytes[0].decode())
