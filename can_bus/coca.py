import time
import keyboard
import sys
arguments=sys.argv
from collections import deque
import os

file_name=sys.argv[1]
file=open(file_name,"r")


#_______________________________________________________________________________________________________________________________

def display_head(): # clear display and write the static content like heaeder and command information
	sys.stdout.write('\u001b[40m') 
	sys.stdout.write('\x1b[2J')
	sys.stdout.write('\x1b[H')
	sys.stdout.write("\u001b[1E")
	sys.stdout.write('\u001B[?25l')
	sys.stdout.write('\u001b[5m')
	sys.stdout.write('\u001b[115C')
	sys.stdout.write('\u001b[37m')
	sys.stdout.write("COLD CAN ANALYSER")
	sys.stdout.write('\u001b[0m')
	sys.stdout.write('\u001b[40m')
	sys.stdout.write("\u001b[1E")
	sys.stdout.write("\u001b[33m")
	sys.stdout.flush()

	print("1: Non-Static masking.") # mask the values that are currently changing
	print("2: Decreasing value masking.(Not yet handling roll over from FF -> 00)") # mask the values that are not strictly increasing + mask values same at start and end of this process
	print("3: Prepare static mask.")
	print("4: Apply static mask.")
	
	print("\n6: PARTY") # majority of stuff should get filtered.
	print("r: Reset mask, b: Binary, h:Hexadecimal")
	print("d: Display masked data and pause filteration process.                           -> l:Lock keyboard")
	sys.stdout.write('\x1b[H')
	sys.stdout.write("\u001b[13E")
	sys.stdout.write("________________________________________________________________________________")
	sys.stdout.write("________________________________________________________________________________")
	sys.stdout.write("_______________________________________________________________________________")
	sys.stdout.write('\x1b[H')
	sys.stdout.write("\u001b[16E")
	sys.stdout.write("________________________________________________________________________________")
	sys.stdout.write("________________________________________________________________________________")
	sys.stdout.write("_______________________________________________________________________________")
	sys.stdout.write("\u001b[37m")
	sys.stdout.flush()

display_head()

can_data={}
process_token=[0,0,0,0,0,0,0]
clear_screen_filtered=0
show_binary=0
temp_color=30
locked=0
start_line_number=18
status_bar="\u001b[16;0H"
start_line="\u001b[18;0H"
mask_7=[]
mask_8=[]
mask_9=[]
mask_0=[]
mask=0

def party_screen(): #this prints "PARTY" all over the screen 
	global temp_color
	global clear_screen_party
	if(clear_screen_party==1):
		temp_color=30
		clear_screen_party=0
	sys.stdout.write("\u001b[2J")
	sys.stdout.write('\x1b[H')
	sys.stdout.write('\u001b[5m')
	temp_color+=1
	if(temp_color==38):
		temp_color=30
	sys.stdout.write('\u001b['+str(temp_color)+'m')
	for x in range(0,1999):
		sys.stdout.write("   PARTY   ")
	sys.stdout.write('\u001b[0m')
	sys.stdout.flush()
	time.sleep(0.5)





def static_mask():
	# temp_time=time.time()
	for x in can_data:
		for y in range(0,len(can_data[x][2])):
			if(can_data[x][3][y]==0):
				can_data[x][2][y]=1
	# sys.stdout.write("\u001b[11;120H")
	# sys.stdout.write("static mask - ")
	# sys.stdout.write(str('{:.9f}'.format(time.time()-temp_time)))
def mask_reset():
	# temp_time=time.time()
	for x in can_data:
		for y in range(0,len(can_data[x][0])):
			can_data[x][2][y]=0
			can_data[x][3][y]=0
	# sys.stdout.write("\u001b[10;120H")
	# sys.stdout.write("mask reset")
	# sys.stdout.write(str('{:.9f}'.format(time.time()-temp_time)))


