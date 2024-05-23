//code for sequencer: step child
//'board' setting needs to be the RP2040 raspberry pi pico
//will overclock at 250MH!!

//uncomment this/define elsewhere to use the capturecard version of the display library!
// #define CAPTURECARD

#include "ChildOS.h"

//CPU 0 setup
void setup() {
  #ifndef HEADLESS
  //doing the same to the screen twoWire connection
  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  #endif
  
  MIDI.start();

  #ifndef HEADLESS
  //starting serial monitor output @ 9600baud
  Serial.begin(9600);

  //start the display and throw an error if it doesn't work
  //(probably not very helpful)
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDR)) {
  }
  //Set the display rotation (which is ~technically~ upside down)
  display.setRotation(UPRIGHT);
  //turn text wrapping off, so our menus look ok
  display.setTextWrap(false);

  // these two string must be exactly 32 chars long
  //                                   01234567890123456789012345678912
  USBDevice.setManufacturerDescriptor("Unsound Systems                 ");
  USBDevice.setProductDescriptor     ("Stepchild V1.0                  ");

  #endif


  // CV.init()

  //setting up the pinouts
  controls.init();
  lowerBoard.initialize();

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
  turnOffLEDs();
  //set the control knobs up w/ default values
  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }
  controls.counterA = 0;
  controls.counterB = 0;
  setNormalMode();
  core0ready = true;
  lastTime = millis();
  bootscreen_2();
  sequence.updateLEDs();
  // lowerBoard.test();
}

//CPU 1 Setup
void setup1() {
  core1ready = true;
  //wait for core0 to initialize the sequence
  while(!core0ready){
  }
}

void loop() {
  controls.readJoystick();
  controls.readButtons();
  mainSequencerButtons();
  stepButtons();
  mainSequencerEncoders();
  defaultJoystickControls(true);
  displaySeq();
  screenSaverCheck();
}

//this cpu handles time-sensitive things
void loop1(){
  //play mode
  if(playing){
    playingLoop();
  }
  //record mode
  else if(recording){
    recordingLoop();
  }
  //default state
  else{
    defaultLoop();
  }
  if(arp.isActive){
    arpLoop();
  }
  ledPulse(16);
}
