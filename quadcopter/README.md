## Basic quadcopter

[quadcopter](https://youtu.be/Zgv_2BgS3I4 "Quadcopter Demo")

#### Suggestion

>Learn how quadcopters work and what components are needed to build a basic one, how those components work 
>and about the control logic (PID).I will not be doing that here as there are a lot of good tutorial out there.
>Try to write the code, its fun and rewarding.

This quadcopter can fly in acro-mode(working on angle mode), uses NRF24L01 for communication (so \~100m Range)
has few-failsafe.

This is not the final code (Lost it :-(  ), if something does not work let me know i will fix it,
will fix things when i get time.

### Upgrades
* Use both ESP32 cores(I am doing a whole rework in ESP-IDF)
* filters for the MPU, try using kalman.It is slow but it can and should be used.
* Good constant's 
	* MPU filteration
	* PID values for pitch,yaw and roll
	* BMP280 filters and thresholds
	* loop time related
* Auto-landing (Tx lost, battery low, out of control).
* altitude hold.







### Tips
* Don't use PS2 joysticks to fly a quadcopter.
* Don't use a very small battery or have multiple battries with you, as you will need a lot of time to tune
and even more time to figure out your mistakes if you are doing something wrong.