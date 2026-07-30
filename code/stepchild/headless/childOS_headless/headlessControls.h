/*

  Code for the stepchild's inputs and pin defs

*/

/*
--------------------------------------
          Pin Definitions
--------------------------------------
*/

#pragma once

//CV Outputs
#define CV4_PIN 17
#define CV3_PIN 5
#define CV2_PIN 6
#define CV1_PIN 15

//Joystick
#define JOYSTICK_X 27
#define JOYSTICK_Y 26

//Misc. Hardware (These are onboard pins)
#define VOLTAGE_PIN 29
#define USB_PIN 24
#define ONBOARD_LED 25

#define PICO_DEFAULT_LED_PIN 0

//buttons/inputs
/*
  These vals don't correspond to hardware pins; they're the bit place value of each buttons' state
  As stored by the stepchild.buttons.mainButtons byte
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

#include "Arduino.h"

class DummyLowerBoard{
  public:
  DummyLowerBoard();
    /*
    Sensitivity values are between 0 (most sensitive) and 111b (7)
    0 = 128x, 1 = 64x, 2 = 32x, 3 = 16x, 4 = 8x, 5 = 4x, 6 = 2x, 7 = 1x
    The data sensitivity byte is stored as (4 bits for sensitivity) (4 bits for the base shift)
    This method only updates sensitivity! Datasheets says most applications shouldn't need to update the base shift values
    */
  void setSensitivity(unsigned char val);
    //Clears the INT bit that gets set whenever a button is pressed
  void clearInterruptBit();
  bool checkInterruptBitAndReset();
    //Sets the LED pins to LED driver mode
  void setLEDsAsOutputs();
  void disableSliders();
  void recalibrate();
    
    //The Main Status Control reg bits are [X DEACT SLEEP DSLEEP X X PWR_LED INT]
  void activeMode();
  void sleepMode();
  void deepSleepMode();
  void inactiveMode();

    //Returns a 16-bit number, the first 13 bits contain the 13 button states (MSB->LSB)
  unsigned short int getButtonStatus();
  unsigned char readButtons();
  void reset();
  void writeLEDs(unsigned short int status);
  void setLED(uint8_t which, bool newState);
  void initialize();
};

class StepchildIO{
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
  StepchildIO();
  void init();
  void readMainButtons();
  void readStepButtons();
  void readEncoderButtons();
  void readJoystick();
  float getJoyX();
  float getJoyY();

  void readButtons();
  void readInputs();
  void debugPrintButtons();
  int8_t readEncoder(bool which);
  bool stepButton(uint8_t which);
  bool anyStepButtons();
  bool mainButtonState(uint8_t which);
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
  void setA(bool val);
  void setB(bool val);
  void countDownB();
  void countDownA();
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
  void writeLEDs(uint16_t state);
  void writeLEDs(uint8_t first, uint8_t last);
  void turnOffLEDs();
  void cycleLEDs(uint16_t delayTimeMs);
  void setLED(uint8_t which, bool state);
};

//HeadlessHardwareInput controls;
