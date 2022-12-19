
import serial
import socket
import time

s = serial.Serial("COM8")
ip = "192.168.3.17"
port = 55051
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
while 1:
    #myString = b"$GPGGA,172814.0,3723.46587704,N,12202.26957864,W,2,6,1.2,18.893,M,-25.669,M,2.0 0031*63"
    myString = s.readline()
    print(myString)
    sock.sendto(myString, (ip, port))
