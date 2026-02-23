#include "pins.h"
#include "StepchildIO.h"

/*
--------------------------------------

  Code for the stepchild's inputs 

--------------------------------------
*/
/*
  Holds all the hardware input functions (and the headless overloads)
  Accessed like controls.PLAY(), buttons.loop()
*/
void StepchildIO::init(){
    //joystick analog inputs
    pinMode(JOYSTICK_X, INPUT);
    pinMode(JOYSTICK_Y, INPUT);

    //onboard pins for LED and reading internal VCC
    pinMode(VOLTAGE_PIN, INPUT);
    pinMode(ONBOARD_LED, OUTPUT);

    //encoders
    pinMode(A_CLOCK, INPUT_PULLUP);
    pinMode(A_DATA, INPUT_PULLUP);
    pinMode(B_CLOCK, INPUT_PULLUP);
    pinMode(B_DATA, INPUT_PULLUP);

    //default startup settings
    mainButtons.writeRegister(MCP23017Register::IOCON, 0b00100000);
    //Setting all button GPIO's to inputs
    mainButtons.writeRegister(MCP23017Register::IODIR_A, 0b11111111);
    mainButtons.writeRegister(MCP23017Register::IODIR_B, 0b11111111);
    //Enabling internal pullup resistors
    mainButtons.writeRegister(MCP23017Register::GPPU_A, 0b11111111);
    mainButtons.writeRegister(MCP23017Register::GPIO_A,0b11111111);
    //slight error on PCB... encoder buttons are pulled DOWN via a resistor (shouldn't be!)
    mainButtons.writeRegister(MCP23017Register::GPPU_B, 0b11111111);
    mainButtons.writeRegister(MCP23017Register::GPIO_B,0b11111111);  

    //CPU1 handles the encoder interrupts, so make sure to .init() the StepchildHardwareInput object in setup1() not setup()
    //Do you need interrupts on both of these lines? Check this!
    attachInterrupt(A_CLOCK,rotaryActionA_Handler, CHANGE);
    attachInterrupt(A_DATA,rotaryActionA_Handler, CHANGE);
    attachInterrupt(B_CLOCK,rotaryActionB_Handler, CHANGE);
    attachInterrupt(B_DATA,rotaryActionB_Handler, CHANGE);

    this->lowerBoard.initialize();
    this->turnOffLEDs();
}

  /*
    READING IN BUTTON STATES
  */
  //These functions update the mainButtons, stepButtons, joystickX/Y, and encoder values
  //stored as members of the hardware input class

  //Reads in the main button values to the controls.mainButtonState variable
void StepchildIO::readMainButtons(){
    this->mainButtonState = ~this->mainButtons.read();
}
  //calls the LowerBoard's read function, which in turn updates the LowerBoard's buttonState var
void StepchildIO::readStepButtons(){
    //only read it if the LB is active!
    if(this->LEDsActive)
      this->stepButtonState = this->lowerBoard.readChangedButtons();
    else
      this->stepButtonState = 0;
}
void StepchildIO::readStepButtonState(){
    this->stepButtonState = this->lowerBoard.readButtons();
}
void StepchildIO::readJoystick(){
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
      this->joystickY = JOY_DOWN;
    else if(val>1000)
      this->joystickY = JOY_UP;
    else
      this->joystickY = CENTER;
}
int16_t StepchildIO::getJoyX(){
    return abs(analogRead(JOYSTICK_X) * joystickScaleFactor);
}

int16_t StepchildIO::getJoyY(){
    return abs(128 - constrain(analogRead(JOYSTICK_Y)*joystickScaleFactor,0,128));
}
//reads in all the inputs!
void StepchildIO::readButtons(){
    //reading in main button states
    this->readMainButtons();
    //reading stepbuttons
    this->readStepButtons();
}
void StepchildIO::readInputs(){
    //reading joystick
    this->readJoystick();
    this->readButtons();
}
  /*
    Reading Encoder Increment/Decrement

    This is called by hardware interrupts on the encoder pins
    The hardware interrupt function is in childOS.h! void StepchildIO::rotaryActionA_Handler() and void StepchildIO::rotaryActionB_Handler()
    This comes from:
    https://www.pinteric.com/rotary.html
    and the C++ implementation at:
    https://github.com/RalphBacon/226-Better-Rotary-Encoder---no-switch-bounce
  */