def show_masked_data():
	# temp_time=time.time()
	global clear_screen_filtered
	global start_line
	global start_line_number
	if(clear_screen_filtered==1):
		display_head()
		clear_screen_filtered=0
	line_number=start_line_number+1
	for x in can_data:
		line_number+=1
		if (x==can_id):
			if(show_binary==0):# head(8)+spaces(7)+data(16,)
				column_shift=16+7+(8*2)+5   
			elif(show_binary==1):
				column_shift=16+7+(8*8)+5  
			line_count="\u001b["+str(line_number)+";"+str(column_shift)+"H"
			sys.stdout.write(line_count)
			for y in range(0,int(len(can_data[x][0])),2):
				if(can_data[x][2][y]==0 or can_data[x][2][y+1]==0):
					sys.stdout.write('\u001b[32m')
				else:
					sys.stdout.write('\u001b[30m')
				if(show_binary==0):
					sys.stdout.write(can_data[x][0][y])
					sys.stdout.write(can_data[x][0][y+1])
				elif(show_binary==1):
					temp_bin=can_data[x][0][y:y+2]
					temp_bin='{0:08b}'.format(int(temp_bin,16))
					sys.stdout.write(temp_bin)
				sys.stdout.write(' ')
			sys.stdout.flush()
	# sys.stdout.write("\u001b[9;120H")
	# sys.stdout.write("show masked")
	# sys.stdout.write(str('{:.9f}'.format(time.time()-temp_time)))

def show_raw_data():
	# temp_time=time.time()
	sys.stdout.write("\u001b[16;0H")
	sys.stdout.write("\u001b[33m")
	sys.stdout.write("Loop Time:")
	sys.stdout.write(str('{:.9f}'.format(time.time()-loop_timer)))
	global start_line_number
	global start_line
	line_number=start_line_number
	line_number+=1
	for x in can_data:
		line_number+=1
		if(x==can_id):
			line_count="\u001b["+str(line_number)+";0H"
			sys.stdout.write(line_count)
			sys.stdout.write('\u001b[35m')
			sys.stdout.write(x)
			sys.stdout.write("  -  ")  
			sys.stdout.write('\u001b[36m')
			for y in range(0,int(len(can_data[x][0])),2):
				if(int(len(can_data[x][0]))<17):
					if(show_binary==0):
						sys.stdout.write(can_data[x][0][y])
						sys.stdout.write(can_data[x][0][y+1])
					elif(show_binary==1):
						temp_bin=can_data[x][0][y:y+2]
						temp_bin='{0:08b}'.format(int(temp_bin,16))
						sys.stdout.write(temp_bin)
					sys.stdout.write(' ')

			if(show_binary==0):
				column_shift=15+7+(8*2)
			elif(show_binary==1):
				column_shift=15+7+(8*8)
			column_shift="\u001b["+str(column_shift)+"G"
			sys.stdout.write(column_shift)  # head(8)+spaces(7)+data(16,)
			sys.stdout.write("  -  ")
			sys.stdout.flush()
	# sys.stdout.write("\u001b[8;120H")
	# sys.stdout.write("show raw")
	# sys.stdout.write(str('{:.9f}'.format(time.time()-temp_time)))


def save_mask():
	mask_file=open("coca_mask.txt","w")
	for temp_1 in can_data:
		temp_mask_string=str(temp_1)+":"+str(can_data[temp_1][2])+"\n"
		mask_file.write(temp_mask_string)
	



while 1:

#_______________________________________________________________________________________________________________

	loop_timer=time.time()
	if keyboard.is_pressed('l'):
		locked= not locked
		time.sleep(0.1)
	if(locked==0): 
		if keyboard.is_pressed('d'):
			process_token=[1,0,0,0,0,0,0]
			clear_screen_filtered=1
		elif keyboard.is_pressed('1'):
			process_token=[0,1,0,0,0,0,0]
		elif keyboard.is_pressed('2'):
			process_token=[0,0,1,0,0,0,0]
		elif keyboard.is_pressed('3'):
			process_token=[0,0,0,1,0,0,0]
		elif keyboard.is_pressed('4'):
			static_mask()
			process_token=[0,0,0,0,1,0,0]
		elif keyboard.is_pressed('5'):
			process_token=[0,0,0,0,0,1,0]
		elif keyboard.is_pressed('6'):
			process_token=[0,0,0,0,0,0,1]
			clear_screen_party=1
		elif keyboard.is_pressed('r'):
			mask_reset()
			display_head()
		elif keyboard.is_pressed('b'):
			show_binary=1
			display_head()
		elif keyboard.is_pressed('h'):
			show_binary=0
			display_head()
		elif keyboard.is_pressed('m'):
			pass
			save_mask()
		elif keyboard.is_pressed('7'):
			mask=7
		elif keyboard.is_pressed('8'):
			mask=8
		elif keyboard.is_pressed('9'):
			mask=9
		elif keyboard.is_pressed('0'):
			mask=0

	data=file.readline()
	if "can" in data:
		sys.stdout.write("\u001b[16;30H")
		if(locked==1):
			sys.stdout.write("\u001b[31m")
		elif(locked==0):
			sys.stdout.write("\u001b[32m")
		sys.stdout.write("LOCK  ->")
		sys.stdout.write("\u001b[16;45H")
		sys.stdout.write("\u001b[31m")
		sys.stdout.write("Latency")


		can_id=str(data[26:34])
		can_value=str(data[35:-1])

