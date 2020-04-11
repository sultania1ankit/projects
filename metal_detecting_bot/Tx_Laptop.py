import socket
from socket import AF_INET, SOCK_DGRAM
import time
import msvcrt


data_send = 0
direction = 0
turn_mode = 0
packet_number = 0
one_second=time.time()*1000.0
loop_timer=0
loop_time=0
packets_sent=0
packets_sent_counter=0
packets_received=0
packets_received_counter=0
packets_delivered=0
packets_expected=0
no_packets=0
no_packets_counter=0
metal_read=0
battery_voltage=0
distance_front=0
distance_back=0
safe_distance=0
distance_safety_enabled=0
safe_distance_atbot=0
distance_safety_enabled_atbot=0
packet_missed=0


#_____________________________________________________

def start_UDPserver():
    global s
    global BUFFER_SIZE
    HOST = '0.0.0.0' 
    PORT = 44444
    BUFFER_SIZE = 16   
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(0.01)
    s.bind((HOST, PORT))
#_____________________________________________________
def keyboard_input():
    global data_send
##    time.sleep(0.01)
    x = msvcrt.kbhit()
    if x: 
        data_send = ord(msvcrt.getch())
##        print(data_send)    

#_____________________________________________________
def udp():
    global data_received
    global packets_received_counter
    global packets_sent_counter
    global no_packets_counter
    global packet_missed
    #start_time=time.time()
    s.sendto(data_send.encode(),('192.168.4.1',44444))# direct 169.254.251.47# thru phone 192.168.43.85
    packets_sent_counter+=1
    try:
        data_received=s.recvfrom(BUFFER_SIZE)
        data_received=data_received[0]
        packets_received_counter+=1
        packet_missed=0
    except socket.timeout:
##        print ('Packet Missed')
        no_packets_counter+=1
        packet_missed=1
        data_received=[0,0,0,0,0,0,0,0,0,0] 

#_____________________________________________________          

      
def process_input():
    global data_send
    global diection
    global turn_mode
    global packet_number
    global distance_safety_enabled
    global safe_distance
    #print(data_send)
    if (data_send==224):
        pass;
    else :
        if(data_send == 119):
            direction=2 # front
        elif (data_send == 115):
            direction =3 # back
        elif( data_send == 100):
            direction = 4 #right
        elif( data_send == 97):
            direction = 5 #left
        else :
            direction =1
        if( data_send == 109 ):
            turn_mode += 1
        elif( data_send == 101):
            distance_safety_enabled+=1
        elif( data_send == 43):
            safe_distance+=1
        elif( data_send == 45):
            safe_distance-=1
    if(safe_distance>40):
        safe_distance=40
    elif(safe_distance<0):
        safe_distance=0
    if(turn_mode ==3):
        turn_mode=0
    if(distance_safety_enabled ==2):
        distance_safety_enabled=0

    packet_number+=1
    if(packet_number==3):
        packet_number=0
    if(packet_number == 0):
        data_send=10000+1000*turn_mode+direction
    if(packet_number == 1):
        data_send=20000+100*distance_safety_enabled+safe_distance
    if(packet_number == 2):
        data_send=10000+1000*turn_mode+direction

    data_send=str(data_send)

#_____________________________________________________

def process_data_received():
    global packets_delivered
    global packets_expected
    global metal_read
    global distance_front
    global distance_back
    global battery_voltage
    global safe_distance_atbot
    global distance_safety_enabled_atbot
    global packet_missed
##    print(data_received[0])
##    print(data_received[1])
##    print(data_received[2])
##    print(data_received[3])
##    print(data_received[4])
##    print(data_received[5])
##    print(data_received[6])
##    print(data_received[7])
##    print(data_received[8])
##    print(data_received[9])
    if(packet_missed==0):
        front_distance=data_received[0]
        battery_voltage=data_received[5]+(data_received[4]/100)
        metal_read=100*data_received[10]+data_received[6]
        packets_delivered=data_received[1]+100*(data_received[2]%10)
        packets_expected= (data_received[2]/10)+10*data_received[3]
        packets_expected=round(packets_expected)
        distance_front=data_received[7]
        distance_back=data_received[8]
        safe_distance_atbot=data_received[9]%100
        distance_safety_enabled_atbot=(data_received[9]/100)%10-1
        distance_safety_enabled_atbot=round(distance_safety_enabled_atbot)
##        print(distance_safety_enabled_atbot)
 
#_____________________________________________________
def each_second():
    global one_second
    global packets_sent
    global packets_received
    global packets_received_counter
    global packets_sent_counter
    global no_packets
    global no_packets_counter
    global loop_time
    global loop_timer
    loop_time=round(time.time()*1000-loop_timer)
    loop_timer=time.time()*1000
    if(time.time()*1000.0-one_second>1000):
        one_second=time.time()*1000.0
        packets_received=packets_received_counter
        packets_received_counter=0
        packets_sent=packets_sent_counter
        packets_sent_counter=0
        no_packets=no_packets_counter
        no_packets_counter=0
        #show_parameters()
        
#_____________________________________________________
def show_parameters():
    global packets_sent
    global data_send
    global packets_received
    global packets_delivered
    global packets_expected
    global no_packets
    global loop_timer
    global received_stuff
    global metal_read
    global distance_front
    global distance_back
    global battery_voltage
    global distance_safety_enabled
    global safe_distance
    global loop_time
    global safe_distance_atbot
    global distance_safety_enabled_atbot
    print('Change Builtin delay to match sent and expected packets to minimize lag in feedback')
    print('_____________________________________________')
    print('')
    print('Loop time: ',loop_time)
    print('')
    print('')
    print('Data Sent: ',data_send)
    print('')
    print('')
    print('Bot Received: ',packets_delivered,'              Packets Received: ',packets_received)
    print('')
    print('')
    print('Packets Sent: ',packets_sent,'              Packets Expected: ',packets_expected)
    print('')
    print('')
    print('No Packets:  ',no_packets)
    print('')
    print('')
    if(metal_read<570):
        print('Metal Read: ',metal_read,'                         Metal Detected')
    else:
        print('Metal Read: ',metal_read,'                         Metal Not Detected')
    print('')
    print('')
    print('Front Distance: ',distance_front,'              Back Distance:  ',distance_back)
    print('')
    print('')
    print('Battery Voltage: ',battery_voltage)
    print('')
    print('')
    if(distance_safety_enabled==1):
        print('Safe Distance:  ',safe_distance,'                       Disatnce  Safety  Enabled')
    else:
        print('Safe Distance:  ',safe_distance,'                       Distance  Safety  Disabled')
    print('')
    print('')
    if(distance_safety_enabled_atbot==1):
        print('Safe Distance at Bot:  ',safe_distance_atbot,'                       Distance  Safety  Enabled at Bot')
    else:
        print('Safe Distance at Bot:  ',safe_distance_atbot,'                       Distance  Safety  Disabled at Bot')
    print('')
    print('')
    print('\n'*15)
    


 #_____________________________________________________       
    
def main_loop():
    start_UDPserver()
    display_damper=0
    while True:
        keyboard_input()
        process_input()
        udp()
        process_data_received()
        each_second()
##        show_parameters()
        display_damper+=1
        if(display_damper==100):
            show_parameters()
            display_damper=0
##        time.sleep(1)
        
        
#_____________________________________________________
main_loop()
