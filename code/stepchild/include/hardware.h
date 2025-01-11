/*
--------------------------------------
          Button Inputs
--------------------------------------
*/

/*
Super helpful thread from working with the MPR121 w/ Wire.h about using i2c SOCs
https://forum.arduino.cc/t/how-to-read-a-register-value-using-the-wire-library/206123/2
*/

/*
	These addresses are hardware defined by pulling A0, A1, A2 pins high&low on the
	LowerBoard PCB. 
					|A0|A1|A2|
	LED MCP23017:	 1	0  0
	Lower Button MCP23017: 0  1  0
  Main Button MCP23017  0 0 1

	As per the MCP23017 datasheet, the I2C addresses are encoded as
	7-bit addresses:
	0100{A2,A1,A0}

	so the LEDs:
	0b0100001
	and the Lower Buttons:
	0b0100010
  and the Main Buttons:
  0b0100100

*/

//buttons/inputs
/*
  These vals don't correspond to hardware pins; they're the bit place value of each buttons' state
  As stored by the controls.mainButtons byte
*/
#define NEW_BUTTON 0
#define SHIFT_BUTTON 1
#define SELECT_BUTTON 2
#define DELETE_BUTTON 3
#define LOOP_BUTTON 4
#define PLAY_BUTTON 5
#define COPY_BUTTON 6
#define MENU_BUTTON 7
#define B_BUTTON 8
#define A_BUTTON 9

#define MCP23017_LED_ADDR          0b0100001 //33 or 0x41
#define MCP23017_LOWER_BUTTON_ADDR 0b0100010 //34 or 0x42
#define MCP23017_MAIN_BUTTON_ADDR  0b0100100//

//range is (5-1023) aka 0-1018
const float joystickScaleFactor = float(128)/float(1018);

#include <MCP23017.h>
class LowerBoard{
  public:
  MCP23017 LEDs = MCP23017(MCP23017_LED_ADDR,Wire);
  MCP23017 Buttons = MCP23017(MCP23017_LOWER_BUTTON_ADDR,Wire);
  //stores LED State so you don't double-update
  uint16_t LEDState = 0b1111111111111111;//this starts as all on bc/that's how the boards power on!
  //stores button state so you can check to see if a button state has changed before pushing an update
  uint16_t buttonState = 0;
  LowerBoard(){}
  /*
    returns a binary number with 1's where buttons HAVE BEEN PRESSED since the last read!
    this is a little confusing, but this->buttonState always represents the actual read state of the buttons
    whereas readButtons() only returns CHANGED buttons
  */
  uint16_t readButtons(){
    return ~this->Buttons.read();
  }
  uint16_t readChangedButtons(){
    //inverted bc of pullup resitors! Button pins read LOW when switches (buttons) are closed (pressed)
    uint16_t newState = ~this->Buttons.read();
    //0 for buttons that haven't changed, 1 for buttons that have
    uint16_t changed = this->buttonState^newState;
    this->buttonState = newState;
    return this->buttonState&changed;
  }
  bool getButtonState(uint8_t which){
    return(this->buttonState>>which)&1;
  }
  //this probably will/should not be used! only changes the stored state of the stepbuttons in memory
  void setButtonState(uint8_t which, bool val){
    if(val)
      this->buttonState |= (val<<which);
    else
      this->buttonState &= (~(1<<which));
  }
  void writeLEDs(unsigned short int state){
    //if the write call is redundant, get outta here!
    if(state == this->LEDState)
      return;
    // Bank A is LEDs 8-15
    uint8_t newBankAState = uint8_t(state>>8);
    uint8_t oldState = uint8_t(this->LEDState>>8);
    //check to see if you should ignore updating this bank
    if(newBankAState != oldState)
      this->LEDs.writeRegister(MCP23017Register::GPIO_A,newBankAState);

    //Bank B is LEDs 0-7
    uint8_t newBankBState = uint8_t(state);
    oldState = uint8_t(this->LEDState);
    //check to see if you should ignore updating this bank
    if(newBankBState != oldState)
      this->LEDs.writeRegister(MCP23017Register::GPIO_B,newBankBState);
    
    this->LEDState = state;
  }
  void setLED(uint8_t which, bool newState){
    uint16_t state = this->LEDState;
    if(newState)
      state |= (1<<which);
    else
      state &= ~(1<<which);
    this->writeLEDs(state);
  }
  void reset(){}

