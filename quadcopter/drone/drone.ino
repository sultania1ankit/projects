// ESP32 DRONE ______ 12/01/2019
// :-)    ____________26/02/2019  (MPU transfered to SPI , and other hardware is reliable)


//____________________Wifi ( for debugging only )________________________
// it was very slow 150ms  , the esp now runs at around 190us 
//so it is of no use until i learn more python , ( use multithreading , or edit the polling time is the UDP library) 
// do turn off before field launch as it is memory hungry
//#include <WiFi.h>
//#include <WiFiUdp.h>
//#include <WiFiAP.h>
//
//const char * udpAddress = "192.168.4.2";
//const int udpPort = 44444;
//const char* assid = "Cold_Drone";
//const char* asecret = "11111114";  // if esp host's ap
//WiFiUDP udp;
//byte data_wifi[15];byte wifi_reducer;
//void create_ap()
//{
//   WiFi.mode(WIFI_AP);
//  //access point part
//  Serial.println("Creating Accesspoint");
//  WiFi.softAP(assid,asecret,7,0,5);
//  Serial.print("IP address:\t");
//  Serial.println(WiFi.softAPIP());
//}
//  void udp_send()
//  {
//  uint8_t buffer[15] = {data_wifi[1],data_wifi[2],data_wifi[3],data_wifi[4],data_wifi[5],data_wifi[6],data_wifi[7],
//  data_wifi[8],data_wifi[9],data_wifi[11],data_wifi[12],data_wifi[13],data_wifi[14]};
//  udp.beginPacket(udpAddress, udpPort);
//  udp.write(buffer,15);
//  udp.endPacket();
//  }

//___________________MPU9250(pitch,yaw,roll)________________________________
boolean mpu_data_stoned; int mpu_data_stoned_counter;
float old_pitch, old_roll;

#include <MPU9250.h>
#define SPI_CLOCK 8000000  // 8MHz clock works.
#define SS_PIN   15 
MPU9250 mpu(SPI_CLOCK, SS_PIN);


//_________________________Watchdog timer__________________________________
#include "esp_system.h"
hw_timer_t *timer = NULL; // timer should be resetted before the declared to prevent the wdt resset ( it is a 0x0000 reset)

//______________________BME280(pressure)_______________________________
#include <BME280I2C.h>
#include <Wire.h>
BME280I2C::Settings settings( // need to work more on it
  BME280::OSR_X1,
  BME280::OSR_X1,
  BME280::OSR_X16,
  BME280::Mode_Normal,
  BME280::StandbyTime_500us,
  BME280::Filter_16, // it also has a filter that we studied in DSP (butterworth maybe)
  BME280::SpiEnable_False,
  0x76 // I2C address. I2C specific.
);
BME280I2C bme(settings);
long height, height_old; int ascending;
int hovering; int height_initial;
unsigned long t_height; int last_height;
boolean bmp_data_stoned; int bmp_data_stoned_counter;


//__________NRF24l01 (Communication)____________________________________________________________
#include <SPI.h>
#include "nRF24L01.h"  // see datasheet during later upgrade
#include "RF24.h"
RF24 radio(4, 5); //green wire 9
unsigned long data_r; unsigned long data_s; int packets_received; int ack_p = 1; byte pipeNo;
unsigned long t_connection; boolean connection_out; byte packet_number; boolean nrf_data_stoned;
int packet1; int packet2;


//_______________________ESC____________________________________________________________________
const int freq = 50; // 20ms divisons
const int resolution_esc = 16;//65535

#define fr_pin 25  //  front-right CCW
#define fl_pin 33  //  front-left  CW
#define br_pin 26  //  back-right  CW
#define bl_pin 32  //  back-left  CCW
const int resolution_led = 8;

//_______________________________lights____________________________________________________________

#define red_light 13
#define blue_light 14
#define green_light 12
#define builtin_light 2
#define battery_pin 34

//_______________________________PID____________________________________________________________

unsigned long pid_time, pid_timer;
byte yaw_r, pitch_r, roll_r, height_r;
float output_pitch, output_roll, output_yaw;
float timer_mpu;
float real_pitch, real_yaw, real_roll;
float thrust_desired_reduced;

//_____________________kalman filter ________________________________________________________
// there was some madwick filter also there that was apparently better 
//    float rate=0;
//    double angle=0;
//    float bias=0;
//    float P00 =0; // error covariance matrix is set like so
//    float P01 = 0;
//    float P10 = 0;
//    float P11 = 0;
//    float Q_angle = 0.001;
//    float Q_bias = 0.003;
//    float R_measure = 0.03;

