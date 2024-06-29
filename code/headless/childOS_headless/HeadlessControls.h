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
    this->counterA += headlessCounterA;
    this->counterB += headlessCounterB;
  }
  void readStepButtons(){
    //Headless mode condition isn't needed! The headless lower board class
    //Has a fallback
    if(LEDsOn)
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
  void readButtons(){
    headlessCounterA = this->counterA;
    headlessCounterB = this->counterB;
    try{
      glfwPollEvents();
    }
    catch(...){
        cout<<"oh shit";
    }
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