  void initialize(){
    //default startup control state
    this->LEDs.writeRegister(MCP23017Register::IOCON, 0b00100000);
    //setting both banks of IO to outputs
    this->LEDs.writeRegister(MCP23017Register::IODIR_A, 0b00000000);
    this->LEDs.writeRegister(MCP23017Register::IODIR_B, 0b00000000);
    //writing the GPIO pins to high, which set the values the Output Latch Registers will turn on&off
    this->LEDs.writeRegister(MCP23017Register::GPIO_A,0b11111111);
    this->LEDs.writeRegister(MCP23017Register::GPIO_B,0b11111111);

    //default startup settings
    this->Buttons.writeRegister(MCP23017Register::IOCON, 0b00100000);
    //Setting all button GPIO's to inputs
    this->Buttons.writeRegister(MCP23017Register::IODIR_A, 0b11111111);
    this->Buttons.writeRegister(MCP23017Register::IODIR_B, 0b11111111);

    //Enabling internal pullup resistors
    this->Buttons.writeRegister(MCP23017Register::GPPU_A, 0b11111111);
    this->Buttons.writeRegister(MCP23017Register::GPPU_B, 0b11111111);

    this->Buttons.writeRegister(MCP23017Register::GPIO_A,0b11111111);
    this->Buttons.writeRegister(MCP23017Register::GPIO_B,0b11111111);
  }
};

//Holds all the hardware input functions (and the headless overloads)
//Accessed like Stepchild.buttons.buttonState(LOOP)?
//or just buttons.play(), buttons.loop()
class StepchildHardwareInput{
  public:

  //stores the 16 step buttons
  uint16_t stepButtonState = 0;

  volatile int8_t counterA = 0;
  volatile int8_t counterB = 0;

  int8_t joystickX = 0;//These can also be compressed
  int8_t joystickY = 0;

  LowerBoard lowerBoard;

  MCP23017 mainButtons = MCP23017(MCP23017_MAIN_BUTTON_ADDR,Wire);
  //stores buttons 1-8, and encoders A, B
  uint16_t mainButtonState = 0;

  //this should probably do all the hardware inits, and get passed a "settings" struct with the pins
  StepchildHardwareInput(){}

  void init(){
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
  

    // CPU0 handles the encoder interrupts, so make sure to .init() the StepchildHardwareInput object in setup() not setup1()
    //Do you need interrupts on both of these lines? Check this!
    attachInterrupt(A_CLOCK,rotaryActionA_Handler, CHANGE);
    attachInterrupt(A_DATA,rotaryActionA_Handler, CHANGE);
    attachInterrupt(B_CLOCK,rotaryActionB_Handler, CHANGE);
    attachInterrupt(B_DATA,rotaryActionB_Handler, CHANGE);
    this->lowerBoard.initialize();
  }

  /*
    READING IN BUTTON STATES
  */
  //These functions update the mainButtons, stepButtons, joystickX/Y, and encoder values
  //stored as members of the hardware input class

  //Reads in the main button values to the controls.mainButtonState variable
  void readMainButtons(){
    this->mainButtonState = ~this->mainButtons.read();
  }
  //calls the LowerBoard's read function, which in turn updates the LowerBoard's buttonState var
  void readStepButtons(){
    //only read it if the LB is active!
    if(LEDsOn)
      this->stepButtonState = this->lowerBoard.readChangedButtons();
    else
      this->stepButtonState = 0;
  }
  void readStepButtonState(){
    this->stepButtonState = this->lowerBoard.readButtons();
  }
  void readJoystick(){
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
  }
  int16_t getJoyX(){
    return abs(analogRead(JOYSTICK_X) * joystickScaleFactor);
  }

