
bool leds[8] = {false,false,false,false,false,false,false,false};
bool headlessStepButtons[8] = {false,false,false,false,false,false,false,false};

class DummyLowerBoard{
  public:
    //This enum taken from:
     enum CAP1214_Register_Address{
      CAP1214_MAIN_CTRL        = 0x00,/**< Main Control.*/
            CAP1214_BTN_STATUS_1    = 0x03,
            CAP1214_BTN_STATUS_2    = 0x04,
            CAP1214_SLIDER_POS        = 0x06,/**< Slider position, Volumetric Data.*/
            CAP1214_VOLUM_STEP        = 0x09,
            CAP1214_DELTA_COUNT        = 0x10,/**< [0x10 -> 0x1D] Sensor Input Delta Count. Stores the delta count for CSx (0x10 = CS1, 0x1D = CS14)*/
            CAP1214_QUEUE_CTRL        = 0x1E,
            CAP1214_DATA_SENSITIV    = 0x1F,/**< Sensitivity Control Controls the sensitivity of the threshold and delta counts and data scaling of the base counts*/
            CAP1214_GLOBAL_CONFIG    = 0x20,
            CAP1214_SENSOR_ENABLE    = 0x21,/**< Sensor Input Enable Controls whether the capacitive touch sensor inputs are sampled*/
            CAP1214_BTN_CONFIG        = 0x22,
            CAP1214_GROUP_CFG_1        = 0x23,
            CAP1214_GROUP_CFG_2        = 0x24,
            CAP1214_FRCE_CALIB        = 0x26, //Recalibrates 0-7
            CAP1214_IRQ_ENABLE_1    = 0x27,
            CAP1214_IRQ_ENABLE_2    = 0x28,
            CAP1214_SENSOR_THRES    = 0x30,/**< [30h -> 37h] Sensor Input Threshold. Stores the delta count threshold to determine a touch for Capacitive Touch Sensor Input x (30h = 1, 37h = 8)*/
            CAP1214_GLOBAL_CONFIG_2    = 0x40,/**Controls whether or not sliders are active*/
      CAP1214_FRCE_CALIB_2  = 0x46, // Recalibrates 8-14
      CAP1214_LED_STATUS_1 = 0x60,
      CAP1214_LED_STATUS_2 = 0x61,
      CAP1214_LED_DIRECTION = 0x70,
      CAP1214_LED_OUTPUT_1 = 0x73,
      CAP1214_LED_OUTPUT_2 = 0x74
    };
    DummyLowerBoard(){}
    void writeByteToRegister(CAP1214_Register_Address reg, unsigned char dataByte){}
    unsigned char readRegister(CAP1214_Register_Address reg){
      return 0;
    }
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
    unsigned char getStateOfMainButtons(){
      unsigned char byteHolder = 0;
      for(uint8_t i = 0; i<8; i++){
        byteHolder = byteHolder | (headlessStepButtons[i]<<(i-8));
      }
      return byteHolder;
    }
    void reset(){
    }
    void writeLEDs(unsigned char status){
      for(uint8_t i = 0; i<8; i++){
        leds[i] = (status>>i)&0b00000001;
      }
    }
    void initialize(){}
};

