//code for sequencer: step child
//'board' setting needs to be the RP2040 raspberry pi pico
//will overclock at 250MH!!

//multicore tinyUSB issue: https://github.com/hathach/tinyusb/discussions/1435
//Possible fix? https://github.com/adafruit/Adafruit_TinyUSB_Arduino/issues/238

#include "ChildOS.h"

//CPU 0 setup
void setup() {
  #ifndef HEADLESS
  //doing the same to the screen twoWire connection
  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);

  SPI.setCS(OLED_CS);
  SPI.setRX(SPI0_RX);
  SPI.setTX(SPI0_TX);
  SPI.setSCK(SPI0_SCK);
  
  #endif
  
  MIDI.start();

  #ifndef HEADLESS
  //starting serial monitor output @ 9600baud
  Serial.begin(9600);
  SPI.begin();

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDR);
  //Set the display rotation (which is ~technically~ upside down)
  display.setRotation(DISPLAY_UPRIGHT);
  //turn text wrapping off, so our menus look ok
  display.setTextWrap(false);

  // these two strings must be exactly 32 characters long:
  //                                   0123456789ABCDEF0123456789ABCDEF
  USBDevice.setManufacturerDescriptor("Alex LaFetra Thompson           ");
  USBDevice.setProductDescriptor     ("Stepchild Firmware 0.9.2        ");
  #endif

  CV.init();

  //setting up the pinouts and the lower board
  controls.init();

  #ifndef HEADLESS
  //wait for tinyUSB to connect, if the USB port is connected (not sure if this is necessary, need to test)
  if(tud_connected()){
    while (!TinyUSBDevice.mounted()) {
      delay(1);
    }
  }
  #endif

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

//CPU 1 Setup
void setup1() {
  core1ready = true;
  //wait for core0 to initialize the sequence
  while(!core0ready){
  }
}

void loop() {
  mainSequence();
  screenSaverCheck();
  // analogWrite(CV1_PIN,65535);
  // analogWrite(CV2_PIN,65535);
  // analogWrite(CV3_PIN,65535);
  // testCVPitches();
}

//this cpu handles time-sensitive things
void loop1(){
  #ifdef HEADLESS
  sequenceState = PlayState(sequence.playState);
  #endif
  ledPulse(16);
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
  if(arp.isActive){
    arpLoop();
  }
}
