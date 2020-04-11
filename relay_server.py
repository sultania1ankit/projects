import sys
import time
import datetime
import socket 
import threading
from threading import Thread 
from socketserver import ThreadingMixIn 
import select


sys.stdout.write('\u001b[40m')
sys.stdout.write('\x1b[2J')
sys.stdout.write('\x1b[H')
# sys.stdout.write('\u001B[?25l')
sys.stdout.write("\n")
sys.stdout.write("\n")
sys.stdout.write('\u001b[5m')
sys.stdout.write('\u001b[115C')
sys.stdout.write('\u001b[37m')
sys.stdout.write("RELAY SERVER (WORK IN PROGRESS)")
sys.stdout.write('\u001b[0m')
sys.stdout.write('\u001b[40m')
sys.stdout.flush()


print("\u001b[33m\n\nTunnel Will Acknowledge the Device,if needed. No data will be received for the Forward Server.\n")

sys.stdout.write('\u001b[32m\nYou can choose the Corresponding Numbers while Selecting Tunnel and Forward IP.(If Ports are also provided they will be auto selected)\n')

stored_IP=[["BLANK","0.0.0.0",0],["NGROK","3.19.114.185"],["Local","234.216.54.345",5446]]

for x in range(1,len(stored_IP)):
    sys.stdout.write('\n\u001b[37m'+str(x)+".\u001b[32m")
    for y in range(0,len(stored_IP[x])):
        if(y==0):
            sys.stdout.write(str(stored_IP[x][y])+" - ")
        elif(y==2):
            sys.stdout.write(":"+str(stored_IP[x][y]))
        else:    
            sys.stdout.write(str(stored_IP[x][y]))



print("\u001b[33m\n\n")


TCP_PORT=int(input("Listener Port: "))
tunnel_ip=str(input("Tunnel Client IP: "))
try:
    pick_int=int(tunnel_ip)
    if(len(stored_IP[pick_int])==2):
        tunnel_ip=stored_IP[pick_int][1]
        tunnel_port=int(input("Tunnel Client Port: "))
    elif(len(stored_IP[pick_int])==3):
        tunnel_ip=stored_IP[pick_int][1]
        tunnel_port=stored_IP[pick_int][2]
except:
    tunnel_port=int(input("Tunnel Client Port: "))

forward_ip=str((input("Forward Server IP(Press 0 to Skip): ")))
try:
    pick_int=int(forward_ip)
    if(len(stored_IP[pick_int])==2):
        forward_ip=stored_IP[pick_int][1]
        forward_port=int(input("Forward Server Port: "))
        forward_flag=int(input("Press 1 to allow Script to run even if Forward server is Down, else press 0: "))
    elif(len(stored_IP[pick_int])==3):
        forward_ip=stored_IP[pick_int][1]
        forward_port=stored_IP[pick_int][2]
        if(forward_port!=0):
            forward_flag=int(input("Press 1 to allow Script to run even if Forward server is Down, else press 0: "))
except:
    forward_port=int(input("Forward Server Port: "))
    forward_flag=int(input("Press 1 to allow Script to run even if Forward server is Down, else press 0: "))


sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
server_address=('0.0.0.0',TCP_PORT)
sock.settimeout(None)
timeout=1  ## this is select timeout not socket timeout
sock.bind(server_address)


