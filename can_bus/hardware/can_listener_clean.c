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
//  in esp 21(TX) and  22(RX)  ( Board connections 5(TX) and 4(RX))
can_message_t message;

TaskHandle_t debug_handle= NULL;
TaskHandle_t packet_handle= NULL;


void debug(void *tt1)
{
can_status_info_t bus_status;
while(1)
{
		can_get_status_info(&bus_status);
		vTaskDelay(1000/portTICK_PERIOD_MS);
		printf("$M:%d$\n",bus_status.rx_missed_count);
}
	vTaskDelete( NULL );
}


void get_packet(void *tt2)
{
	while(1)
	{
		can_receive(&message, pdMS_TO_TICKS(2000));
		printf("%03x#", message.identifier);
		for (int i = 0; i < message.data_length_code; i++) 
		{printf("%02x", message.data[i]);}
		printf("\n");
	}
	vTaskDelete( NULL );
}


void app_main()
{
    //Initialize configuration structures using macro initializers
    // can_general_config_t g_config = CAN_GENERAL_CONFIG_DEFAULT(TX_PIN, RX_PIN,CAN_MODE_LISTEN_ONLY);
    can_general_config_t g_config = {CAN_MODE_LISTEN_ONLY,TX_PIN, RX_PIN,-1,-1,0,5,0,0};
    can_timing_config_t t_config = CAN_TIMING_CONFIG_500KBITS();
    can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();
    // can_filter_config_t f_config={.acceptance_code = (0x13C << 21),.acceptance_mask = ~(CAN_STD_ID_MASK << 21),.single_filter = true};

    if (can_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        printf("Driver installed\n"); 
    } else {
        printf("Failed to install driver\n");
        return;
    }

    if (can_start() == ESP_OK) {
        printf("Driver started\n");
    } else {
        printf("Failed to start driver\n");
        return;
    }

	if (can_reconfigure_alerts(CAN_ALERT_ALL, NULL) == ESP_OK) {
	    printf("Alerts reconfigured\n");
	} else {
	    printf("Failed to reconfigure alerts");
	}

	uart_set_baudrate(UART_NUM_0, 921000);
	// uart_set_baudrate(UART_NUM_0, 230400);

	// printf("_____________________________________LOOP_______________________________________________\n");

xTaskCreatePinnedToCore(
  get_packet,               /* Function to implement the task */
  "get_packet",              /* Name of the task */
  10000,                    /*Stack size in words*/ 
  (void*) 0,         /* Task input parameter */
  5,                      /* Priority of the task */
  &packet_handle,                    /* Task handle. */
  1);  

xTaskCreatePinnedToCore(
  debug,               /* Function to implement the task */
  "debug",              /* Name of the task */
  10000,                   /* Stack size in words */
  (void*) 0,         /* Task input parameter */
  0,                      /* Priority of the task */
  &debug_handle,                    /* Task handle. */
  0);                      /* Core where the task should run */
} 


// make queue if nothing works (not sure it will  help ) (** try large buffer is CAN controller )
// print time is  around (150us     180us)
// read time gets as low as 2us ()


// i20 2018 diesel / sunny 