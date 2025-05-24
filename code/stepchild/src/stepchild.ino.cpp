# 1 "/var/folders/1l/rytzh70141xgg88yzt_pm9940000gn/T/tmphbgn44wu"
#include <Arduino.h>
# 1 "/Users/alex/Desktop/ongoing/Stepchild Project/stepchild/code/stepchild/src/stepchild.ino"







#include "ChildOS.h"
void headlessSetup();
void setup();
void setup1();
void testCV();
void loop();
void loop1();
#line 10 "/Users/alex/Desktop/ongoing/Stepchild Project/stepchild/code/stepchild/src/stepchild.ino"
void headlessSetup(){
  MIDI.start();

  controls.init();

  srand(1);

  loadSettings();

  sequence.init(SP404MK2_TEMPLATE);


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

void setup() {


  MIDI.start();


  Serial.begin(9600);




  USBDevice.setManufacturerDescriptor("Alex LaFetra Thompson           ");
  USBDevice.setProductDescriptor ("ChildOS V0.9.5                  ");


  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.begin();


  SPI1.setCS(OLED_CS);
  SPI1.setRX(SPI1_RX);
  SPI1.setTX(SPI1_TX);
  SPI1.setSCK(SPI1_SCK);
  SPI1.begin();


  display.init();


  CV.init();


  controls.init();


  if(tud_connected()){
    while (!TinyUSBDevice.mounted()) {
      delay(1);
    }
  }


  srand(1);

  loadSettings();


  sequence.init(SP404MK2_TEMPLATE);


  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }

  core0ready = true;
  lastTime = millis();
  graphics.bootscreen_3();
}
#endif


void setup1() {
  core1ready = true;

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


void loop1(){
  #ifdef HEADLESS
  sequenceState = PlayState(sequence.playState);
  #endif
  ledPulse(16);
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

  if(arp.isActive){
    sequence.arpLoop();
  }
}