
/*
----------------------
        SLEEP
----------------------

RP2040 has both a sleep mode AND a dormant mode, dormant is even more low power.
Sleep example:
https://github.com/raspberrypi/pico-playground/blob/master/sleep/hello_sleep/hello_sleep.c
Dormant example:
https://github.com/raspberrypi/pico-playground/blob/master/sleep/hello_dormant/hello_dormant.c
*/

/*
normal sleep mode keeps core 1 running as usual (so midi i/o still works)
turns off LEDs and the screen, and sleeps core0 while intermittently checking buttons
this is kind of a "lite" sleep mode, only saves power by turning off screen and LEDs
but both CPU's are still running at full speed

This COULD actually "turn off" the screen via SSD1306_command but it looks like I'd need to physically reset it
using the reset pin... which isn't always exposed on the breakout boards
*/

// uint16_t sleepTime = 30000;//10s
uint16_t sleepTime = 30000;//10s

//60,000ms = 1min
const unsigned long long deepSleepTime = 600000;//120s

void leaveSleepMode(){
  lastTime = millis();
}
void enterSleepMode(){
  display.clearDisplay();
  display.display();
  controls.clearButtons();
  controls.turnOffLEDs();
  while(true){
    sleep_ms(500);
    if(controls.anyActiveInputs()){
      leaveSleepMode();
      break;
    }
  }

}

//Deep sleep actually pauses core1 and sleeps core0... not sure how energy saving this is
// tho compared to fully going dormant
#ifdef HEADLESS
  void enterDeepSleepMode(){
    return;
  }
  void leaveDeepSleepMode(){
    return;
  }
#else
void leaveDeepSleepMode(){
  rp2040.resumeOtherCore();
}
void enterDeepSleepMode(){
  //turn off power consuming things
  display.clearDisplay();
  display.display();
  controls.clearButtons();
  controls.turnOffLEDs();

  //idle core1
  rp2040.idleOtherCore();
  //sleep until a change is detected on 
  while(true){
    sleep_ms(1000);
    //when input is detected, wake up
    if(controls.anyActiveInputs()){
      leaveSleepMode();
      return;
    }
  }
}
#endif

