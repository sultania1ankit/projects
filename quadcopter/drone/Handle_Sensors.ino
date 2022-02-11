void read_battery()
{
  battery_read_reducer++;
  if(battery_read_reducer==battery_read_reduction){battery_read_reducer=1;}
  if(battery_read_reducer==1)
  {
  battery_voltage=analogRead(battery_pin);
  battery_voltage=battery_voltage*0.01769;//0.01769
//  Serial.println(battery_voltage);
  }
}


void lights()
{
    // 4 leds ( blink + on ) -- 8 states
  // standby  , landing , shutdown , batterylow, started 
  // 4red 5blue 6green 7builtin
  if(connection_out==1){ledcWrite(4,0);ledcWrite(5,120);ledcWrite(6,0);}
  else if(mpu_data_stoned==1){ledcWrite(4,0);ledcWrite(5,255);ledcWrite(6,0);}
  else if(battery_voltage<battery_20){ledcWrite(4,255);ledcWrite(5,0);ledcWrite(6,0);}
  else if(battery_voltage<battery_40){ledcWrite(4,110);ledcWrite(5,0);ledcWrite(6,0);}
  else if(start==1){ledcWrite(4,0);ledcWrite(5,0);ledcWrite(6,255);}
  if(land==1){ledcWrite(7,100);}
  else if(shut_down==1){ledcWrite(7,255);}
  else{ledcWrite(7,0);}
//  
}

void prepare_packet()
{ 
  long reduced_roll,reduced_yaw,reduced_pitch,reduced_battery;
  reduced_roll=real_roll+45;
  reduced_yaw=real_yaw+45;
  reduced_pitch=real_pitch+45;
  reduced_battery=(100*battery_voltage);
//  Serial.println(reduced_battery);
//  Serial.print(" PRY   :");Serial.print(reduced_pitch);Serial.print("                ");Serial.print(reduced_roll);Serial.print("              ");Serial.println(reduced_yaw);
  packet_number++;
  if (packet_number==4){packet_number=1;}
  switch(packet_number)
  {
  case 1:data_s=1000000000+(10000000*reduced_roll)+(100000*reduced_pitch)+(1000*reduced_yaw)+(100*shut_down)+(10*land)+9;break;
  case 2:data_s=2000000000+(100000*height)+(10*reduced_battery)+start;break; //this is actual line
  case 3:data_s=3000000000+(100000*t_loop)+(10*thrust_fl)+4;break;  // this is to get thrust on the battery display
  }
//  Serial.println(data_s);
}

void extract_packet()
{
//  Serial.println(data_r);
  if(data_r==3124529845){shut_down=1;start=0;land=0;}
  else if(data_r==3432654345){shut_down=0;start=1;land=0;}
  else if(data_r==3987453653){shut_down=0;start=0;land=1;}
  else if(data_r/1000000000==1 && data_r%1000==999)
   { // yaw height roll pitch
    data_r=data_r%1000000000;
    yaw_r=data_r/100000000;data_r=data_r%100000000;
    height_r=data_r/10000000;data_r=data_r%10000000;
    roll_r=data_r/100000;data_r=data_r%100000;
    pitch_r=data_r/1000;packet1++;packet2=0;
   }
   else if (data_r=2545545545){packet2++;packet1=0;}
   
//   Serial.print(packet1);Serial.print("     ");Serial.println(packet2);
   
   if(packet1>nrf_data_stoned_threshold){nrf_data_stoned=1;}
   else if(packet2>nrf_data_stoned_threshold){nrf_data_stoned=1;}
   else{nrf_data_stoned=0;}
   
//   Serial.print("received data:  ");Serial.print(yaw_r);Serial.print("                   ");Serial.print(height_r);
//   Serial.print("              ");Serial.print(roll_r);Serial.print("           ");Serial.println(pitch_r);
}

