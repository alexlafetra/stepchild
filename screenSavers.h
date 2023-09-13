//60000/1000 = 60 seconds
const uint16_t sleepTime = 60000;
//120,000ms = 2min
const uint32_t deepSleepTime = 120000;

//turns off screen and LEDs, sends pico to deep sleep (Not done yet!)
#ifndef HEADLESS
void deepSleep(){
  if(itsbeen(sleepTime)){
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    clearButtons();
    turnOffLEDs();
    while(true){
      delay(1);
      if(anyActiveInputs()){
        lastTime = millis();
        break;
      }
    }
    display.ssd1306_command(SSD1306_DISPLAYON);
    updateLEDs();
  }
}
#else
void deepSleep(){
    display.clearDisplay();
    display.display();
    while(true){
        if(anyActiveInputs){
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
    cassette.rotate(1,1);
    rotationAmount++;
    if(rotationAmount>360){
      done = true;
      rotationAmount = 0;
    }
    else{
      done = false;
    }
    if(anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(itsbeen(sleepTime) && done){
      return;
    }
  }
}

void screenSaver_droplets(){
  const uint8_t maxReps = 5;//for how many rings
  const uint8_t spacing = 10;//for the spacing
  const uint8_t xCoord = 64;
  const uint8_t  yCoord = 32;
  animOffset = 0;
  bool done = false;
  while(true){
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
          drawEllipse(xCoord, yCoord, animOffset/3-spacing*i+sin(animOffset)*(i%2), animOffset/8-spacing*i/3,SSD1306_WHITE);
      }
    }
    display.display();
    if(animOffset<yCoord){
      animOffset+=5;
    }
    else
      animOffset+=6;
    if(animOffset>=8*spacing*maxReps/3+8*32+20){
      animOffset = 0;
      done = true;
    }
    else{
      done = false;
    }
    if(anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(itsbeen(sleepTime) && done){
      return;
    }
  }
}

void screenSaver_moon(){
  bool waxing = true;
  bool done = false;
  int16_t counter = 0;
  while(true){
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
    if(anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(itsbeen(sleepTime) && done){
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
    if(anyActiveInputs()){
      lastTime = millis();
      return;
    }
    else if(itsbeen(sleepTime) && done){
      return;
    }
  }
}

void screenSaver(){
  //vector that holds all the screen savers
  vector<void (*)()> screenSaverList = {screenSaver_droplets,screenSaver_cassette,screenSaver_moon};
  uint8_t which = random(0,screenSaverList.size());
  //running the screen saver
  lastTime = millis();
  screenSaverList[which]();
}

void screenSaverCheck(){
  while(itsbeen(sleepTime)){
    // screenSaver_cassette();
    screenSaver();
    if(itsbeen(deepSleepTime)){
      deepSleep();
    }
  }
}
