
// need to work a lot here 

void handle_shutdown()
{


Serial.println("Handling shutdown");
thrust=3277;

    // 4red 5blue 6green 7builtin
  while(start==0)
  { 
  ledcWrite(0,3277);  //65535  - (3277  6554)
  ledcWrite(1,3277);
  ledcWrite(2,3277);
  ledcWrite(3,3277);
  get_nrf();extract_packet();prepare_packet();
  ledcWrite(4,0);ledcWrite(5,0);ledcWrite(6,120);
  timerWrite(timer, 0); //reset timer (feed watchdog)
  }
  if(start==1){ledcWrite(4,0);ledcWrite(5,0);ledcWrite(6,255);}
  
}

void handle_landing()
{
  Serial.println("handling landing");
// 4red 5blue 6green 7builtin
  if(land==1){ledcWrite(4,0);ledcWrite(5,0);ledcWrite(6,120);}
  if(start==1){ledcWrite(4,0);ledcWrite(5,0);ledcWrite(6,255);}
  
  pitch_r=31;
  roll_r=29;
  yaw_r=4;

  if(ascending>-2){height=2;}
  else {height=4;}

      // set input to ideal and stabilize and start decending

    delay(1000); //to test wdt 
  
}

void handle_connection_out()
{
  if(millis()-t_connection>5000){land=1;start=0;}
  Serial.println("connection out");
  pitch_r=31;
  roll_r=29;
  yaw_r=4;
  height_r=4;
  
  
}
