//buttons/inputs
#ifndef HEADLESS
  unsigned long lastTime;
  //incremented from an interrupt when rotary encoders are moved
  volatile int16_t counterA;
  volatile int16_t counterB;
#endif
int8_t x,y;
bool joy_Press,n,sel,shift,del,play,track_Press,note_Press, track_clk, note_clk,loop_Press, copy_Press, menu_Press;
bool step_buttons[8] = {false,false,false,false,false,false,false,false};


bool justOneButton(){
  int score = n+sel+shift+del+loop_Press+play+copy_Press+menu_Press+track_Press+note_Press;
  if(score == 1)
    return true;
  else
    return false;
}

bool just(bool button){
  //if more than one button is pressed, return false
  if(!justOneButton)
    return false;
  //check every button
  else{
    if(n && &n == &button)
      return true;
    else if(sel && &sel == &button)
      return true;
    else if(shift && &shift == &button)
      return true;
    else if(del && &del == &button)
      return true;
    else if(loop_Press && &loop_Press == &button)
      return true;
    else if(menu_Press && &menu_Press == &button)
      return true;
    else if(play && &play == &button)
      return true;
    else if(copy_Press && &copy_Press == &button)
      return true;
    else if(track_Press && &track_Press == &button)
      return true;
    else if(note_Press && &note_Press == &button)
      return true;
    else
      return false;
  }
}

int8_t readEncoder(bool encoder){
  int8_t r;
  int8_t l;
  
  static uint8_t lrmem = 3;
  static int lrsum = 0;
  const int8_t TRANS[] = {0, -1, 1, 14, 1, 0, 14, -1, -1, 14, 0, 1, 14, 1, -1, 0};

  int clk_Pin;
  int data_Pin;

  // Read BOTH pin states to deterimine validity of rotation (ie not just switch bounce)
  if(!encoder){
    l = digitalRead(track_clk_Pin);
    r = digitalRead(track_data_Pin);
  }
  else{
    l = digitalRead(note_clk_Pin);
    r = digitalRead(note_data_Pin);
  }

  // Move previous value 2 bits to the left and add in our new values
  lrmem = ((lrmem & 0x03) << 2) + 2 * l + r;

  // Convert the bit pattern to a movement indicator (14 = impossible, ie switch bounce)
  lrsum += TRANS[lrmem];

  /* encoder not in the neutral (detent) state */
  if (lrsum % 4 != 0)
  {
      return 0;
  }

  /* encoder in the neutral state - clockwise rotation*/
  if (lrsum == 4)
  {
      lrsum = 0;
      return -1;
  }

  /* encoder in the neutral state - anti-clockwise rotation*/
  if (lrsum == -4)
  {
      lrsum = 0;
      return 1;
  }

  // An impossible rotation has been detected - ignore the movement
  lrsum = 0;
  return 0;
}

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
    try{
        glfwPollEvents();
    }
    catch(...){
        cout<<"oh shit";
    }
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

  //grabbing values from the byte
  for(int digit = 0; digit<8; digit++){
    buttons[digit] = (bits_buttons>>digit)&1;
  }

  if(LEDsOn){
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
    try{
        glfwPollEvents();
    }
    catch(...){
        cout<<"oh shit";
    }
    n = newKeyVal;
    shift = shiftKeyVal;
    sel = selectKeyVal;
    del = deleteKeyVal;
    loop_Press = loopKeyVal;
    play = playKeyVal;
    copy_Press = copyKeyVal;
    menu_Press = menuKeyVal;
    x = xKeyVal;
    y = yKeyVal;

    for(uint8_t i = 0; i<8; i++){
      step_buttons[i] = headlessStepButtons[i];
    }
}
#endif

#ifndef HEADLESS
void readButtons(){
  // joy_Press = !digitalRead(joy_press_Pin);
  track_Press = digitalRead(encoderB_Button);
  note_Press = digitalRead(encoderA_Button);
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