//___________________________________________________________________________________________
boolean shut_down; boolean start; boolean land;
int thrust;
int thrust_fr, thrust_fl, thrust_br, thrust_bl;
//___________________________________________________________________________________________

unsigned long loop_timer;int t_loop;
float battery_voltage; int battery_read_reducer;


//_______________________***  CONFIGURABLE PARAMETERS  ***___________________________________________________________

//Go to startup section for additional parameters

// MPU
float trustfactor_gyro = 0.98;
int mpu_data_stoned_threshold = 10;


// PID
// for Ki and Kd micros time is reduced by a factor so as to have resonable gains.
//but making this factor less than loop time makes a " x/0" condition causing full throttle(6557) on all motors
// loop time is around 190 us
int pid_time_reducer=100;

float kpp = 1.2, kip = 0, kdp = 0; // Pitch 
float kpr = 1.2, kir = 0, kdr = 0; // Roll


int roll_thrust_limit = 350, pitch_thrust_limit = 350, yaw_thrust_limit = 25;
int roll_thrust_limit_factor = 4, pitch_thrust_limit_factor = 4;  //* for quick control 
int yaw_activation_thrust=800; // above this thrust only the yaw controls activate
// 328 in 10% of thrust (3277)
// to lift 40% of thrust is needed ,1350
// total 3277 -- 84% is  2750
int vertical_thrust_limit = 1500;
int full_throttle_limit = 6000; // 3277*2=6554


float kpy = 3, kiy = 0, kdy = 0; // Yaw ( not in use)
float yaw_tolerance = 0.3; byte yaw_sensitivity = 1; byte yaw_selfcorrect_sensitivity = 0.5; // 

//NRF
int connection_out_time = 350; // milliseconds
int nrf_data_stoned_threshold = 500;


//BMP
unsigned long height_adjust = 97000;
int ascending_threshold = 5;
int hovering_threshold = 1000; // not working
int height_barrier = 60;
int bmp_data_stoned_threshold = 500;

// Battery (Li-Po  3S 1500mAh)
// very precise but not accurate , ( maybe due to arduino core , might get fixed in next sdk package)
byte battery_read_reduction = 5500;
float battery_40 = 10.3;
float battery_20 = 9.6;

// Watchdog timer
const int wdtTimeout = 800;  //time in ms to trigger the watchdog


//____^^^^^^^____________***  CONFIGURABLE PARAMETERS  ***___________________^^^^^^^^^^^_____________________________


