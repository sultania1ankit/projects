import socket
import sys
import time
import datetime

Port=int(input("\nPort:"))

sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
server_address=('0.0.0.0',Port)
sock.bind(server_address)
sock.listen(1)


def main():
    while True:
        try:
            print("Waiting for Device. . . . . .")
            connection, client_address= sock.accept()
            connection.settimeout(360)
            print("Connection from", client_address)
            data=0
            while(data!=b''):
                # sock.settimeout(10)
                data=connection.recv(4096)
                print("Packet received on  -  ",datetime.datetime.fromtimestamp(time.time()+((5*60+30)*60)).strftime("%A %d %B %Y %I:%M%p"))
                print("Raw Packet:",data)
                byte_packet=[""]*len(data)
                for x in range(0,len(data)):
                    byte_packet[x]=ord(chr(data[x]))
                print("Length of received Packet is ",len(data))
                print("Byte packet:",byte_packet)
                text_packet=""
                for x in range(0,len(byte_packet)):
                    text_packet+=chr(byte_packet[x])
                print("Packet in String :",text_packet)
                print("__________________________________________________________________________________________________________________________________")
        except Exception as exception:
            print("Exception - ",exception)

        finally:
            print("Cleaning Connections.")
            try:
                connection.close()
            except Exception as exception:
                print(exception)

main()

