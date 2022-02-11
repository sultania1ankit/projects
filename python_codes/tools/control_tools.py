import sys
import os
import select
import subprocess
import time



def get_arg(argument_number=1,default_argument=0):
    """check for an argument or return default argument"""
    try:
        argument=sys.argv[argument_number]
        return argument 
    except:
        return default_argument


def input_time(block_time=1, wait_time=7):
    print("Press 1 to send command ...")
    i, o, e = select.select([sys.stdin], [], [], block_time)
    if(i):
        if((sys.stdin.readline().strip()) == "1"):
            print("You have ", wait_time, " seconds.")
            print("Command :")
            i1, o1, e1 = select.select([sys.stdin], [], [], wait_time)
            if(i1):
                return (sys.stdin.readline().strip())
            else:
                return "empty"
    print("Time Up!")