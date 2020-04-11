import time
import serial
import os
import keyboard
from collections import deque

# 500 packets per second  in 500Kbps bus.  
# (0.002)  2ms (read packet + make array + print)

packet_reset=5000
packet_high=3500
packet_fading=2  # not to be changed

import sys
arguments=sys.argv
# print(arguments)
file_name=arguments[1]


can_data={}
usb_buffer=deque([],100)
data=1
sys.stdout.write('\u001b[40m')
sys.stdout.write('\x1b[2J')
sys.stdout.write('\x1b[H')
sys.stdout.write('\u001B[?25l')
sys.stdout.write("\n")
sys.stdout.write("\n")
sys.stdout.write('\u001b[5m')
sys.stdout.write('\u001b[115C')
sys.stdout.write('\u001b[37m')
sys.stdout.write("CAN ANALYSER")
sys.stdout.write('\u001b[0m')
sys.stdout.write('\u001b[40m')
sys.stdout.flush()



# __________________________________________________________________________________________________________

file=open(file_name,"r")
		
while 1:
	loop_timer=time.time()
	data=file.readline()
	if "can" in data:
		can_id=str(data[26:29])
		can_value=str(data[30:-1])


# __________________________________________________________________________________________________________

		bin_value=""
		for x in range(0,len(can_value),2):
			bin_value+='{0:08b}'.format(int(can_value[x:x+2],16))[2::]

		if (can_id not in can_data):
			can_data[can_id]=[bin_value,[0]*len(bin_value),[0]*len(bin_value)]
			for x in range(0,len(bin_value)-1):
				if(can_data[can_id][0][x]=='1'):
					can_data[can_id][1][x]=packet_reset


		if(can_data[can_id][0]!=bin_value):
			for x in range(0,len(bin_value)-1):
				if(can_data[can_id][0][x]!=bin_value[x]):
					can_data[can_id][1][x]=packet_reset
			can_data[can_id][0]=bin_value

		for x in can_data:
			for y in range(0,len(can_data[x][0])):
				if(can_data[x][1][y]>1):
					can_data[x][1][y]-=1


		if keyboard.is_pressed('m'):
			for x in can_data:
				for y in range(0,len(can_data[x][0])-1):
					if(can_data[x][1][y]!=0):
						can_data[x][2][y]=1
		if(keyboard.is_pressed('n')):
			for x in can_data:
				for y in range(0,len(can_data[x][0])-1):
					can_data[x][2][y]=0
		
		count=8
		for x in can_data:
			count+=1
			line_count="\u001b["+str(count)+"E"
			sys.stdout.write('\x1b[H')
			sys.stdout.write(line_count)
			sys.stdout.write('\u001b[35m')
			sys.stdout.write(x)
			sys.stdout.write("    ")
			for y in range(0,len(can_data[x][0])-1):
				if(can_data[x][2][y]==0):
					if(can_data[x][1][y]>packet_high):
						sys.stdout.write('\u001b[31m')
					elif(can_data[x][1][y]>packet_fading):
						sys.stdout.write('\u001b[33m')
					elif(can_data[x][1][y]==1):
						sys.stdout.write('\u001b[36m')
					elif(can_data[x][1][y]==0):
						sys.stdout.write('\u001b[32m')
					else:
						sys.stdout.write('\u001b[37m')
				else:
					sys.stdout.write('\u001b[30m')
				sys.stdout.write(can_data[x][0][y])

		

		sys.stdout.write('\x1b[H')
		sys.stdout.write("\u001b[6E")
		sys.stdout.write('\u001b[37m')
		sys.stdout.write("Loop Time: ")
		loop_time=time.time()-loop_timer;
		sys.stdout.write(str(loop_time))
		sys.stdout.flush()
	

