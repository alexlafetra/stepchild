/*
--------------------------------------
          Pin Definitions
--------------------------------------
*/

//I2C bus for the LB and screen
#define I2C_SDA 8
#define I2C_SCL 9

//buttons/inputs
/*
  These vals don't correspond to hardware pins; they're the bit place value of each buttons' state
  As stored by the controls.mainButtons byte
*/
#define NEW_BUTTON 7
#define SHIFT_BUTTON 6
#define SELECT_BUTTON 5
#define DELETE_BUTTON 4
#define LOOP_BUTTON 3
#define PLAY_BUTTON 2
#define COPY_BUTTON 1
#define MENU_BUTTON 0

#define STEPBUTTONS_DATA 12
#define LOWERBOARD_INTERRUPT_PIN 3//unused as of rn
#define BUTTONS_DATA 13
#define BUTTONS_LOAD 14
#define BUTTONS_CLOCK_IN 15
#define BUTTONS_CLOCK_ENABLE 16

//Encoders
#define B_BUTTON 17
#define B_CLOCK 18
#define B_DATA 19
#define A_BUTTON 20
#define A_CLOCK 21
#define A_DATA 22

//Joystick
#define JOYSTICK_X 27
#define JOYSTICK_Y 26

//Misc. Hardware
#define VOLTAGE_PIN 29
#define USB_PIN 24
#define ONBOARD_LED 25

/*
--------------------------------------
          Button Inputs
--------------------------------------
*/

#ifndef HEADLESS
#include "libraries/MCP23017/src/MCP23017.cpp"

/*
Super helpful thread from working with the MPR121 w/ Wire.h
https://forum.arduino.cc/t/how-to-read-a-register-value-using-the-wire-library/206123/2
*/

/*
	These addresses are hardware defined by pulling A0, A1, A2 pins high&low on the
	LowerBoard PCB. 
					|A0|A1|A2|
	LED MCP23017:	 1	0  0
	Button MCP23017: 0  1  0

	As per the MCP23017 datasheet, the I2C addresses are encoded as
	7-bit addresses:
	0100{A2,A1,A0}

	so the LEDs:
	0b0100001
	and the Buttons:
	0b0100010
*/

#define MCP23017_LED_ADDR 0b0100001
#define MCP23017_BUTTON_ADDR 0b0100010

