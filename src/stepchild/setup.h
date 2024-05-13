#ifndef HEADLESS
//CPU 0 setup
void setup() {
  //--------------------------------------------------------------------
  //                              Hardware
  //--------------------------------------------------------------------
  //doing the same to the screen twoWire connection
  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);

  MIDI.start();
  
  //starting serial monitor output @ 9600baud
  // Serial.begin(921600);
  Serial.begin(9600);

  //start the display and throw an error if it doesn't work
  //(probably not very helpful)
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDR)) {
    Serial.println("Display won't turn on!");
    Serial.flush();
  }

  //Set the display rotation (which is ~technically~ upside down)
  display.setRotation(UPRIGHT);
  //turn text wrapping off, so our menus look ok
  display.setTextWrap(false);

  // these two string must be exactly 32 chars long
  //                                   01234567890123456789012345678912
  USBDevice.setManufacturerDescriptor("Unsound Systems                 ");
  USBDevice.setProductDescriptor     ("Stepchild V1.0                  ");

  // CV.init()

  //wait for tinyUSB to connect, if the USB port is connected (not sure if this is necessary, need to test)
  if(tud_connected()){
    while (!TinyUSBDevice.mounted()) {
      delay(1);
    }
  }

  //setting up the pinouts
  controls.init();
  lowerBoard.initialize();
  // lowerBoard.test();

  //--------------------------------------------------------------------//
  //                              Software                              //
  //--------------------------------------------------------------------//
  //seeding random number generator
  srand(1);
  //setting up sequence w/ 16 tracks, 768 steps
  initSeq(16,768);
  //turn off LEDs (since they might be in some random configuration)
  turnOffLEDs();
  //set the control knobs up w/ default values
  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }
  counterA = 0;
  counterB = 0;
  setNormalMode();

  core0ready = true;
  lastTime = millis();
  bootscreen_2();
  // testCVPitches();
}
#endif
#ifdef HEADLESS
void setup(){
  MIDI.start();

  //Set the display rotation (which is ~technically~ upside down)
  display.setRotation(UPRIGHT);
  //turn text wrapping off, so our menus look ok
  display.setTextWrap(false);

  //--------------------------------------------------------------------//
  //                              Software                              //
  //--------------------------------------------------------------------//
  //seeding random number generator
  srand(1);
  //setting up sequence w/ 16 tracks, 768 steps
  initSeq(16,768);
  //turn off LEDs (since they might be in some random configuration)
  turnOffLEDs();
  //set the control knobs up w/ default values
  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }
  counterA = 0;
  counterB = 0;
  setNormalMode();

  core0ready = true;
  lastTime = millis();
  bootscreen_2();
  // while(true){
  //   display.clearDisplay();
  //   printCursive(10,29,"unsound instruments",1);
  //   display.display();
  // }
  updateLEDs();
}
#endif

//CPU 1 Setup
void setup1() {
  core1ready = true;
  while(!core0ready){
  }
}