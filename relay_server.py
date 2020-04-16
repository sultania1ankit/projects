


 # DEVICE<-----------------> Relay_Server<-----------------> Primary Server
 #                                        ----------------->Secondary Server



import sys
import time
import datetime
import socket 
import threading
from threading import Thread 
from socketserver import ThreadingMixIn 
import select


sys.stdout.write('\u001b[40m') # clear screen and display header
sys.stdout.write('\x1b[2J')
sys.stdout.write('\x1b[H')
# sys.stdout.write('\u001B[?25l')
sys.stdout.write("\n")
sys.stdout.write("\n")
sys.stdout.write('\u001b[5m')
sys.stdout.write('\u001b[115C')
sys.stdout.write('\u001b[37m')
sys.stdout.write("RELAY SERVER")
sys.stdout.write('\u001b[0m')
sys.stdout.write('\u001b[40m')
sys.stdout.flush()


print("\u001b[33m\n\nPrimary Will Acknowledge the Device,if needed. No data will be received for the secondary server.\n")

sys.stdout.write('\u001b[32m\nYou can choose the Corresponding Numbers while Selecting primary and secondary IP.(If Ports are also provided they will be auto selected)\n')

stored_IP=[["BLANK","0.0.0.0",0],["NGROK","3.19.114.185"],["Local","234.216.54.345",5446]]

for display_IP in range(1,len(stored_IP)): #print saved IP and ports
    sys.stdout.write('\n\u001b[37m'+str(display_IP)+".\u001b[32m")
    for display_port in range(0,len(stored_IP[display_IP])):
        if(display_port==0):
            sys.stdout.write(str(stored_IP[display_IP][display_port])+" - ")
        elif(display_port==2):
            sys.stdout.write(":"+str(stored_IP[display_IP][display_port]))
        else:    
            sys.stdout.write(str(stored_IP[display_IP][display_port]))



print("\u001b[33m\n\n")


TCP_PORT=int(input("Listener Port: "))
primary_server=str(input("Primary Client IP: "))
try: #check if saved IP port are being used
    pick_int=int(primary_server)
    if(len(stored_IP[pick_int])==2):
        primary_server=stored_IP[pick_int][1]
        primary_port=int(input("Primary Client Port: "))
    elif(len(stored_IP[pick_int])==3):
        primary_server=stored_IP[pick_int][1]
        primary_port=stored_IP[pick_int][2]
except:
    primary_port=int(input("Primary Client Port: "))

secondary_server=str((input("Secondary Server IP(Press 0 to Skip): ")))
try: #check if saved IP port are used or option is skipped.
    pick_int=int(secondary_server)
    if(len(stored_IP[pick_int])==2):
        secondary_server=stored_IP[pick_int][1]
        secondary_port=int(input("Secondary Server Port: "))
        secondary_priority=int(input("Press 1 to allow Script to run even if secondary server is Down, else press 0: "))
    elif(len(stored_IP[pick_int])==3):
        secondary_server=stored_IP[pick_int][1]
        secondary_port=stored_IP[pick_int][2]
        if(secondary_port!=0):
            secondary_priority=int(input("Press 1 to allow Script to run even if secondary server is Down, else press 0: "))
except:
    secondary_port=int(input("Secondary Server Port: "))
    secondary_priority=int(input("Press 1 to allow Script to run even if secondary server is Down, else press 0: ")) 


sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM) #setup socket for reading data stream
sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
server_address=('0.0.0.0',TCP_PORT)
sock.settimeout(None)
timeout=1  ## this is select timeout not socket timeout
sock.bind(server_address)


