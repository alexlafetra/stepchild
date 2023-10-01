//MIDI PINS
const uint8_t rxPin = 1;
const uint8_t txPin_1 = 0;
const uint8_t txPin_2 = 4;
const uint8_t txPin_3 = 3;
const uint8_t txPin_4 = 2;

const uint8_t Screen_SDA = 8;
const uint8_t Screen_SCL = 9;

//ENCODERS
const unsigned char note_press_Pin = 17;
const unsigned char note_clk_Pin = 18;
const unsigned char note_data_Pin = 19;

const unsigned char track_press_Pin = 20;
const unsigned char track_clk_Pin = 21;
const unsigned char track_data_Pin = 22;

//incremented from an interrupt when rotary encoders are moved
#ifndef HEADLESS
volatile int8_t counterA;
volatile int8_t counterB;
#endif

//SHIFT REGISTERS
// const unsigned char dataPin_LEDS = 9;//V0.4
const unsigned char dataPin_LEDS = 7;
const unsigned char latchPin_LEDS = 10;
const unsigned char clockPin_LEDS = 11;

const unsigned char buttons_clockEnable = 16;
const unsigned char buttons_dataIn = 13;
const unsigned char buttons_load = 14;
const unsigned char buttons_clockIn = 15;
//doubling up on load, clockin, and clockenable (since i'll read from them simultaneously)
const unsigned char stepButtons_dataIn = 12;

//JOYSTICK
const unsigned char y_Pin = 26;
const unsigned char x_Pin = 27;

//MISC. HARDWARE
const unsigned char Vpin = 29;
const unsigned char usbPin = 24;

const unsigned char ledPin = 6;
const unsigned char onboard_ledPin = 25;

//buttons/inputs
#ifndef HEADLESS
unsigned long lastTime;
#endif
int8_t x,y;
bool joy_Press,n,sel,shift,del,play,track_Press,note_Press, track_clk, note_clk,loop_Press, copy_Press, menu_Press;
bool step_buttons[8] = {false,false,false,false,false,false,false,false};

// #ifndef HEADLESS
// int8_t encoderA = 0,encoderB = 0;
// #endif

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
#endif
#ifdef HEADLESS
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

#endif
#ifdef HEADLESS
void readButtons_MPX(){
    n = newKeyVal;
    shift = shiftKeyVal;
    sel = selectKeyVal;
    del = deleteKeyVal;
    loop_Press = loopKeyVal;
    play = playKeyVal;
    copy_Press = copyKeyVal;
    menu_Press = menuKeyVal;
    glfwPollEvents();
}
#endif

#ifndef HEADLESS
void readButtons(){
  // joy_Press = !digitalRead(joy_press_Pin);
  track_Press = digitalRead(track_press_Pin);
  note_Press = digitalRead(note_press_Pin);
  readButtons_MPX();
}
#endif
#ifdef HEADLESS
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
#endif
#ifdef HEADLESS
void resetEncoders(){
  return;
}
#endif
