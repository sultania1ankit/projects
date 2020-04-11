import serial
import time
import sys
import os
import simple_watchdog_timer
file_name=str(sys.argv[1])

def wdt_reset(a):
	wdt.pause()
	print("WATCH DOG RESET OCCURED")
	ser.setDTR(0)
	ser.setDTR(1)
	time.sleep(3)
	wdt.update()
	wdt.resume()

wdt=simple_watchdog_timer.WDT(check_interval_sec=2,trigger_delta_sec=2,callback=wdt_reset)


ser=serial.Serial(file_name,921000)
vcan_file=open("/home/ankit_sultania/cold_codes/CAN/sniffing_can/vcan.log","w")

def read_serial():
	wdt.update()
	data=ser.readline()
	ip=data.decode()
	if "$" in ip:
		print("Rejected - ",ip)
	elif "#" in ip:
		print(ip)
		ip="("+str("%.6f"%time.time())+") "+"vcan0 "+ip
		vcan_file=open("/home/ankit_sultania/cold_codes/CAN/sniffing_can/vcan.log","a")
		vcan_file.write(ip)
		vcan_file.close()
	

while (1):
	try:
		read_serial()
	except:
		print("UNHANDELED ERROR GENERATED")
		time.sleep(1)
		pass


























# import serial
# import time
# import sys

# ser=serial.Serial('/dev/ttyUSB0',921000)
# file=open("can_logs.log","w")

# while(1):
# 	data=ser.readline()
# 	ip=data.decode()
# 	ip="("+str("%.6f"%time.time())+") "+"vcan0 "+ip
# 	print(ip)
# 	file=open("can_logs.log","a")
# 	file.write(ip)
# 	file.close()
	