//screenSavers
void screenSaver_cassette(){
  WireFrame cassette = makeCassette();
  cassette.scale = 4;
  uint8_t rotationAmount = 0;
  bool done = false;
  while(true){
    display.clearDisplay();
    cassette.render();
    display.display();
    controls.cycleLEDs(150);
    cassette.rotate(1,1);
    rotationAmount++;
    if(rotationAmount>360){
      done = true;
      rotationAmount = 0;
    }
    if(controls.anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(utils.itsbeen(deepSleepTime)){
      return;
    }
  }
}

void screenSaver_ripples(){
  const uint8_t maxReps = 5;//for how many rings
  const uint8_t spacing = 10;//for the spacing
  const uint8_t xCoord = 64;
  const uint8_t  yCoord = 32;
  animOffset = 0;
  bool done = false;
  while(true){
    controls.cycleLEDs(150);
    display.clearDisplay();
    if(animOffset<=32){
      if(animOffset>8){//drops and reflection
        display.drawCircle(xCoord, animOffset-8, 1+sin(animOffset), SSD1306_WHITE);
        display.drawCircle(xCoord, screenHeight-(animOffset-8), 1+sin(animOffset), SSD1306_WHITE);
      }
      display.drawCircle(xCoord, animOffset, 3+sin(animOffset), SSD1306_WHITE);
      display.drawCircle(xCoord, screenHeight-animOffset, 3+sin(animOffset), SSD1306_WHITE);
    }
    else if(animOffset>yCoord){
      int reps = (animOffset-yCoord)/spacing+1;
      if(reps>maxReps){
        reps = maxReps;
      }
      for(int i = 0; i<reps; i++){
        if(animOffset/3-spacing*i+sin(animOffset)*(i%2)<(screenWidth+16))
            graphics.drawEllipse(xCoord, yCoord, animOffset/3-spacing*i+sin(animOffset)*(i%2), animOffset/8-spacing*i/3,SSD1306_WHITE);
      }
    }
    display.display();
    if(animOffset<yCoord){
      animOffset+=5;
    }
    else
      animOffset+=6;
    if(animOffset>=8*spacing*maxReps/3+8*32+28){
      animOffset = 0;
      done = true;
    }
    if(controls.anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(utils.itsbeen(deepSleepTime)){
      return;
    }
  }
}

void screenSaver_moon(){
  bool waxing = true;
  bool done = false;
  int16_t counter = 0;
  while(true){
    controls.cycleLEDs(150);
    float phase = counter;
    if(phase >= 86){
      waxing =! waxing;
      done = true;
    }
    else{
      done = false;
    }
    display.clearDisplay();
    graphics.drawMoon(phase, waxing);
    display.display();
    counter+=waxing?1:-1;;
    if(controls.anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(utils.itsbeen(deepSleepTime)){
      return;
    }
  }
}

void screenSaver_template(){
  bool done = false;
  //loop that runs while the screensaver is active
  while(true){
    display.clearDisplay();
    //put your rendering code here!
    display.display();
    //checking if any buttons are pressed and breaking out of the loop if so
    if(controls.anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(utils.itsbeen(deepSleepTime)){
      return;
    }
  }
}

void screenSaver_die(){
  bool done = false;
  WireFrame cube = makeCube(20);
  cube.xPos = 64;
  cube.yPos = 32;
  cube.scale = 1.8;
  WireFrame dots = makeDieDots(64,32,0,1.8);
  dots.dotSize = 3;
  //loop that runs while the screensaver is active
  while(true){
    cube.setRotation(millis()/1000,0);
    cube.setRotation(millis()/1000,1);
    dots = makeDieDots(64,32,1+2.0*sin(millis()/200),1.8);
    dots.setRotation(millis()/1000,0);
    dots.setRotation(millis()/1000,1);
    display.clearDisplay();
    cube.render();
    dots.render();
    display.display();
    //checking if any buttons are pressed and breaking out of the loop if so
    if(controls.anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(utils.itsbeen(deepSleepTime)){
      return;
    }
  }
}

void screenSaver_prams(){
  vector<Raindrop> prams;
  const uint8_t maxPrams = 8;
  //loop that runs while the screensaver is active
  while(true){
    //generate a random number of prams each frame (but making sure there are never more than 'maxPrams')
    for(int8_t i = random(0,maxPrams-prams.size()); i>0; i--){
      prams.push_back(Raindrop(random(0,screenWidth),5,1));
    }
    display.clearDisplay();
    display.drawBitmap(29,0,stepchild_fullscreen,76,64,1);
    vector<Raindrop> temp;
    for(uint8_t i = 0; i<prams.size(); i++){
      prams[i].render(carriage_bmp,14,15);
      prams[i].update();
      //only keeping the prams that aren't offscreen
      if(prams[i].y<screenWidth)
        temp.push_back(prams[i]);
    }
    display.display();
    prams.swap(temp);
    //Serial.println(prams.size());

    //checking if any buttons are pressed and breaking out of the loop if so
    if(controls.anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(utils.itsbeen(deepSleepTime)){
      return;
    }
  }
}

void screenSaver_droplets(){
  vector<Raindrop> drops;
  uint8_t maxDrops = 20;
  //loop that runs while the screensaver is active
  while(true){
    //generate a random number of prams each frame (but making sure there are never more than 'maxPrams')
    for(int8_t i = random(0,maxDrops-drops.size()); i>0; i--){
      drops.push_back(Raindrop(random(0,screenWidth),10,1));
    }
//    double u;
//    maxDrops = abs(ceil(30.0*modf(sin(float(millis())/100.0),&u)));
    display.clearDisplay();
    vector<Raindrop> temp;
    for(uint8_t i = 0; i<drops.size(); i++){
      drops[i].render(false);
      drops[i].update();
      //only keeping the prams that aren't offscreen
      if(drops[i].y<screenWidth)
        temp.push_back(drops[i]);
    }
    display.display();
    drops.swap(temp);
    //checking if any buttons are pressed and breaking out of the loop if so
    if(controls.anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(utils.itsbeen(deepSleepTime)){
      return;
    }
  }
}

void screenSaver_keys(){
  bool done = true;
  //loop that runs while the screensaver is active
  while(true){
    display.clearDisplay();
    drawKeys(0,5,getOctave(36),14,true);//always start on a C, for simplicity
    display.display();
    //checking if any buttons are pressed and breaking out of the loop if so
    if(controls.anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(utils.itsbeen(deepSleepTime)){
      return;
    }
  }
}

void screenSaver_text(){
  bool done = true;
  int16_t textStart = 0;
  const String text = "midi sequencer music computer silent instrument pocket composer compact beat machine quiet thing note book track editor music haiku live looper midi generator midi sequencer music computer silent instrument pocket composer compact beat machine quiet thing note book track editor music haiku live looper midi generator";
  const int16_t textLength = text.length()*2-45;
  while(true){
    display.clearDisplay();
    printSmall(textStart,1,text,1);    
    printSmall(textStart+16,9,text,1);
    printSmall(textStart-13,17,text,1);
    printSmall(textStart+6,25,text,1);
    printSmall(textStart+3,33,text,1);
    printSmall(textStart+7,41,text,1);
    printSmall(textStart+1,49,text,1);
    printSmall(textStart-9,57,text,1);
    display.display();
    textStart--;
    if(textStart == -textLength){
      done = true;
      textStart = 0;
    }
    //checking if any buttons are pressed and breaking out of the loop if so
    if(controls.anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(utils.itsbeen(deepSleepTime)){
      return;
    }
  }
}

//inspired by the drum machine from Iglooghost's music vide:
//https://www.youtube.com/watch?v=5IfB819O7qg
void screenSaver_playing(){
  vector<uint8_t> trackIDsWithNotes = getTracksWithNotes();
  while(true){
    controls.readButtons();
    if(controls.anyActiveInputs()){
      lastTime = millis();
      break;
    }
    else if(utils.itsbeen(deepSleepTime)){
      return;
    }
    display.clearDisplay();
    // display.drawFastVLine(64,0,64,1);
    graphics.drawDottedLineV(64,0,64,2);
    for(uint16_t step = 0; step<128; step++){
      for(uint8_t track:trackIDsWithNotes){
        int32_t start = sequence.playheadPos - 64;
        if(start+step<0)
          continue;
        if(track>8)
          break;
        if(sequence.lookupTable[track][start+step]){
          Note note = sequence.noteData[track][sequence.lookupTable[track][start+step]];
          if(note.startPos == (start+step)){
            if(sequence.playheadPos<note.endPos && sequence.playheadPos>=note.startPos)
              display.fillRect(step,track*8,note.endPos-note.startPos,7,1);
            else
              display.drawRect(step,track*8,note.endPos-note.startPos,7,1);
          }
          else if(note.startPos<(start) && note.endPos > start){
            if(sequence.playheadPos<note.endPos && sequence.playheadPos>=note.startPos)
              display.fillRect(-1,track*8,note.endPos-start,7,1);
            else
              display.drawRect(-1,track*8,note.endPos-start,7,1);
          }
        }
      }
    }
    display.display();
  }
}


void screenSaver(){
  //vector that holds all the screen savers
  if(sequence.playing() || sequence.recording()){
    screenSaver_playing();
    return;
  }
  //running a random screen saver from the list
  vector<void (*)()> screenSaverList = {screenSaver_prams,screenSaver_droplets,screenSaver_cassette,screenSaver_ripples,screenSaver_text};
  screenSaverList[random(0,screenSaverList.size())]();
  screenSaver_droplets();
}

void screenSaverCheck(){
  while(utils.itsbeen(sleepTime)){
    screenSaver();
    if(utils.itsbeen(deepSleepTime)){
      enterSleepMode();
    }
  }
}
