#define HEADLESS

#include "../../include/childOS.h"

//cpu0 setup
void setup(){
  stepchild.midi.init();
  //setting up the pinouts and the lower board
  stepchild.buttons.init();
  //seeding random number generator
  srand(1);
  //load settings
  loadSettings();
  //setting up sequence w/ 16 tracks, 768 steps
  stepchild.init(SP404MK2_TEMPLATE);

  //set the control knobs up w/ default values
  for(uint8_t i = 0; i<16; i++){
    controlKnobs[i].cc = i+1;
  }
  core0ready = true;
  lastTime = millis();
  while(!core1ready){
  }
  
}
void loop(){
  sequenceState = PlayState(stepchild.playState);
  ledPulse(16);
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

//CPU 1 setup
void setup1() {
  while(!core0ready){
  }
  //start display
  stepchild.display.init();
  stepchild.display.setTextColor(SSD1306_WHITE);
  stepchild.display.setTextSize(1);
  graphics.bootscreen_3();
  // graphics.bootscreen_3();
  core1ready = true;
  lastTime = millis();
}

void loop1(){
  // testSolarSystem();
  mainSequence();
  screenSaverCheck();
}

void cpu0() {
  // setup sequence data
  setup();
  while (!glfwWindowShouldClose(window)) {
    loop();
  }
}

void cpu1() {
  setup1();
  while (!glfwWindowShouldClose(window)) {
    loop1();
  }
}

int main() {
  delay(1000);
  // setup graphics window
  window = initGlfw();
  while (!openGLready) {
  }

  // launch the cpu1 thread to run the clock
  thread core1(cpu0);

  // and then launch into the main thread
  cpu1();

  // wait for the other thread to exit before killing the window
  core1.join();

  // when you're ready to exit, close the window
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
