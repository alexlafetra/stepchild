//buttons/inputs
#define NEW_BUTTON 0
#define SHIFT_BUTTON 1
#define SELECT_BUTTON 2
#define DELETE_BUTTON 3
#define LOOP_BUTTON 4
#define PLAY_BUTTON 5
#define COPY_BUTTON 6
#define MENU_BUTTON 7

#define A_BUTTON 8
#define B_BUTTON 9

#define STEPBUTTONS_DATA 12
#define BUTTONS_DATA 13
#define BUTTONS_LOAD 14
#define BUTTONS_CLOCK_IN 15
#define BUTTONS_CLOCK_ENABLE 16

#define JOYSTICK_X 26
#define JOYSTICK_Y 27

//Holds all the hardware input functions (and the headless overloads)
//Accessed like Stepchild.buttons.buttonState(LOOP)?
//or just buttons.play(), buttons.loop()
class StepchildInput{
  public:

  //this should probably do all the hardware inits, and get passed a "settings" struct with the pins
  StepchildInput(){

  }
  //stores buttons 1-8
  uint8_t topButtons = 0;
  //stores the 13 step buttons
  uint16_t stepButtons = 0;

  //reads in inputs
  void readTopButtons(){
    digitalWrite(BUTTONS_LOAD,LOW);
    digitalWrite(BUTTONS_LOAD,HIGH);
    digitalWrite(BUTTONS_CLOCK_IN, HIGH);
    digitalWrite(BUTTONS_CLOCK_ENABLE,LOW);
    topButtons = shiftIn(BUTTONS_DATA, BUTTONS_CLOCK_IN, LSBFIRST);
    digitalWrite(BUTTONS_CLOCK_ENABLE, HIGH);
  }
  void readStepButtons(){
    digitalWrite(BUTTONS_LOAD,LOW);
    digitalWrite(BUTTONS_LOAD,HIGH);
    digitalWrite(BUTTONS_CLOCK_IN, HIGH);
    digitalWrite(BUTTONS_CLOCK_ENABLE,LOW);
    stepButtons = shiftIn(STEPBUTTONS_DATA, BUTTONS_CLOCK_IN, LSBFIRST);
    digitalWrite(BUTTONS_CLOCK_ENABLE, HIGH);
  }
  bool buttonState(uint8_t which){
    return topButtons>>which & 1;
  }
  bool stepButtonState(uint8_t which){
    return stepButtons>>which & 1;
  }
  bool n(){
    return buttonState(NEW_BUTTON);
  }
};

StepchildInput buttons;

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
    uint8_t capButtons = lowerBoard.getStateOfMainButtons();
    for(uint8_t digit = 0; digit<8; digit++){
      step_buttons[digit] = (capButtons>>digit)&1;
    }
    lowerBoard.reset();
  }
  else{
    for(uint8_t digit = 0; digit<8; digit++){
      step_buttons[digit] = 0;
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

void restartSerial(unsigned int baud){
  Serial.end();
  delay(1000);
  Serial.begin(baud);
}

void hardReset(){
  rp2040.reboot();
}

void resetUSBInterface(){
}

//update mode
void enterBootsel(){
  // display.clearDisplay();
  // display.drawBitmap(0,0,childOS,128,64,SSD1306_WHITE);
  // display.display();
  reset_usb_boot(1<<PICO_DEFAULT_LED_PIN,0);
}



//Sleep---------------------------
//pause core1 (you need to call this fn from core0)
bool sleeping = false;
#ifdef HEADLESS
  void enterSleepMode(){
    return;
  }
  void leaveSleepMode(){
    return;
  }
#else
void leaveSleepMode(){
  //vv these break it? for some reason
  // Serial.end();
  // startSerial();
  restartDisplay();
  rp2040.resumeOtherCore();
  // rp2040.restartCore1(); <-- this also breaks it
  while(!core1ready){//wait for core1
    // //Serial.println("waiting");
    // Serial.flush();
  }
  // //Serial.println("ready");
  // Serial.flush();
  return;
}
void enterSleepMode(){
  //turn off power consuming things
  display.clearDisplay();
  display.display();
  clearButtons();
  turnOffLEDs();
  
  sleeping = true;
  //wait for core1 to sleep
  while(core1ready){
  }
   //idle core1
  rp2040.idleOtherCore();
  //sleep until a change is detected on 
  while(sleeping){
    long time  = millis();
    sleep_ms(1000);
    if(anyActiveInputs()){
      sleeping = false;
    }
    // //Serial.println("sleeping...");
    // //Serial.print(millis()-time);
    // Serial.flush();
  }
  //wake up
  // //Serial.println("gooood morning");
  // Serial.flush();
  leaveSleepMode();
}
#endif


#define BATTSCALE 0.00966796875
//3.0*3.3/1024.0;
//idk why ^^this isn't 4095.0, but it ain't

float getBattLevel(){
  //So when USB is in, VSYS is ~5.0
  //When all 3AA's are in, if they're 1.5v batts VSYS is ~4.5
  //But if they're 1.2v batts VSYS is ~3.6;
  float val = float(analogRead(Vpin))*BATTSCALE;
  return val;
}