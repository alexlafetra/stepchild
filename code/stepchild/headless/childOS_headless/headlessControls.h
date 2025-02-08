/*

  Code for the stepchild's inputs and pin defs

*/

/*
--------------------------------------
          Pin Definitions
--------------------------------------
*/

//MIDI I/O
#define MIDI_OUT_1 0
#define MIDI_IN 1
#define MIDI_OUT_4 2
#define MIDI_OUT_3 3
#define MIDI_OUT_2 4

//CV Outputs
#define CV4_PIN 17
#define CV3_PIN 5
#define CV2_PIN 6
#define CV1_PIN 15

//I2C bus for the MCP23017's
#define I2C_SDA 8
#define I2C_SCL 9

//SPI Bus for the screen
#define SPI1_RX 12
#define SPI1_TX 11
#define OLED_CS 13
#define OLED_DC 16
#define OLED_RESET 7
#define SPI1_SCK 10

//Encoders (these are hardware interrupts on the Pico)
#define B_CLOCK 19
#define B_DATA 18
#define A_CLOCK 22
#define A_DATA 21

//Joystick
#define JOYSTICK_X 27
#define JOYSTICK_Y 26

//Misc. Hardware (These are onboard pins)
#define VOLTAGE_PIN 29
#define USB_PIN 24
#define ONBOARD_LED 25

//Extra GPIO Access
#define AUX_GPIO 20
#define AUX_ADC 28

#define PICO_DEFAULT_LED_PIN 0

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
#define A_BUTTON 8
#define B_BUTTON 9

class DummyLowerBoard{
  public:
    DummyLowerBoard(){}
    /*
    Sensitivity values are between 0 (most sensitive) and 111b (7)
    0 = 128x, 1 = 64x, 2 = 32x, 3 = 16x, 4 = 8x, 5 = 4x, 6 = 2x, 7 = 1x
    The data sensitivity byte is stored as (4 bits for sensitivity) (4 bits for the base shift)
    This method only updates sensitivity! Datasheets says most applications shouldn't need to update the base shift values
    */
    void setSensitivity(unsigned char val){
    }
    //Clears the INT bit that gets set whenever a button is pressed
    void clearInterruptBit(){}
    bool checkInterruptBitAndReset(){
      return true;
    }
    //Sets the LED pins to LED driver mode
    void setLEDsAsOutputs(){
    }
    void disableSliders(){
    }
    void recalibrate(){
    }
    
    //The Main Status Control reg bits are [X DEACT SLEEP DSLEEP X X PWR_LED INT]
    void activeMode(){
    }
    void sleepMode(){
    }
    void deepSleepMode(){
    }
    void inactiveMode(){
    }

    //Returns a 16-bit number, the first 13 bits contain the 13 button states (MSB->LSB)
    unsigned short int getButtonStatus(){
      return 0;
    }
    unsigned char readButtons(){
      unsigned char byteHolder = 0;
      for(uint8_t i = 0; i<8; i++){
        byteHolder = byteHolder | (headlessStepButtons[i]<<(i-8));
      }
      return byteHolder;
    }
    void reset(){
    }
    void writeLEDs(unsigned short int status){
      for(uint8_t i = 0; i<16; i++){
        leds[i] = (status>>i)&1;
      }
    }
    void setLED(uint8_t which, bool newState){
        uint16_t state = 0;
        for(uint8_t i = 0; i<16; i++){
            if(leds[i]){
                state |= 1<<i;
            }
        }
      if(newState)
        state |= (1<<which);
      else
        state &= ~(1<<which);
      this->writeLEDs(state);
    }
    void initialize(){}
};

class HeadlessHardwareInput{
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
    bool LEDsActive = true;
  DummyLowerBoard lowerBoard;
  HeadlessHardwareInput(){}
  void init(){}
  void readMainButtons(){
    int temp[8] = {newKeyVal,shiftKeyVal,selectKeyVal,deleteKeyVal,loopKeyVal,playKeyVal,copyKeyVal,menuKeyVal};
    uint8_t states = 0;
    for(uint8_t i = 0; i<8;i++){
      states |= temp[7-i]<<i;
    }
    this->mainButtons = states;
  }
  void readStepButtons(){
    //Headless mode condition isn't needed! The headless lower board class
    //Has a fallback
    if(LEDsActive)
      this->stepButtons = this->lowerBoard.readButtons();
    else
      this->stepButtons = 0;
  }
  void readEncoderButtons(){
    this->setB(encAPRESS);
    this->setA(encBPRESS);
  }
  void readJoystick(){
    this->joystickX = xKeyVal;
    this->joystickY = yKeyVal;
  }
  float getJoyX(){
      return 0;
  }
  float getJoyY(){
      return 0;
  }

