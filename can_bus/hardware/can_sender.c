#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/can.h"
#include "driver/uart.h"
#include "driver/gpio.h"



#define TX_PIN 5
#define RX_PIN 4

TaskHandle_t control_handle= NULL;
TaskHandle_t packet_handle= NULL;

int packet_tx_delay=1000; // in millis
int one_packet=2;

can_message_t message;

// void debug(void *tt1)
// {
// 	while(1)
// 	{
// 		can_status_info_t bus_status;
// 		if(can_get_status_info(&bus_status)== ESP_OK)
// 		{	
// 			printf("Driver State: %d\n",bus_status.state);
// 			printf("Msgs to Transmit: %d\n",bus_status.msgs_to_tx);
// 			printf("Msgs to Receive: %d\n",bus_status.msgs_to_rx);
// 			printf("Tx error: %d\n",bus_status.tx_error_counter);
// 			printf("Rx error: %d\n",bus_status.rx_error_counter);
// 			printf("Tx Failed: %d\n",bus_status.tx_failed_count);
// 			printf("Rx Missed: %d\n",bus_status.rx_missed_count);
// 			printf("Arb Lost: %d\n",bus_status.arb_lost_count);
// 			printf("Bus Error: %d\n",bus_status.bus_error_count);
// 		}
    	

// 		uint32_t alerts_triggered;
// 		if(can_read_alerts(&alerts_triggered, pdMS_TO_TICKS(300))==ESP_OK)
// 		{printf("Alerts Found\n");
// 		printf("Alert  %d\n",alerts_triggered);}
// 		else{printf("No Alerts Found\n");}
// 	}
// 	vTaskDelete( NULL );
// }



// CAN_MSG_FLAG_EXTD         Message is in Extended Frame Format (29bit ID)
// CAN_MSG_FLAG_RTR          Message is a Remote Transmit Request
// CAN_MSG_FLAG_SS           Transmit message using Single Shot Transmission (Message will not be retransmitted upon error or loss of arbitration). Unused for received message.
// CAN_MSG_FLAG_SELF         Transmit message using Self Reception Request (Transmitted message will also received by the same node). Unused for received message.
// CAN_MSG_FLAG_DLC_NON_COMP Message’s Data length code is larger than 8. This will break compliance with CAN2.0B
// CAN_MSG_FLAG_NONE         Clears all bit fields. Equivalent to a Standard Frame Format (11bit ID) Data Frame.

