#include <Wire.h>
// #include "cap1214.h"

// #define SDA_PIN 0
// #define SCL_PIN 1
#define INTERRUPT_PIN 3

#define LB_ADDR 0x28 //0101000

/*

Address of the CAP1214 is 7 bits: 0101000
and then either a 1 or a 0: 0 for writing, 1 for reading
*/

/*
Super helpful thread from working with the MPR121 w/ Wire.h
https://forum.arduino.cc/t/how-to-read-a-register-value-using-the-wire-library/206123/2
*/

uint8_t status = 88;
//Reverse bits in a byte [CITE]
unsigned char reverseBits(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

/*
CAP1214 Class for read/interacting w Cap1214
CITE!
*/
#ifndef HEADLESS
class CAP1214{
  public:
    enum CAP1214_Register_Address{
      CAP1214_MAIN_CTRL		= 0x00,/**< Main Control.*/
			CAP1214_BTN_STATUS_1	= 0x03,
			CAP1214_BTN_STATUS_2	= 0x04,
			CAP1214_SLIDER_POS		= 0x06,/**< Slider position, Volumetric Data.*/
			CAP1214_VOLUM_STEP		= 0x09,
			CAP1214_DELTA_COUNT		= 0x10,/**< [0x10 -> 0x1D] Sensor Input Delta Count. Stores the delta count for CSx (0x10 = CS1, 0x1D = CS14)*/
			CAP1214_QUEUE_CTRL		= 0x1E,
			CAP1214_DATA_SENSITIV	= 0x1F,/**< Sensitivity Control Controls the sensitivity of the threshold and delta counts and data scaling of the base counts*/
			CAP1214_GLOBAL_CONFIG	= 0x20,
			CAP1214_SENSOR_ENABLE	= 0x21,/**< Sensor Input Enable Controls whether the capacitive touch sensor inputs are sampled*/
			CAP1214_BTN_CONFIG		= 0x22,
			CAP1214_GROUP_CFG_1		= 0x23,
			CAP1214_GROUP_CFG_2		= 0x24,
			CAP1214_FRCE_CALIB		= 0x26, //Recalibrates 0-7
			CAP1214_IRQ_ENABLE_1	= 0x27,
			CAP1214_IRQ_ENABLE_2	= 0x28,
			CAP1214_SENSOR_THRES	= 0x30,/**< [30h -> 37h] Sensor Input Threshold. Stores the delta count threshold to determine a touch for Capacitive Touch Sensor Input x (30h = 1, 37h = 8)*/
			CAP1214_GLOBAL_CONFIG_2	= 0x40,/**Controls whether or not sliders are active*/
      CAP1214_FRCE_CALIB_2  = 0x46, // Recalibrates 8-14
      CAP1214_LED_STATUS_1 = 0x60,
      CAP1214_LED_STATUS_2 = 0x61,
      CAP1214_LED_DIRECTION = 0x70,
      CAP1214_LED_OUTPUT_1 = 0x73,
      CAP1214_LED_OUTPUT_2 = 0x74
    };
    CAP1214(){

    }
    void writeByteToRegister(CAP1214_Register_Address reg, unsigned char dataByte){
      Wire.beginTransmission(LB_ADDR);
      Wire.write(reg);
      Wire.write(dataByte);
      unsigned char error = Wire.endTransmission();
      if(error){
        Serial.print("Something went wrong while talking to CAP1214! Code ");
        Serial.println(error);
      }
    }
    unsigned char readRegister(CAP1214_Register_Address reg){
      Wire.beginTransmission(LB_ADDR);
      Wire.write(reg);
      //if something goes wrong with the transmission, return
      unsigned char error = Wire.endTransmission();
      if(error){
        Serial.print("Something went wrong! Code ");
        Serial.println(error);
        return 0;
      }
      Wire.requestFrom(LB_ADDR,1);
      return Wire.read();
    }
    /*
    Sensitivity values are between 0 (most sensitive) and 111b (7)
    0 = 128x, 1 = 64x, 2 = 32x, 3 = 16x, 4 = 8x, 5 = 4x, 6 = 2x, 7 = 1x
    The data sensitivity byte is stored as (4 bits for sensitivity) (4 bits for the base shift)
    This method only updates sensitivity! Datasheets says most applications shouldn't need to update the base shift values
    */
    void setSensitivity(unsigned char val){
      //bounds check
      if(val>7)
        val = 7;
      //Combine value with the default base shift value, so that you don't overwrite it w something weird
      this->writeByteToRegister(CAP1214_DATA_SENSITIV,(val<<4)|0b1111);
      
    }
    //Clears the INT bit that gets set whenever a button is pressed
    void clearInterruptBit(){
      unsigned char status = this->readRegister(CAP1214_MAIN_CTRL);
      status = status & 0b11111110;
      this->writeByteToRegister(CAP1214_MAIN_CTRL,status);
    }
    bool checkInterruptBitAndReset(){
      return false;
    }
    //Sets the LED pins to LED driver mode
    void setLEDsAsOutputs(){
      this->writeByteToRegister(CAP1214_LED_DIRECTION,0b11111111);
    }
    void disableSliders(){
      this->writeByteToRegister(CAP1214_GLOBAL_CONFIG_2,0b00000010);
    }
    void recalibrate(){
      this->writeByteToRegister(CAP1214_FRCE_CALIB,0b11111111);
      this->writeByteToRegister(CAP1214_FRCE_CALIB_2,0b11111111);
    }

    //The Main Status Control reg bits are [X DEACT SLEEP DSLEEP X X PWR_LED INT]
    void activeMode(){
      this->writeByteToRegister(CAP1214_MAIN_CTRL,0b00000000);
    }
    void sleepMode(){
      this->writeByteToRegister(CAP1214_MAIN_CTRL,0b00100000);
    }
    void deepSleepMode(){
      this->writeByteToRegister(CAP1214_MAIN_CTRL,0b00010000);
    }
    void inactiveMode(){
      this->writeByteToRegister(CAP1214_MAIN_CTRL,0b01000000);
    }

    //Returns a 16-bit number, the first 13 bits contain the 13 button states (MSB->LSB)
    unsigned short int getButtonStatus(){
      //Check and see if the INT flag is set
      // unsigned char buttonStatus = this->readRegister(CAP1214_MAIN_CTRL);

      unsigned char buttons1 = this->readRegister(CAP1214_BTN_STATUS_1);
      unsigned char buttons2 = this->readRegister(CAP1214_BTN_STATUS_2);
      /*
      buttons1 is [UP,   DOWN,   CS6,  CS5,  CS4, CS3, CS2, CS1]
      buttons2 is [CS14, CS13,  cS12, CS11, CS10, cS9, CS8, CS7]
      So we need to shift buttons1 to get rid of up/down flags, then concatenate both bytes, then shift them back!
      This gives us [0,1,2,3,4,5,6,7,8,9,10,11,12,13,NA,NA].
      */
      unsigned short int buttonsBoth = ((buttons1 & 0b00111111)) | (buttons2<<6);
      // this->clearInterruptBit();
      return buttonsBoth;
    }
    void reset(){
      this->clearInterruptBit();
    }
    //For the stepchild, only writes to LEDs 1-8
    void writeLEDs(unsigned char status){
      writeByteToRegister(CAP1214_LED_OUTPUT_1,status);
    }
    //This is another stepchild-specific function that returns a byte representing the state of the 8 big keys
    unsigned char getStateOfMainButtons(){
      unsigned short int data = this->getButtonStatus();
      //you only want 0,2,4,5,7,9,11,12
      //So 0b0001010110101011
      unsigned char mainButtons = 0;
      // mainButtons = mainButtons | ((unsigned char)((data&0b0001000000000000)>>5));
      // mainButtons = mainButtons | ((unsigned char)((data&0b0000010000000000)>>4));
      // mainButtons = mainButtons | ((unsigned char)((data&0b0000000100000000)>>3));
      // mainButtons = mainButtons | ((unsigned char)(data&0b0000000010000000)>>3);
      // mainButtons = mainButtons | ((unsigned char)(data&0b0000000000100000)>>2);
      // mainButtons = mainButtons | ((unsigned char)(data&0b0000000000001000)>>1);
      // mainButtons = mainButtons | ((unsigned char)(data&0b0000000000000010));
      // mainButtons = mainButtons | ((unsigned char)(data&0b0000000000000001));
      //this one is bit-reversed
      mainButtons = mainButtons | ((unsigned char)((data&0b0001000000000000)>>12));
      mainButtons = mainButtons | ((unsigned char)((data&0b0000010000000000)>>9));
      mainButtons = mainButtons | ((unsigned char)((data&0b0000000100000000)>>6));
      mainButtons = mainButtons | ((unsigned char)(data&0b0000000010000000)>>4);
      mainButtons = mainButtons | ((unsigned char)(data&0b0000000000100000)>>1);
      mainButtons = mainButtons | ((unsigned char)(data&0b0000000000001000)<<2);
      mainButtons = mainButtons | ((unsigned char)(data&0b0000000000000010)<<5);
      mainButtons = mainButtons | ((unsigned char)(data&0b0000000000000001)<<7);
      return mainButtons;
    }
    void initialize(){
        // Wire.setSDA(0);
        // Wire.setSCL(1);
        // Wire.begin();
        this->setLEDsAsOutputs();
        this->disableSliders();
        this->setSensitivity(7);
        this->recalibrate();
    }

};
CAP1214 lowerBoard;
#endif

void lightLEDs(){
  // uint16_t data = lowerBoard.getButtonStatus();
  uint8_t data = lowerBoard.getStateOfMainButtons(); 
  lowerBoard.writeLEDs(data);
  Serial.print("buttons:");
  Serial.println(data);
}
