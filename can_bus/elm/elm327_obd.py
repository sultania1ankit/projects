import serial 
import time



valid_list=['1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','0','N','T','O'," "]
pid_points=['00','20','40','60','80','A0']


def get_value(id1,save_it=1,with_space=0):
	ip='01'+id1+'\r\n'
	ip=ip.encode()
	ser.write(ip)
	time.sleep(0.4)
	temp_char=0
	temp_result=''
	while(temp_char!=b''):
		temp_char=ser.read()
		temp_char_value=str(temp_char)
		temp_char_value=temp_char_value[2:len(temp_char_value)-1]
		len_valid_list=len(valid_list)-1
		if(with_space==1):
			len_valid_list=len_valid_list+1
		for temp_counter in range(0,len_valid_list):
			if(temp_char_value==valid_list[temp_counter]):
				temp_result=temp_result+temp_char_value
	print(temp_result)
	if(temp_result[0:2]=="41" and save_it==1):
		f.write(temp_result[2:4])
		f.write(",")
		f.write(temp_result[4:len(temp_result)])
		f.write(",")
		f.write("\n")
		return temp_result[4:len(temp_result)]





def get_available_id(ip1,location):
	ip1=int(ip1,16)
	ip1=bin(ip1)
	ip1=ip1[2:len(ip1)]
	# print(ip1)
	# print(len(ip1))
	temp_result=[]
	for temp_1 in range(0,len(ip1)):
		if (ip1[temp_1]=='1'):
			temp_hex=hex(temp_1+1+32*location)[2:4]
			if(len(temp_hex)==1):
				temp_hex='0'+temp_hex
			temp_result.append(temp_hex)
	return temp_result



ser = serial.Serial(
    port='/dev/rfcomm0',\
    baudrate=9600,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
        timeout=0)
f=open("obd_data.csv","w+")

total_pids_available=0
for temp_point in range(0,len(pid_points)-1):
	if(temp_point==0 or available_list[-1]==pid_points[temp_point]):
		temp_start_list=(get_value(pid_points[temp_point]))
		available_list=get_available_id(temp_start_list,temp_point)
		print(available_list)
		total_pids_available=total_pids_available+len(available_list)-1
		for temp_available in range(0,len(available_list)):	
			get_value(available_list[temp_available])


total_pids_available=total_pids_available+1
f.write("Total PID,")
f.write(total_pids_available)
f.write(",\n")

ser.close()
f.close()

# def extract_sample():
# 	f=open("obd_parameters_sample.csv","r")
# 	data=f.read()
# 	temp_line_ends=[]
# 	for temp_scan1 in range(0,len(data)):
# 		if(data[temp_scan1]=="\n"):
# 			temp_id=data[temp_scan1+1:temp_scan1+3]
# 			if(len(temp_id)!=0):
# 				if(temp_id[1]==","):
# 					temp_id='0'+temp_id[0]
# 			temp_line_ends.append([temp_id,temp_scan1])
# 	return (data,temp_line_ends)



# sample_data=extract_sample()[0]
# line_ends=extract_sample()[1]
# print(line_ends)


# def id_line(id2):
# 	id2=int(str(id2),16)
# # 	# print(id2)
# 	temp_data_length=(line_ends[id2][1]-line_ends[id2-1][1])
# # 	# print(line_ends[id2-1][1],"            ",temp_data_length)
# 	return(sample_data[line_ends[id2][1]:line_ends[id2][1]+temp_data_length])
# print(id_line(8))
# # print(line_ends[3][1]-line_ends[2][1])