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
  sequence.init(16,768);
  //turn off LEDs (since they might be in some random configuration)
  controls.turnOffLEDs();
  //set the control knobs up w/ default values
  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }
  setNormalMode();
  core0ready = true;
  lastTime = millis();
  graphics.bootscreen_2();
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
  USBDevice.setProductDescriptor     ("Stepchild Firmware 0.9.2        ");

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
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDR);
  //Set the display rotation (which is ~technically~ upside down)
  display.setRotation(DISPLAY_UPRIGHT);
  //turn text wrapping off, so our menus look ok
  display.setTextWrap(false);

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
  sequence.init(16,768);

  //turn off LEDs (since they might be in some random configuration)
  controls.turnOffLEDs();
  //set the control knobs up w/ default values
  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }

  setNormalMode();
  core0ready = true;
  lastTime = millis();
  graphics.bootscreen_2();
}
#endif

//CPU 1 Setup
void setup1() {
  core1ready = true;
  //wait for core0 to initialize the sequence
  while(!core0ready){
  }
}

void loop() {
  // mainSequence();
  // screenSaverCheck();

  // testAllInputs();
  testCVPitches();
}

//this cpu handles time-sensitive things
void loop1(){
  #ifdef HEADLESS
  sequenceState = PlayState(sequence.playState);
  #endif
  // ledPulse(16);
  switch(sequence.playState){
    case StepchildSequence::PLAYING:
      playingLoop();
      break;
    case StepchildSequence::RECORDING:
      recordingLoop();
      break;
    case StepchildSequence::LIVELOOPING:
    case StepchildSequence::STOPPED:
      defaultLoop();
      break;
  }
  //run the arpeggiator, if it's active
  if(arp.isActive){
    arpLoop();
  }
}
