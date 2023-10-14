//buttons/inputs
#ifndef HEADLESS
  unsigned long lastTime;
  //incremented from an interrupt when rotary encoders are moved
  volatile int8_t counterA;
  volatile int8_t counterB;
#endif
int8_t x,y;
bool joy_Press,n,sel,shift,del,play,track_Press,note_Press, track_clk, note_clk,loop_Press, copy_Press, menu_Press;
bool step_buttons[8] = {false,false,false,false,false,false,false,false};


#ifndef HEADLESS
void readJoystick(){
  float yVal = analogRead(x_Pin);
  float xVal = analogRead(y_Pin);
  x = 0;
  y = 0;
  if (xVal > 1000) {
    y = 1;
  }
  else if(xVal < 24) {
    y = -1;
  }
  if (yVal > 1000) {
    x = -1;
  }
  else if(yVal < 24) {
    x = 1;
  }
}
#else
void readJoystick(){
    glfwPollEvents();
    x = xKeyVal;
    y = yKeyVal;
}
#endif

#ifndef HEADLESS
void readButtons_MPX(){
  bool buttons[8];
  digitalWrite(buttons_load,LOW);
  digitalWrite(buttons_load,HIGH);
  digitalWrite(buttons_clockIn, HIGH);
  digitalWrite(buttons_clockEnable,LOW);
  unsigned char bits_buttons = shiftIn(buttons_dataIn, buttons_clockIn, LSBFIRST);
  digitalWrite(buttons_clockEnable, HIGH);
  // Serial.println(bits_buttons);
  // Serial.flush();

  //grabbing values from the byte
  for(int digit = 0; digit<8; digit++){
    buttons[digit] = (bits_buttons>>digit)&1;
  }

  if(stepButtonsAreActive){
    digitalWrite(buttons_load,LOW);
    digitalWrite(buttons_load,HIGH);
    digitalWrite(buttons_clockIn, HIGH);
    digitalWrite(buttons_clockEnable,LOW);
    unsigned char bits_stepButtons = shiftIn(stepButtons_dataIn, buttons_clockIn, MSBFIRST);
    digitalWrite(buttons_clockEnable, HIGH);
    for(int digit = 0; digit<8; digit++){
      step_buttons[digit] = !((bits_stepButtons>>digit)&1);
    }
  }

  //setting each button accordingly
  n = !buttons[7];
  shift = !buttons[6];
  sel = !buttons[5];
  del = !buttons[4];
  loop_Press = !buttons[3];
  play = !buttons[2];
  copy_Press = !buttons[1];
  menu_Press = !buttons[0];
}
#else
void readButtons_MPX(){
    n = newKeyVal;
    shift = shiftKeyVal;
    sel = selectKeyVal;
    del = deleteKeyVal;
    loop_Press = loopKeyVal;
    play = playKeyVal;
    copy_Press = copyKeyVal;
    menu_Press = menuKeyVal;
    try{
        glfwPollEvents();
    }
    catch(...){
        cout<<"oh shit";
    }
}
#endif

#ifndef HEADLESS
void readButtons(){
  // joy_Press = !digitalRead(joy_press_Pin);
  track_Press = digitalRead(track_press_Pin);
  note_Press = digitalRead(note_press_Pin);
  readButtons_MPX();
}
#else
void readButtons(){
  note_Press = encAPRESS;
  track_Press = encBPRESS;
  encAPRESS = 0;
  encBPRESS = 0;
  readButtons_MPX();
}
#endif

#ifndef HEADLESS
void resetEncoders(){
  counterA = 0;
  counterB = 0;
}
#else
void resetEncoders(){
  counterA = 0;
  counterB = 0;
  return;
}
#endif
