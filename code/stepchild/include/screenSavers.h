//turns off screen and LEDs, sends pico to deep sleep (Not done yet!)
#ifndef HEADLESS
void deepSleep(){
  if(utils.itsbeen(sleepTime)){
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    controls.clearButtons();
    controls.turnOffLEDs();
    while(true){
      delay(1);
      if(controls.anyActiveInputs()){
        lastTime = millis();
        break;
      }
    }
    display.ssd1306_command(SSD1306_DISPLAYON);
    sequence.updateLEDs();
  }
}
#else
void deepSleep(){
    display.clearDisplay();
    display.display();
    while(true){
        if(controls.anyActiveInputs()){
            break;
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
    // else{
    //   done = false;
    // }
    if(controls.anyActiveInputs()){
      lastTime = millis();
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
    // else{
    //   done = false;
    // }
    if(controls.anyActiveInputs()){
      lastTime = millis();
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
    drawMoon(phase, waxing);
    display.display();
    counter+=waxing?1:-1;;
    if(controls.anyActiveInputs()){
      lastTime = millis();
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
  }
}

void screenSaver_keys(){
  bool done = true;
  //loop that runs while the screensaver is active
  while(true){
    display.clearDisplay();
    drawKeys(0,5,getOctave(keyboardPitch),14,true);//always start on a C, for simplicity
    display.display();
    //checking if any buttons are pressed and breaking out of the loop if so
    if(controls.anyActiveInputs()){
      lastTime = millis();
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
  }
}

//inspired by the drum machine from Iglooghost's music vide:
//https://www.youtube.com/watch?v=5IfB819O7qg
void screenSaver_playing(){
  while(true){
    controls.readButtons();
    if(controls.anyActiveInputs()){
      lastTime = millis();
      break;
    }
    display.clearDisplay();
    display.drawFastVLine(64,0,64,1);
    // graphics.drawDottedLineH(64,0,64,2);
    vector<uint8_t> trackIDsWithPitches;
    for(uint8_t i = 0; i<sequence.trackData.size(); i++){
      if(sequence.noteData[i].size()>1){
        trackIDsWithPitches.push_back(i);
      }
    }
    for(uint16_t step = 0; step<128; step++){
      for(uint8_t track:trackIDsWithPitches){
        int32_t start = playheadPos - 64;
        if(start+step<0)
          continue;
        if(track>8)
          break;
        if(sequence.lookupTable[track][start+step]){
          Note note = sequence.noteData[track][sequence.lookupTable[track][start+step]];
          if(note.startPos == (start+step)){
            if(playheadPos<note.endPos && playheadPos>=note.startPos)
              display.fillRect(step,track*8,note.endPos-note.startPos,7,1);
            else
              display.drawRect(step,track*8,note.endPos-note.startPos,7,1);
          }
          else if(note.startPos<(start) && note.endPos > start){
            if(playheadPos<note.endPos && playheadPos>=note.startPos)
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
  if(playing || recording){
    screenSaver_playing();
    return;
  }
  //running a random screen saver from the list
  vector<void (*)()> screenSaverList = {screenSaver_prams,screenSaver_droplets,screenSaver_cassette,screenSaver_ripples,screenSaver_text};
  screenSaverList[random(0,screenSaverList.size())]();
}

void screenSaverCheck(){
  while(utils.itsbeen(sleepTime)){
    screenSaver();
    if(utils.itsbeen(deepSleepTime)){
      deepSleep();
    }
  }
}
