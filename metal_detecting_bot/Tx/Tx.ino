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
RF24 radio(10,9);// calculate no. of flase fail attemmpt count 
 
#define rx A2 // pinnumbers for inputs
#define ry A0
#define lx A3
#define ly A1
#define rb 8
#define lb 2

//LiquidCrystal lcd(3, 4, 5, 6, 7, 8); 8 reused 4th might be used by oled (not sure)
// new variables (do not delete)

boolean ib0;boolean ib1; // inputs
byte id1;byte id0; 
byte data_input;  

byte display_lag;boolean extra_display; // display 
String op_l;String op_r;unsigned long t_loop;

int packets_blank;int packets_fail;// packet details
int packets_sent;int packets_rec;
boolean packets_display;unsigned long t_packetdisplay;
int packets_sent_disp;int packets_rec_disp;
int packets_blank_disp;int packets_fail_disp;

int packets_received_there;unsigned int data_r;byte received_packet_no;// received things
byte safe_distance_received;boolean enable_distance_received;
byte distance_front;byte distance_back;

byte safe_distance;boolean enable_distance;byte tr;// transmitted things
unsigned long t_buttondelay1;unsigned long t_buttondelay2;
unsigned int data_s;

float v_lifepo4;float v_buckop;byte time_carloop; // custom_setup
byte wdt_count;byte car_reset_reason;byte car_error_loop;
//byte packet_no;

//variables to be deleted



void setup() {
  //Serial.begin(115200);
  pinMode(2,INPUT);pinMode(8,INPUT);
  pinMode(A0,INPUT);pinMode(A1,INPUT);
  pinMode(A2,INPUT);pinMode(A3,INPUT);
  digitalWrite(2,HIGH);
  digitalWrite(8,HIGH);
  radio.begin();
  radio.setChannel(108);
  //radio.setAddressWidth(3);
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  //radio.setRetries(15,1);                 // Smallest time between retries, max no. of retries
  radio.setPayloadSize(2);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.openWritingPipe(0xABCDABCD71LL);        // Both radios listen on the same pipes by default, and switch when writing
  //radio.openReadingPipe(1,pipes[1]);
  radio.stopListening();
  t_packetdisplay=millis();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  logo();
  initial_message();  
  }

void loop()
{
controller_input();
data_to_transmit();
Tx();
data_extraction();
packet_display_correction();
display_lag+=1;
t_loop=millis()-t_loop;
if(display_lag==50){oled();display_lag=0;}
t_loop=millis();
}
// for car direction , turning mode, safe_distance