#_______________________________________________________________________________________________________________
		# temp_time=time.time()
		if can_id not in can_data:
			mask=[0]*len(can_value)
			step2_mask=[0]*len(can_value)
			old_can_value=can_value
			can_data[can_id]=[can_value,old_can_value,mask,step2_mask,deque([],50)]
			can_data[can_id][4].append(can_data[can_id][0])
		else:
			old_can_value=can_data[can_id][0]
			can_data[can_id][0]=can_value
			can_data[can_id][1]=old_can_value
			can_data[can_id][4].append(can_data[can_id][0])
		# sys.stdout.write("\u001b[7;120H")
		# sys.stdout.write("can id update - ")
		# sys.stdout.write(str('{:.9f}'.format(time.time()-temp_time)))

#_______________________________________________________________________________________________________________
		

				
			


#_______________________________________________________________________________________________________________

		if(process_token[1]==1):
			# temp_time=time.time()
			for x in can_data:
				for y in range(0,len(can_data[x][0])):
					if(can_data[x][0][y]!=can_data[x][1][y]):
						can_data[x][2][y]=1
			# sys.stdout.write("\u001b[6;120H")
			# sys.stdout.write("Non-static mask")
			# sys.stdout.write(str('{:.9f}'.format(time.time()-temp_time)))


#---------------------------------------------------------------------


		elif(process_token[2]==1):
			# temp_time=time.time()
			for x in can_data:
				temp_queue=can_data[x][4]
				# print(temp_queue)
				int_temp_queue=[]
				for y in temp_queue:
					temp_queue_2=[]
					for z in range(0,len(y),2):
						temp_queue_2.append(int(y[z:z+2],16))
					# print(temp_queue_2)
					int_temp_queue.append(temp_queue_2)
					# print(int_temp_queue)


				# if(x=="158"):
				#   print(int_temp_queue)
				
				temp_data_length=len(int_temp_queue[0])
				for y in range(0,temp_data_length):
					for z in range(0,len(int_temp_queue)-1):
						if(int_temp_queue[z][y]>int_temp_queue[z+1][y]):
							can_data[x][2][y*2]=1
							can_data[x][2][(y*2)+1]=1
			# sys.stdout.write("\u001b[13;120H")
			# sys.stdout.write("Non decreasing-")
			# sys.stdout.write(str('{:.9f}'.format(time.time()-temp_time)))




#---------------------------------------------------------------------

		elif(process_token[3]==1):
			# temp_time=time.time()
			for x in can_data:
				for y in range(0,len(can_data[x][0])):
					if(can_data[x][0][y]!=can_data[x][1][y]):
						can_data[x][3][y]=1
			# sys.stdout.write("\u001b[12;120H")
			# sys.stdout.write("static mask")
			# sys.stdout.write(str('{:.9f}'.format(time.time()-temp_time)))
#---------------------------------------------------------------------

		elif(process_token[6]==1):
			party_screen()

#---------------------------------------------------------------------
		elif(process_token[0]==1):
			show_masked_data()
#---------------------------------------------------------------------
		
		show_raw_data()


#_______________________________________________________________________________________________________________
	else:
		sys.stdout.write("\u001b[16;45H")
		sys.stdout.write("\u001b[32m")
		sys.stdout.write("Latency")
		time.sleep(0.01)



