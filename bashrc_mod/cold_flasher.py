import os
import sys

mcu=sys.argv[1]
project=sys.argv[2]

avr_try="avrdude -v -p m328p -b 19200 -c avrisp -P /dev/ttyACM0 -U flash:w:/home/ankit_sultania/MPLABXProjects/RTOS_try1.X/dist/default/production/RTOS_try1.X.production.hex"
avr_main="avrdude -v -p m328p -b 19200 -c avrisp -P /dev/ttyACM0 -U flash:w:/home/ankit_sultania/MPLABXProjects/m328_tester.X/dist/default/production/m328_tester.X.production.hex"
avr_boot="avrdude -v -p m328p -b 19200 -c avrisp -P /dev/ttyACM0 -U flash:w:/home/ankit_sultania/MPLABXProjects/boot_loader.X/dist/default/production/boot_loader.X.production.hex"

#STM 32 skilancer folder
stm_flash_DF_pre_production="st-flash write /home/ankit_sultania/STM32CubeIDE/skilancer/DF_pre_production/Debug/DF_pre_production.bin 0x8000000"
stm_flash_Model_A="st-flash write /home/ankit_sultania/STM32CubeIDE/skilancer/Model_A/Debug/Model_A.bin 0x8000000"
stm_flash_Model_B="st-flash write /home/ankit_sultania/STM32CubeIDE/skilancer/Model_B/Debug/Model_B.bin 0x8000000"
stm_flash_Model_B_22="st-flash write /home/ankit_sultania/STM32CubeIDE/skilancer/Model_B_22/Debug/Model_B_22.bin 0x8000000"
stm_flash_Model_B_31="st-flash write /home/ankit_sultania/STM32CubeIDE/skilancer/Model_B_31/Debug/Model_B_31.bin 0x8000000"
stm_flash_Model_C="st-flash write /home/ankit_sultania/STM32CubeIDE/skilancer/Model_C/Debug/Model_C.bin 0x8000000"
stm_flash_Model_D_05="st-flash write /home/ankit_sultania/STM32CubeIDE/skilancer/Model_D_05/Debug/Model_D_05.bin 0x8000000"



if(mcu=="STM32F"):
	if(project=="A"):
		os.system(stm_flash_Model_A)
	elif(project=="B"):
		os.system(stm_flash_Model_B)
	elif(project=="B_22"):
		os.system(stm_flash_Model_B_22)
	elif(project=="B_31"):
		os.system(stm_flash_Model_B_31)
	elif(project=="C"):
		os.system(stm_flash_Model_C)
	else:
		sys.stdout.write("No project named \""+str(project)+"\" is associated with MCU \""+str(mcu)+"\" .\n")
		sys.stdout.flush()

elif(mcu=="STM32L"):
	if(project=="DF_pre"):
		os.system(stm_flash_Model_A)
	elif(project=="DF_05"):
		os.system(stm_flash_Model_B)
	else:
		sys.stdout.write("No project named \""+str(project)+"\" is associated with MCU \""+str(mcu)+"\" .\n")
		sys.stdout.flush()
elif(mcu=="STM8L"):
	sys.stdout.write("No projects found.\n")
	sys.stdout.flush()
else:
	sys.stdout.write("MCU not registered.\n")
	sys.stdout.flush()



# avrdude -v -p m328p -b 19200 -c avrisp -P /dev/ttyACM0 -U flash:w:ssd1306_128x64_i2c.ino.hex 




	