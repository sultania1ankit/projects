## Metal Detecting Bot

This bot(toy car) can be controlled by laptop, can avoid some big obstacles and can detect some metallic things.

![image](../images/bot.jpg)

### How Things Work

#### Communication
* The bot starts a Wifi access point
* Connect to this network from your latop (credentials are in the code)
* Run the "Tx_laptop.py" to host a UDP server 
* Bot connects as a UDP Client
>The above flow is correct if the user is either using laptop or external controller's Wifi.
>However if the user wishes to use external controller with NRF24l01, the code for the same has also been added.

#### Distance 

I am using 2 ultrasonic sensors to measure distance on the front and back side of the bot.
if the safety bit has been turned on by the user the bot will try to maintain a safe distance on both sides,
this distance can be changed from the controller due to the fact that ultrasonic sensors are not good at increased 
ranges the max range has been capped at 40cm.

#### Metal Detection.

There is a LC oscillator powered by a 555 timer, the micro controller read the analog voltage in this LC circuit
and filters it to get a number. When a metal is brough close(this is a very basic and bad circuit so metal has to very close)
to the detector coil(inductor) the inductance of the coil changes, changing the LC oscillator's frequeny and the volatge 
read by the micro controller changes.

#### Tips

If you plan to remove this metal detector circuit part and use this bot as a base for for other projects(which is the case with me)
* To drive the wheels solder a small circuit with 2xL293D and SN74HC595N(serial to parallel converter) and write a small 
library for it, this allows use to very easily connect the drive using only 3 wires and you can just call functions to move the bot
which becomes very handy. This save you 5 pins and also save the pain of loose jumper connections.
* Primary battery pack should be high voltage,i am using a LiFePO4 pack but would recommend Li-ion
* Use good motor's and wheels if possible.  


______________________________________________________________________________