void control(void *tt2)
{
    char a0[20],a1[20];
    // char *p0=&a0;
	while(1)
	{
		vTaskDelay(10/portTICK_PERIOD_MS);
        // scanf("%s",p0);
        gets(a0);
        int count=0;for (int i=0;i<20;i++){if(a0[i]==a1[i]){count++;}}
        // printf("count is %d\n",count);
        if(count!=20)
        {
        	
        	printf("Echo:%s\n",a0);


	       	if(a0[0]=='s')
       		{one_packet=a0[1]-'0';printf("Sending type is %d\n",one_packet);}
       		else if(a0[0]=='h')
       		{char temp_head[8];
       		for (int i=1;i<9;i++)
       		{ temp_head[i-1]=a0[i];}
       		 message.identifier=(int)strtol(temp_head, NULL, 16);printf("CAN ID is %x\n",message.identifier);
       		}
       		else if(a0[0]=='l')
       		{message.data_length_code=a0[1]-'0';printf("DLC is %d\n",message.data_length_code);}
   			else if(a0[0]=='t')
       		{packet_tx_delay=a0[4]-'0';packet_tx_delay+=10*(a0[3]-'0');packet_tx_delay+=100*(a0[2]-'0');packet_tx_delay+=1000*(a0[1]-'0');printf("Tx Delay is %d",packet_tx_delay);}
   			else if(a0[0]=='b')
       		{ char temp_body[message.data_length_code][2];
       			for(int i=0;i<message.data_length_code;i++)
       			{
       				for (int j=0;j<2;j++)
       				{
       					temp_body[i][j]=a0[(i+i)+j+1];
       				}
       				message.data[i]=(int)strtol(temp_body[i], NULL, 16);
       			}
       			printf("Body is ");
       			for(int i=0;i<message.data_length_code;i++)
   				{printf("%x ",message.data[i]);}

       		}


        else if(a0[0]=='f')
        {
          if(a0[1]=='1'){message.flags=CAN_MSG_FLAG_NONE;}
          if(a0[1]=='2'){message.flags=CAN_MSG_FLAG_EXTD;}
          if(a0[1]=='3'){message.flags=CAN_MSG_FLAG_SS;}
          if(a0[1]=='4'){message.flags=CAN_MSG_FLAG_RTR;}
          if(a0[1]=='5'){message.flags=CAN_MSG_FLAG_DLC_NON_COMP;}
        }
   			else if(a0[0]=='d')
       		{

        printf("\n%x  -  ",message.identifier);
        for(int i=0;i<message.data_length_code;i++){printf("%x ",message.data[i]);}
        printf("  |  ");
        if(message.flags==CAN_MSG_FLAG_NONE)        {     printf("NONE  ");}
        else if(message.flags==CAN_MSG_FLAG_EXTD)        {printf("EXTD  ");}
        else if(message.flags==CAN_MSG_FLAG_SS)          {printf("SS    ");}
        else if(message.flags==CAN_MSG_FLAG_RTR)         {printf("RTR   ");}
        else if(message.flags==CAN_MSG_FLAG_DLC_NON_COMP){printf("NO_DLC");}
        printf("  -> %dms.\n",packet_tx_delay);

        printf("Command         Range\n");
        printf("Header(h):      00000000-1FFFFFFF (Flag dependent)\n");
        printf("DLC(l):         0-8 \n");
        printf("Tx Delay(t):    0000-9999ms (*Use 4 Digits)\n");
        printf("Body(b):        00-ff\n");
        printf("Flag(f):        1-5  None/Ext/SS/RTR/NO_DLC\n");
        printf("Send(s):        1(Stop)-2(One packet)-3(Continous).  *SS is different\n");
        printf("Press 'd' to display this ^\n\n");
       		}


       	else if(a0[0]=='r')  // to enable single packet tx allowing file replay
       	{
       		one_packet=1;
       		char temp_head[8];
       		for (int i=1;i<9;i++)
       		{ temp_head[i-1]=a0[i];}
       		message.identifier=(int)strtol(temp_head, NULL, 16);
       		message.data_length_code=a0[10]-'0';
       		char temp_body[message.data_length_code][2];
       			for(int i=0;i<message.data_length_code;i++)
       			{
       				for (int j=0;j<2;j++)
       				{
       					temp_body[i][j]=a0[(i+i)+j+12];
       				}
       				message.data[i]=(int)strtol(temp_body[i], NULL, 16);
       			}
       		if(a0[9]=='#' && a0[11]=='#'){one_packet=2;}

       	}



   			// else if(a0[0]=='')
       		// {}
        }
       




    for (int i=0;i<20;i++){a1[i]=a0[i];}
	}
	vTaskDelete( NULL );
}



