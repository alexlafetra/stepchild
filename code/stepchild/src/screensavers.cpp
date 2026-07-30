#include "Stepchild.h"
#include "graphics/WireFrame.h"
#include "applications/Raindrop.h"

using namespace std;

//screenSavers
void screenSaver_cassette(){
  WireFrame cassette = makeCassette();
  cassette.rotate(30,0);
  cassette.rotate(15,2);
  cassette.scale = 4;
  uint16_t rotationAmount = 0;
  bool done = false;
  while(true){
    stepchild.display.clearDisplay();
    cassette.render();
    stepchild.display.display();
    stepchild.buttons.cycleLEDs(150);
    cassette.rotate(1,1);
    rotationAmount++;
    if(rotationAmount>360){
      done = true;
      rotationAmount = 0;
    }
    if(stepchild.buttons.anyActiveInputs()){
      stepchild.lastTime = millis();
      return;
    }
    else if(stepchild.itsbeen(stepchild.deepSleepTime)){
      return;
    }
  }
}

void screenSaver_ripples(){
  const uint8_t maxReps = 5;//for how many rings
  const uint8_t spacing = 10;//for the spacing
  const uint8_t xCoord = 64;
  const uint8_t  yCoord = 32;
  uint16_t animOffset = 0;
  bool done = false;
  while(true){
    stepchild.buttons.cycleLEDs(150);
    stepchild.display.clearDisplay();
    if(animOffset<=32){
      if(animOffset>8){//drops and reflection
        stepchild.display.drawCircle(xCoord, animOffset-8, 1+sin(animOffset), SSD1306_WHITE);
        stepchild.display.drawCircle(xCoord, stepchild.SCREEN_HEIGHT-(animOffset-8), 1+sin(animOffset), SSD1306_WHITE);
      }
      stepchild.display.drawCircle(xCoord, animOffset, 3+sin(animOffset), SSD1306_WHITE);
      stepchild.display.drawCircle(xCoord, stepchild.SCREEN_HEIGHT-animOffset, 3+sin(animOffset), SSD1306_WHITE);
    }
    else if(animOffset>yCoord){
      int reps = (animOffset-yCoord)/spacing+1;
      if(reps>maxReps){
        reps = maxReps;
      }
      for(int i = 0; i<reps; i++){
        if(animOffset/3-spacing*i+sin(animOffset)*(i%2)<(stepchild.SCREEN_WIDTH+16))
            graphics.drawEllipse(xCoord, yCoord, animOffset/3-spacing*i+sin(animOffset)*(i%2), animOffset/8-spacing*i/3,SSD1306_WHITE);
      }
    }
    stepchild.display.display();
    if(animOffset<yCoord){
      animOffset+=5;
    }
    else
      animOffset+=6;
    if(animOffset>=(8*spacing*maxReps/3+8*32+108)){
      animOffset = 0;
      done = true;
    }
    delay(3);
    if(stepchild.buttons.anyActiveInputs()){
      stepchild.lastTime = millis();
      return;
    }
    else if(stepchild.itsbeen(stepchild.deepSleepTime)){
      return;
    }
  }
}

void screenSaver_moon(){
  bool waxing = true;
  bool done = false;
  int16_t counter = 0;
  while(true){
    stepchild.buttons.cycleLEDs(150);
    float phase = counter;
    if(phase >= 86){
      waxing =! waxing;
      done = true;
    }
    else{
      done = false;
    }
    stepchild.display.clearDisplay();
    graphics.drawMoon(phase, waxing);
    stepchild.display.display();
    counter+=waxing?1:-1;;
    if(stepchild.buttons.anyActiveInputs()){
      stepchild.lastTime = millis();
      return;
    }
    else if(stepchild.itsbeen(stepchild.deepSleepTime)){
      return;
    }
  }
}

void screenSaver_template(){
  //loop that runs while the screensaver is active
  while(true){
    stepchild.display.clearDisplay();
    //put your rendering code here!
    stepchild.display.display();
    //checking if any buttons are pressed and breaking out of the loop if so
    if(stepchild.buttons.anyActiveInputs()){
      stepchild.lastTime = millis();
      return;
    }
    else if(stepchild.itsbeen(stepchild.deepSleepTime)){
      return;
    }
  }
}