class LowerBoard{
  public:
  MCP23017 LEDs = MCP23017(MCP23017_LED_ADDR,Wire);
  MCP23017 Buttons = MCP23017(MCP23017_BUTTON_ADDR,Wire);
  LowerBoard(){}
  unsigned short int readButtons(){
    // uint8_t buttonsA = this->Buttons.readRegister(MCP23017Register::GPIO_A);
    // uint8_t buttonsB = this->Buttons.readRegister(MCP23017Register::GPIO_B);

    // return uint16_t(buttonsB)<<8 | uint16_t(buttonsA);
    return 0;
  }
  void writeLEDs(unsigned short int state){
    // //swap led 6 and 7, bc of a wiring fuckup!
    // uint8_t led6 = 0b00100000 & uint8_t(state);
    // uint8_t led7 = 0b01000000 & uint8_t(state);
    // if(led6)
    //   state |= 0b0000000001000000;
    // else
    //   state &= 0b1111111110111111;
    // if(led7)
    //   state |= 0b0000000000100000;
    // else
    //   state &= 0b1111111111011111;

    // //Bank A is LEDs 8-15
    //   uint8_t bankAState = uint8_t(state>>8);
    // this->LEDs.writeRegister(MCP23017Register::GPIO_A,bankAState);

    // //Bank B is LEDs 0-7
    // uint8_t bankBState = uint8_t(state);
    // this->LEDs.writeRegister(MCP23017Register::GPIO_B,bankBState);
  }
  void reset(){}
  void initialize(){
  // //default startup control state
	// this->LEDs.writeRegister(MCP23017Register::IOCON, 0b00100000);
	// //setting both banks of IO to outputs
	// this->LEDs.writeRegister(MCP23017Register::IODIR_A, 0b00000000);
	// this->LEDs.writeRegister(MCP23017Register::IODIR_B, 0b00000000);
	// //writing the GPIO pins to high, which set the values the Output Latch Registers will turn on&off
	// this->LEDs.writeRegister(MCP23017Register::GPIO_A,0b11111111);
	// this->LEDs.writeRegister(MCP23017Register::GPIO_B,0b11111111);

	// //default startup settings
	// this->Buttons.writeRegister(MCP23017Register::IOCON, 0b00100000);
	// //Setting all GPIO's to inputs
	// this->Buttons.writeRegister(MCP23017Register::IODIR_A, 0b11111111);
	// this->Buttons.writeRegister(MCP23017Register::IODIR_B, 0b11111111);
	// //Disabling internal pullup resistors (since we have a pulldown resistor)
	// // this->Buttons.writeRegister(MCP23017Register::GPPU_A, 0b00000000);
	// // this->Buttons.writeRegister(MCP23017Register::GPPU_B, 0b00000000);
	// this->Buttons.writeRegister(MCP23017Register::GPPU_A, 0b11111111);
	// this->Buttons.writeRegister(MCP23017Register::GPPU_B, 0b11111111);

	// this->Buttons.writeRegister(MCP23017Register::GPIO_A,0b11111111);
	// this->Buttons.writeRegister(MCP23017Register::GPIO_B,0b11111111);

  }
  void test(){
    // uint8_t i = 0;
    // uint16_t oldState = 0;
    // while(true){
    //   // uint16_t state = this->readButtons();
    //   uint16_t state = this->Buttons.read();
    //   this->writeLEDs(state);

    //   i++;
    //   i%=16;
    // }
  }
};

LowerBoard lowerBoard;
#else
DummyLowerBoard lowerBoard;
#endif


//Holds all the hardware input functions (and the headless overloads)
//Accessed like Stepchild.buttons.buttonState(LOOP)?
//or just buttons.play(), buttons.loop()
class StepchildHardwareInput{
  public:
  //stores buttons 1-8
  uint8_t mainButtons = 0;
  //stores the 13 step buttons
  uint16_t stepButtons = 0;
  volatile int8_t counterA = 0;
  volatile int8_t counterB = 0;
  //7th bit is A, 8th bit is B
  //as in:  0b000000AB
  uint8_t encoderButtons = 0;
  int8_t joystickX = 0;
  int8_t joystickY = 0;

  //this should probably do all the hardware inits, and get passed a "settings" struct with the pins
  StepchildHardwareInput(){}

  void init(){
    #ifndef HEADLESS
    //joystick analog inputs
    pinMode(JOYSTICK_X, INPUT);
    pinMode(JOYSTICK_Y, INPUT);

    //onboard pins for LED and reading internal VCC
    pinMode(VOLTAGE_PIN, INPUT);
    pinMode(ONBOARD_LED, OUTPUT);

    //encoders
    pinMode(A_BUTTON, INPUT_PULLUP);
    pinMode(A_CLOCK, INPUT_PULLUP);
    pinMode(A_DATA, INPUT_PULLUP);
    pinMode(B_BUTTON, INPUT_PULLUP);
    pinMode(B_CLOCK, INPUT_PULLUP);
    pinMode(B_DATA, INPUT_PULLUP);

    //button bit controls.SHIFT() reg
    pinMode(BUTTONS_DATA, INPUT);
    pinMode(BUTTONS_LOAD, OUTPUT);
    pinMode(BUTTONS_CLOCK_IN, OUTPUT);
    pinMode(BUTTONS_CLOCK_ENABLE, OUTPUT);

    // CPU0 handles the encoder interrupts
    //Do you need interrupts on both of these lines? Check this!
    attachInterrupt(A_CLOCK,rotaryActionA_Handler, CHANGE);
    attachInterrupt(A_DATA,rotaryActionA_Handler, CHANGE);
    attachInterrupt(B_CLOCK,rotaryActionB_Handler, CHANGE);
    attachInterrupt(B_DATA,rotaryActionB_Handler, CHANGE);
    #endif
  }

