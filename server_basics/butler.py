import sys
import time
import datetime
import socket 
import threading
from threading import Thread 
from socketserver import ThreadingMixIn 
import select
from tcp_tools import * 
import mysql.connector
from sql_tools import *
    
from slack_webhook import Slack

from datetime import date
import logging

slack_notify = Slack(url='https://hooks.slack.com/services/lalalalalal') # my AWS_notify 

    
request_read_freq=10 # seconds
auto_start_freq=85 # seconds
request_timeout_freq=30 #seconds

sys.stdout.write('\u001b[40m') # clear screen and display header
sys.stdout.write('\x1b[2J')
sys.stdout.write('\x1b[H')
# sys.stdout.write('\u001B[?25l')
sys.stdout.write("\n")
sys.stdout.write("\n")
sys.stdout.write('\u001b[5m')
sys.stdout.write('\u001b[115C')
sys.stdout.write('\u001b[37m')
sys.stdout.write("COLD BUTLER")
sys.stdout.write('\u001b[0m')
sys.stdout.write('\u001b[40m')
sys.stdout.flush()


print("\u001b[33m\n\n")

TCP_PORT=8764

sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM) #setup socket for reading self.device_packet stream
sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
server_address=('0.0.0.0',TCP_PORT)
sock.settimeout(None)
connection_timeout=1  ## this is select timeout not socket timeout
sock.bind(server_address)

stdin_timeout=0.2



class ClientThread(Thread):  # Accept a new connection request and connect to primary and secondary servers
 
    def __init__(self,ip,port,conn): 
        Thread.__init__(self) 
        sys.stdout.write("New server socket request from " + ip + ":" + str(port)+". . .       ")
        self.ip = ip 
        self.port = port
        self.conn=conn 
        self.conn.settimeout(None)
        self.connect_kill_timer=int(time.time())
        self.device_id=0
        self.request_call_timer=0
        self.timer_config=0
        self.auto_start_timer=0
        self.block_auto_start=0
        self.runtime=0
        self.warning_timer=int(time.time())
        self.slack_warning=0
        self.request_timeout_timer=0
        print("Connection Request Accepted.")

 
    def run(self): 

        self.device_packet=0
        try:
            while(self.device_packet!=b''):
                if(self.device_id!=0):

                self.conn_ready=select.select([self.conn],[],[],connection_timeout)
                if(self.conn_ready[0]):
                    try:
                        self.device_packet = self.conn.recv(4096)
                        sys.stdout.write('\u001b[33m') #yellow
                        self.connect_kill_timer=int(time.time()) #watchdog timer for thread
                        byte_packet=[""]*len(self.device_packet)
                        for x in range(0,len(self.device_packet)):
                            byte_packet[x]=ord(chr(self.device_packet[x]))
                        print("ID:", self.name, "| Time:", datetime.datetime.fromtimestamp(time.time()+((5*60+30)*60)).strftime("%A %d %B %Y %I:%M%p"), "| Length of received Packet is ", len(self.device_packet), "| ", "Byte Packet :", byte_packet)


                    except Exception as exception:
                        print("ID:", self.name, "| ","Device receive loop - ",exception,"\nBreaking |")
                        break

                    # quick commands 
                self.ready=select.select([sys.stdin],[],[],stdin_timeout)
                if(self.ready[0]):
                    sys.stdout.write('\u001b[37m') #white
                    ip=sys.stdin.readline()
                    ip=ip[0]
                    if(ip=='1'):
                        print(self.device_id, " - BASIC CONFIG")
                    elif(ip == '2'):
                        print(self.device_id, " - TIMESTAMP CONFIG -",cmd_time)
                    elif(ip == '3'):
                        print(self.device_id, " - SSID CONFIG")
                    elif(ip=='r'):
                        print(self.device_id , " - REQ")
                    elif(ip=='m'):
                        print(self.device_id, " - MEM")
                    elif(ip=='q'):
                        print(self.device_id, " - QRN")
                    elif(ip=='s'):
                        print(self.device_id, " - STP")
                    elif(ip=='t'):
                        print(self.device_id, " - RTN")
                    elif(ip == 'R'):
                        print(self.device_id, " - RST")
                    elif(ip == 'c'):
                        print(self.device_id, " - CLF")
                    elif(ip == 'C'):
                        print(self.device_id, " - CLN")
                    elif(ip == 'S'):
                        print(self.device_id, " - SSD")
                    elif(ip=='i'):
                        for Th in threads:
                            print("Thread - ID ", Th.name, "is", Th.is_alive())

                if((int(time.time())-self.connect_kill_timer)>(60*10)): #if watchdog overflow, kill thread
                    sys.stdout.write('\u001b[35m') #magenta
                    print("ID:", self.name, "| ", "Timer overflow occured.")
                    break

        except Exception as exception:
            sys.stdout.write('\u001b[35m') #magenta
            print("ID:", self.name, "| ", "Closing Conn - ", exception)
            self.conn.close()
        else:
            sys.stdout.write('\u001b[35m') #magenta
            print("ID:", self.name, "| ","No exception raised,Closing Conn.")
            self.conn.close()
 
threads=[]
sock.listen(3)

while True:
    try:
        (conn, (ip,port)) = sock.accept()
        sys.stdout.write('\u001b[35m') #magenta
        newthread = ClientThread(ip,port,conn) 
        newthread.start() 
        threads.append(newthread)
        threads=threading.enumerate()[1::]
        for Th in threads:
            print("Thread - ID ", Th.name, "is" ,Th.is_alive())
    except Exception as exception:
        print("Accept loop - ",exception)

for t in threads: # not being used  
    t.join() 

