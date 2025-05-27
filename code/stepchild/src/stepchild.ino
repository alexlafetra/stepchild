//code for sequencer: step child
//'board' setting needs to be the RP2040 raspberry pi pico
//will overclock at 250MH!!

//multicore tinyUSB issue: https://github.com/hathach/tinyusb/discussions/1435
//Possible fix? https://github.com/adafruit/Adafruit_TinyUSB_Arduino/issues/238

#include "ChildOS.h"

void headlessSetup(){
  MIDI.init();
  //setting up the pinouts and the lower board
  controls.init();
  //seeding random number generator
  srand(1);
  //load settings
  loadSettings();
  //setting up sequence w/ 16 tracks, 768 steps
  sequence.init(SP404MK2_TEMPLATE);

  //set the control knobs up w/ default values
  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }
  sequence.setNormalMode();
  core0ready = true;
  lastTime = millis();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  graphics.bootscreen_3();
}
#ifndef HEADLESS

//CPU 1 setup
void setup1() {
  while(!core0ready){
  }

  //setting up the pinouts and the lower board
  controls.init();


  //seeding random number generator
  srand(1);
  //load settings
  loadSettings();

  //setting up sequence w/ 16 tracks, 768 steps
  sequence.init(SP404MK2_TEMPLATE);

  //set the control knobs up w/ default values
  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }

  //start display
  display.init();
  graphics.bootscreen_3();

  core1ready = true;
  lastTime = millis();
}
#endif

void loop1() {
  mainSequence();
  screenSaverCheck();
}

//CPU 0 Setup
void setup() {
  //setup MIDI ports/IO
  MIDI.init();

  //starting serial monitor output @ 9600baud for USB communication
  Serial.begin(9600);

  //Set USB device info
  // these two strings must be exactly 32 characters long:
  //                                   0123456789ABCDEF0123456789ABCDEF
  USBDevice.setManufacturerDescriptor("Alex LaFetra Thompson           ");
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

  //setup CV pins, frequency
  CV.init();

  //wait for tinyUSB to connect, if the USB port is connected (not sure if this is necessary, need to test)
  if(tud_connected()){
    while (!TinyUSBDevice.mounted()) {
      delay(1);
    }
  }

  //core 0 can start now
  core0ready = true;

  //wait for core0 to initialize the sequence
  while(!core1ready){
  }
}
//this cpu handles time-sensitive things
void loop(){
  #ifdef HEADLESS
  sequenceState = PlayState(sequence.playState);
  #endif
  ledPulse(16);
  MIDI.processCore1Messages();
  MIDI.read();
  switch(sequence.playState){
    case PLAYING:
      sequence.playingLoop();
      break;
    case RECORDING:
      sequence.recordingLoop();
      break;
    case STOPPED:
      sequence.defaultLoop();
      break;
  }
  //run the arpeggiator, if it's active
  if(arp.isActive){
    sequence.arpLoop();
  }
}
