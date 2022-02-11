import socket
import sys
import time
import datetime

filename="default.log"
try:
    if(sys.argv[1]):
        filename=str(sys.argv[1])
except Exception as exception:
    # print(exception)
    print("Default filename will be used.(Use argument to change)")

print("Filename:",filename)
print("Only byte packet will be saved and displayed.If you wish to display and save string also, uncomment lines 50-54")
Port=int(input("\nPort:"))

sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
server_address=('0.0.0.0',Port)
sock.bind(server_address)
sock.listen(1)

def save_raw_packet(filename,data):
    f=open(filename,"a")
    f.write(data)
    f.write("\n")
    f.close()


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
                save_raw_packet(filename,byte_packet)
                # text_packet=""
                # for x in range(0,len(byte_packet)):
                #     text_packet+=chr(byte_packet[x])
                # print("Packet in String :",text_packet)
                # save_raw_packet(filename,text_packet)
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

