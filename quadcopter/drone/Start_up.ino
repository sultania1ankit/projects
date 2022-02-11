void start_mpu()
{

// imu.setGyroFSR(250); // Set gyro to 2000 dps
// Accel options are +/- 2, 4, 8, or 16 g
// imu.setAccelFSR(2); // Set accel to +/-2g
// imu.setLPF(3); // Set LPF corner frequency to 5Hz
// imu.setSampleRate(310); // Set sample rate to 10Hz
// imu.setCompassSampleRate(50); // Set mag rate to 10Hz

// all the above options are there in the library

//        {0b00000101,MPUREG_CONFIG},  // gyro lpf set to 10hz delay of 17.85ms and 1khz sample rate  and fifo set to overwrite last values 
//        {0b00000101, MPUREG_ACCEL_CONFIG_2}, // lpf 10hz ,delay 35.70 ms , 1 khz sample rate
        
SPI.begin();
  mpu.init(1, 0); //
  int temp_gyrox = 0, temp_gyroy = 0, temp_gyroz = 0;
  for (int i = 0; i < 2000; i++)
  {
    mpu.read_gyro();
    float gyroX = mpu.gyro_data[0];  // by 57.3
    float gyroY = mpu.gyro_data[1];
    float gyroZ = mpu.gyro_data[2];
    temp_gyrox = temp_gyrox + gyroX;
    temp_gyroy = temp_gyroy + gyroY;
    temp_gyroz = temp_gyroz + gyroZ;
    delay(1);
    //  Serial.println(temp_gyroz);
    if (i == 1999)
    {
      gyrox_offset = temp_gyrox / 2000;
      gyroy_offset = temp_gyroy / 2000;
      gyroz_offset = temp_gyroz / 2000;
    }
  }
// there is  a calibration done at every startup so keep the quad leveled.
// the data is sent back to mpu to be calibrated and temprature compensated and factory trimmed.
//mpu.calib_acc();
//mpu.calib_mag();
}

void start_bme()
{
  while(!Serial) {} // Wait

  Wire.begin();

 while(!bme.begin())
{
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
}
delay(100);
 height=bme.pres();   //**  height is actually pressure it decreases as altitude increases.
 height_initial=height-height_adjust;
 t_height=millis();
}

void start_nrf()
{
  radio.begin();
  radio.setChannel(108);
  //radio.setAddressWidth(3);// not much speed difference
  radio.setAutoAck(1);                   
  radio.enableAckPayload();               
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(15,15);                
  radio.setPayloadSize(4);                
//radio.openWritingPipe(pipes[1]);        
  radio.openReadingPipe(1,0xABCDABCD71LL);// try smaller address to find speed change
  radio.startListening();           
}

void start_lights()
{
  pinMode(red_light,OUTPUT);
  pinMode(blue_light,OUTPUT);
  pinMode(green_light,OUTPUT);
  pinMode(builtin_light,OUTPUT);
  ledcSetup(4, 1, resolution_led);
  ledcSetup(5, 1, resolution_led);
  ledcSetup(6, 1, resolution_led);
  ledcSetup(7, 1, resolution_led);
  ledcAttachPin(red_light,4);
  ledcAttachPin(blue_light,5);
  ledcAttachPin(green_light,6);
  ledcAttachPin(builtin_light,7);
  // 4 leds ( blink + on ) -- 8 states
  // standby  , landing , shutdown , batterylow, started 
}

void start_esc()
{
  pinMode(fr_pin,OUTPUT);
  pinMode(fl_pin,OUTPUT);
  pinMode(br_pin,OUTPUT);
  pinMode(bl_pin,OUTPUT);
  ledcSetup(0, freq, resolution_esc);
  ledcSetup(1, freq, resolution_esc);
  ledcSetup(2, freq, resolution_esc);
  ledcSetup(3, freq, resolution_esc);
  ledcAttachPin(fr_pin,0);  // fr
  ledcAttachPin(fl_pin,1);  //fl
  ledcAttachPin(br_pin,2);  // br
  ledcAttachPin(bl_pin,3);  //bl
  for(int i=0;i<3;i++)
  {
  ledcWrite(0,3277);  
  ledcWrite(1,3277);  
  ledcWrite(2,3277);  
  ledcWrite(3,3277); 
  delay(50);
  }
}
void IRAM_ATTR resetModule() {
  // dying wish
  Serial.println("WDT RESET");
  ledcWrite(0,3277);  //65535  - (3277  6554)
  ledcWrite(1,3277);
  ledcWrite(2,3277);
  ledcWrite(3,3277);delay(10);
  ledcWrite(0,3277);  //65535  - (3277  6554)
  ledcWrite(1,3277);
  ledcWrite(2,3277);
  ledcWrite(3,3277);delay(10);
  esp_restart(); //restart
}
void start_wdt()
{
    timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt
}