  /*
    READING IN BUTTON STATES
  */
  //These functions update the mainButtons, stepButtons, joystickX/Y, and encoder values
  //stored as members of the hardware input class

  void readMainButtons(){
    #ifndef HEADLESS
    //Shifting in the 8 main button values from the MPX
    digitalWrite(BUTTONS_LOAD,LOW);
    digitalWrite(BUTTONS_LOAD,HIGH);
    digitalWrite(BUTTONS_CLOCK_IN, HIGH);
    digitalWrite(BUTTONS_CLOCK_ENABLE,LOW);
    this->mainButtons = shiftIn(BUTTONS_DATA, BUTTONS_CLOCK_IN, LSBFIRST);
    digitalWrite(BUTTONS_CLOCK_ENABLE, HIGH);

    //invert logic values bc low voltage ==> button is pressed
    this->mainButtons = ~this->mainButtons;

    #else
    int temp[8] = {newKeyVal,shiftKeyVal,selectKeyVal,deleteKeyVal,loopKeyVal,playKeyVal,copyKeyVal,menuKeyVal};
    uint8_t states = 0;
    for(uint8_t i = 0; i<8;i++){
      states |= temp[i]<<i;
    }
    this->mainButtons = states;
    this->counterA += headlessCounterA;
    this->counterB += headlessCounterB;
    #endif
  }
  void readStepButtons(){
    //Headless mode condition isn't needed! The headless lower board class
    //Has a fallback
    if(LEDsOn)
      this->stepButtons = lowerBoard.readButtons();
    else
      this->stepButtons = 0;
  }
  void readEncoderButtons(){
    #ifndef HEADLESS
    this->encoderButtons = (0|(digitalRead(A_BUTTON)<<1|digitalRead(B_BUTTON)));
    #else
    this->setB(encAPRESS);
    this->setA(encBPRESS);
    #endif
  }
  void readJoystick(){
    #ifndef HEADLESS
    //X
    float val = analogRead(JOYSTICK_X);
    if(val<24)
      this->joystickX = 1;
    else if(val>1000)
      this->joystickX = -1;
    else
      this->joystickX = 0;
    //Y
    val = analogRead(JOYSTICK_Y);
    if(val<24)
      this->joystickY = -1;
    else if(val>1000)
      this->joystickY = 1;
    else
      this->joystickY = 0;
    #else
    this->joystickX = xKeyVal;
    this->joystickY = yKeyVal;
    #endif
  }
  
  //reads in all the inputs!
  void readButtons(){
    #ifdef HEADLESS
    headlessCounterA = this->counterA;
    headlessCounterB = this->counterB;
    try{
      glfwPollEvents();
    }
    catch(...){
        cout<<"oh shit";
    }
    #endif
    //reading in main button states
    this->readMainButtons();
    //reading in encoder press states
    this->readEncoderButtons();
    //reading stepbuttons
    this->readStepButtons();
    //reading joystick
    this->readJoystick();
  }
  void debugPrintButtons(){
    Serial.println("-- main buttons --");
    for(uint8_t i = 0; i<8; i++){
      Serial.print((this->mainButtons>>i)&1);
    }
    Serial.print("\n");
    Serial.println("X:"+stringify(this->joystickX)+" ("+stringify(analogRead(JOYSTICK_X))+")");
    Serial.println("Y:"+stringify(this->joystickY)+" ("+stringify(analogRead(JOYSTICK_Y))+")");
    Serial.flush();
  }
  // This is called by hardware interrupts on the encoder pins
  //The hardware interrupt function is in childOS.h! void rotaryActionA_Handler() and void rotaryActionB_Handler()
  //This comes from:
  //https://www.pinteric.com/rotary.html
  //and the C++ implementation at:
  //https://github.com/RalphBacon/226-Better-Rotary-Encoder---no-switch-bounce