void pid_controller()
{
  float error_pitch, integral_pitch, derivative_pitch, last_error_pitch; // define local variables
  float error_roll, integral_roll, derivative_roll, last_error_roll;
  //  float error_yaw,integral_yaw,derivative_yaw,last_error_yaw;



  int pitch_desired = pitch_r - 31;  // center out the received data
  if(pitch_desired!=0){pitch_desired=map(pitch_desired,-31,29,-10,10);}
  int roll_desired = roll_r - 29;
  if(roll_desired!=0){roll_desired=map(roll_desired,-29,31,-10,10);}
  int thrust_desired = (height_r - 4);
  int yaw_desired = yaw_r - 4;


  pid_time = micros() - pid_timer; pid_timer = micros(); // find time for pid_control


  error_pitch = pitch_desired - real_pitch; // pitch control   
  integral_pitch = integral_pitch + (error_pitch * (pid_time/pid_time_reducer));  // sometimes looptime is less than 1000us.
  derivative_pitch = (error_pitch - last_error_pitch) / (pid_time/pid_time_reducer);// pid_time_reducer is <500.   | not true now.
  output_pitch = output_pitch + (kpp*error_pitch) + (kip*integral_pitch) + (kdp*derivative_pitch);
  last_error_pitch = error_pitch;

  error_roll = roll_desired - real_roll; // roll control
  integral_roll = integral_roll + (error_roll * (pid_time/pid_time_reducer ));
  derivative_roll = (error_roll - last_error_roll) / (pid_time/pid_time_reducer);
  output_roll = output_roll + kpr * error_roll + kir * integral_roll + kdr * derivative_roll;
  last_error_roll = error_roll;

  //    error_yaw=yaw_r-real_yaw;
  //    integral_yaw=integral_yaw+(error_yaw*pid_time);
  //    derivative_yaw=(error_yaw-last_error_yaw)/pid_time;
  //    output_yaw=kpy*error_yaw+kiy*integral_yaw+kdy*derivative_yaw;
  //    last_error_yaw=error_yaw;

  if(thrust>yaw_activation_thrust+3277)
  {
  output_yaw = output_yaw + (yaw_desired * yaw_sensitivity);

  if (real_yaw > yaw_tolerance) {output_yaw = output_yaw - yaw_selfcorrect_sensitivity;}
  if (real_yaw < -yaw_tolerance) {output_yaw = output_yaw + yaw_selfcorrect_sensitivity;}

  if (real_yaw == 0 && yaw_desired == 0) {output_yaw = 0;}
  }
  else{output_yaw=0;}

  float output_pitch_magnitude=abs(output_pitch);
  float error_pitch_magnitude=abs(error_pitch);
  if(output_pitch_magnitude>error_pitch_magnitude*pitch_thrust_limit_factor)
  {output_pitch_magnitude=error_pitch_magnitude*pitch_thrust_limit_factor;
  if(output_pitch>0){output_pitch=output_pitch_magnitude;}else{output_pitch=-output_pitch_magnitude;}}

  float output_roll_magnitude=abs(output_roll);
  float error_roll_magnitude=abs(error_roll);
  if(output_roll_magnitude>error_roll_magnitude*roll_thrust_limit_factor)
  {output_roll_magnitude=error_roll_magnitude*roll_thrust_limit_factor;
  if(output_roll>0){output_roll=output_roll_magnitude;}else{output_roll=-output_roll_magnitude;}}

  if (output_pitch > pitch_thrust_limit) {output_pitch = pitch_thrust_limit;}
  else if (output_pitch < -pitch_thrust_limit) {output_pitch = -pitch_thrust_limit;}
  
  if (output_roll > roll_thrust_limit) {output_roll = roll_thrust_limit;}
  else if (output_roll < -roll_thrust_limit) {output_roll = -roll_thrust_limit;}

  if (output_yaw > yaw_thrust_limit) {output_yaw = yaw_thrust_limit;}
  else if (output_yaw < -yaw_thrust_limit) {output_yaw = -yaw_thrust_limit;}

//  Serial.print("Output Pitch :");Serial.print(output_pitch);Serial.print("               ");Serial.println(error_pitch);
//  Serial.print("Output Roll :");Serial.print(output_roll);Serial.print("               ");Serial.println(error_roll);
////  Serial.print("Output Yaw :");Serial.println(output_yaw);
//  Serial.println("");Serial.println("");Serial.println("");Serial.println("");Serial.println("");


  float thrust_increment=thrust_desired;
  thrust_desired_reduced=thrust_desired_reduced+(thrust_increment/20);
  if (thrust_desired_reduced < 3277) {thrust_desired_reduced = 3277;}
  else if (thrust_desired_reduced > (vertical_thrust_limit + 3277)) {thrust_desired_reduced = (vertical_thrust_limit + 3277);}
  thrust = thrust_desired_reduced;
  if (thrust < 3277) {thrust = 3277;}else if (thrust > (vertical_thrust_limit + 3277)) {thrust = (vertical_thrust_limit + 3277);}
  //     at 11.4 volt  4600 thrust drone lifted  ->  less than 40% thrust  :-)

}

