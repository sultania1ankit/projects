void logo()
{
  display.clearDisplay();
  display.setTextSize(1);
display.setTextColor(WHITE);
display.setFont(&FreeSerifBoldItalic12pt7b);
display.setCursor(2,35);
display.print("Cold Mango");
display.display();
if(digitalRead(2)==1){delay(1800);}
  }

void initial_message()
{
  display.clearDisplay();
  display.setFont();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Use left Joystick fornavigaion");
  display.println("");
  display.println("Use right Joystick   for special controls");
  display.println("");
  display.println("Enjoy......");
  display.display();
  if(digitalRead(2)==1){delay(700);}
  }

