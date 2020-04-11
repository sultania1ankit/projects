#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/can.h"
#include "driver/uart.h"


#define TX_PIN 5
#define RX_PIN 4






void app_main()
{

	uart_set_baudrate(UART_NUM_0, 921000);
	can_general_config_t g_config = {CAN_MODE_LISTEN_ONLY,TX_PIN, RX_PIN,-1,-1,0,5,0,0};
	can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();
	// can_timing_config_t t1_config=CAN_TIMING_CONFIG_12_5KBITS();
	// can_timing_config_t t2_config=CAN_TIMING_CONFIG_16KBITS();
	// can_timing_config_t t3_config=CAN_TIMING_CONFIG_20KBITS();
	can_timing_config_t t1_config=CAN_TIMING_CONFIG_25KBITS();
	can_timing_config_t t2_config=CAN_TIMING_CONFIG_50KBITS();
	can_timing_config_t t3_config=CAN_TIMING_CONFIG_100KBITS();
	can_timing_config_t t4_config=CAN_TIMING_CONFIG_125KBITS();
	can_timing_config_t t5_config=CAN_TIMING_CONFIG_250KBITS();
	can_timing_config_t t6_config=CAN_TIMING_CONFIG_500KBITS();
	can_timing_config_t t7_config=CAN_TIMING_CONFIG_800KBITS();
	can_timing_config_t t8_config=CAN_TIMING_CONFIG_1MBITS();
	can_message_t message;
	int bitrate=0;
	while(1)
	{
		switch(bitrate)
		{
		// case 0: printf("12.5KBITS   ->   ");if (can_driver_install(&g_config, &t1_config, &f_config) == ESP_OK) {printf("Driver installed-"); } else {printf("Failed to install driver-");return;}break;
		// case 1: printf("16KBITS   ->   ");  if (can_driver_install(&g_config, &t2_config, &f_config) == ESP_OK) {printf("Driver installed-"); } else {printf("Failed to install driver-");return;}   break;  
		// case 2: printf("20KBITS   ->   ");  if (can_driver_install(&g_config, &t3_config, &f_config) == ESP_OK) {printf("Driver installed-"); } else {printf("Failed to install driver-");return;}   break;    
		case 0: printf("25KBITS     ->   ");  if (can_driver_install(&g_config, &t1_config, &f_config) == ESP_OK) {printf("Driver installed-"); } else {printf("Failed to install driver-");return;}   break;   
		case 1: printf("50KBITS     ->   ");  if (can_driver_install(&g_config, &t2_config, &f_config) == ESP_OK) {printf("Driver installed-"); } else {printf("Failed to install driver-");return;}   break;    
		case 2: printf("100KBITS    ->   "); if (can_driver_install(&g_config, &t3_config, &f_config) == ESP_OK) {printf("Driver installed-"); } else {printf("Failed to install driver-");return;} break;
		case 3: printf("125KBITS    ->   "); if (can_driver_install(&g_config, &t4_config, &f_config) == ESP_OK) {printf("Driver installed-"); } else {printf("Failed to install driver-");return;} break;
		case 4: printf("250KBITS    ->   "); if (can_driver_install(&g_config, &t5_config, &f_config) == ESP_OK) {printf("Driver installed-"); } else {printf("Failed to install driver-");return;} break;
		case 5: printf("500KBITS    ->   "); if (can_driver_install(&g_config, &t6_config, &f_config) == ESP_OK) {printf("Driver installed-"); } else {printf("Failed to install driver-");return;} break;
		case 6: printf("800KBITS    ->   "); if (can_driver_install(&g_config, &t7_config, &f_config) == ESP_OK) {printf("Driver installed-"); } else {printf("Failed to install driver-");return;} break;
		case 7: printf("1000KBITS   ->   "); if (can_driver_install(&g_config, &t8_config, &f_config) == ESP_OK) {printf("Driver installed-"); } else {printf("Failed to install driver-");return;}  break;

		}
		bitrate++;
		if(bitrate==8){bitrate=0;}
				

    if (can_start() == ESP_OK) {printf("Driver started  |  ");} else {printf("Failed to start driver  |  ");return;}

    	
    	can_receive(&message, pdMS_TO_TICKS(300));
		printf("%03x#", message.identifier);
		for (int i = 0; i < message.data_length_code; i++) 
		{printf("%02x", message.data[i]);}
		printf("\n");

	if (can_stop() == ESP_OK) {} else {return;}
	if (can_driver_uninstall() == ESP_OK) {} else {return;}
	// if (can_stop() == ESP_OK) {printf("Driver stopped-");} else {printf("Failed to stop driver");return;}
	// if (can_driver_uninstall() == ESP_OK) {printf("Driver uninstalled\n");} else {printf("Failed to uninstall driver\n");return;}
	vTaskDelay(10/portTICK_PERIOD_MS);
}

}