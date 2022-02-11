import os
import time

data_file=open("amaze.csv","r")
data_file=data_file.read()
tag_file=open("obd_tag.csv","r")
tag_file=tag_file.read()
obd_file=open("obd_file.csv","w")

temp_data=[]
temp_value=[]
temp_location=0
for x in range(0,len(data_file)):	
	if(data_file[x]=="\n"):
		temp_data.append(data_file[temp_location:x])
		temp_location=x+1
# print(temp_data)
temp_data2=[]
temp_location=0
for x in range(0,len(temp_data)):
	for y in range(0,len(temp_data[x])):
		if(temp_data[x][y]==","):
			temp_data2.append(temp_data[x][0:y])
			break
for x in range(0,len(temp_data)):
	temp_location=0
	for y in range(0,len(temp_data[x])):
		if(temp_data[x][y]=="," and temp_location==0):
			temp_location=y
		if(temp_data[x][y]=="," and temp_location!=y):
			temp_value.append(temp_data[x][temp_location+1:y])
			break

pid=temp_data2
tag=[]
temp_location=0
for x in range(0,len(tag_file)):	
	if(tag_file[x]=="\n"):
		tag.append(tag_file[temp_location:x])
		temp_location=x+1

tag_name=[]
for x in range(0,len(pid)):
	tag_name.append(tag[int(pid[x],16)+2])

# print(pid)
# print(temp_value)
# print(tag_name)	
obd_file.write("PID,Raw_value,Tag-PID,Bytes,Description,Min value,Max value,Units,Formula")
for x in range(0,len(pid)):
	line=pid[x]+","+temp_value[x]+","+tag_name[x]+"\n"
	# print(line)
	obd_file.write(line)

obd_file.close()
time.sleep(0.5)
os.system("xdg-open obd_file.csv")