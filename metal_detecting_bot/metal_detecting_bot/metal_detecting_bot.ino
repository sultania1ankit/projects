#include <M_shield.h>
M_shield m(12,14,27);

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiAP.h>


const char * ssid = "coldspot";
const char * pwd = "11111113";
//const char * udpAddress = "169.254.251.47";

const char * udpAddress = "192.168.4.2";
const char* assid = "Regera";
const char* asecret = "11111114";
const int udpPort = 44444;
WiFiUDP udp;

unsigned long loop_time_tracker;unsigned long loop_time;
unsigned long one_second;
byte data_s[10];char *data_r;
unsigned int data_received;
unsigned long time_last_input;boolean communication_out;
unsigned long packet_compensation_time;
unsigned int packets_received_counter;unsigned int packets_received;
unsigned int packets_sent;unsigned int packets_sent_counter;
unsigned long data_packet1;unsigned long data_packet2;unsigned long data_packet3;
//______________________________________________________
byte turn_mode;byte movement;


//______________________________________________________

#define battery_pin 34 //13 is from ADC channel 2 , used by wifi also 
float battery_voltage;
float battery_voltage_sum;
int battery_read_reducer;
int battery_avg_factor;
//5.72k    3.66

//______________________________________________________
int metal_read;int metal_read_average;int metal_value;byte metal_average_counter;
#define metal_pin 35


//______________________________________________________
#define trigpin2 0
#define echopin2 15
#define trigpin 26
#define echopin 25
unsigned long t_avoid_ultra_error;unsigned long distance_front;
boolean distance_invalid;unsigned long distance_back;
byte safe_distance;boolean distance_safety_enabled;
boolean selfdrive_token_front;boolean selfdrive_token_back;
unsigned int previous_back_distance;unsigned int previous_front_distance; // if safe_distance>distance  .. previous ++ ---else preivous=0 
//______________________________________________________



//______________________________________________________

void setup() {
Serial.begin(115200);
m.mno();
//connectToWifi();
create_ap();
udp.begin(udpPort);
one_second=millis();
m.mno();
pinMode(echopin,INPUT);
pinMode(echopin2,INPUT);
pinMode(trigpin,OUTPUT);
pinMode(trigpin2,OUTPUT);
pinMode(battery_pin,INPUT);
pinMode(metal_pin,INPUT);
analogReadResolution(18);
//analogReadResolution(10);
}

void connectToWifi()
{
WiFi.begin(ssid, pwd);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
void create_ap()
{
    WiFi.mode(WIFI_AP);
 

  //access point part
  Serial.println("Creating Accesspoint");
  WiFi.softAP(assid,asecret,7,0,5);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());
}


  void udp_send()
  {
  uint8_t buffer[11] = {data_s[1],data_s[2],data_s[3],data_s[4],data_s[5],data_s[6],data_s[7],data_s[8],data_s[9],data_s[10],data_s[11]};
  udp.beginPacket(udpAddress, udpPort);
  udp.write(buffer,11);
  udp.endPacket();
  packets_sent_counter+=1;
  }

  
  void udp_receive()
  { 
    uint8_t buffer[50];
     memset(buffer, 0, 50);//processing incoming packet, must be called before reading the buffer
  udp.parsePacket();//receive response from server
  if(udp.read(buffer, 50) > 0){
//    Serial.print("Received: ");
    data_r=(char *)buffer;
    time_last_input=millis();
    packets_received_counter += 1;
//    Serial.println("Tried to Receive");
//    Serial.print(data_r[0]);Serial.print(data_r[1]);Serial.print(data_r[2]);Serial.print(data_r[3]);Serial.println(data_r[4]);
    data_received=10000*(data_r[0]-'0')+1000*(data_r[1]-'0')+100*(data_r[2]-'0')+10*(data_r[3]-'0')+(data_r[4]-'0');
  } 
  }

void  process_input()
{

 if((data_received/10000) == 1 )
 { if(data_received%10==1){if(packet_compensation_time==0){packet_compensation_time=millis();}}
  else {packet_compensation_time=0;}}
 if(packet_compensation_time==0)
{
 if(data_received/10000 == 1){
 turn_mode=(data_received/1000)%10;
 movement=data_received%10;}
}
else if(millis()-packet_compensation_time>70){movement=1;}

if((data_received/10000) ==2)
{
  safe_distance=data_received%100;
  distance_safety_enabled=(data_received/100)%10;
}

}

void basic_drive()
{
  // turn_mode 0-1-2     rotate-turn-tend
  // movement 1-2-3-4-5  stop-forward-back-right-left
  switch(turn_mode)
 {
  case 0:
  switch(movement)
  {
    case 1:m.mno();break;
    case 2:m.mf();break;
    case 3:m.mb();break;
    case 4:m.mrr();break;
    case 5:m.mrl();break;
  }break;
    case 1:
  switch(movement)
  {
    case 1:m.mno();break;
    case 2:m.mf();break;
    case 3:m.mb();break;
    case 4:m.mr();break;
    case 5:m.ml();break;
  }break;
    case 2:
  switch(movement)
  {
    case 1:m.mno();break;
    case 2:m.mf();break;
    case 3:m.mb();break;
    case 4:m.mtr();break;
    case 5:m.mtl();break;
  }break;
 }
}

