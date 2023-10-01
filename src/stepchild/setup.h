#ifndef HEADLESS

//CPU 0 setup
void setup() {
  //--------------------------------------------------------------------
  //                              Hardware
  //--------------------------------------------------------------------
  //setting MIDI serial ports to non-default pins so they don't conflict
  //with other stepchild features
  Serial1.setRX(rxPin);
  Serial1.setTX(txPin_1);
  Serial2.setTX(txPin_2);
  //doing the same to the screen twoWire connection
  Wire.setSDA(Screen_SDA);
  Wire.setSCL(Screen_SCL);

  startMIDI();
  
  //starting serial monitor output @ 9600baud
  Serial.begin(9600);

  //start the display and print an error if it doesn't work
  //(probably not very helpful)
  if(!display.begin(SSD1306_SWITCHCAPVCC, i2c_Address)) {
    Serial.println("Error starting the display!");
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

  //wait for tinyUSB to connect, if the USB port is connected (not sure if this is necessary, need to test)
  if(tud_connected()){
    while (!TinyUSBDevice.mounted()) {
      delay(1);
    }
  }

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
  bootscreen();
}
#endif

//CPU 1 Setup
void setup1() {
  core1ready = true;
  while(!core0ready){
  }
}