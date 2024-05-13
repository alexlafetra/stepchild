
bool leds[16] = {false,false,false,false,false,false,false,false,
                 false,false,false,false,false,false,false,false};
bool headlessStepButtons[16] = {false,false,false,false,false,false,false,false,
                                false,false,false,false,false,false,false,false};

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
    void initialize(){}
};