  int8_t readEncoder(bool which){
    int8_t r;
    int8_t l;

    static uint8_t lrmem = 3;
    static int lrsum = 0;
    const int8_t TRANS[] = {0, -1, 1, 14, 1, 0, 14, -1, -1, 14, 0, 1, 14, 1, -1, 0};

    int clk_Pin;
    int data_Pin;

    // Read BOTH pin states to deterimine validity of rotation (ie not just switch bounce)
    if(!which){
      l = digitalRead(A_CLOCK);
      r = digitalRead(A_DATA);
    }
    else{
      l = digitalRead(B_CLOCK);
      r = digitalRead(B_DATA);
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
        return 1;
    }

    /* encoder in the neutral state - anti-clockwise rotation*/
    if (lrsum == -4)
    {
        lrsum = 0;
        return -1;
    }

    // An impossible rotation has been detected - ignore the movement
    lrsum = 0;
    return 0; 
  }

  /*
    Reading the stored state of inputs
  */

  //returns a specific stepbutton state from the stored stepButtons var
  bool stepButton(uint8_t which){
    return (1<<which)&(this->stepButtons);
  }
  //returns true if any are pressed (used for the sequenceClock.BPM tap function)
  bool anyStepButtons(){
    return this->stepButtons;
  }

  //returns the stored state of one of the 8 main buttons
  bool mainButtonState(uint8_t which){
    //if it's one of the main buttons, grab it from the mainButtons var
    if(which<8)
      return (this->mainButtons>>which) & 1;
    //if it's not one of the 8, return 0 (en error)
    return 0;
  }
  bool A(){
    return (this->encoderButtons & 0b00000010);
  }
  bool B(){
    return (this->encoderButtons & 0b00000001);
  }
  bool NEW(){
    return this->mainButtonState(NEW_BUTTON);
  }
  bool SHIFT(){
    return this->mainButtonState(SHIFT_BUTTON);
  }
  bool SELECT(){
    return this->mainButtonState(SELECT_BUTTON);
  }
  bool DELETE(){
    return this->mainButtonState(DELETE_BUTTON);
  }
  bool LOOP(){
    return this->mainButtonState(LOOP_BUTTON);
  }
  bool PLAY(){
    return this->mainButtonState(PLAY_BUTTON);
  }
  bool COPY(){
    return this->mainButtonState(COPY_BUTTON);
  }
  bool MENU(){
    return this->mainButtonState(MENU_BUTTON);
  }

  bool anyActiveInputs(){
    this->readButtons();
    this->readJoystick();
    return (this->mainButtons || this->encoderButtons || this->joystickX || this->joystickY);
  }

  /*
    Setting Button States
  */

  //These probably shouldn't need to be used much, but they're here for completeness and so
  //The headless mode can set them
  void setA(bool val){
    if(val)
      this->encoderButtons |= 0b00000010;
    else
      this->encoderButtons &= 0b11111101;
  }
  void setB(bool val){
    if(val)
      this->encoderButtons |= 0b00000001;
    else
      this->encoderButtons &= 0b11111110;
  }
  void setMainButton(uint8_t which, bool val){
    if(val)
      this->mainButtons |= (val<<which);
    else
      this->mainButtons &= (~(1<<which));
  }
  void setNEW(bool val){
    this->setMainButton(NEW_BUTTON,val);
  }
  void setSHIFT(bool val){
    this->setMainButton(SHIFT_BUTTON,val);
  }
  void setSELECT(bool val){
    this->setMainButton(SELECT_BUTTON,val);
  }
  void setDELETE(bool val){
    this->setMainButton(DELETE_BUTTON,val);
  }
  void setLOOP(bool val){
    this->setMainButton(LOOP_BUTTON,val);
  }
  void setPLAY(bool val){
    this->setMainButton(PLAY_BUTTON,val);
  }
  void setCOPY(bool val){
    this->setMainButton(COPY_BUTTON,val);
  }
  void setMENU(bool val){
    this->setMainButton(MENU_BUTTON,val);
  }
  void setStepButton(uint8_t which, bool val){
    if(val)
      this->stepButtons |= (val<<which);
    else
      this->stepButtons &= (~(1<<which));
  }
};

