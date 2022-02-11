import datetime
import time
import os
import sys
import socket
import select

host = socket.gethostname()
if(host == "NandiPumps"):
    sys.path.append(os.path.abspath(
        "/home/pi/codes/python_codes/tools/"))
    from terminal_control_tools import *
    from data_manipulation_tools import *
elif(host == "coldlapy"):
    sys.path.append(os.path.abspath(
        "/home/ankit_sultania/codes/python_codes/tools/"))
    from terminal_control_tools import *
    from data_manipulation_tools import *


stdin_timeout = 0.2

header_text = "You little human bitch"
header_text_2 = "HOW HARD IS IT TO REMEMBER ONE THING !!"

HEADER_COLOR = 15
LINES_COLOR = 220

LABELS_COLOR = 178
PARAMETER_NAME_COLOR = 37
PARAMETER_VALUE_COLOR = 75
INFO_COLOR = 228

# Get terminal info
TERMINAL_HEIGHT, TERMINAL_WIDTH = get_terminal_info()
try:
    # Setup terminal
    select_background_color(16)
    terminal_erase(4)
    goto_top()

    # Starting Drawing
    select_text_color(HEADER_COLOR)
    private_mode(1)
    text_mode(2)
    header_length = len(header_text)
    move_cursor_to(int(TERMINAL_HEIGHT/3),
                   int(((TERMINAL_WIDTH-header_length)/2)))
    sys.stdout.write(header_text)
    header_length = len(header_text_2)
    move_cursor_to(int(TERMINAL_HEIGHT/3) + 1,
                   int(((TERMINAL_WIDTH-header_length)/2)))
    sys.stdout.write(header_text_2)
    draw_line_under(int(TERMINAL_HEIGHT/6), LINES_COLOR, TERMINAL_WIDTH)
    draw_line_under(2, LINES_COLOR, TERMINAL_WIDTH)
    move_cursor_to(int(TERMINAL_HEIGHT/3) + int(TERMINAL_HEIGHT/6)+2, 0)
    # wait for keystroke before closing
    ip = input()
except KeyboardInterrupt:
    restore_terminal()
finally:
    # resetup terminal
    restore_terminal()

