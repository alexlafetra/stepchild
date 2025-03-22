//code for sequencer: step child
//'board' setting needs to be the RP2040 raspberry pi pico
//will overclock at 250MH!!

//multicore tinyUSB issue: https://github.com/hathach/tinyusb/discussions/1435
//Possible fix? https://github.com/adafruit/Adafruit_TinyUSB_Arduino/issues/238

#include "ChildOS.h"

void headlessSetup(){
  MIDI.start();
  //setting up the pinouts and the lower board
  controls.init();
  //seeding random number generator
  srand(1);
  //load settings
  loadSettings();
  //setting up sequence w/ 16 tracks, 768 steps
  // sequence.init(16,768); 
  sequence.init(SP404MK2_TEMPLATE);
  //turn off LEDs (since they might be in some random configuration)
  controls.turnOffLEDs();
  //set the control knobs up w/ default values
  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }
  sequence.setNormalMode();
  core0ready = true;
  lastTime = millis();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  // graphics.bootscreen_2();
}
#ifndef HEADLESS
//CPU 0 setup
void setup() {
  
  //setup MIDI ports/IO
  MIDI.start();

  //starting serial monitor output @ 9600baud for USB communication
  Serial.begin(9600);
  //Set USB device info
  // these two strings must be exactly 32 characters long:
  //                                   0123456789ABCDEF0123456789ABCDEF
  USBDevice.setManufacturerDescriptor("Alex LaFetra Thompson           ");
  // USBDevice.setProductDescriptor     ("Stepchild Firmware 0.9.5        ");
  USBDevice.setProductDescriptor     ("ChildOS V0.9.5                  ");

  //start I^2C bus to communicate with MCP23017's
  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.begin();

  //Start SPI to communicate w/ screen
  SPI1.setCS(OLED_CS);
  SPI1.setRX(SPI1_RX);
  SPI1.setTX(SPI1_TX);
  SPI1.setSCK(SPI1_SCK);
  SPI1.begin();

  //start display
  display.init();

  //setup CV pins, frequency
  CV.init();

  //setting up the pinouts and the lower board
  controls.init();

  //wait for tinyUSB to connect, if the USB port is connected (not sure if this is necessary, need to test)
  if(tud_connected()){
    while (!TinyUSBDevice.mounted()) {
      delay(1);
    }
  }

  //seeding random number generator
  srand(1);
  //load settings
  loadSettings();
  //setting up sequence w/ 16 tracks, 768 steps
  // sequence.init(16,768);
  sequence.init(SP404MK2_TEMPLATE);
  // sequence.init(GENERIC_KEYBOARD_TEMPLATE);

  //turn off LEDs (since they might be in some random configuration)
  controls.turnOffLEDs();
  //set the control knobs up w/ default values
  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }

  sequence.setNormalMode();
  core0ready = true;
  lastTime = millis();
  // graphics.bootscreen_2();
}
#endif

//CPU 1 Setup
void setup1() {
  core1ready = true;
  //wait for core0 to initialize the sequence
  while(!core0ready){
  }
}

void testCV(){
  uint16_t TESTVAL = 65535;
  uint16_t gateVal = 0;
  while(true){
    analogWrite(CV1_PIN,TESTVAL);
    analogWrite(CV2_PIN,TESTVAL);
    analogWrite(CV3_PIN,TESTVAL);
    TESTVAL++;
    TESTVAL%=PWM_MAX_VAL;
    // delayMicroseconds(10);
    if(utils.itsbeen(100)){
      lastTime = millis();
      gateVal = 65535-gateVal;
    }
    display.clearDisplay();
    printSmall(0,0,stringify(TESTVAL),1);
    display.display();
  }
}

void loop() {
  mainSequence();
  screenSaverCheck();
}

//this cpu handles time-sensitive things
void loop1(){
  #ifdef HEADLESS
  sequenceState = PlayState(sequence.playState);
  #endif
  ledPulse(16);
  switch(sequence.playState){
    case PLAYING:
      sequence.playingLoop();
      break;
    case RECORDING:
      sequence.recordingLoop();
      break;
    case LIVELOOPING:
    case STOPPED:
      sequence.defaultLoop();
      break;
  }
  //run the arpeggiator, if it's active
  if(arp.isActive){
    sequence.arpLoop();
  }
}