StepchildHardwareInput controls;

bool justOneButton(){
  int score = controls.NEW()+controls.SELECT() +controls.SHIFT()+controls.DELETE()+controls.LOOP()+controls.PLAY()+controls.COPY()+controls.MENU()+controls.A()+controls.B();
  if(score == 1)
    return true;
  else
    return false;
}

#ifndef HEADLESS
void readButtons_MPX(){
  bool buttons[8];
  digitalWrite(BUTTONS_LOAD,LOW);
  digitalWrite(BUTTONS_LOAD,HIGH);
  digitalWrite(BUTTONS_CLOCK_IN, HIGH);
  digitalWrite(BUTTONS_CLOCK_ENABLE,LOW);
  unsigned char bits_buttons = shiftIn(BUTTONS_DATA, BUTTONS_CLOCK_IN, LSBFIRST);
  digitalWrite(BUTTONS_CLOCK_ENABLE, HIGH);

  //grabbing values from the byte
  for(int digit = 0; digit<8; digit++){
    buttons[digit] = (bits_buttons>>digit)&1;
  }

  if(LEDsOn){
    uint8_t capButtons = lowerBoard.readButtons();
    for(uint8_t digit = 0; digit<8; digit++){
      controls.setStepButton(digit,(capButtons>>digit)&1); 
    }
    lowerBoard.reset();
  }
  else{
    for(uint8_t digit = 0; digit<8; digit++){
      controls.setStepButton(digit,0);
    }
  }
  //setting each button accordingly
  controls.setNEW(!buttons[7]);
  controls.setSHIFT(!buttons[6]);
  controls.setSELECT(!buttons[5]);
  controls.setDELETE(!buttons[4]);
  controls.setLOOP(!buttons[3]);
  controls.setPLAY(!buttons[2]);
  controls.setCOPY(!buttons[1]);
  controls.setMENU(!buttons[0]);
}

#else
void readButtons_MPX(){
    try{
        glfwPollEvents();
    }
    catch(...){
        cout<<"oh shit";
    }
    controls.setNEW(newKeyVal);
    controls.setSHIFT(shiftKeyVal);
    controls.setSELECT(selectKeyVal);
    controls.setDELETE(deleteKeyVal);
    controls.setLOOP(loopKeyVal);
    controls.setPLAY(playKeyVal);
    controls.setCOPY(copyKeyVal);
    controls.setMENU(menuKeyVal);

    for(uint8_t i = 0; i<8; i++){
      controls.setStepButton(i,headlessStepButtons[i]);
    }
}
#endif

void resetEncoders(){
  controls.counterA = 0;
  controls.counterB = 0;
}

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


