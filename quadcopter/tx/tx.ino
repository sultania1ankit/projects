//_____________________________14-1-19______________________________________________
//_____________________________Libraries______________________________________________
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <math.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerifBoldItalic12pt7b.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
RF24 radio(10,9);// calculate no. of flase fail attemmpt count *
//___________________________________________________________________________

//___________________________________________________________________________

//___________________________________________________________________________

//___________________________________________________________________________

void setup_pins()
{
  pinMode(2,INPUT);pinMode(8,INPUT);
  pinMode(A0,INPUT);pinMode(A1,INPUT);
  pinMode(A2,INPUT);pinMode(A3,INPUT);
  digitalWrite(2,HIGH);
  digitalWrite(8,HIGH);
}

void logo()
{
  display.clearDisplay();
  display.setTextSize(1);
display.setTextColor(WHITE);
display.setFont(&FreeSerifBoldItalic12pt7b);
display.setCursor(2,35);
display.print("Cold Mango");
display.display();
//if(digitalRead(2)==1){delay(1800);}
  display.clearDisplay();
  display.setFont();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  }



boolean ib0;boolean ib1; // inputs
unsigned long id1x;unsigned long id0x;
unsigned long id1y;unsigned long id0y;
unsigned long data_s;


byte display_lag;unsigned long t_loop,loop_timer;
unsigned long t_start;

boolean shut_down;boolean start;boolean land;

boolean shutting_down;boolean starting;boolean landing;

int r_pitch;int r_yaw;
int r_roll;int r_height;
int r_battery;
int drone_loop_time;int thrust;

byte packet_number;
int packets_blank;int packets_fail;// packet details
int packets_sent;int packets_rec;
boolean packets_display;unsigned long t_packetdisplay;
int packets_sent_disp;int packets_rec_disp;
int packets_blank_disp;int packets_fail_disp;
boolean start_disp;


int packets_received_there;unsigned long data_r;byte received_packet_no;// received things



void setup_nrf()
{
  radio.begin();
  radio.setChannel(108);
  //radio.setAddressWidth(3);
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  //radio.setRetries(15,1);                 // Smallest time between retries, max no. of retries
  radio.setPayloadSize(4);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.openWritingPipe(0xABCDABCD71LL);        // Both radios listen on the same pipes by default, and switch when writing
  //radio.openReadingPipe(1,pipes[1]);
  radio.stopListening();
  t_packetdisplay=millis();
}

void controller_input()
{

  // A 0 1 2 3 , joystick 
//  leftjy  a1 updown a3 right left
// right jy a2 rightleft a0 updown
// d8 switch jy 2 ,pull it up 
  if(digitalRead(2)==0){ib0=1;}else{ib0=0;}//top
  if(digitalRead(8)==0){ib1=1;}else{ib1=0;}//bottom

  id0x=map(analogRead(A3),0,1023,0,9);//yaw
  id0y=map(analogRead(A1),0,1023,0,9);//height
  id1x=map(analogRead(A2),0,1023,0,60);//roll
  id1y=map(analogRead(A0),0,1023,0,60);//pitch

  if(ib1==1 && id0y<1){land=1;}
  else if(ib1==1 && id0y>8){start=1;start_disp=1;}
  else if(ib0==1 && id1y>55){shut_down=1;}

//  if(ib0==1 && ib1==1 && millis()-t_start>4000)
//  { t_start=millis();start=0;
//    while(digitalRead(2)==0 && digitalRead(8)==0)
//    {if(millis()-t_start>1500){start=1;break;}}}

 }
 
