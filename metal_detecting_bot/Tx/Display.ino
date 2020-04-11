void oled()
  {
//  packets_rec_disp=123;packets_sent_disp=123;packets_received_there=123;packets_blank_disp=123;packets_fail_disp=324;
//  data_r=22233;data_s=12327;data_input=34;v_buckop=5.43;v_lifepo4=12.82;time_carloop=13;
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(t_loop);
  display.print("/");
  display.print(time_carloop);
  display.setCursor(38,0);
  display.print("R");
  display.print(tr);
  display.setCursor(60,0);
  display.print(packets_rec_disp);
  display.setCursor(78,0);
  display.print("/");
  display.print(packets_received_there);
  display.setCursor(103,0);
  display.print("/");
  display.print(packets_sent_disp);
  display.setCursor(0,13);
  display.print(v_lifepo4);
  display.print("/");
  display.setCursor(40,13);
  display.print(v_buckop);
  display.setCursor(84,13);
  display.print(packets_blank_disp);
  display.setCursor(103,13);
  display.print("/");
  display.print(packets_fail_disp);
  display.drawLine(0, 22, 127, 22, WHITE);//___________________________________________
  if(extra_display==1)
  {display.setCursor(0,28);
  display.print("Reset Watchdog count");
  display.setCursor(0,40);
  display.print("Reset car");
  }
  else{
  display.setCursor(0,28);
  display.print(data_r);
  display.setCursor(60,28);
  display.print(data_input);
  display.setCursor(73,28);
  display.print("/");
  display.print(data_s);
  display.setCursor(0,40);
  display.print("D:");
  display.print(distance_front);
  display.print("|");
  display.print(distance_back);
  display.setCursor(60,40);
  display.print(safe_distance_received);
  display.print("|");
  display.print(enable_distance_received);
  display.print("/");
  display.print(safe_distance);
  display.print("|");
  display.print(enable_distance);
  //______________________________________________________________
  display.setCursor(0,52);
  display.print(op_l);
  display.setCursor(38,52);
  display.print(car_reset_reason);
  display.print("/");
  display.print(car_error_loop);
  display.print("/");
  display.print(wdt_count);
  display.setCursor(90,52);
  display.print(op_r);}
  display.display();
  }