void restartDisplay(){
  #ifndef HEADLESS
  Wire.end();
  // Wire.begin();
  display.begin(SCREEN_ADDR,true);
  display.display();
  #endif
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
    if(controls.anyActiveInputs()){
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
  float val = float(analogRead(VOLTAGE_PIN))*BATTSCALE;
  return val;
}

void lightLEDs(){
  // uint16_t data = lowerBoard.getButtonStatus();
  uint8_t data = lowerBoard.readButtons(); 
  lowerBoard.writeLEDs(data);
  Serial.print("buttons:");
}


void writeLEDs(uint8_t led, bool state){
  if(LEDsOn){
    bool leds[8];
    leds[led] = state;
    writeLEDs(leds);
  }
}
//this one turns on a range of LEDS
void writeLEDs(uint8_t first, uint8_t last){
  uint8_t dat = 0;
  if(LEDsOn){
    for(int i = 0; i<8; i++){
      dat = dat<<1;
      if(i >= first && i <= last){
        dat++;
      }
    }
  }
  dat = ((dat * 0x0802LU & 0x22110LU) | (dat * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
  // sending data to controls.SHIFT() reg
  // digitalWrite(latchPin_LEDS, LOW);
  // shiftOut(dataPin_LEDS, clockPin_LEDS, MSBFIRST, dat);
  // digitalWrite(latchPin_LEDS, HIGH);
  lowerBoard.writeLEDs(dat);
}
void writeLEDs(bool leds[16]){
  uint16_t dat = 0;
  if(LEDsOn){
    for(int i = 0; i<16; i++){
      dat = dat<<1;
      if(leds[i]){
        dat++;
      }
    }
  }
  dat = ((dat * 0x0802LU & 0x22110LU) | (dat * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
  lowerBoard.writeLEDs(dat);
}

void turnOffLEDs(){
  uint8_t dat = 0;
  // sending data to controls.SHIFT() reg
  lowerBoard.writeLEDs(dat);
}


//sets all the buttons to 0 (useful for making the animations transition nicely)
void clearButtons(){
  for(int i = 0; i < 8; i++){
    controls.setStepButton(i,false);
  }
  controls.counterA = 0;
  controls.counterB = 0;
  controls.joystickX = false;
  controls.joystickY = false;
  controls.setA(false);
  controls.setB(false);
  controls.setNEW(false);
  controls.setSHIFT(false);
  controls.setSELECT(false);
  controls.setDELETE(false);
  controls.setLOOP(false);
  controls.setPLAY(false);
  controls.setCOPY(false);
  controls.setMENU(false) ;
}

#ifndef HEADLESS
//checks all inputs
bool anyActiveInputs(){
  //normal buttons
  digitalWrite(BUTTONS_LOAD,LOW);
  digitalWrite(BUTTONS_LOAD,HIGH);
  digitalWrite(BUTTONS_CLOCK_IN, HIGH);
  digitalWrite(BUTTONS_CLOCK_ENABLE,LOW);
  unsigned char bits_buttons = shiftIn(BUTTONS_DATA, BUTTONS_CLOCK_IN, LSBFIRST);
  digitalWrite(BUTTONS_CLOCK_ENABLE, HIGH);

  for(int digit = 0; digit<8; digit++){
    if(!((bits_buttons>>digit)&1))//if any of these are high, return true
      return true;
  }

  //stepButtons
  if(LEDsOn){
    digitalWrite(BUTTONS_LOAD,LOW);
    digitalWrite(BUTTONS_LOAD,HIGH);
    digitalWrite(BUTTONS_CLOCK_IN, HIGH);
    digitalWrite(BUTTONS_CLOCK_ENABLE,LOW);
    unsigned char bits_stepButtons = shiftIn(STEPBUTTONS_DATA, BUTTONS_CLOCK_IN, LSBFIRST);
    digitalWrite(BUTTONS_CLOCK_ENABLE, HIGH);
  
    for(int digit = 0; digit<8; digit++){
      if(!((bits_stepButtons>>digit)&1))
        return true;
    }
  }

  controls.readJoystick();
  if(controls.joystickX || controls.joystickY)
    return true;
  //encoder presses
  if(digitalRead(A_BUTTON) || digitalRead(B_BUTTON)){
    return true;
  }
  //encoder clicks
  if(controls.counterA || controls.counterB){
    controls.counterA = 0;
    controls.counterB = 0;
    return true;
  }

  return false;
}
#else
bool anyActiveInputs(){
  readButtons();
  if(controls.joystickX || controls.joystickY || controls.NEW() || controls.SHIFT() || controls.SELECT()  || controls.DELETE() || controls.LOOP() || controls.PLAY() || controls.COPY() || controls.MENU() || controls.counterA || controls.counterB || controls.B() || controls.A())
    return true;
  else
    return false;
}
#endif