void get_bme()
{
 height=bme.pres();   //**  height is actually pressure it decreases as altitude increases.
 height=height-height_adjust;

 if(height_old==height){bmp_data_stoned_counter++;}else{bmp_data_stoned_counter=0;}
 if(bmp_data_stoned_counter>bmp_data_stoned_threshold){bmp_data_stoned=1;}else{bmp_data_stoned=0;}

 
 if(height>height_old){ascending--;}
 else if(height<height_old){ascending++;}

//if(millis()-t_height>700)
//{ 
//  t_height=millis();
//  if(height+1<last_height){ascending++;}
//  else if(height-1>last_height){ascending--;}
//  last_height=height;
//}

 if(ascending>ascending_threshold){ascending=ascending_threshold;}
 else if(ascending<-ascending_threshold){ascending=-ascending_threshold;}

 
// if(height==height_old)
// {hovering++;}
// else{hovering=hovering-40;}
//
// if(hovering>hovering_threshold){hovering=hovering_threshold;}
// else if(hovering<0){hovering=0;}

if(height<height_initial-height_barrier){land=1;start=0;}



 height_old=height;
//
// Serial.print("height: ");Serial.print(height);
// Serial.print("             ascending: ");Serial.print(ascending);
// Serial.print("             hovering: ");Serial.println(hovering);
 
}


void get_nrf()
{
  while(radio.available(&pipeNo))
{radio.writeAckPayload(pipeNo,&data_s,4);
 ack_p=1;packets_received +=1;
 t_connection=millis();connection_out=0;
 radio.read( &data_r, 4 );
// Serial.println(data_r);
      }
}

void get_mpu()
{
   mpu.read_acc();
   mpu.read_gyro();
  float accelX = mpu.accel_data[0];
  float accelY = mpu.accel_data[1];
  float accelZ = mpu.accel_data[2];
  float gyroX = mpu.gyro_data[0];  // by 57.3 
  float gyroY = mpu.gyro_data[1];
  float gyroZ = mpu.gyro_data[2];

  
  float raw_pitch = atan2 (accelY ,( sqrt ((accelX * accelX) + (accelZ * accelZ))));
  float raw_roll = atan2(-accelX ,( sqrt((accelY * accelY) + (accelZ * accelZ))));
//   
//   //   float Yh = (magY * cos(roll)) - (magZ * sin(roll));
////   float Xh = (magX * cos(pitch))+(magY * sin(roll)*sin(pitch)) + (magZ * cos(roll) * sin(pitch));
////
////   yaw =  atan2(Yh, Xh);
//
    raw_roll = raw_roll*57.32;
    raw_pitch = raw_pitch*57.32;
//  //yaw = yaw*57.32;
//    
////    Serial.print("ACC                                    :");Serial.print(accelX);Serial.print("    ");Serial.print(accelY);Serial.print("    ");Serial.println(accelZ);
////    Serial.print("GYR                                    :");Serial.print(gyroX);Serial.print("    ");Serial.print(gyroY);Serial.print("    ");Serial.println(gyroZ);
////    Serial.print("MAG                                    :");Serial.print(magX);Serial.print("    ");Serial.print(magY);Serial.print("    ");Serial.println(magZ);
////    Serial.print(gyrox_offset);Serial.print("        ");Serial.print(gyroy_offset);Serial.print("        ");Serial.println(gyroz_offset);
//
//
//  // complimentry filter 
    float t_mpu=micros()-timer_mpu;
    timer_mpu=micros();
    real_pitch=trustfactor_gyro*(real_pitch+((gyroX*t_mpu)/1000000))+(1-trustfactor_gyro)*raw_pitch;
    real_roll=trustfactor_gyro*(real_roll+((gyroY*t_mpu)/1000000))+(1-trustfactor_gyro)*raw_roll;
//    real_yaw=trustfactor_gyro*(real_yaw+((gyroZ*t_mpu)/1000000));
//
//
//    if(old_pitch==real_pitch){Serial.print("pitch");}
//    if(old_roll==real_roll){Serial.print("roll");}

    if(old_pitch==real_pitch && old_roll==real_roll){mpu_data_stoned_counter++;}
    else{ mpu_data_stoned_counter=0;}
//    Serial.println(mpu_data_stoned_counter);
    if(mpu_data_stoned_counter>mpu_data_stoned_threshold){mpu_data_stoned=1;}else{mpu_data_stoned=0;}
    
    old_pitch=real_pitch;
    old_roll=real_roll;




//   //kalman filter
//   rate = gyroX-bias;
//   angle=angle+(rate*t_mpu)/1000000;
//    // Update estimation error covariance - Project the error covariance ahead
//    P00 += (t_mpu)/1000000 * ((t_mpu*P11)/100000 - P01 - P10 + Q_angle);
//    P01 -= (t_mpu)/1000000 * P11;
//    P10 -= (t_mpu)/1000000 * P11;
//    P11 += Q_bias * t_mpu;
//   
//    float S = P00 + R_measure; // Estimate error
//
//    float K[2]; // Kalman gain - This is a 2x1 vector
//    K[0] = P00 / S;
//    K[1] = P10 / S;
//    
//    float y = pitch - angle; // Angle difference
//
//    angle += K[0] * y;
//    bias += K[1] * y;
//
//    float P00_temp = P00;
//    float P01_temp = P01;
//
//    P00 -= K[0] * P00_temp;
//    P01 -= K[0] * P01_temp;
//    P10 -= K[1] * P00_temp;
//    P11 -= K[1] * P01_temp;
    

    
//    Serial.print(real_roll);Serial.print(" "); 
//    Serial.print(raw_pitch);Serial.print(" ");
//    Serial.println(raw_pitch);

}