int8_t StepchildIO::readEncoder(bool which){
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
bool StepchildIO::stepButton(uint8_t which){
    return (this->stepButtonState>>which)&1;
}
  //returns true if any are pressed (used for the sequenceClock.BPM tap function)
bool StepchildIO::anyStepButtons(){
    return this->stepButtonState;
}
  //returns the stored state of one of the 8 main buttons
bool StepchildIO::getMainButtonState(uint8_t which){
    return (this->mainButtonState>>which)&1;
}
bool StepchildIO::A(){
    return this->getMainButtonState(A_BUTTON);
}
bool StepchildIO::B(){
    return this->getMainButtonState(B_BUTTON);
}
bool StepchildIO::NEW(){
    return this->getMainButtonState(NEW_BUTTON);
}
bool StepchildIO::SHIFT(){
    return this->getMainButtonState(SHIFT_BUTTON);
}
bool StepchildIO::SELECT(){
    return this->getMainButtonState(SELECT_BUTTON);
}
bool StepchildIO::DELETE(){
    return this->getMainButtonState(DELETE_BUTTON);
}
bool StepchildIO::LOOP(){
    return this->getMainButtonState(LOOP_BUTTON);
}
bool StepchildIO::PLAY(){
    return this->getMainButtonState(PLAY_BUTTON);
}
bool StepchildIO::COPY(){
    return this->getMainButtonState(COPY_BUTTON);
}
bool StepchildIO::MENU(){
    return this->getMainButtonState(MENU_BUTTON);
}
bool StepchildIO::UP(){
    return joystickY == JOY_UP;
}
bool StepchildIO::DOWN(){
    return joystickY == JOY_DOWN;
}
bool StepchildIO::LEFT(){
    return joystickX == JOY_LEFT;
}
bool StepchildIO::RIGHT(){
    return joystickX == JOY_RIGHT;
}

bool StepchildIO::anyActiveInputs(){
    this->readButtons();
    this->readStepButtonState();
    this->readJoystick();
    return ((this->mainButtonState&~0b1100000000) || this->joystickX || this->joystickY || this->stepButtonState);
}

  /*
    Setting Button States
  */

  //These probably shouldn't need to be used much, but they're here for completeness and so
  //The headless mode can set them
void StepchildIO::setA(bool val){
    if(val)
      this->mainButtonState |= 0b0000000100000000;
    else
      this->mainButtonState &= 0b1111111011111111;
}
void StepchildIO::setB(bool val){
    if(val)
      this->mainButtonState |= 0b0000001000000000;
    else
      this->mainButtonState &= 0b1111110111111111;
}
void StepchildIO::setMainButton(uint8_t which, bool val){
    if(val)
      this->mainButtonState |= (val<<which);
    else
      this->mainButtonState &= (~(1<<which));
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
    this->lowerBoard.setButtonState(which,val);
}
void StepchildIO::resetEncoders(){
    this->counterA = 0;
    this->counterB = 0;
}
void StepchildIO::clearButtons(){
    this->resetEncoders();
    this->mainButtonState = 3<<8;
    this->joystickX = 0;
    this->joystickY = 0;
    this->stepButtonState = 0;
}
void StepchildIO::countDownA(){
    if(counterA)
     counterA += counterA<0?1:-1;
}
void StepchildIO::countDownB(){
    if(counterB)
     counterB += counterB<0?1:-1;
}
  /*
    LEDs
  */
  //passes a 16b number to the lower board
void StepchildIO::writeLEDs(uint16_t state){
    this->lowerBoard.writeLEDs(state);
}
  //turns on a range of LEDs
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
  //cycles one LED on, moving it from left to right. delayTimeMS sets the speed of the cycle in milliseconds.
void StepchildIO::cycleLEDs(uint16_t delayTimeMs){
    uint8_t which = (millis()/delayTimeMs)%16;
    uint16_t state = 1<<which;
    this->writeLEDs(state);
}
void StepchildIO::setLED(uint8_t which, bool state){
    this->lowerBoard.setLED(which,state);
}