import sys
import os
import subprocess
import time

# def mouse_tracking:
# def beeps:

#check if font size can be changed

def get_terminal_info():
    """ Get teminal dimensions from system"""
    subprocess1 = subprocess.Popen("tput lines", shell=True, stdout=subprocess.PIPE)
    height = subprocess1.stdout.read()
    subprocess2 = subprocess.Popen("tput cols", shell=True, stdout=subprocess.PIPE)
    width = subprocess2.stdout.read()
    height = int(height.decode("utf-8"))
    width = int(width.decode("utf-8"))
    return height,width

def display_head(head,color,width):
    """Display header message"""
    header_length = len(head)
    select_background_color(16)
    terminal_erase(4)
    goto_top()
    select_text_color(color)
    private_mode(1)
    text_mode(6)
    move_cursor_to(1,int(((width-header_length)/2)))
    sys.stdout.write(head)

def display_info_message(message,color,width):
    """ Some information to be displayed"""
    text_mode(1)
    select_background_color(16)
    select_text_color(color)
    text_mode(4)
    cursor_navigate(5,2)
    terminal_erase(5)
    sys.stdout.write(message)

def display_message_at(message,color,width,line):
    """ Some information to be displayed"""
    text_mode(1)
    select_background_color(16)
    select_text_color(color)
    text_mode(4)
    move_cursor_to(line,0)
    terminal_erase(5)
    sys.stdout.write(message)
    text_mode(1)

def display_commands(commands,width):
    """ keystroke command """
    text_mode(1)
    select_background_color(16)
    select_text_color(69)
    text_mode(2)
    cursor_navigate(5, 2)
    sys.stdout.write(commands)
    text_mode(1)

def draw_line_under(lines_gap,color,width):
    """draw dotted line "lines_gap" under current line"""
    select_text_color(color)
    sys.stdout.write("\u001b["+str(lines_gap)+"E")
    for temp1 in range(0, width):
        sys.stdout.write("_")


def draw_line_on_row(row_number,color, width):
    """Draw dotted line at row number provided"""
    goto_top()
    select_text_color(color)
    sys.stdout.write("\u001b["+str(row_number)+"E")
    for temp1 in range(0, width):
        sys.stdout.write("_")

def select_text_color(color_code):
    """Text Color"""
    sys.stdout.write("\x1b[38;5;"+str(color_code)+"m")


def select_background_color(color_code):
    """Background Color"""
    sys.stdout.write("\x1b[48;5;"+str(color_code)+"m")


def terminal_erase(code):
    """
    1-clears the screen\n
    2-clears from cursor until end of screen\n
    3-clears from cursor to beginning of screen\n
    4-clears entire screen\n
    5-clears the current line\n
    6-clears from cursor to end of line\n
    7-clears from cursor to start of line\n
    8-clears entire line\n
    """
    if(code== 1):
        cmd="J"	# clears the screen
    if(code== 2):
        cmd="0J"	# clears from cursor until end of screen
    if(code== 3):
        cmd="1J"	# clears from cursor to beginning of screen
    if(code== 4):
        cmd="2J"	# clears entire screen
    if(code== 5):
        cmd="K"	# clears the current line
    if(code== 6):
        cmd="0K"	# clears from cursor to end of line
    if(code== 7):
        cmd="1K"	# clears from cursor to start of line
    if(code== 8):
        cmd="2K"	# clears entire line
    sys.stdout.write("\x1b["+cmd)


def move_cursor_to(x,y):
    """Move to X,Y location"""
    sys.stdout.write("\u001b["+str(x)+";"+str(y)+"H")

def cursor_navigate(code,value):
    """
    1-moves cursor up # lines\n
    2-moves cursor down # lines\n
    3-moves cursor right # columns\n
    4-moves cursor left # columns\n
    5-moves cursor to beginning of next line, # lines down\n
    6-moves cursor to beginning of previous line, # lines down\n
    7-moves cursor to column #\n
    """
    if(code==1):
        cmd="A" #moves cursor up # lines
    if(code==2):
        cmd="B" #moves cursor down # lines
    if(code==3):
        cmd="C" #moves cursor right # columns
    if(code==4):
        cmd="D" #moves cursor left # columns
    if(code==5):
        cmd="E" #moves cursor to beginning of next line, # lines down
    if(code==6):
        cmd="F" #moves cursor to beginning of previous line, # lines down
    if(code==7):
        cmd="G" #moves cursor to column #

                # "6n"	request cursor position (reports as ESC[#;#R)
                # "s"	save cursor position
                # "u"	restores the cursor to the last saved position

    sys.stdout.write("\x1b["+str(value)+cmd)



def goto_top():
    """ Go to start"""
    sys.stdout.write('\x1b[H')

def private_mode(code):
    """
	1-make cursor invisible\n
	2-make cursor visible\n
	3-restore screen\n
	4-save screen\n
	5-enables the alternative buffer\n
	6-disables the alternative buffer\n
    """
    if(code==1):
        cmd="?25l"  #	make cursor invisible
    if(code==2):
        cmd="?25h"  #	make cursor visible
    if(code==3):
        cmd="?47l"  #	restore screen
    if(code==4):
        cmd="?47h"  #	save screen
    if(code==5):
        cmd="?1049h"    #	enables the alternative buffer
    if(code==6):
        cmd="?1049l"    #	disables the alternative buffer
    sys.stdout.write("\x1b["+cmd)

def text_mode(code):
    """
    1-reset all\n
    2-bold mode.\n
    3-dim/faint mode.\n
    4-italic mode.\n
    5-underline mode.\n
    6-blinking mode\n
    7-inverse/reverse mode\n
    8-invisible mode\n
    9-strikethrough mode\n
    """
    # ESC[1;34;{...}m	Set graphics modes for cell, separated by semicolon (;).
    if(code==1):
        cmd="0m" #	reset all modes (styles and colors)
    if(code==2):
        cmd="1m" #	set bold mode.
    if(code==3):
        cmd="2m" #	set dim/faint mode.
    if(code==4):
        cmd="3m" #	set italic mode.
    if(code==5):
        cmd="4m" #	set underline mode.
    if(code==6):
        cmd="5m" #	set blinking mode
    if(code==7):
        cmd="7m" #	set inverse/reverse mode
    if(code==8):
        cmd="8m" #	set invisible mode
    if(code==9):
        cmd="9m" #	set strikethrough mode.
    sys.stdout.write("\x1b["+cmd)

def restore_terminal():
    """ To be used when ending the function.\n 
    Used in KeyboardInterrupt exception"""
    text_mode(1)
    private_mode(2)
    goto_top()
    terminal_erase(4)
    sys.stdout.flush()


#---------------- These are second order tools ------------------
