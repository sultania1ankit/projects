import time
import os
import sys
import socket
import select


def use_default_file(filename):
    """This function will check for the file in root program directory"""
    print("Looking for the Default file \'"+filename+"\'.")
    if(os.path.isfile(filename)):
        user_input=input("\'"+filename+"\'"+" found, Do you want to use this file (yes/no): ")
        user_input=user_input.lower()
        if(user_input=='y' or user_input=='yes'):
            return 1
        else:
            return 0
    else:
        print("\'"+filename+"\' not found.")
        return 0

def get_input_file(argument_number=1,default_file=0):
    """ returns input file to work with,checks for input file at argument_number"""
    try:
        filename = sys.argv[argument_number]
        if(os.path.isfile(filename)):
            return filename
        else:
            print("The file \'"+str(sys.argv[argument_number])+ "\' does not exist")
            if(default_file!=0):
                if(use_default_file(default_file)):
                    return default_file
                else:
                    return 0
            else:
                return 0
    except:
        print("No file provided.")
        if(default_file!=0):
            if(use_default_file(default_file)):
                return default_file
            else:
                return 0   
        else:
            return 0





# write logging module ( python logger not reliable)

# def save_data(filename, data):
#     time_stamp = int(time.time())
#     time_stamp = str(time_stamp+((5*60+30)*60))
#     csv_string = time_stamp+','+data[1:-1]+"\n"
#     f = open(filename, "a")
#     f.write(csv_string)
#     f.close()


# def save_ping(filename):
#     time_stamp = int(time.time())
#     time_stamp = str(time_stamp+((5*60+30)*60))
#     csv_string = time_stamp+'\n'
#     f = open(filename, "a")
#     f.write(csv_string)
#     f.close()


# def save_raw_packet(filename, data):  # 0- byte array , 1- string
#     f = open(filename, "a")
#     f.write(data)
#     f.write("\n")
#     f.close()


#__________________LOGGING HELPERS_______________________________________


