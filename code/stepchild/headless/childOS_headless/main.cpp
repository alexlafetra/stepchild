#ifndef HEADLESS
#warning "HEADLESS not defined in build settings>preprocessor macros, files might not compile for headless mode correctly"
#define HEADLESS
#endif

#include "Stepchild.h"
#include "mainSequence.h"
#include "screensavers.h"

extern GLFWwindow* window;
extern bool openGLready;
extern PlayState sequenceState;

using namespace std;

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
void setup(){

}
void setup1(){
  //start display
  stepchild.display.init();
  graphics.bootscreen_3();
  stepchild.lastTime = millis();
}
void loop(){
  sequenceState = PlayState(stepchild.playState);
  // stepchild.midi.processCore1Messages();
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

void cpu_0(){
  setup();
  while(true){
    loop();
  }
}

void loop1(){
  mainSequence();
  screenSaverCheck();
}

int main() {
  stepchild.init();
  window = initGlfw();
  while (!openGLready) {
  }
  // launch the cpu1 thread to run the clock
  thread core1(cpu_0);

  // and then launch into the main thread
  emscripten_set_main_loop(loop1, 0, 1); // 0 = use requestAnimationFrame, 1 = infinite loop takeover
  return 0;
}
#else
//cpu0 setup
void setup(){
//  stepchild.init();
}

void loop(){
  sequenceState = PlayState(stepchild.playState);
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
  //start display
  stepchild.display.init();
  // graphics.bootscreen_3();
  stepchild.lastTime = millis();
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
  //this was needed for glfw to launch a window... nobody knew why! Xcode seems to work without the delay now
//  delay(1000);
  stepchild.init();
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
#endif
