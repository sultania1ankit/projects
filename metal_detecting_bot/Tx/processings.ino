// A 0 1 2 3 , joystick 
//  leftjy  a1 updown a3 right left
// right jy a2 rightleft a0 updown
// d8 switch jy 2 ,pull it up 
void controller_input()
{
  if(digitalRead(2)==0){ib0=1;}else{ib0=0;}//top
  if(digitalRead(8)==0){ib1=1;}else{ib1=0;}//bottom

  if (analogRead(A3)<100){id0=1;}
  else if (analogRead(A3)>900){id0=2;}
  else if (analogRead(A1)<100){id0=3;}
  else if (analogRead(A1)>900){id0=4;}
  else {id0=0;}

  if (analogRead(A2)<100){id1=1;}
  else if (analogRead(A2)>900){id1=2;}
  else if (analogRead(A0)<100){id1=3;}
  else if (analogRead(A0)>900){id1=4;}
  else {id1=0;}

    if(id0==0){op_l="Idle";}
    else if(id0==1){op_l="Right";}
    else if(id0==2){op_l="Left";}
    else if(id0==3){op_l="Down";}
    else if(id0==4){op_l="Up";}
    if(ib0==1){op_l="Select";}

    if(id1==0){op_r="Idle";}
    else if(id1==2){op_r="Right";}
    else if(id1==1){op_r="Left";}
    else if(id1==4){op_r="E+";} //down
    else if(id1==3){op_r="Tr+";} // up
    if(ib1==1 && id1==0){op_r="Select";}
  
  data_input=(id0+5*ib0)+10*(id1+5*ib1);
  if(data_input==55){extra_display=1;}
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
    if (!radio.write( &data_s, 2 )){packets_fail+=1;}
    else{if(!radio.available()){packets_blank +=1;}
      else{while(radio.available() ){packets_rec+=1;
          radio.read( &data_r, 2 );//          Serial.println(gotByte);
}}}}

void packet_reset()
{
  if(packets_display==1)
{
  packets_sent=0;packets_rec=0;packets_blank=0;packets_fail=0;  packets_display=0;}
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
{ received_packet_no=data_r/10000;
 switch(received_packet_no)
 {
  case 1:distance_front=(data_r%10000)/100;distance_back=data_r%100;    break;
  case 2:enable_distance_received=(data_r%1000)/100;safe_distance_received=data_r%100;    break;
  case 3:
  if(((data_r%10000)/1000)==1){v_lifepo4=data_r%10000;v_lifepo4=v_lifepo4/100;}
  else if(((data_r%10000)/1000)==2) {v_buckop=data_r%1000;v_buckop=v_buckop/100;}   break; 
  case 4:switch((data_r/1000)%10) 
 {case 1:time_carloop=data_r%1000;  break;
 case 2:wdt_count=data_r%1000;  break;
 case 3:car_reset_reason=(data_r%1000)/100;car_error_loop=data_r%100;break;
 }
  break;
  case 5:packets_received_there=data_r%1000;    break;
 }
}
void data_to_transmit()
{// below hundred xy , x it tr , y is direction
  if(millis()-t_buttondelay1>1000 && ib0==0)
  {t_buttondelay1=millis();
  
  if(id1==3){tr=tr+1;}
  if(tr==3){tr=0;}

  if(id1==4){ 
  if(enable_distance==1){enable_distance=0;}
  else if (enable_distance==0){enable_distance=1;}}
  }
  if(millis()-t_buttondelay2>100 && ib0==1)
  {t_buttondelay2=millis();
    if(id1==4){safe_distance-=1;op_r="Sd-";}
    else if(id1==3){safe_distance+=1;op_r="Sd+";}
    if(safe_distance==36){safe_distance=1;}
    else if(safe_distance==0){safe_distance=35;}
  }
  
  if(id0==1){data_s=1;}//right 
  else if(id0==2){data_s=2;}//left
  else if(id0==3){data_s=3;}//down
  else if(id0==4){data_s=4;}//up
  else {data_s=0;}

  if(id1==2){data_s=1;}//right
  else if(id1==1){data_s=2;}//left
  if(id1==2 && id0==3){data_s=6;}//right
  else if(id1==1 && id0==3){data_s=7;}//left
  /// abcde  a= enable_distance , bc=safe_distance , d=tr, e=direction  
  data_s=10000*(enable_distance+1)+100*safe_distance  +10*(tr+1)+(data_s+1);  // tr+1 is transmitted
  if(extra_display==1)
  {
    if(id0==3){data_s=29999;extra_display=0;}  // wdt count reset
    else if(id0==4){data_s=19999;extra_display=0;} // car reset
  }
  if(ib1==1 && id0==4){data_s=19999;}
  else if(ib1==1 && id0==3){data_s=29999;}

}

