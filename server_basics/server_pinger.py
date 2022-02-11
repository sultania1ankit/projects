import socket
import sys
import time
import time
import os
import sys
import select
# my imports

host = socket.gethostname()
if(host == "NandiPumps"):
    sys.path.append(os.path.abspath(
        "/home/pi/codes/python_codes/tools/"))
    from terminal_control_tools import *
    from data_manipulation_tools import *
    from file_handling_tools import *
    from control_tools import *
    from bad_tcp_tools import *
elif(host == "coldlapy"):
    sys.path.append(os.path.abspath(
        "/home/ankit_sultania/codes/python_codes/tools/"))
    from terminal_control_tools import *
    from data_manipulation_tools import *
    from file_handling_tools import *
    from control_tools import *
    from bad_tcp_tools import *

try:
    if(sys.argv[1]):
        ip=str(sys.argv[1])
except:
    ip=str(input("\nIP:"))

try:
    if(sys.argv[2]):
        port=int(sys.argv[2])
except:
    port=int(input("\nPort:"))

t=1
try:
    if(sys.argv[3]):
        t=float(sys.argv[3])
except Exception as exception:
    print(exception)
    print("Default Ping Time of 1 second will be used.(Use Argument 3 to change)")

ping_msg=b'123'

try:
    if(sys.argv[4]):
        ping_msg=make_bytes(sys.argv[4],1)
except Exception as exception:
    print(exception)
    print("Default Ping msg 123 will be used.(Use Argument 4 to change)")



sys.stdout.write("Connecting . . .    ")
s=socket.socket()
try:
    s.connect((ip,port))
    print("     Connected.")
except Exception as exception:
    print(exception)

print("Starting Pinging at frequency "+str(1/t))
while (1):
    print("Pinging with "+str(ping_msg))
    s.sendall(ping_msg)
    time.sleep(t)