//void handle_wifi()
//{
//  byte wifi_pitch1,wifi_roll1,wifi_pitch2,wifi_roll2,wifi_yaw1,wifi_yaw2;
//  wifi_pitch1=real_pitch*100;
//  wifi_pitch2=wifi_pitch1%100;
//  wifi_pitch1=real_pitch;
//  wifi_roll1=real_roll*100;
//  wifi_roll2=wifi_roll1%100;
//  wifi_roll1=real_roll;
//  wifi_yaw1=real_yaw*100;
//  wifi_yaw2=wifi_yaw1%100;
//  wifi_yaw1=real_yaw;
//  data_wifi[1]=wifi_pitch1;data_wifi[2]=wifi_pitch2;
//  data_wifi[3]=wifi_roll1;data_wifi[4]=wifi_roll2;
//  data_wifi[5]=wifi_yaw1;data_wifi[6]=wifi_yaw2;
//
//  byte fr11,fr12,fl11,fl12,br11,br12,bl11,bl12;
//  fr11=thrust_fr/100;fr12=thrust_fr%100;
//  fl11=thrust_fl/100;fl12=thrust_fl%100;
//  br11=thrust_br/100;br12=thrust_br%100;
//  bl11=thrust_bl/100;bl12=thrust_bl%100;
//  data_wifi[7]=fr11;data_wifi[8]=fr12;
//  data_wifi[9]=fl11;data_wifi[10]=fl12;
//  data_wifi[11]=br11;data_wifi[12]=br12;
//  data_wifi[13]=bl11;data_wifi[14]=bl12;
//  udp_send();
//  for(int i=1;i<15;i++)
//  {
//  Serial.println(data_wifi[i]);
//  }Serial.println("______________________________________________________");
//}

void control_arm()
{

//    Serial.print(limb_angle); Serial.print("        C:      "); Serial.println(clamper_angle);
  if (custom1 == 0 && custom2 == 0 && millis() - arm_timer > arm_delay)
  {
    arm_timer = millis();
    if (arm_select == 2)
    {
      if (arm_angle == 2) {
        limb_angle++;
      }
      else if (arm_angle == 3) {
        limb_angle--;
      }

      if (limb_angle > limb_max_angle) {
        limb_angle = limb_max_angle;
      }
      else if (limb_angle < limb_min_angle) {
        limb_angle = limb_min_angle;
      }

      ledcWrite(8, limb_angle);
    }

    else if (arm_select == 3)
    {
      if (arm_angle == 2) {
        clamper_angle++;
      }
      else if (arm_angle == 3) {
        clamper_angle--;
      }

      if (clamper_angle > clamper_max_angle) {
        clamper_angle = clamper_max_angle;
      }
      else if (clamper_angle < clamper_min_angle) {
        clamper_angle = clamper_min_angle;
      }

      ledcWrite(9, clamper_angle);
    }
  }
  else
  {
    if (custom1 == 1) {
      limb_angle = limb_grab_angle;
      clamper_angle = clamper_grab_angle;
    }
    else if (custom2 == 1) {
      limb_angle = limb_standby_angle;
      clamper_angle = clamper_standby_angle;
    }
    ledcWrite(8, limb_angle); ledcWrite(9, clamper_angle);
  }
}
