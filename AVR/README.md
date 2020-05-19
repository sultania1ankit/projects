## AVR Walker (Incomplete)

* UART_handle(Python) - Communication,Upload code via Bootloader
* Bootloader - seprate handles for FLASH and EEPROM
* Handles(Functions) for various peripherals 

We will see how the UART_handle can be used for basic debugging and modify fuse bits.\
I have used XC8 compiler, everything has been optimised for speed not for size.\
We will also discuss compiler optimizations for XC8 and makefile concepts.\
Some basic tips for optimised codes for microcontrollers.

We will use ATMEGA328P for this AVR tutorial because . . . ARDUINO 

#### Tools:-

* Linux - Windows user will have to modify the UART_handle,See comments in the same.
* MPLAB v5.30 - Compilier used was XC8 v2.10 
* Python - To build UART_handle because it is the arduino of software.
* 2 Arduino Uno Boards - One will be a AVR-ISP programmer, second one will be the target.We can use the on-board USB to TTL conveter of the second board for communication via UART.
                         You can use any setup having AVR-ISP programmer,ATMEGA328P and a USB to TTL converter and few jumpers.   
                         Using 2 Uno's - 2 USB cables - one ICSP header cable and 1 jumper for reset pin is the best.

#### Future Goals:-

Using these tools to build...
* RTOS for some AVR controller. (Mostly ATMEGA328P)
* Complete Core for ATTINY45. (This might take some time) 


#### Sources:-

* AVR-GCC guide (http://ww1.microchip.com/downloads/en/DeviceDoc/MPLAB_XC8_C_Compiler_User_Guide_for_AVR.pdf)
* XC8 Compiler Guide(http://ww1.microchip.com/downloads/en/devicedoc/50002053g.pdf)
* Microchip developer help (https://microchipdeveloper.com/)

______________________________________________________________________________________________________________________________________________________________________
