class ClockMenu:public StepchildMenu{
  public:
    float tVal;
    float angle;
    ClockMenu(){
      tVal = micros();
      angle = 1;
      coords = CoordinatePair(0,0,35,64);
    }
    void displayMenu();
    void drawSmallStepchild(uint8_t, uint8_t);
    void drawSwingCurve(int8_t,int8_t);
    void updatePendulum();
    bool clockMenuControls();
    void updateStepButtons();
};

void ClockMenu::updatePendulum(){
  //for the clock pendulum
  if(sequenceClock.isSwinging)
    angle += 1*(micros()-tVal)/(sequenceClock.uSecPerStep)+(sequenceClock.swingAmplitude/2000)*sin(PI*int(angle)/45);
  else
    angle += 1*(micros()-tVal)/(sequenceClock.uSecPerStep);
  tVal = micros();
}

void ClockMenu::updateStepButtons(){
  uint16_t LEDs = 1<<((millis()/sequenceClock.mSecPerStep()/24)%16);
  controls.writeLEDs(LEDs);
}

//Draws a small graphic of the Stepchild with the top left corner at (x1, y1)
void ClockMenu::drawSmallStepchild(uint8_t x1, uint8_t y1){
  //aspect ratio is ~11:9
  display.fillRoundRect(x1, y1, 22, 18, 3, SSD1306_WHITE);
  //screen
  display.fillRect(x1+5,y1+3,12,8,SSD1306_BLACK);
  //buttons
  for(uint8_t i = 0; i<4; i++){
    display.drawPixel(x1+2,y1+5+i*2,SSD1306_BLACK);
  }
  for(uint8_t i = 0; i<4; i++){
    display.drawPixel(x1+5+i*2,y1+15,SSD1306_BLACK);
  }
  //joystick
  display.fillCircle(x1+17,y1+14,2,SSD1306_BLACK);
  //encoders
  display.fillCircle(x1+19,y1+5,1,SSD1306_BLACK);
  display.fillCircle(x1+19,y1+9,1,SSD1306_BLACK);
}

void ClockMenu::drawSwingCurve(int8_t xPos, int8_t yPos){
  display.fillRect(xPos,16,96,48,SSD1306_BLACK);
  //If swing is off, don't draw the curve
  if(!sequenceClock.isSwinging){
    printSmall(xPos+(screenWidth-xPos)/2-6,42,"off",1);
    if(millis()%1000>500){
      printSmall(xPos+(screenWidth-xPos)/2-10,54,"[sel]",1);
    }
    display.drawFastVLine(xPos,16,48,1);
    display.drawFastHLine(xPos,16,screenWidth-xPos,1);
    return;
  }
  //starting point is the viewstart, end point is viewend
  float oldPoint = 0;
  uint16_t mid = (yPos);
  for(float i = sequence.viewStart; i<sequence.viewEnd; i+=0.5){
    float y1 = float(22.0*sequenceClock.swingOffset(i))/sequenceClock.uSecPerStep;
    if(y1<0)
      graphics.shadeLineV(xPos+(i-sequence.viewStart)*sequence.viewScale,mid+ceil(y1),-ceil(y1),2);
    else
      graphics.shadeLineV(xPos+(i-sequence.viewStart)*sequence.viewScale,mid,ceil(y1),2);
    if(i == sequence.viewStart)
        display.drawLine(xPos,oldPoint+yPos+1,xPos,y1+yPos+1,SSD1306_WHITE);
    else
      display.drawLine(xPos+(i-0.5-sequence.viewStart)*sequence.viewScale,oldPoint+yPos+1,xPos+(i-sequence.viewStart)*sequence.viewScale,y1+yPos+1,SSD1306_WHITE);
    oldPoint = y1;
  }

  //loop points
  if(isInView(sequence.loopData[sequence.activeLoop].start)){
    display.drawFastVLine(trackDisplay + (sequence.loopData[sequence.activeLoop].start)*sequence.viewScale,16,screenHeight-16,SSD1306_WHITE);
  }
  if(isInView(sequence.loopData[sequence.activeLoop].end)){
    display.drawFastVLine(trackDisplay + (sequence.loopData[sequence.activeLoop].end)*sequence.viewScale,16,screenHeight-16,SSD1306_WHITE);
  }
  //playhead
  if(playing || recording){
    display.drawFastVLine(trackDisplay + ((playing ? playheadPos:recheadPos)-sequence.viewStart)*sequence.viewScale,16,screenHeight-16,SSD1306_WHITE);
  }
  display.drawFastHLine(xPos,16,screenWidth-xPos,1);
}

