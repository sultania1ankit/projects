### Controller Area Network

This is for someone who wants to read CAN data from vehicles and analyse it.\
I have used ESP-IDF for ESP32 programming and the other codes are in python and have been tested in Linux.

#### Content:-
Hardware to read and write CAN packets(25Kbps to 1Mbps).\
Log the messages and replay them later.\
Analyse the data with help of some masks.\
ELM327 help tool.
____________________________________________________________________________________________________________________

Go to hardware folder and follow the steps to make sure that you are getting data in "vcan.log".(There is a sample vcan.log file attached)
 
* Now "sudo python3 coca.py vcan.log".
* You can change the file name from vcan.log and later use "coca.py new_name".
* you should see someting like "raw_coca.png".
* "binary_can_analyser.py" is a socketcan compatible tool so you will need to converter this file by removeing 5 zeros as stated in hardware folder.

#### ELM327

ELM327 is used to query OBD PIDS from the ECU via OBD port.(The cheap variants do not comply with the whole command set and hence cannot transmit arbitrary messages)
It is better to use "can_sender.c" to send mesages on the CAN network.

* Use bluetoothctl to establish a connection with the ELM.
* run "elm327_obd.py" to get a csv file with PID and raw values.
* run "elm_viewer.py" to add labels and ranges to the csv file. (Samples are provided)

_____________________________________________________________________________________________________________________

