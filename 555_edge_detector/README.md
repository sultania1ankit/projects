
## __Edge detection using 555 timer__

This circuit is used to generate variable width pulses on two outputs in response to change in input signal.(Refer to the timing diagram below)\
The pulse length of positive edge and negative can be independelty adjusted by changing the 100K resistor or/and 2.2uF capacitor (1.1RC).\
This circuit can also be made using AND gate with delayed inputs.

Some information about car locking mechanism.

> There are various way to lock/unlock a car like using keyfob, button controls on the driver side and by physically moving the lock position on the doors.
Here we are intrested in locking/unlocking by physically moving the locking mechanism on the doors, if you notice carefully all doors except the driver can independently lock/unlock their doors but when the driver locks/unlocks the door all other doors follow the same. As long as the car supports this feature you could do this trick in the car.

The actuator on the driver side has 5 wires instead of 2, the other 3 wires are two microswitches that provide the ECU with the position of the locking mechanism. We will refer to the 2 solenoid/motor wires as actuator wires and the other 3 wires as feedback wires.

### Internal wiring
![alt text](lock_internal_wiring.png "lock_internal_wiring")

We have two options:-
* (Method 1) We can simply apply voltage to the actuator wires in required polarity to lock and unlock the mechanism, tricking the car into thinking that someone has pulled or pushed the lock.
* (Method 2) We can simply control the feedback signals and give the car whatever signal we want. 


> Our goal was to integrate this with telematic devices in such a manner that all available devices in our fleet could provide this feature. So we had to use only one digital output from the device and make the system robust.  

#### Method 1 (Basic block circuit shared)
:red_circle:Needs power connections.\
:red_circle:Prone to failure. (telematic system could power the actuator in one direction while vehicle is doing it in the other directions making a short circuit)\
:white_check_mark:Engages only when required. (If designed accordingly, system failure will not affect functionality of the vehicle)

#### Method 2 (Microcontroller based design)
:red_circle:Full time active system.(If the system fails it will affect the functionalitiy of the vehicle)\
:white_check_mark:No power connections required.\
:white_check_mark:Less points of failure, system could also be made smart to give additional features.


> __The method 2 is very reliable and was used to design the product, you should use a automotive grade low power 8-Bit microcontroller with the bare minimum memory and perpherials. Power conversion circuit should be able to handle fluctuations in input power. logic for the code is preety simple, the designer should make sure the system does not malfunction as it could heavily coufuse the vehicles ECU.__

_Apart from the system malfunctioning there are few special cases that also need to be considered like when the actuator goes bad the ECU can determine the same and ignore input from the feeback mechanism but the systems logic could prevent the ECU from determining that the actuator has failed and malfunction in a different way._  


## Timing
![alt text](timing_diagram.png "Timing Diagram")
## Circuit Design
![alt text](circuit_diagram.png "Circuit Diagram")


____________________________________________________________________________________________________
