
# __________________________________________codec-12________________________________________________________

def zero_prefixer(ip, length):  # make message length as 4 bytes after coversion to hex
    temp_length = len(ip)//2
    temp_length = hex(temp_length)
    temp_length = temp_length[2:4]
    temp_length = basic_zero_prefixer(temp_length, 8)
    return(temp_length)


def basic_zero_prefixer(ip, length):
    if(len(ip) != length):
        temp1 = ""
        for x in range(0, length-len(ip)):
            temp1 = temp1+"0"
        ip = temp1+ip
    return(ip)


def get_crc(ip, is_hex=0):
    pac = ip
    if(is_hex == 1):
        packet = ["" for x in range(int(len(pac)/2))]
        for x in range(0, (len(pac)), 2):
            y = int(x/2)
            packet[y] = pac[x:x+2]
        total_bytes = len(packet)
        for x in range(0, total_bytes):
            packet[x] = int(packet[x], 16)
    else:
        packet = pac
        total_bytes = len(packet)
    CRC = 0
    for byte_number in range(0, total_bytes):
        CRC = CRC ^ packet[byte_number]
        for bit_number in range(0, 8):
            carry = CRC & 1
            CRC = CRC >> 1
            if carry == 1:
                CRC = CRC ^ 0xA001
    return(CRC)


# ___________________________________________Utility_______________________________________________________
def make_bytes(ip, packet_type=0):  # 0-byte-array , 1- string , 2- hex string
    if(packet_type == 1):
        ip = get_byte_array(ip, 1)
    elif(packet_type == 2):
        ip = get_byte_array(ip, 2)
    reply = b''
    # print(ip)
    for x in range(0, len(ip)):
        # print(bytes([ip[x]]))
        reply += bytes([ip[x]])
        # print(reply)
    return reply


# default is from encodec bytes, from string use 1 , from hex use 2
def get_byte_array(ip, input_type=0):
    if(input_type == 1):
        reply = [0]*len(ip)
        for x in range(0, len(ip)):
            reply[x] = ord(ip[x])
    elif(input_type == 2):
        reply = [0]*(len(ip)//2)
        for x in range(0, len(ip), 2):
            reply[x//2] = [int(ip[x:x+2], 16)][0]
    elif(input_type == 0):
        reply = [""]*len(ip)
        for x in range(0, len(ip)):
            reply[x] = ord(chr(ip[x]))
    return reply


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


def byte2hex(ip):
    b = ""
    a = ""
    for x in range(0, len(ip)):
        a = str(hex(ip[x])[2:4])
        b = b+a
    return b  # byte array to hex string


def byte2int(a):
    temp1 = ''
    for x in range(0, len(a)):
        a[x] = hex(a[x])
        a[x] = a[x][2:4]
        if(len(a[x]) == 1):
            a[x] = '0'+a[x]
        temp1 = temp1+a[x]
    return (int(temp1, 16))  # byte array to int


def byte2string(ip):
    temp1 = ""
    for x in range(0, len(ip)):
        temp1 += chr(ip[x])
    return temp1

def find_occurance(primary_string,byte_to_find):
    length = len(primary_string)
    found_at=0
    for current_byte in range (0,length):
        # print(primary_string[current_byte])
        if(primary_string[current_byte] == ord(byte_to_find)):
            found_at=current_byte
            break
    return found_at
