import socket
from socket import AF_INET, SOCK_DGRAM
import time
import random
import matplotlib.pyplot as plt
import numpy 
from drawnow import *

pitch = []
roll=[]
plt.ion()  # tell matplotlib that we will handle live data

#_____________________________________________________

def start_UDPserver():
    global s
    global BUFFER_SIZE
    HOST = '0.0.0.0' 
    PORT = 44444
    BUFFER_SIZE = 20
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(0.5)
    s.bind((HOST, PORT))
#_____________________________________________________
def udp():
    global data_received
    #start_time=time.time()
    # s.sendto(data_send.encode(),('192.168.4.1',44444))# direct 169.254.251.47# thru phone 192.168.43.85
    try:
        data_received=s.recvfrom(BUFFER_SIZE)
        data_received=data_received[0]
    except socket.timeout:
        data_received=[0,0,0,0,0,0,0,0,0,0,0,0,0,0] 

#_____________________________________________________
def makefig():
    plt.title('Drone MPU')
    plt.grid(True)
    plt.ylabel('Pitch')
    plt.ylim(-50,50)
    plt.plot(pitch,'r',label='Pitch')
    plt.legend(loc='upper left')
    plt2=plt.twinx()
    plt.ylim(-50,50)
    plt2.plot(roll,'b',label='Roll')
    plt2.set_ylabel('Roll')
##    plt2.ticklabel_format(useOffset=False)  # force matplotlib to not auto scale y axis
    plt2.legend(loc='upper right')

#_____________________________________________________
def main_loop():
    start_UDPserver()
    while True:
        start_time=time.time()
        global data_received
        global pitch
        global roll
        udp()
        print(data_received[0])
##        thrust_fr=data_received[6]*100+data_received[7]
##        thrust_fl=data_received[8]*100+data_received[9]
##        thrust_br=data_received[10]*100+data_received[11]
##        thrust_bl=data_received[12]*100+data_received[13]
##        print (thrust_bl,'                                                             ',thrust_br)
##        print (thrust_fl,'                                                               ',thrust_fr)
##        print('')
##        print('________________FACE____________________')
##        print (data_received[1])
##        pitch1=data_received[0]*100+(data_received[1])
##        roll1=data_received[2]*100+(data_received[3])
##        roll1=roll1/100
##        pitch1=pitch1/100
##        if(roll1>100):
##            roll1=roll1-256
##        if(pitch1>100):
##            pitch1=pitch1-256
##        print(pitch1)
##        pitch.append(pitch1)
##        roll.append(roll1)
##        pitch = pitch[-20:]
##        roll=roll[-20:]
##        drawnow(makefig)
####        time.sleep(1)
##        print(time.time()*1000-start_time*1000)
        
        
#_____________________________________________________
main_loop()