class ClientThread(Thread):  # Accept a new connection request and connect to primary and secondary servers
 
    def __init__(self,ip,port,conn): 
        Thread.__init__(self) 
        sys.stdout.write("New server socket request from " + ip + ":" + str(port)+". . .       ")
        self.ip = ip 
        self.port = port
        self.conn=conn 
        self.conn.settimeout(None)
        self.connect_kill_timer=int(time.time())
        print("Connection Request Accepted.")
        sys.stdout.write("Connecting to primary ... ")
        self.primary=socket.socket()
        try:
            self.primary.connect((primary_server,primary_port))
            self.primary.setblocking(0)
            print("Connected to primary.")
        except Exception as exception:
            print("Error Connecting to primary, will proceed and retry at later stage  -",exception)
        if(secondary_port!=0):
            sys.stdout.write("Connecting to secondary Server...")
            self.secondary=socket.socket()
            try:
                self.secondary.connect((secondary_server,secondary_port))
                print("Connected to secondary Server.")
            except Exception as exception:
                print("Error Connecting to secondary Server, will proceed and retry at later stage  -",exception)

 
    def run(self): 
        #receive data from device
        #send to primary
        #send to secondary server if it is set, if transmission failed and secondary_prioprity is set then kill the thread.
        #if data is received from the primary server, send it to device.

        # Color codes
        # Data from device - orange
        # ACk from primary - green
        # Thread related information - magenta

        self.device_packet=0
        self.server_ack=0
        try:
            while(self.device_packet!=b'' and self.server_ack!=b''): #if data from device sent it to primary and secondary
                # print(self,"TimeStamp:",datetime.datetime.fromtimestamp(time.time()+((5*60+30)*60)).strftime("%A %d %B %Y %I:%M%p"))
                # print(self,self.conn)
                self.conn_ready=select.select([self.conn],[],[],timeout)
                if(self.conn_ready[0]):
                    try:
                        self.device_packet = self.conn.recv(4096)
                        sys.stdout.write('\u001b[33m') #yellow
                        print(self,"Packet from Device received on  -  ",datetime.datetime.fromtimestamp(time.time()+((5*60+30)*60)).strftime("%A %d %B %Y %I:%M%p"))
                        self.connect_kill_timer=int(time.time()) #watchdog timer for thread
                        print(self,"Device PKT: ",self.device_packet)
                        sys.stdout.write("Sending to primary. . .       ")
                        try:
                            self.primary.sendall(self.device_packet)
                            print("Sent to primary.")
                        except BrokenPipeError:
                            print("Not sent to primary.")
                            self.primary.close()
                            self.primary=socket.socket()
                            print("Retrying connection to primary. . .")
                            self.primary.connect((primary_server,primary_port))
                            self.primary.setblocking(0)
                            print("Connected to primary.")
                            self.primary.sendall(self.device_packet)
                            print("Sent to primary.")
                        except Exception as exception:
                            print("While secondarying to primary -",exception)

                        if(secondary_port!=0):
                            try:
                                sys.stdout.write("Sending to secondary Server. .  ")
                                self.secondary.sendall(self.device_packet)
                                print("Sent to secondary Server.")
                            except Exception as exception:
                                print("Not sent to secondary Server - ",exception)
                                print("Trying to reconnect to secondary Server.")
                                try:
                                    self.secondary=socket.socket()
                                    self.secondary.connect((secondary_server,secondary_port))
                                    print("Connected to secondary Server.")
                                    self.secondary.sendall(self.device_packet)
                                    print("Sent to secondary Server.")
                                except Exception as exception:
                                    if(secondary_priority==1):
                                        print("Error Connecting to secondary Server, will pass-",exception)
                                    else:
                                        print("Error Connecting to secondary Server, will terminate - ",exception)
                                        break

                        print("_______________________________________________________________________________________________________________________________________________________________________")

                    except Exception as exception:
                        print("Device receive loop - ",exception,"\nBreaking |")
                        break



                self.primary_ready=select.select([self.primary],[],[],timeout) # If data is received from primary send it to device.
                if(self.primary_ready[0]):
                    try:
                        self.server_ack=self.primary.recv(4096)
                        self.connect_kill_timer=int(time.time())
                        sys.stdout.write('\u001b[32m') #green
                        print(self,"Packet from primary received on  -  ",datetime.datetime.fromtimestamp(time.time()+((5*60+30)*60)).strftime("%A %d %B %Y %I:%M%p"))
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


                if((int(time.time())-self.connect_kill_timer)>(60*10)): #if watchdog overflow, kill thread
                    sys.stdout.write('\u001b[35m') #magenta
                    print("Timer overflow occured.")
                    break


        except Exception as exception:
            sys.stdout.write('\u001b[35m') #magenta
            print(self,"Closing Conn and primary - ",exception)
            self.conn.close()
            self.primary.close()
            if(secondary_port!=0):
                print("Closing secondary.")
                self.secondary.close()
        else:
            sys.stdout.write('\u001b[35m') #magenta
            print(self,"No exception raised,Closing Conn and primary.")
            self.conn.close()
            self.primary.close()
            if(secondary_port!=0):
                print("Closing secondary.")
                self.secondary.close()



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





