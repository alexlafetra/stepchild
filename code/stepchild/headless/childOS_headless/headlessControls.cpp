
/*

  Code for the stepchild's inputs and pin defs

*/

/*
--------------------------------------
          Pin Definitions
--------------------------------------
*/

#include "headlessControls.h"
#include "headlessOpenGL.h"

extern int xKeyVal;//arrow keys
extern int yKeyVal;

//encoders
extern int encASTATE;//0, //k
extern int encAPRESS;//o
extern int encBSTATE;//p, ,
extern int encBPRESS;//l
extern int headlessCounterA, headlessCounterB;

//buttons
extern int newKeyVal;//1
extern int shiftKeyVal;//q
extern int selectKeyVal;//a
extern int deleteKeyVal;//z

extern int loopKeyVal;//x
extern int playKeyVal;//c
extern int copyKeyVal;//v
extern int menuKeyVal;//b

extern bool leds[16];
extern bool headlessStepButtons[16];

  	DummyLowerBoard::DummyLowerBoard(){}
    /*
    Sensitivity values are between 0 (most sensitive) and 111b (7)
    0 = 128x, 1 = 64x, 2 = 32x, 3 = 16x, 4 = 8x, 5 = 4x, 6 = 2x, 7 = 1x
    The data sensitivity byte is stored as (4 bits for sensitivity) (4 bits for the base shift)
    This method only updates sensitivity! Datasheets says most applications shouldn't need to update the base shift values
    */
    void DummyLowerBoard::setSensitivity(unsigned char val){
    }
    //Clears the INT bit that gets set whenever a button is pressed
    void DummyLowerBoard::clearInterruptBit(){}
    bool DummyLowerBoard::checkInterruptBitAndReset(){
      return true;
    }
    //Sets the LED pins to LED driver mode
    void DummyLowerBoard::setLEDsAsOutputs(){
    }
    void DummyLowerBoard::disableSliders(){
    }
    void DummyLowerBoard::recalibrate(){
    }
    
    //The Main Status Control reg bits are [X DEACT SLEEP DSLEEP X X PWR_LED INT]
    void DummyLowerBoard::activeMode(){
    }
    void DummyLowerBoard::sleepMode(){
    }
    void DummyLowerBoard::deepSleepMode(){
    }
    void DummyLowerBoard::inactiveMode(){
    }

    //Returns a 16-bit number, the first 13 bits contain the 13 button states (MSB->LSB)
    unsigned short int DummyLowerBoard::getButtonStatus(){
      return 0;
    }
    unsigned char DummyLowerBoard::readButtons(){
      unsigned char byteHolder = 0;
      for(uint8_t i = 0; i<8; i++){
        byteHolder = byteHolder | (headlessStepButtons[i]<<(i-8));
      }
      return byteHolder;
    }
    void DummyLowerBoard::reset(){
    }
    void DummyLowerBoard::writeLEDs(unsigned short int status){
      for(uint8_t i = 0; i<16; i++){
        leds[i] = (status>>i)&1;
      }
    }
    void DummyLowerBoard::setLED(uint8_t which, bool newState){
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
    void DummyLowerBoard::initialize(){}

  StepchildIO::StepchildIO(){}
  void StepchildIO::init(){}
  void StepchildIO::readMainButtons(){
    int temp[8] = {newKeyVal,shiftKeyVal,selectKeyVal,deleteKeyVal,loopKeyVal,playKeyVal,copyKeyVal,menuKeyVal};
    uint8_t states = 0;
    for(uint8_t i = 0; i<8;i++){
      states |= temp[7-i]<<i;
    }
    this->mainButtons = states;
  }
  void StepchildIO::readStepButtons(){
    //Headless mode condition isn't needed! The headless lower board class
    //Has a fallback
    if(LEDsActive)
      this->stepButtons = this->lowerBoard.readButtons();
    else
      this->stepButtons = 0;
  }
  void StepchildIO::readEncoderButtons(){
    this->setB(encAPRESS);
    this->setA(encBPRESS);
  }
  void StepchildIO::readJoystick(){
    this->joystickX = xKeyVal;
    this->joystickY = yKeyVal;
  }
  float StepchildIO::getJoyX(){
      return 0;
  }
  float StepchildIO::getJoyY(){
      return 0;
  }

  void StepchildIO::readButtons(){
    try{
      glfwPollEvents();
    }
    catch(...){
        std::cout<<"oh shit";
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
  }
  void StepchildIO::readInputs(){
    readButtons();
    readJoystick();
  }
  void StepchildIO::debugPrintButtons(){}
  int8_t StepchildIO::readEncoder(bool which){return 0;}
  bool StepchildIO::stepButton(uint8_t which){
    return (this->stepButtons>>which)&1;
  }
  bool StepchildIO::anyStepButtons(){
    return this->stepButtons;
  }
  bool StepchildIO::mainButtonState(uint8_t which){
    if(which<8)
      return (this->mainButtons>>which) & 1;
    return 0;
  }
  bool StepchildIO::A(){
    return (this->encoderButtons & 0b00000010);
  }
  bool StepchildIO::B(){
    return (this->encoderButtons & 0b00000001);
  }
  bool StepchildIO::NEW(){
    return this->mainButtonState(NEW_BUTTON);
  }
  bool StepchildIO::SHIFT(){
    return this->mainButtonState(SHIFT_BUTTON);
  }
  bool StepchildIO::SELECT(){
    return this->mainButtonState(SELECT_BUTTON);
  }
  bool StepchildIO::DELETE(){
    return this->mainButtonState(DELETE_BUTTON);
  }
  bool StepchildIO::LOOP(){
    return this->mainButtonState(LOOP_BUTTON);
  }
  bool StepchildIO::PLAY(){
    return this->mainButtonState(PLAY_BUTTON);
  }
  bool StepchildIO::COPY(){
    return this->mainButtonState(COPY_BUTTON);
  }
  bool StepchildIO::MENU(){
    return this->mainButtonState(MENU_BUTTON);
  }
  bool StepchildIO::UP(){
    return joystickY == 1;
  }
  bool StepchildIO::DOWN(){
    return joystickY == -1;
  }
  bool StepchildIO::LEFT(){
    return joystickX == 1;
  }
  bool StepchildIO::RIGHT(){
    return joystickX == -1;
  }
  bool StepchildIO::anyActiveInputs(){
    this->readButtons();
    this->readStepButtons();
    this->readJoystick();
    return (this->mainButtons || this->encoderButtons || this->joystickX || this->joystickY || this->stepButtons);
  }
  void StepchildIO::setA(bool val){
    if(val)
      this->encoderButtons |= 0b00000010;
    else
      this->encoderButtons &= 0b11111101;
  }
  void StepchildIO::setB(bool val){
    if(val)
      this->encoderButtons |= 0b00000001;
    else
      this->encoderButtons &= 0b11111110;
  }
    void StepchildIO::countDownB(){
        if(counterB)
         counterB += counterB<0?1:-1;
    }
    void StepchildIO::countDownA(){
        if(counterA)
         counterA += counterA<0?1:-1;
    }
  void StepchildIO::setMainButton(uint8_t which, bool val){
    if(val)
      this->mainButtons |= (val<<which);
    else
      this->mainButtons &= (~(1<<which));
  }
  void StepchildIO::setNEW(bool val){
    this->setMainButton(NEW_BUTTON,val);
  }
  void StepchildIO::setSHIFT(bool val){
    this->setMainButton(SHIFT_BUTTON,val);
  }
  void StepchildIO::setSELECT(bool val){
    this->setMainButton(SELECT_BUTTON,val);
  }
  void StepchildIO::setDELETE(bool val){
    this->setMainButton(DELETE_BUTTON,val);
  }
  void StepchildIO::setLOOP(bool val){
    this->setMainButton(LOOP_BUTTON,val);
  }
  void StepchildIO::setPLAY(bool val){
    this->setMainButton(PLAY_BUTTON,val);
  }
  void StepchildIO::setCOPY(bool val){
    this->setMainButton(COPY_BUTTON,val);
  }
  void StepchildIO::setMENU(bool val){
    this->setMainButton(MENU_BUTTON,val);
  }
  void StepchildIO::setStepButton(uint8_t which, bool val){
    if(val)
      this->stepButtons |= (val<<which);
    else
      this->stepButtons &= (~(1<<which));
  }
  void StepchildIO::resetEncoders(){
    this->counterA = 0;
    this->counterB = 0;
  }
  void StepchildIO::clearButtons(){
    this->resetEncoders();
    this->mainButtons = 0;
    this->encoderButtons = 0;
    this->joystickX = 0;
    this->joystickY = 0;
    this->stepButtons = 0;
  }
  void StepchildIO::writeLEDs(uint16_t state){
    this->lowerBoard.writeLEDs(state);
  }
  void StepchildIO::writeLEDs(uint8_t first, uint8_t last){
    uint16_t data = 0;
    for(uint8_t i = 0; i<(last-first); i++){
      data |= 1<<(first+i);
    }
    this->writeLEDs(data);
  }
  void StepchildIO::turnOffLEDs(){
    this->lowerBoard.writeLEDs(0);
  }
  void StepchildIO::cycleLEDs(uint16_t delayTimeMs){
    uint8_t which = (millis()/delayTimeMs)%16;
    uint16_t state = 1<<which;
    this->writeLEDs(state);
  }
    void StepchildIO::setLED(uint8_t which, bool state){
      this->lowerBoard.setLED(which,state);
    }

//HeadlessHardwareInput controls;