void ClockMenu::displayMenu(){
  updatePendulum();
  updateStepButtons();
  display.clearDisplay();
  //if you're on the swing menu, no need to draw the seq
  if(cursor != 2)
    drawSeq(false,false,false,false,false);
  //lines
  display.drawFastHLine(trackDisplay,headerHeight,screenWidth-trackDisplay,SSD1306_WHITE);
  uint8_t x1 = 0;
  int8_t x2 = 0;//needs to be signed! for whatever reason, you use this to do the tiny clock hands and it needs
  uint8_t x3 = 0;//to be able to accept -1,1 from a cosine function
  switch(cursor){
    //sequenceClock.BPM
    case 0:
      {
      //sequenceClock.BPM label
      display.fillRoundRect(coords.start.y+screenWidth-52,36,70,34,6,SSD1306_BLACK);
      display.drawRoundRect(coords.start.y+screenWidth-52,36,70,34,6,SSD1306_WHITE);
      String b = stringify(sequenceClock.BPM);
      while(b.length()<3){
        b = "0"+b;
      }
      if((millis()%1000)>500)
        print7SegString(coords.start.y+screenWidth-48,40,b,true);
      else
        print7SegString(coords.start.y+screenWidth-48,40,b,false);
      printCursive(coords.start.y+76,3,"bpm",SSD1306_WHITE);
      x1 = 10+((millis()/200)%2);
      graphics.drawArrow(42+coords.start.y,13+19+x1+2,3,2,false);
      }
      break;
    //swing amplitude
    case 1:
      {
      x2 = 10+((millis()/200)%2);
      graphics.drawArrow(54+coords.start.y,13+27+x2+2,3,2,false);
      drawSwingCurve(coords.start.y+32,40);
      display.drawFastHLine(coords.start.y+70,16,58,SSD1306_WHITE);
      printCursive(coords.start.y+76,3,"swing",SSD1306_WHITE);
      if(!sequenceClock.isSwinging)
        break;
      printSmall(109,4,stringify(float(sequenceClock.swingAmplitude*100)/float(sequenceClock.uSecPerStep))+"%",SSD1306_WHITE);
      }
      break;
    //swing sub div
    case 2:
      {
      x2 = 10+((millis()/200)%2);
      graphics.drawArrow(54+coords.start.y,13+27+x2+2,3,2,false);
      drawSwingCurve(coords.start.y+32,40);
      display.drawFastHLine(coords.start.y+70,16,58,SSD1306_WHITE);
      printCursive(coords.start.y+76,3,"swing",SSD1306_WHITE);
      if(!sequenceClock.isSwinging)
        break;
      graphics.printFraction_small(109,4,stepsToMeasures(sequenceClock.swingSubDiv));
      }
      break;
    //internal/external
    case 3:
      {
      x3 = 10+((millis()/200)%2);
      graphics.drawArrow(66+coords.start.y,13+31+x3+2,3,2,false);
      if(clockSource == EXTERNAL_CLOCK){
        graphics.drawBanner(84,52,"external");
        drawSmallStepchild(88,30+2*((millis()/400)%2));
        display.drawBitmap(95,15+((millis()/400)%2),down_arrow,9,12,SSD1306_WHITE);
      }
      else{
        graphics.drawBanner(84,52,"internal");
        drawSmallStepchild(88,30+2*((millis()/400)%2));
        display.drawBitmap(95,15+((millis()/400)%2),up_arrow,9,12,SSD1306_WHITE);
      }
      printCursive(coords.start.y+76, 3, "source",SSD1306_WHITE);
      }
      break;
  }
  //sequenceClock.BPM
  display.fillRoundRect(38+coords.start.y,13,9,19+x1,3,SSD1306_BLACK);
  display.drawRoundRect(38+coords.start.y,13,9,19+x1,3,SSD1306_WHITE);
  display.setRotation(1);
  printSmall(screenHeight-29-x1,coords.start.y+40,"sequenceClock.BPM",SSD1306_WHITE);
  display.setRotation(2);
  //swing
  display.fillRoundRect(50+coords.start.y,13,9,27+x2,3,SSD1306_BLACK);
  display.drawRoundRect(50+coords.start.y,13,9,27+x2,3,SSD1306_WHITE);
  display.setRotation(1);
  printSmall(screenHeight-37-x2,coords.start.y+52,"swing",SSD1306_WHITE);
  display.setRotation(2);
  //source
  display.fillRoundRect(62+coords.start.y,13,9,31+x3,3,SSD1306_BLACK);
  display.drawRoundRect(62+coords.start.y,13,9,31+x3,3,SSD1306_WHITE);
  display.setRotation(1);
  printSmall(screenHeight-41-x3,coords.start.y+64,"source",SSD1306_WHITE);
  display.setRotation(2);
  //title
  display.fillRect(34+coords.start.y-2,0,39,16,SSD1306_BLACK);
  display.setCursor(34+coords.start.y-2,5);
  display.setFont(&FreeSerifItalic9pt7b);
  display.print("Clock");
  display.setFont();
  //clock animation
  if(clockSource == INTERNAL_CLOCK){
      display.fillRect(0,coords.start.y,32,screenHeight-coords.start.y,SSD1306_BLACK);
      graphics.drawPendulum(16,coords.start.y+23,26,tVal);
      display.fillRect(10,coords.start.y+23,12,10,SSD1306_BLACK);
      display.drawBitmap(6,coords.start.y+5,clock_1_bmp,20,38,SSD1306_WHITE);
      x2 = 3*cos(millis()/200);
      int8_t y2 = 3*((millis()/400)%2);
      display.drawLine(15,coords.start.y+15,15+x2,coords.start.y+15+y2,SSD1306_BLACK);
  }
  else{
      display.fillRect(0,coords.start.y,32,screenHeight-coords.start.y,SSD1306_BLACK);
      graphics.drawPendulum(16,coords.start.y+23,26,90);
      display.fillRect(10,coords.start.y+23,12,10,SSD1306_BLACK);
      display.drawBitmap(6,coords.start.y+5,clock_1_bmp,20,38,SSD1306_WHITE);
      display.drawLine(15,coords.start.y+15,15,coords.start.y+13,SSD1306_BLACK);
  }
  display.display();
}