void safe_drive()
{
  // turn_mode 0-1-2     rotate-turn-tend
  // movement 1-2-3-4-5  stop-forward-back-right-left
  if(movement==4 || movement==5)
  {
    basic_drive();
  }
  else if(selfdrive_token_front==0 && selfdrive_token_back==0){basic_drive();}
  else if(selfdrive_token_front==1 && selfdrive_token_back==1){m.mno();}
  else if(selfdrive_token_front==1 && selfdrive_token_back==0){m.mb();}
  else if(selfdrive_token_front==0 && selfdrive_token_back==1){m.mf();}

}
void display_parameters()
{
  Serial.print("Communication out:   ");Serial.println(communication_out);
  Serial.print("Data Received:  ");Serial.println(data_received);
  Serial.print("Data sent:  ");Serial.print(data_packet1);Serial.print("       ");Serial.print(data_packet2);Serial.print("       ");Serial.println(data_packet3);
  Serial.print("Loop time: ");Serial.println(loop_time);
  Serial.print("Turn Mode: ");Serial.println(turn_mode);
  Serial.print("Movement: ");Serial.println(movement);
  Serial.print("Packets received: ");Serial.println(packets_received);
  Serial.print("packets Sent:  ");Serial.println(packets_sent);
  Serial.print("Distance Front:   ");Serial.println(distance_front);
  Serial.print("Distance Back:   ");Serial.println(distance_back);
  Serial.print("Battery Voltage: ");Serial.println(battery_voltage);
  Serial.print("Safe Distance: ");Serial.println(safe_distance);
  Serial.print("Distance safety Enabled: ");Serial.println(distance_safety_enabled);
  Serial.print("Metal Value:  ");Serial.println(metal_value);
Serial.println("_______________________________________________________________________________________");
  
}

void each_second()
{ 
  if(millis()-one_second>1000)
  { 
    one_second=millis();
    packets_received=packets_received_counter;
    packets_received_counter=0;
    packets_sent=packets_sent_counter;
    packets_sent_counter=0;
  }
}

void prepare_packet()
{ 
  data_packet1=packets_received+1000*packets_sent;
//  Serial.println(data_packet1);
  data_packet2=100*battery_voltage;
//  Serial.println(data_packet2);
  data_s[1]=distance_front;
  data_s[2]=data_packet1%100;
  data_s[3]=(data_packet1/100)%100;
  data_s[4]=data_packet1/10000;
  data_s[5]=data_packet2%100;
  data_s[6]=(data_packet2/100)%100;
  data_s[7]=metal_read%100;
  data_s[11]=metal_read/100;
  data_s[8]=distance_front;
  data_s[9]=distance_back;
  data_s[10]=safe_distance+100*(1+distance_safety_enabled);
}

void read_battery()
{
//  Serial.println(analogRead(battery_pin));
  battery_read_reducer++;
  if(battery_read_reducer==10)
  {
    battery_voltage_sum+=analogRead(battery_pin);
    battery_avg_factor++;
    battery_read_reducer=0;
    if(battery_avg_factor==10)
    {
      battery_voltage=battery_voltage_sum/10;
      battery_voltage=(battery_voltage/4096)*5.72*3.55;
//      Serial.println(battery_voltage);
      battery_voltage_sum=0;
      battery_avg_factor=0;
    }
  }
}

void get_distance_front()
{
distance_invalid=0;
digitalWrite(trigpin,LOW);delayMicroseconds(2);
digitalWrite(trigpin,HIGH);delayMicroseconds(10);
digitalWrite(trigpin,LOW);
t_avoid_ultra_error=millis();
while(digitalRead(echopin)==0){if(millis()-t_avoid_ultra_error>1){distance_invalid=1;break;}}
if(distance_invalid==0)
{
distance_front=micros();
while (digitalRead(echopin)==1){if(micros()-distance_front>2380){break;}}
distance_front=micros()-distance_front;
distance_front=distance_front*0.034/2;}
//Serial.print("Distance Front:   ");Serial.println(distance_front);
}

void get_distance_back()
{
distance_invalid=0;
digitalWrite(trigpin2,LOW);delayMicroseconds(2);
digitalWrite(trigpin2,HIGH);delayMicroseconds(10);
digitalWrite(trigpin2,LOW);
t_avoid_ultra_error=millis();
while(digitalRead(echopin2)==0){if(millis()-t_avoid_ultra_error>1){distance_invalid=1;break;}}
if(distance_invalid==0)
{
distance_back=micros();
while (digitalRead(echopin2)==1){if(micros()-distance_back>2380){break;}}
distance_back=micros()-distance_back;
distance_back=distance_back*0.034/2;}
//Serial.print("Distance Back:   ");Serial.println(distance_back);
}

void selfdrive_tokens()
{
  if(distance_front<safe_distance){previous_front_distance+=1;}else{previous_front_distance=0;}
  if(distance_back<safe_distance){previous_back_distance+=1;}else{previous_back_distance=0;}
  if(previous_front_distance>=3){selfdrive_token_front=1;}else{selfdrive_token_front=0;}
  if(previous_back_distance>=3){selfdrive_token_back=1;}else{selfdrive_token_back=0;}
  }
void get_distance()
{
  get_distance_front();
  get_distance_back();
  selfdrive_tokens();
}
void search_metal()
{
metal_read=analogRead(metal_pin);
metal_read_average+=metal_read;
metal_average_counter++;
if(metal_average_counter==50)
{
  metal_value=metal_read_average/50;
  metal_average_counter=0;
  metal_read_average=0;
//  Serial.println(metal_value);
}
}
void loop() {
each_second();
get_distance();
read_battery();
search_metal();
prepare_packet();
udp_send();
udp_receive();
loop_time=millis()-loop_time_tracker;
//Serial.print("Loop time: ");Serial.println(loop_time);
loop_time_tracker=millis();
process_input();
if(loop_time_tracker-time_last_input<200)
{
  communication_out=0;
if(distance_safety_enabled==1){safe_drive();}else{basic_drive();}
}
else{communication_out=1;m.mno();}
//display_parameters();
//delay(100);
}
