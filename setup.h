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
  Wire.setSDA(8);

  startMIDI();
  
  //starting serial monitor output @ 9600baud
  Serial.begin(9600);

  //start the display and print an error if it doesn't work
  //(probably not very helpful)
  if(!display.begin(SSD1306_SWITCHCAPVCC, i2c_Address)) {
    Serial.println("Error starting the display!");
  }
  display.setRotation(UPRIGHT);
  display.setTextWrap(false);

  // these two string must be exactly 32 chars long
  //                                   01234567890123456789012345678912
  USBDevice.setManufacturerDescriptor("Silent Instruments Inc.         ");
  USBDevice.setProductDescriptor     ("Stepchild V0.4                  ");

  setupPins();

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
#else
void setup() {

display.display();
display.setRotation(UPRIGHT);

//seeding random number generator
srandom(1);

initSeq(16,768);
updateLEDs();

bootscreen();

setNormalMode();
turnOffLEDs();
core0ready = true;
lastTime = millis();
displaySeq();
}
#endif

//CPU 1 Setup
void setup1() {
  core1ready = true;
  while(!core0ready){
  }
}