bool ClockMenu::clockMenuControls(){
  controls.readButtons();
  controls.readJoystick();
  if(utils.itsbeen(200)){
    if(controls.MENU()){
      lastTime = millis();
      return false;
    }
    if(controls.PLAY()){
      togglePlayMode();
      lastTime = millis();
    }
    if(controls.joystickX != 0){
      if(controls.joystickX == 1 && cursor>0){
        cursor--;
        if(cursor == 2)
          cursor--;
        lastTime = millis();
      }
      else if(controls.joystickX == -1 && cursor<3){
        cursor++;
        if(cursor == 2)
          cursor++;
        lastTime = millis();
      }
    }
    if(controls.SELECT() ){
      if(cursor == 1 || cursor == 2){
        sequenceClock.isSwinging = !sequenceClock.isSwinging;
        lastTime = millis();
      }
      else if(cursor == 3){
        clockSource = !clockSource;
        lastTime = millis();
      }
    }
  }
  while(controls.counterA != 0){
    if(controls.counterA >= 1){
      switch(cursor){
        //sequenceClock.BPM
        case 0:
          if(!controls.SHIFT())
            sequenceClock.setBPM(sequenceClock.BPM+10);
          else
            sequenceClock.setBPM(sequenceClock.BPM+1);
          break;
        case 2:
          cursor = 1;
          break;
        //swing val
        //(complicated checking bc you're scaling it
        // and it needs to switch between division and mult. when
        //if goes from + to -)
        case 1:
          if(!sequenceClock.isSwinging)
            break;
          if(!controls.SHIFT()){
            sequenceClock.swingAmplitude+=sequenceClock.uSecPerStep/10;
          }
          else{
            sequenceClock.swingAmplitude+=sequenceClock.uSecPerStep/100;
          }
          if(abs(sequenceClock.swingAmplitude)>sequenceClock.uSecPerStep)
            sequenceClock.swingAmplitude = sequenceClock.swingAmplitude<0?-sequenceClock.uSecPerStep:sequenceClock.uSecPerStep;
          break;
        //source
        case 3:
          clockSource = !clockSource;
          break;
      }
    }
    else if(controls.counterA <= -1){
      switch(cursor){
        //sequenceClock.BPM
        case 0:
          if(!controls.SHIFT())
            sequenceClock.setBPM(sequenceClock.BPM-10);
          else
            sequenceClock.setBPM(sequenceClock.BPM-1);
          lastTime = millis();
          break;
        case 2:
          cursor = 1;
          break;
        //swing val
        //(complicated checking bc you're scaling it
        // and it needs to switch between division and mult. when
        //if goes from + to -)
        case 1:
          if(!sequenceClock.isSwinging)
            break;
          if(!controls.SHIFT()){
            sequenceClock.swingAmplitude-=sequenceClock.uSecPerStep/10;
          }
          else{
            sequenceClock.swingAmplitude-=sequenceClock.uSecPerStep/100;
          }
          if(abs(sequenceClock.swingAmplitude)>sequenceClock.uSecPerStep)
            sequenceClock.swingAmplitude = sequenceClock.swingAmplitude<0?-sequenceClock.uSecPerStep:sequenceClock.uSecPerStep;
          break;
        //source
        case 3:
          clockSource = !clockSource;
          break;
      }
    }
    controls.counterA += controls.counterA<0?1:-1;
  }
  while(controls.counterB != 0){
    if(controls.counterB >= 1){
      switch(cursor){
        case 1:
          cursor = 2;
          break;
        //swing subdiv
        case 2:
          if(!sequenceClock.isSwinging)
            break;
          if(controls.SHIFT() && sequenceClock.swingSubDiv<768)
            sequenceClock.swingSubDiv++;
          else if(sequenceClock.swingSubDiv%24)
            sequenceClock.swingSubDiv+=24-sequenceClock.swingSubDiv%24;
          else if(sequenceClock.swingSubDiv<=384)
            sequenceClock.swingSubDiv *= 2;
          break;
      }
    }
    else if(controls.counterB <= -1){
      switch(cursor){
        case 1:
          cursor = 2;
          break;
        //swing subdiv
        case 2:
          if(!sequenceClock.isSwinging)
            break;
          if(controls.SHIFT() && sequenceClock.swingSubDiv>1)
            sequenceClock.swingSubDiv--;
          else if(sequenceClock.swingSubDiv%24){
            sequenceClock.swingSubDiv-=sequenceClock.swingSubDiv%24;
            if(sequenceClock.swingSubDiv == 0)
              sequenceClock.swingSubDiv = 1;
          }
          else if(sequenceClock.swingSubDiv >= 2)
            sequenceClock.swingSubDiv /= 2;
          break;
      }
    }
    controls.counterB += controls.counterB<0?1:-1;
  }
  return true;
}


