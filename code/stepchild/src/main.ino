//code for sequencer: step child
//'board' setting needs to be the RP2040 raspberry pi pico
//will overclock at 250MH!!

//multicore tinyUSB issue: https://github.com/hathach/tinyusb/discussions/1435
//Possible fix? https://github.com/adafruit/Adafruit_TinyUSB_Arduino/issues/238

/*

  Header file for ChildOS

*/
#ifndef HEADLESS

#define FIRMWARE_VERSION '0.9.2'

#include "Stepchild.h"
#include "mainSequence.h"
#include "screensavers.h"

using namespace std;

//CPU 1 setup
void setup1() {
  //start display
  stepchild.display.init();
  // graphics.bootscreen_3();
  stepchild.lastTime = millis();
}

void loop1() {
  mainSequence();
  screenSaverCheck();
}

//CPU 0 Setup
void setup() {
  stepchild.init();
}

//this cpu handles time-sensitive things
void loop(){
  // ledPulse(16);
  stepchild.midi.processCore1Messages();
  stepchild.midi.read();
  switch(stepchild.playState){
    case PLAYING:
      stepchild.playingLoop();
      break;
    case RECORDING:
      stepchild.recordingLoop();
      break;
    case STOPPED:
      stepchild.defaultLoop();
      break;
  }
  //run the arpeggiator, if it's active
  if(stepchild.arpeggiator.isActive){
    stepchild.arpLoop();
  }
}


#endif
