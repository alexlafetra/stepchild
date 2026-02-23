#pragma once
#include <Wire.h>
#include <MCP23017.h>
#include "pins.h"


//buttons/inputs
/*
  These vals don't correspond to hardware pins; they're the bit place value of each buttons' state
  As stored by the controls.mainButtonState byte
*/
#define NEW_BUTTON 0
#define SHIFT_BUTTON 1
#define SELECT_BUTTON 2
#define DELETE_BUTTON 3
#define LOOP_BUTTON 4
#define PLAY_BUTTON 7
#define COPY_BUTTON 6
#define MENU_BUTTON 5
#define B_BUTTON 8
#define A_BUTTON 9

//range is (5-1023) aka 0-1018
const float joystickScaleFactor = float(128)/float(1018);

//prototypes for the rotary encoder callbacks, defined here so the control object can attach them to interrupts
void rotaryActionA_Handler();
void rotaryActionB_Handler();

enum JoystickValue:int8_t{
  CENTER = 0,
  JOY_LEFT = 1,
  JOY_RIGHT = -1,
  JOY_UP = 1,
  JOY_DOWN = -1
};

/*
Super helpful thread from working with the MPR121 w/ Wire.h about using i2c SOCs
https://forum.arduino.cc/t/how-to-read-a-register-value-using-the-wire-library/206123/2
*/

/*
  I2C addresses for each MCP23017 (set using resistors on the PCB)

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

#define MCP23017_LED_ADDR          0b0100001 //33 or 0x41
#define MCP23017_LOWER_BUTTON_ADDR 0b0100010 //34 or 0x42
#define MCP23017_MAIN_BUTTON_ADDR  0b0100100 //36 or 0x24

class StepButtonBoard{
  public:
  MCP23017 LEDs = MCP23017(MCP23017_LED_ADDR,Wire);
  MCP23017 Buttons = MCP23017(MCP23017_LOWER_BUTTON_ADDR,Wire);
  //stores LED State so you don't double-update
  uint16_t LEDState = 0b1111111111111111;//this starts as all on bc/that's how the boards power on!
  //stores button state so you can check to see if a button state has changed before pushing an update
  uint16_t buttonState = 0;
  StepButtonBoard(){}
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
    //turn the LEDs off
    this->LEDs.writeRegister(MCP23017Register::GPIO_A,0);
    this->LEDs.writeRegister(MCP23017Register::GPIO_B,0);

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

/*
  Holds all the hardware input functions (and the headless overloads)
  Accessed like stepchild.buttons.PLAY(), buttons.loop()
*/
class StepchildIO{
  public:

  //stores the 16 step buttons
  uint16_t stepButtonState = 0;

  //counters that are changed by the encoder interrupts
  volatile int8_t counterA = 0;
  volatile int8_t counterB = 0;

  bool LEDsActive = true;

  //Joystick values
  int8_t joystickX = 0;//These can also be compressed in a future update into the mainButtonState variable
  int8_t joystickY = 0;

  StepButtonBoard lowerBoard;
  MCP23017 mainButtons = MCP23017(MCP23017_MAIN_BUTTON_ADDR,Wire);
  //stores buttons 1-8, and encoders A, B
  uint16_t mainButtonState = 0;

  //this should probably do all the hardware inits, and get passed a "settings" struct with the pins
  StepchildIO(){}

  void init();

  /*
    READING IN BUTTON STATES
  */
  //These functions update the mainButtons, stepButtons, joystickX/Y, and encoder values
  //stored as members of the hardware input class

  //Reads in the main button values to the stepchild.buttons.mainButtonState variable
  void readMainButtons();
  //calls the LowerBoard's read function, which in turn updates the LowerBoard's buttonState var
  void readStepButtons();
  void readStepButtonState();
  void readJoystick();
  int16_t getJoyX();
  int16_t getJoyY();
  //reads in all the inputs!
  void readButtons();
  void readInputs();
  /*
    Reading Encoder Increment/Decrement

    This is called by hardware interrupts on the encoder pins
    The hardware interrupt function is in childOS.h! void rotaryActionA_Handler() and void rotaryActionB_Handler()
    This comes from:
    https://www.pinteric.com/rotary.html
    and the C++ implementation at:
    https://github.com/RalphBacon/226-Better-Rotary-Encoder---no-switch-bounce
  */

  int8_t readEncoder(bool which);

  /*
    Reading the stored state of inputs
  */

  //returns a specific stepbutton state from the stored stepButtons var
  bool stepButton(uint8_t which);
  //returns true if any are pressed (used for the sequenceClock.BPM tap function)
  bool anyStepButtons();
  //returns the stored state of one of the 8 main buttons
  bool getMainButtonState(uint8_t which);
  bool A();
  bool B();
  bool NEW();
  bool SHIFT();
  bool SELECT();
  bool DELETE();
  bool LOOP();
  bool PLAY();
  bool COPY();
  bool MENU();
  bool UP();
  bool DOWN();
  bool LEFT();
  bool RIGHT();
  bool anyActiveInputs();

  /*
    Setting Button States
  */

  //These probably shouldn't need to be used much, but they're here for completeness and so
  //The headless mode can set them
  void setA(bool val);
  void setB(bool val);
  void setMainButton(uint8_t which, bool val);
  void setNEW(bool val);
  void setSHIFT(bool val);
  void setSELECT(bool val);
  void setDELETE(bool val);
  void setLOOP(bool val);
  void setPLAY(bool val);
  void setCOPY(bool val);
  void setMENU(bool val);
  void setStepButton(uint8_t which, bool val);
  void resetEncoders();
  void clearButtons();
  void countDownA();
  void countDownB();
  /*
    LEDs
  */
  //passes a 16b number to the lower board
  void writeLEDs(uint16_t state);
  //turns on a range of LEDs
  void writeLEDs(uint8_t first, uint8_t last);
  void turnOffLEDs();
  //cycles one LED on, moving it from left to right. delayTimeMS sets the speed of the cycle in milliseconds.
  void cycleLEDs(uint16_t delayTimeMs);
  void setLED(uint8_t which, bool state);
};