void control_esc()
{
  //_________________________________DRONE HARDWARE VISUAL___________________________________________________
  //
  //                      pitch_back (-)
  //
  //              BL                    BR
  //
  // roll_left(-)         CENTER                  roll_right (+)
  //
  //              FL                    FR
  //
  //                      pitch_front (+)                                 *from this top view CW is +ve yaw
  //
  //                  (antenna and usb)
  //________________________________________________________________________________________________________

  thrust_fr = thrust - output_roll - output_pitch + output_yaw;
  thrust_fl = thrust + output_roll - output_pitch - output_yaw;
  thrust_br = thrust - output_roll + output_pitch - output_yaw; 
  thrust_bl = thrust + output_roll + output_pitch + output_yaw;
  //    thrust_fl=thrust_fr=thrust_bl=thrust_br=thrust;
  //      thrust_bl=thrust;
//    Serial.println("");Serial.println("");Serial.println("");Serial.println("");Serial.println("");Serial.println("");
//    Serial.print(thrust_bl);Serial.print("                     ");Serial.println(thrust_br);
//    Serial.println("");
//    Serial.print(thrust_fl);Serial.print("                     ");Serial.println(thrust_fr);
//    Serial.println("_________________ FRONT_____________________");
//    Serial.println("");Serial.println("");Serial.println("");Serial.println("");Serial.println("");Serial.println("");




  //         ____________________________full throttle protection while testing________________________________
  if (thrust_fr > full_throttle_limit) {thrust_fr = full_throttle_limit;} // 
  if (thrust_fl > full_throttle_limit) {thrust_fr = full_throttle_limit;} // at 50% (with PID, so less than 50% maybe) throttle drone did not lift :-(
  if (thrust_br > full_throttle_limit) {thrust_fr = full_throttle_limit;}
  if (thrust_bl > full_throttle_limit) {thrust_fr = full_throttle_limit;}
  //____________________________________________________________________________________________________________


  //________________________ESC signal conditining (1ms to 2ms duty length in 50ms pulse)______________________
  //  ***(doubt) - pwm pulse rate is 50ms , if i change duty cycle before 50ms what happens
  //rewriting the same duty to PWM can also resatrt the cycle ( oscilloscope needed)


  if (thrust_fr > 6554) {thrust_fr = 6554;} else if (thrust_fr < 3277) {thrust_fr = 3277;}
  if (thrust_br > 6554) {thrust_br = 6554;} else if (thrust_br < 3277) {thrust_br = 3277;}
  if (thrust_fl > 6554) { thrust_fl = 6554;} else if (thrust_fl < 3277) {thrust_fl = 3277;}
  if (thrust_bl > 6554) {thrust_bl = 6554;} else if (thrust_bl < 3277) {thrust_bl = 3277;}


  ledcWrite(0, thrust_fr);                                 ledcWrite(1, thrust_fl);


  ledcWrite(2, thrust_br);                                 ledcWrite(3, thrust_bl);

  //         ____________________________________________________________
}

//_________________________________________________________________________

void setup()
{

  // software upgardes needed - semaphores , kalman filter , flight modes( indoor safety , takeoff level using )
  // hardware upgrades needed - battery , gps , magnetometer(is present) ,nrf upgarde  , proximity (tof)
  // either prop or prop mounts are bad.

//  Serial.begin(115200);    // turnit off for better loop time after completion.
//  Serial.println("STARTED");
  delay(1000); // To let mpu calibrate without noise
  start_mpu();             // accel offset not needed
  //start_bme();             // might need to change filtering and mode due to props
  start_lights();
  analogReadResolution(10);  // accuracy is bad as fuck. read espressif blog ( problem will be fixed in next update).
  start_esc();
  start_nrf();              // full potential yet not known( datasheet)
  ledcWrite(6, 100); ledcWrite(7, 100);
//  ledcWrite(0,6554);ledcWrite(1,6554);ledcWrite(2,6554);ledcWrite(3,6554);delay(5000); // To calibrate ESC
//  ledcWrite(0,3277);ledcWrite(1,3277);ledcWrite(2,3277);ledcWrite(3,3277);delay(5000); // always connect battery first then the USB to avoid recalibration.
  while (start == 0) {get_nrf();extract_packet();}
  ledcWrite(7, 0);
  delay(4000);
  // Comment while final upload - only for debugging
  //create_ap(); //esp to host ap
  //udp.begin(udpPort);
  start_wdt();
  nrf_data_stoned = 0;
}


void loop() {

  // This can loop at around 4khz   :D
  get_mpu(); 
  //  //wifi_reducer++;if(wifi_reducer==150){handle_wifi();wifi_reducer=0;} // sensors section
  //  //get_bme();
  read_battery(); // handle low battery
  t_loop=micros()-loop_timer;loop_timer=micros();
  prepare_packet();
  get_nrf();// sending is done in the ack. packet
//  Serial.print(nrf_data_stoned);Serial.print(mpu_data_stoned);Serial.println(bmp_data_stoned);
  if (nrf_data_stoned == 1 || mpu_data_stoned == 1 || bmp_data_stoned == 1) {start = 0; land = 1;} // handle stoned_data (by wdt reset for now)
  if (millis() - t_connection > connection_out_time) {connection_out = 1;start = 0;handle_shutdown();}
  timerWrite(timer, 0); //reset timer (feed watchdog)
  extract_packet();
  lights();
  if (shut_down == 1) {handle_shutdown();}
  else if (land == 1) {handle_landing();}
  else if (connection_out == 1) {handle_connection_out();}
  pid_controller();
  control_esc();  

}