void oled()
{
  display.clearDisplay();
  // sending stuff
  display.setCursor(0,0);
  display.print("P:");display.print(id1y);display.print("/");display.print(r_pitch);
  display.setCursor(0,11);
  display.print("R:");display.print(id1x);display.print("/");display.print(r_roll);
  display.setCursor(0,23);
  display.print("Y:"); display.print(id0x);display.print("/");display.print(r_yaw);
  display.setCursor(0,34);
  display.print("H:");display.print(id0y);display.print("/");display.print(r_height);
  display.setCursor(0,45);
  display.print("T:");display.print(thrust);
  display.setCursor(0,56);
  display.print("Time:");display.print(t_loop);
  display.drawLine(60, 0, 60, 63, WHITE);//___________________________________________
  // received stuff
  display.setCursor(64,0);
  display.print("SR:");display.print(packets_sent_disp);display.print("/");display.print(packets_rec_disp);
  display.setCursor(64,11);
  display.print("BF:");display.print(packets_blank_disp);display.print("/");display.print(packets_fail_disp);
  display.setCursor(64,23);
  display.print("SSL:");display.print(starting);display.print(shutting_down);display.print(landing);
  display.setCursor(64,34);
  
  display.setCursor(64,45);
  display.print("LT:");display.print(drone_loop_time);
  display.setCursor(64,56);
  float battery_display=r_battery;battery_display=battery_display/100;
  display.print("Bat:");display.print(battery_display);

       if(start_disp==1)
  {
  display.clearDisplay();
  display.setCursor(20,30);
  display.print("ACTIVATING");
  display.display();
  display.display();start_disp=0;
  }

  display.display();
}
void Tx()
{    
  if(millis()-t_packetdisplay>1000)
  {
     t_packetdisplay=millis();
//     Serial.print(packets_rec);Serial.print("/");Serial.print(packets_blank);Serial.print("/");Serial.println(packets_sent);
//     Serial.println(fail);
     packets_display=1;}
    packets_sent +=1;
    if (!radio.write( &data_s, 4 )){packets_fail+=1;}
    else{if(!radio.available()){packets_blank +=1;}
      else{while(radio.available() ){packets_rec+=1;
          radio.read( &data_r, 4 );//          Serial.println(gotByte);
}}}}

void packet_reset()
{
  if(packets_display==1)
{
  packets_sent=0;packets_rec=0;packets_blank=0;packets_fail=0;packets_display=0;}
  }

void packet_display_correction()
{
    if(packets_display==1){
    packets_rec_disp=packets_rec;
    packets_sent_disp=packets_sent;
    packets_blank_disp=packets_blank;
    packets_fail_disp=packets_fail;
     packet_reset();}
}

void data_extraction()
{ 
//Serial.println(data_r);
  received_packet_no=data_r/1000000000;
  data_r=data_r%1000000000;
 switch(received_packet_no)
 {
  case 1:
  if(data_r%10==9){
  r_roll=data_r/10000000;data_r=data_r%10000000;
  r_pitch=data_r/100000;data_r=data_r%100000;
  r_yaw=data_r/1000;data_r=data_r%1000;
  shutting_down=data_r/100;data_r=data_r%100;
  landing=data_r/10;data_r=data_r%10;break;}
  else{break;}
  case 2:r_height=data_r/100000;data_r=data_r%100000;
  r_battery=data_r/10;data_r=data_r%10;
  starting=data_r;break;
  case 3:
  if(data_r%10==4)
  {drone_loop_time=data_r/100000;data_r=data_r%100000;
  thrust=data_r/10;data_r=data_r%10;
  break;}
  else{break;}
 }
}
void data_to_transmit()
{ 
  if(shut_down==1){data_s=3124529845;shut_down=0;}
  else if(start==1){data_s=3432654345;start=0;}
  else if(land==1){data_s=3987453653;land=0;}
  else{
  packet_number++;
  if (packet_number==4){packet_number=1;}
  switch(packet_number)
  {
  case 1:data_s=1000000000+(100000000*id0x)+(10000000*id0y)+(100000*id1x)+(1000*id1y)+999;break;
  case 2:data_s=2545545545;
  case 3:data_s=1000000000+(100000000*id0x)+(10000000*id0y)+(100000*id1x)+(1000*id1y)+999;break;
  }}

//  Serial.println(data_s);

}



void setup() {
//Serial.begin(115200);
 setup_pins();
 setup_nrf();
 display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 logo();
}

void loop() {
controller_input();
//if((data_s/1000000000)==3){delay(300);}  // prevent thrust while restarting
data_to_transmit();
Tx();
data_extraction();
packet_display_correction();
display_lag+=1;
t_loop=micros()-loop_timer;loop_timer=micros();
if(display_lag==50){oled();display_lag=0;}

}