class ClientThread(Thread): 
 
    def __init__(self,ip,port,conn): 
        Thread.__init__(self) 
        sys.stdout.write("New server socket request from " + ip + ":" + str(port)+". . .       ")
        self.ip = ip 
        self.port = port
        self.conn=conn 
        self.conn.settimeout(None)
        self.connect_kill_timer=int(time.time())
        print("Connection Request Accepted.")
        sys.stdout.write("Connecting to Tunnel ... ")
        self.tunnel=socket.socket()
        try:
            self.tunnel.connect((tunnel_ip,tunnel_port))
            self.tunnel.setblocking(0)
            print("Connected to Tunnel.")
        except Exception as exception:
            print("Error Connecting to Tunnel, will proceed and retry at later stage  -",exception)
        if(forward_port!=0):
            sys.stdout.write("Connecting to Forward Server...")
            self.forward=socket.socket()
            try:
                self.forward.connect((forward_ip,forward_port))
                print("Connected to Forward Server.")
            except Exception as exception:
                print("Error Connecting to Forward Server, will proceed and retry at later stage  -",exception)

 
    def run(self):
        self.device_packet=0
        self.server_ack=0
        try:
            while(self.device_packet!=b'' and self.server_ack!=b''):
                # print(self,"TimeStamp:",datetime.datetime.fromtimestamp(time.time()+((5*60+30)*60)).strftime("%A %d %B %Y %I:%M%p"))
                # print(self,self.conn)
                self.conn_ready=select.select([self.conn],[],[],timeout)
                if(self.conn_ready[0]):
                    try:
                        self.device_packet = self.conn.recv(4096)
                        sys.stdout.write('\u001b[33m') #yellow
                        print(self,"Packet from Device received on  -  ",datetime.datetime.fromtimestamp(time.time()+((5*60+30)*60)).strftime("%A %d %B %Y %I:%M%p"))
                        self.connect_kill_timer=int(time.time())
                        print(self,"Device PKT: ",self.device_packet)
                        sys.stdout.write("Sending to tunnel. . .       ")
                        try:
                            self.tunnel.sendall(self.device_packet)
                            print("Sent to Tunnel.")
                        except BrokenPipeError:
                            print("Not sent to tunnel.")
                            self.tunnel.close()
                            self.tunnel=socket.socket()
                            print("Retrying connection to tunnel. . .")
                            self.tunnel.connect((tunnel_ip,tunnel_port))
                            self.tunnel.setblocking(0)
                            print("Connected to Tunnel.")
                            self.tunnel.sendall(self.device_packet)
                            print("Sent to Tunnel.")
                        except Exception as exception:
                            print("While forwarding to Tunnel -",exception)

                        if(forward_port!=0):
                            try:
                                sys.stdout.write("Sending to Forward Server. .  ")
                                self.forward.sendall(self.device_packet)
                                print("Sent to Forward Server.")
                            except Exception as exception:
                                print("Not sent to Forward Server - ",exception)
                                print("Trying to reconnect to Forward Server.")
                                try:
                                    self.forward=socket.socket()
                                    self.forward.connect((forward_ip,forward_port))
                                    print("Connected to Forward Server.")
                                    self.forward.sendall(self.device_packet)
                                    print("Sent to Forward Server.")
                                except Exception as exception:
                                    if(forward_flag==1):
                                        print("Error Connecting to Forward Server, will pass-",exception)
                                    else:
                                        print("Error Connecting to Forward Server, will terminate - ",exception)
                                        break

                        print("_______________________________________________________________________________________________________________________________________________________________________")

                    except Exception as exception:
                        print("Device receive loop - ",exception,"\nBreaking |")
                        break



                self.tunnel_ready=select.select([self.tunnel],[],[],timeout)
                if(self.tunnel_ready[0]):
                    try:
                        self.server_ack=self.tunnel.recv(4096)
                        self.connect_kill_timer=int(time.time())
                        sys.stdout.write('\u001b[32m') #green
                        print(self,"Packet from Tunnel received on  -  ",datetime.datetime.fromtimestamp(time.time()+((5*60+30)*60)).strftime("%A %d %B %Y %I:%M%p"))
                        print(self,"ACK: ",self.server_ack)
                        sys.stdout.write("ACK'ing Device. . .       ")
                        try:
                            self.conn.sendall(self.server_ack)
                        except Exception as exception:
                            print("While Forwading to Device - ",exception)
                        print("Device ACK'ed.")
                        print("_______________________________________________________________________________________________________________________________________________________________________")
                    except Exception as exception:
                        print("ACK loop - ",exception,"\nBreaking |")
                        break


                if((int(time.time())-self.connect_kill_timer)>(60*10)):
                    sys.stdout.write('\u001b[35m') #magenta
                    print("Timer overflow occured.")
                    break


        except Exception as exception:
            sys.stdout.write('\u001b[35m') #magenta
            print(self,"Closing Conn and Tunnel - ",exception)
            self.conn.close()
            self.tunnel.close()
            if(forward_port!=0):
                print("Closing Forward.")
                self.forward.close()
        else:
            sys.stdout.write('\u001b[35m') #magenta
            print(self,"No exception raised,Closing Conn and Tunnel.")
            self.conn.close()
            self.tunnel.close()
            if(forward_port!=0):
                print("Closing Forward.")
                self.forward.close()



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
        print ("Total",len(threads),"Threads are Active.\n",threads) # complete this line and add WDT 
    except Exception as exception:
        print("Accept loop - ",exception)



for t in threads: # not being used  
    t.join() 





