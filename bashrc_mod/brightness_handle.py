import os
import sys


M_16="xrandr --output DP-4 --brightness "
M_27="xrandr --output DP-2 --brightness "
M_22="xrandr --output HDMI-0 --brightness "



brightness = int(sys.argv[2])
brightness /=100

if(brightness>1):
	brightness=1
if(brightness<0):
	brightness=0

if(sys.argv[1]=="M_16"):
	cmd=M_16+str(brightness)
	os.system(cmd)
elif(sys.argv[1]=="M_22"):
	cmd=M_22+str(brightness)
	os.system(cmd)
elif(sys.argv[1]=="M_27"):
	cmd=M_27+str(brightness)	
	os.system(cmd)
elif(sys.argv[1]=="MM"):
	cmd=M_16+str(brightness)	
	os.system(cmd)
	cmd=M_22+str(brightness)	
	os.system(cmd)
	cmd=M_27+str(brightness)	
	os.system(cmd)