  int16_t getJoyY(){
    return abs(128 - constrain(analogRead(JOYSTICK_Y)*joystickScaleFactor,0,128));
  }
  //reads in all the inputs!
  void readButtons(){
    //reading in main button states
    this->readMainButtons();
    //reading stepbuttons
    this->readStepButtons();
    //reading joystick
    this->readJoystick();
  }
  void debugPrintButtons(){
    Serial.println("-- main buttons --");
    for(uint8_t i = 0; i<8; i++){
      Serial.print((this->mainButtonState>>i)&1);
    }
    // Serial.println("X:"+stringify(this->joystickX)+" ("+stringify(analogRead(JOYSTICK_X))+")");
    // Serial.println("Y:"+stringify(this->joystickY)+" ("+stringify(analogRead(JOYSTICK_Y))+")");
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
    return (this->stepButtonState>>which)&1;
  }
  //returns true if any are pressed (used for the sequenceClock.BPM tap function)
  bool anyStepButtons(){
    return this->stepButtonState;
  }
  //returns the stored state of one of the 8 main buttons
  bool getMainButtonState(uint8_t which){
    return (this->mainButtonState>>which) & 1;
  }
  bool A(){
    return !(this->getMainButtonState(A_BUTTON));
  }
  bool B(){
    return !(this->getMainButtonState(B_BUTTON));
  }
  bool NEW(){
    return this->getMainButtonState(NEW_BUTTON);
  }
  bool SHIFT(){
    return this->getMainButtonState(SHIFT_BUTTON);
  }
  bool SELECT(){
    return this->getMainButtonState(SELECT_BUTTON);
  }
  bool DELETE(){
    return this->getMainButtonState(DELETE_BUTTON);
  }
  bool LOOP(){
    return this->getMainButtonState(LOOP_BUTTON);
  }
  bool PLAY(){
    return this->getMainButtonState(PLAY_BUTTON);
  }
  bool COPY(){
    return this->getMainButtonState(COPY_BUTTON);
  }
  bool MENU(){
    return this->getMainButtonState(MENU_BUTTON);
  }

  bool anyActiveInputs(){
    this->readButtons();
    this->readStepButtonState();
    this->readJoystick();
    Serial.println("main: "+stringify(mainButtonState));
    Serial.println("X: "+stringify(joystickX));
    Serial.println("Y: "+stringify(joystickY));
    Serial.println("Stepbuttons: "+stringify(stepButtonState));
    return ((this->mainButtonState&~0b1100000000) || this->joystickX || this->joystickY || this->stepButtonState);
  }

  /*
    Setting Button States
  */

  //These probably shouldn't need to be used much, but they're here for completeness and so
  //The headless mode can set them
  void setA(bool val){
    if(val)
      this->mainButtonState |= 0b0000000100000000;
    else
      this->mainButtonState &= 0b1111111011111111;
  }
  void setB(bool val){
    if(val)
      this->mainButtonState |= 0b0000001000000000;
    else
      this->mainButtonState &= 0b1111110111111111;
  }
  void setMainButton(uint8_t which, bool val){
    if(val)
      this->mainButtonState |= (val<<which);
    else
      this->mainButtonState &= (~(1<<which));
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
    this->lowerBoard.setButtonState(which,val);
  }
  void resetEncoders(){
    this->counterA = 0;
    this->counterB = 0;
  }
  void clearButtons(){
    this->resetEncoders();
    this->mainButtonState = 0;
    this->joystickX = 0;
    this->joystickY = 0;
    this->stepButtonState = 0;
  }
  void countDownA(){
    if(counterA)
     counterA += counterA<0?1:-1;
  }
  void countDownB(){
    if(counterB)
     counterB += counterB<0?1:-1;
  }
  /*
    LEDs
  */
  //passes a 16b number to the lower board
  void writeLEDs(uint16_t state){
    this->lowerBoard.writeLEDs(state);
  }
  //turns on a range of LEDs
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
  //cycles one LED on, moving it from left to right. delayTimeMS sets the speed of the cycle in milliseconds.
  void cycleLEDs(uint16_t delayTimeMs){
    uint8_t which = (millis()/delayTimeMs)%16;
    uint16_t state = 1<<which;
    this->writeLEDs(state);
  }
  void setLED(uint8_t which, bool state){
    this->lowerBoard.setLED(which,state);
  }
};