void screenSaver_die(){
  WireFrame cube = makeCube(20);
  cube.offset.x = 64;
  cube.offset.y = 32;
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
    stepchild.display.clearDisplay();
    cube.render();
    dots.render();
    stepchild.display.display();
    //checking if any buttons are pressed and breaking out of the loop if so
    if(stepchild.buttons.anyActiveInputs()){
      stepchild.lastTime = millis();
      return;
    }
    else if(stepchild.itsbeen(stepchild.deepSleepTime)){
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
      prams.push_back(Raindrop(random(0,stepchild.SCREEN_WIDTH),5,1));
    }
    stepchild.display.clearDisplay();
    vector<Raindrop> temp;
    for(uint8_t i = 0; i<prams.size(); i++){
      prams[i].render(carriage_bmp,14,15);
      prams[i].update();
      //only keeping the prams that aren't offscreen
      if(prams[i].y<stepchild.SCREEN_WIDTH)
        temp.push_back(prams[i]);
    }
    stepchild.display.display();
    prams.swap(temp);

    //checking if any buttons are pressed and breaking out of the loop if so
    if(stepchild.buttons.anyActiveInputs()){
      stepchild.lastTime = millis();
      return;
    }
    else if(stepchild.itsbeen(stepchild.deepSleepTime)){
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
      drops.push_back(Raindrop(random(0,stepchild.SCREEN_WIDTH),10,1));
    }
//    double u;
//    maxDrops = abs(ceil(30.0*modf(sin(float(millis())/100.0),&u)));
    stepchild.display.clearDisplay();
    vector<Raindrop> temp;
    for(uint8_t i = 0; i<drops.size(); i++){
      drops[i].render(false);
      drops[i].update();
      //only keeping the prams that aren't offscreen
      if(drops[i].y<stepchild.SCREEN_WIDTH)
        temp.push_back(drops[i]);
    }
    stepchild.display.display();
    drops.swap(temp);
    //checking if any buttons are pressed and breaking out of the loop if so
    if(stepchild.buttons.anyActiveInputs()){
      stepchild.lastTime = millis();
      return;
    }
    else if(stepchild.itsbeen(stepchild.deepSleepTime)){
      return;
    }
  }
}

void screenSaver_keys(){
  //loop that runs while the screensaver is active
  while(true){
    stepchild.display.clearDisplay();
    graphics.drawKeys(0,5,stepchild.getOctave(36),14,true);//always start on a C, for simplicity
    stepchild.display.display();
    //checking if any buttons are pressed and breaking out of the loop if so
    if(stepchild.buttons.anyActiveInputs()){
      stepchild.lastTime = millis();
      return;
    }
    else if(stepchild.itsbeen(stepchild.deepSleepTime)){
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
    stepchild.display.clearDisplay();
    graphics.printSmall(textStart,1,text,1);    
    graphics.printSmall(textStart+16,9,text,1);
    graphics.printSmall(textStart-13,17,text,1);
    graphics.printSmall(textStart+6,25,text,1);
    graphics.printSmall(textStart+3,33,text,1);
    graphics.printSmall(textStart+7,41,text,1);
    graphics.printSmall(textStart+1,49,text,1);
    graphics.printSmall(textStart-9,57,text,1);
    stepchild.display.display();
    textStart--;
    if(textStart == -textLength){
      done = true;
      textStart = 0;
    }
    //checking if any buttons are pressed and breaking out of the loop if so
    if(stepchild.buttons.anyActiveInputs()){
      stepchild.lastTime = millis();
      return;
    }
    else if(stepchild.itsbeen(stepchild.deepSleepTime)){
      return;
    }
  }
}

//inspired by the drum machine from Iglooghost's music vide:
//https://www.youtube.com/watch?v=5IfB819O7qg
void screenSaver_playing(){
  vector<uint8_t> trackIDsWithNotes = stepchild.getTracksWithNotes();
  while(true){
    stepchild.buttons.readButtons();
    if(stepchild.buttons.anyActiveInputs()){
      stepchild.lastTime = millis();
      break;
    }
    else if(stepchild.itsbeen(stepchild.deepSleepTime)){
      return;
    }
    stepchild.display.clearDisplay();
    // stepchild.display.drawFastVLine(64,0,64,1);
    graphics.drawDottedLineV(64,0,64,2);
    for(uint16_t step = 0; step<128; step++){
      for(uint8_t track:trackIDsWithNotes){
        int32_t start = stepchild.playheadPos - 64;
        if(start+step<0)
          continue;
        if(track>8)
          break;
        if(stepchild.lookupTable[track][start+step]){
          Note note = stepchild.noteData[track][stepchild.lookupTable[track][start+step]];
          if(note.startPos == (start+step)){
            if(stepchild.playheadPos<note.endPos && stepchild.playheadPos>=note.startPos)
              stepchild.display.fillRect(step,track*8,note.endPos-note.startPos,7,1);
            else
              stepchild.display.drawRect(step,track*8,note.endPos-note.startPos,7,1);
          }
          else if(note.startPos<(start) && note.endPos > start){
            if(stepchild.playheadPos<note.endPos && stepchild.playheadPos>=note.startPos)
              stepchild.display.fillRect(-1,track*8,note.endPos-start,7,1);
            else
              stepchild.display.drawRect(-1,track*8,note.endPos-start,7,1);
          }
        }
      }
    }
    stepchild.display.display();
  }
}


void screenSaver(){
  //vector that holds all the screen savers
  if(stepchild.playing() || stepchild.recording()){
    screenSaver_playing();
    return;
  }
  //running a random screen saver from the list
  vector<void (*)()> screenSaverList = {screenSaver_droplets,screenSaver_cassette,screenSaver_ripples,screenSaver_die};
  screenSaverList[random(0,screenSaverList.size())]();
}

void screenSaverCheck(){
  while(stepchild.itsbeen(stepchild.sleepTime)){
    screenSaver();
    if(stepchild.itsbeen(stepchild.deepSleepTime)){
      stepchild.enterSleepMode();
    }
  }
}