void send_packet(void *tt3)
{	
	while(1)
	{

  // 18db33f1#0201000000000000  ->   18daf110#064100be3eb81355

  // message.identifier = 0x294;
  // message.data_length_code = 8;
  // message.data[0]=0x00;
  // message.data[1]=0x16;
  // message.data[2]=0x00;
  // message.data[3]=0x00;
  // message.data[4]=0x9c;
  // message.data[5]=0x40;
  // message.data[6]=0x00;
  // for (int z=0;z<4;z++)
  // {
  //   if(z==0){message.data[7]=0x6;}
  //   else if(z==1){message.data[7]=0x15;}
  //     else if(z==2){message.data[7]=0x24;}
  //       else if(z==3){message.data[7]=0x33;}
  



		if(one_packet==2){
			if (can_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {printf("%x-",message.identifier);for(int i=0;i<message.data_length_code;i++){printf("%x ",message.data[i]);}printf("\n");} 
			else{printf("Failed to queue message for transmission\n");}
		one_packet=1;}
		
		if(one_packet==3){
    if (can_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {printf("%x-",message.identifier);for(int i=0;i<message.data_length_code;i++){printf("%x ",message.data[i]);}printf("\n");} 
		else{printf("Failed to queue message for transmission\n");}
		}

		vTaskDelay(packet_tx_delay/portTICK_PERIOD_MS);
		// }
	}
	vTaskDelete( NULL );
}














void app_main()
{
    //Initialize configuration structures using macro initializers
    can_general_config_t g_config = CAN_GENERAL_CONFIG_DEFAULT(TX_PIN, RX_PIN, CAN_MODE_NO_ACK);
    can_timing_config_t t_config = CAN_TIMING_CONFIG_500KBITS();
    can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();
    // can_filter_config_t f_config={.acceptance_code = (0x7DF << 21),.acceptance_mask = ~(CAN_STD_ID_MASK << 21),.single_filter = true};


    //Install CAN driver
    if (can_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        printf("Driver installed\n");
    } else {
        printf("Failed to install driver\n");
        return;
    }

    //Start CAN driver
    if (can_start() == ESP_OK) {
        printf("Driver started\n");
    } else {
        printf("Failed to start driver\n");
        return;
    }


 //    uint32_t alerts_needed = CAN_ALERT_ALL ;
	// if (can_reconfigure_alerts(alerts_needed, NULL) == ESP_OK) {
	//     printf("Alerts reconfigured\n");
	// } else {
	//     printf("Failed to reconfigure alerts");
	// }

	// uart_set_baudrate(UART_NUM_0, 921000);

	// printf("_____________________________LOOP__________________________________\n");


    message.identifier=0xabc;
    message.data_length_code=1;
    message.data[0]=0xaa;
    message.flags=CAN_MSG_FLAG_EXTD;




        printf("\n%x  -  ",message.identifier);
        for(int i=0;i<message.data_length_code;i++){printf("%x ",message.data[i]);}
        printf("  |  ");
        if(message.flags==CAN_MSG_FLAG_NONE)        {     printf("NONE  ");}
        else if(message.flags==CAN_MSG_FLAG_EXTD)        {printf("EXTD  ");}
        else if(message.flags==CAN_MSG_FLAG_SS)          {printf("SS    ");}
        else if(message.flags==CAN_MSG_FLAG_RTR)         {printf("RTR   ");}
        else if(message.flags==CAN_MSG_FLAG_DLC_NON_COMP){printf("NO_DLC");}
        printf("  -> %dms.\n",packet_tx_delay);

        printf("Command         Range\n");
        printf("Header(h):      00000000-1FFFFFFF (Flag dependent)\n");
        printf("DLC(l):         0-8 \n");
        printf("Tx Delay(t):    0000-9999ms (*Use 4 Digits)\n");
        printf("Body(b):        00-ff\n");
        printf("Flag(f):        1-5  None/Ext/SS/RTR/NO_DLC\n");
        printf("Send(s):        1(Stop)-2(One packet)-3(Continous).  *SS is different\n");
        printf("Press 'd' to display this ^\n\n");




	xTaskCreatePinnedToCore(
	  control,               /* Function to implement the task */
	  "control",              /* Name of the task */
	  10000,                    /*Stack size in words*/ 
	  (void*) 0,         /* Task input parameter */
	  5,                      /* Priority of the task */
	  &control_handle,                    /* Task handle. */
	  0);  

	  xTaskCreatePinnedToCore(
	  send_packet,               /* Function to implement the task */
	  "send_packet",              /* Name of the task */
	  10000,                    /*Stack size in words*/ 
	  (void*) 0,         /* Task input parameter */
	  5,                      /* Priority of the task */
	  &packet_handle,                    /* Task handle. */
	  1);  

}