void tapBpm(){
  int activeLED = 0;
  // bool leds[8] = {0,0,0,0,0,0,0,0};
  uint16_t leds = 1;
  long t1;
  long timeE;
  long timeL;
  bool time1 = false;
  display.fillRect(0,8,screenWidth,30,SSD1306_BLACK);
  display.drawFastHLine(0,8,screenWidth,SSD1306_WHITE);
  display.drawFastHLine(0,40,screenWidth,SSD1306_WHITE);
  display.setCursor(46,12);
  display.print("sequenceClock.BPM");
  display.setFont(&FreeSerifItalic9pt7b);
  display.setCursor(64-stringify(int(sequenceClock.BPM)).length()*4,35);
  print7SegNumber(64,35,sequenceClock.BPM,true);
  // display.print(int(sequenceClock.BPM));
  display.setFont();
  display.display();
  t1 = millis();
  while(true){
    //checking if it's been a 1/4 note
    timeE = micros()-timeL;
    if(timeE  >= sequenceClock.uSecPerStep*24){
      timeL = micros();
      // leds[activeLED] = 0;
      leds = leds<<1;
      if(leds == 32768)
        leds = 1;
      activeLED += 1;
      controls.writeLEDs(leds);
    }
    controls.readButtons();
    controls.readJoystick();
    if(controls.MENU()){
      lastTime = millis();
      break;
    }
    if(utils.itsbeen(75)){
      if(controls.anyStepButtons()){
        sequenceClock.setBPM(float(60000)/float(millis()-t1));
        t1 = millis();
        display.fillRect(0,8,screenWidth,30,SSD1306_BLACK);
        display.drawFastHLine(0,8,screenWidth,SSD1306_WHITE);
        display.drawFastHLine(0,40,screenWidth,SSD1306_WHITE);
        display.setCursor(46,12);
        display.print("sequenceClock.BPM");
        display.setFont(&FreeSerifItalic9pt7b);
        display.setCursor(64-stringify(int(sequenceClock.BPM)).length()*4,35);
        display.print(sequenceClock.BPM);
        display.setFont();
        display.display();
        controls.clearButtons();
        lastTime = millis();
      }
    }
    while(controls.counterA != 0){
      if(controls.counterA >= 1){
        if(controls.SHIFT()){
          sequenceClock.setBPM(sequenceClock.BPM+1);
        }
        else{
          sequenceClock.setBPM(sequenceClock.BPM+10);
        }
      }
      if(controls.counterA <= -1){
        if(controls.SHIFT()){
          sequenceClock.setBPM(sequenceClock.BPM-1);
        }
        else{
          sequenceClock.setBPM(sequenceClock.BPM-10);
        }
      }
    }
  }
}

void clockMenu(){
  ClockMenu clockMenu;
  clockMenu.slideIn(IN_FROM_BOTTOM,20);
  while(clockMenu.clockMenuControls()){
    clockMenu.displayMenu();
  }
  clockMenu.slideOut(OUT_FROM_BOTTOM,16);
}