StepchildHardwareInput controls;

void hardReset(){
  rp2040.reboot();
}

//update mode
void enterBootsel(){
  display.clearDisplay();
  display.drawBitmap(0,0,childOS,128,64,SSD1306_WHITE);
  display.display();
  reset_usb_boot(1<<PICO_DEFAULT_LED_PIN,0);
}

bool isConnectedToUSBPower(){
  return digitalRead(USB_PIN);
}

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

void maxCurrentDrawTest(){
  controls.writeLEDs(0b1111111111111111);
  display.fillRect(0,0,128,64,1);
  display.display();
  while(true){
  }
}

//pulses the onboard LED
void ledPulse(uint8_t speed){
  //use abs() so that it counts DOWN when millis() overflows into the negatives
  //Multiply by 4 so that it's 'saturated' for a while --> goes on, waits, then pulses
  analogWrite(ONBOARD_LED,4*abs(int8_t(millis()/speed)));
}

void testButton(uint8_t bit){
  do{
    controls.readMainButtons();
  }
  while(!(controls.mainButtonState&(1<<bit)));
}

/*
  Test routine that prompts the user to use each input, one by one, on the Stepchild to see if everything is working correctly
*/
void testAllInputs(){
  Serial.println("--- Input Test! ---");
  Serial.print("Press 'New': ");
  Serial.flush();
  testButton(NEW_BUTTON);
  Serial.println("NEW!");
  Serial.print("Press 'Shift': ");
  Serial.flush();
  testButton(SHIFT_BUTTON);
  Serial.println("SHIFT!");
  Serial.print("Press 'Select': ");
  Serial.flush();
  testButton(SELECT_BUTTON);
  Serial.println("SELECT!");
  Serial.print("Press 'Delete': ");
  Serial.flush();
  testButton(DELETE_BUTTON);
  Serial.println("DELETE!");
  Serial.print("Press 'Loop': ");
  Serial.flush();
  testButton(LOOP_BUTTON);
  Serial.println("LOOP!");
  Serial.print("Press 'Play': ");
  Serial.flush();
  testButton(PLAY_BUTTON);
  Serial.println("PLAY!");
  Serial.print("Press 'Copy': ");
  Serial.flush();
  testButton(COPY_BUTTON);
  Serial.println("COPY!");
  Serial.print("Press 'Menu': ");
  Serial.flush();
  testButton(MENU_BUTTON);
  Serial.println("Menu!");
  Serial.print("Press 'A': ");
  Serial.flush();
  testButton(A_BUTTON);
  Serial.println("A!");
  Serial.print("Press 'B': ");
  Serial.flush();
  testButton(B_BUTTON);
  Serial.println("B!");

  //Encoder Dials
  controls.clearButtons();
  Serial.print("Roll A: ");
  while(!controls.counterA){};
  Serial.println(controls.counterA>0?"UP!":"DOWN!");
  controls.clearButtons();
  Serial.print("Roll B: ");
  while(!controls.counterB){};
  Serial.println(controls.counterB>0?"UP!":"DOWN!");

  //Joystick
}
