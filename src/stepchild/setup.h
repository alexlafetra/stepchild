void setupPins(){
  #ifndef HEADLESS
  //joystick analog inputs
  pinMode(x_Pin, INPUT);
  pinMode(y_Pin, INPUT);

  //onboard pins for LED and reading internal VCC
  pinMode(Vpin, INPUT);

  pinMode(onboard_ledPin, OUTPUT);

  //encoders
  pinMode(encoderB_Button, INPUT_PULLUP);
  pinMode(encoderA_Button, INPUT_PULLUP);
  pinMode(track_clk_Pin, INPUT_PULLUP);
  pinMode(note_clk_Pin, INPUT_PULLUP);
  pinMode(track_data_Pin, INPUT_PULLUP);
  pinMode(note_data_Pin, INPUT_PULLUP);

  //button bit shift reg
  pinMode(buttons_load, OUTPUT);
  pinMode(buttons_clockEnable, OUTPUT);
  pinMode(buttons_clockIn, OUTPUT);
  pinMode(buttons_dataIn, INPUT);

  //LED bit shift reg
  pinMode(latchPin_LEDS, OUTPUT);
  pinMode(clockPin_LEDS, OUTPUT);
  pinMode(dataPin_LEDS, OUTPUT);

  // CPU0 handles the encoder interrupts
  attachInterrupt(track_clk_Pin,rotaryActionA_Handler, CHANGE);
  attachInterrupt(track_data_Pin,rotaryActionA_Handler, CHANGE);
  attachInterrupt(note_clk_Pin,rotaryActionB_Handler, CHANGE);
  attachInterrupt(note_data_Pin,rotaryActionB_Handler, CHANGE);
  #endif
}

#ifndef HEADLESS
//CPU 0 setup
void setup() {
  //--------------------------------------------------------------------
  //                              Hardware
  //--------------------------------------------------------------------
  //doing the same to the screen twoWire connection
  Wire.setSDA(Screen_SDA);
  Wire.setSCL(Screen_SCL);

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
  USBDevice.setManufacturerDescriptor("Silent Instruments Inc.         ");
  USBDevice.setProductDescriptor     ("Stepchild V0.4                  ");

  //setting up the pinout
  setupPins();

  // CV.init();

  //wait for tinyUSB to connect, if the USB port is connected (not sure if this is necessary, need to test)
  if(tud_connected()){
    while (!TinyUSBDevice.mounted()) {
      delay(1);
    }
  }
  lowerBoard.initialize();

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