  void readButtons(){
    try{
      glfwPollEvents();
    }
    catch(...){
        cout<<"oh shit";
    }
      counterA += headlessCounterA;
      counterB += headlessCounterB;
      encASTATE += headlessCounterA;
      encBSTATE += headlessCounterB;
      headlessCounterA = 0;
      headlessCounterB = 0;
    this->readMainButtons();
    this->readEncoderButtons();
    this->readStepButtons();
    this->readJoystick();
  }
  void debugPrintButtons(){}
  int8_t readEncoder(bool which){return 0;}
  bool stepButton(uint8_t which){
    return (this->stepButtons>>which)&1;
  }
  bool anyStepButtons(){
    return this->stepButtons;
  }
  bool mainButtonState(uint8_t which){
    if(which<8)
      return (this->mainButtons>>which) & 1;
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
  bool UP(){
    return joystickY == 1;
  }
  bool DOWN(){
    return joystickY == -1;
  }
  bool LEFT(){
    return joystickX == 1;
  }
  bool RIGHT(){
    return joystickX == -1;
  }
  bool anyActiveInputs(){
    this->readButtons();
    this->readStepButtons();
    this->readJoystick();
    return (this->mainButtons || this->encoderButtons || this->joystickX || this->joystickY || this->stepButtons);
  }
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
    void countDownB(){
        if(counterB)
         counterB += counterB<0?1:-1;
    }
    void countDownA(){
        if(counterA)
         counterA += counterA<0?1:-1;
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
  void resetEncoders(){
    this->counterA = 0;
    this->counterB = 0;
  }
  void clearButtons(){
    this->resetEncoders();
    this->mainButtons = 0;
    this->encoderButtons = 0;
    this->joystickX = 0;
    this->joystickY = 0;
    this->stepButtons = 0;
  }
  void writeLEDs(uint16_t state){
    this->lowerBoard.writeLEDs(state);
  }
  void writeLEDs(uint8_t first, uint8_t last){
    uint16_t data = 0;
    for(uint8_t i = 0; i<(last-first); i++){
      data |= 1<<(first+i);
    }
    this->writeLEDs(data);
  }
  void turnOffLEDs(){
    this->lowerBoard.writeLEDs(0);
  }
  void cycleLEDs(uint16_t delayTimeMs){
    uint8_t which = (millis()/delayTimeMs)%16;
    uint16_t state = 1<<which;
    this->writeLEDs(state);
  }
    void setLED(uint8_t which, bool state){
      this->lowerBoard.setLED(which,state);
    }
};

HeadlessHardwareInput controls;
//
////update mode
//void enterBootsel(){
//  display.clearDisplay();
//  display.drawBitmap(0,0,childOS,128,64,SSD1306_WHITE);
//  display.display();
//  reset_usb_boot(1<<PICO_DEFAULT_LED_PIN,0);
//}
//
//bool isConnectedToUSBPower(){
//  return digitalRead(USB_PIN);
//}
//
//#define BATTSCALE 0.00966796875
////3.0*3.3/1024.0;
////idk why ^^this isn't 4095.0, but it ain't
//
//float getBattLevel(){
//  //So when USB is in, VSYS is ~5.0
//  //When all 3AA's are in, if they're 1.5v batts VSYS is ~4.5
//  //But if they're 1.2v batts VSYS is ~3.6;
//  float val = float(analogRead(VOLTAGE_PIN))*BATTSCALE;
//  return val;
//}
//
//void maxCurrentDrawTest(){
//  controls.writeLEDs(0b1111111111111111);
//  display.fillRect(0,0,128,64,1);
//  display.display();
//  while(true){
//  }
//}
//
////pulses the onboard LED
//void ledPulse(uint8_t speed){
//  //use abs() so that it counts DOWN when millis() overflows into the negatives
//  //Multiply by 4 so that it's 'saturated' for a while --> goes on, waits, then pulses
//  analogWrite(ONBOARD_LED,4*abs(int8_t(millis()/speed)));
//}
