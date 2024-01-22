//Draws a small graphic of the Stepchild with the top left corner at (x1, y1)
void drawSmallStepchild(uint8_t x1, uint8_t y1){
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

void drawSwingCurve(int8_t xPos, int8_t yPos){
  display.fillRect(xPos,16,96,48,SSD1306_BLACK);

  //If swing is off, don't draw the curve
  if(!swung){
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
  for(float i = viewStart; i<viewEnd; i+=0.5){
    float y1 = float(22.0*swingOffset(i))/MicroSperTimeStep;
    if(y1<0)
      shadeLineV(xPos+(i-viewStart)*scale,mid+ceil(y1),-ceil(y1),2);
    else
      shadeLineV(xPos+(i-viewStart)*scale,mid,ceil(y1),2);
    if(i == viewStart)
        display.drawLine(xPos,oldPoint+yPos+1,xPos,y1+yPos+1,SSD1306_WHITE);
    else
      display.drawLine(xPos+(i-0.5-viewStart)*scale,oldPoint+yPos+1,xPos+(i-viewStart)*scale,y1+yPos+1,SSD1306_WHITE);
    oldPoint = y1;
  }

  //loop points
  if(isInView(loopData[activeLoop].start)){
    display.drawFastVLine(trackDisplay + (loopData[activeLoop].start)*scale,16,screenHeight-16,SSD1306_WHITE);
  }
  if(isInView(loopData[activeLoop].end)){
    display.drawFastVLine(trackDisplay + (loopData[activeLoop].end)*scale,16,screenHeight-16,SSD1306_WHITE);
  }
  //playhead
  if(playing || recording){
    display.drawFastVLine(trackDisplay + ((playing ? playheadPos:recheadPos)-viewStart)*scale,16,screenHeight-16,SSD1306_WHITE);
  }
  display.drawFastHLine(xPos,16,screenWidth-xPos,1);
}


void Menu::displayClockMenu(float tVal,uint8_t cursor){
  //lines
  display.drawFastHLine(trackDisplay,headerHeight,screenWidth-trackDisplay,SSD1306_WHITE);
  uint8_t x1 = 0;
  int8_t x2 = 0;//needs to be signed! for whatever reason, you use this to do the tiny clock hands and it needs
  uint8_t x3 = 0;//to be able to accept -1,1 from a cosine function
  switch(cursor){
    //bpm
    case 0:
      {
      //bpm label
      display.fillRoundRect(coords.y1+screenWidth-52,36,70,34,6,SSD1306_BLACK);
      display.drawRoundRect(coords.y1+screenWidth-52,36,70,34,6,SSD1306_WHITE);
      String b = stringify(bpm);
      while(b.length()<3){
        b = "0"+b;
      }
      if((millis()%1000)>500)
        print7SegString(coords.y1+screenWidth-48,40,b,true);
      else
        print7SegString(coords.y1+screenWidth-48,40,b,false);
      printCursive(coords.y1+76,3,"bpm",SSD1306_WHITE);
      x1 = 10+((millis()/200)%2);
      drawArrow(42+coords.y1,13+19+x1+2,3,2,false);
      }
      break;
    //swing amplitude
    case 1:
      {
      x2 = 10+((millis()/200)%2);
      drawArrow(54+coords.y1,13+27+x2+2,3,2,false);
      drawSwingCurve(coords.y1+32,40);
      display.drawFastHLine(coords.y1+70,16,58,SSD1306_WHITE);
      printCursive(coords.y1+76,3,"swing",SSD1306_WHITE);
      if(!swung)
        break;
      printSmall(109,4,stringify(float(swingVal*100)/float(MicroSperTimeStep))+"%",SSD1306_WHITE);
      }
      break;
    //swing sub div
    case 2:
      {
      x2 = 10+((millis()/200)%2);
      drawArrow(54+coords.y1,13+27+x2+2,3,2,false);
      drawSwingCurve(coords.y1+32,40);
      display.drawFastHLine(coords.y1+70,16,58,SSD1306_WHITE);
      printCursive(coords.y1+76,3,"swing",SSD1306_WHITE);
      if(!swung)
        break;
      printFraction_small(109,4,stepsToMeasures(swingSubDiv));
      }
      break;
    //internal/external
    case 3:
      {
      x3 = 10+((millis()/200)%2);
      drawArrow(66+coords.y1,13+31+x3+2,3,2,false);
      if(clockSource == EXTERNAL){
        drawBanner(84,52,"external");
        drawSmallStepchild(88,30+2*((millis()/400)%2));
        display.drawBitmap(95,15+((millis()/400)%2),down_arrow,9,12,SSD1306_WHITE);
        // display.drawBitmap(91,19+2*((millis()/400)%2),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
        // display.drawBitmap(95,37+((millis()/400)%2),down_arrow,9,12,SSD1306_WHITE);
      }
      else{
        drawBanner(84,52,"internal");
        drawSmallStepchild(88,30+2*((millis()/400)%2));
        display.drawBitmap(95,15+((millis()/400)%2),up_arrow,9,12,SSD1306_WHITE);
      }
      printCursive(coords.y1+76, 3, "source",SSD1306_WHITE);
      }
      break;
  }

  //bpm
  display.fillRoundRect(38+coords.y1,13,9,19+x1,3,SSD1306_BLACK);
  display.drawRoundRect(38+coords.y1,13,9,19+x1,3,SSD1306_WHITE);
  display.setRotation(1);
  printSmall(screenHeight-29-x1,coords.y1+40,"bpm",SSD1306_WHITE);
  display.setRotation(2);

  //swing
  display.fillRoundRect(50+coords.y1,13,9,27+x2,3,SSD1306_BLACK);
  display.drawRoundRect(50+coords.y1,13,9,27+x2,3,SSD1306_WHITE);
  display.setRotation(1);
  printSmall(screenHeight-37-x2,coords.y1+52,"swing",SSD1306_WHITE);
  display.setRotation(2);

  //source
  display.fillRoundRect(62+coords.y1,13,9,31+x3,3,SSD1306_BLACK);
  display.drawRoundRect(62+coords.y1,13,9,31+x3,3,SSD1306_WHITE);
  display.setRotation(1);
  printSmall(screenHeight-41-x3,coords.y1+64,"source",SSD1306_WHITE);
  display.setRotation(2);

  //title
  display.fillRect(34+coords.y1-2,0,39,16,SSD1306_BLACK);
  display.setCursor(34+coords.y1-2,5);
  display.setFont(&FreeSerifItalic9pt7b);
  display.print("Clock");
  display.setFont();

    //clock animation
    if(clockSource == INTERNAL){
        display.fillRect(0,activeMenu.coords.y1,32,screenHeight-activeMenu.coords.y1,SSD1306_BLACK);
        drawPendulum(16,activeMenu.coords.y1+23,26,tVal);
        display.fillRect(10,activeMenu.coords.y1+23,12,10,SSD1306_BLACK);
        display.drawBitmap(6,activeMenu.coords.y1+5,clock_1_bmp,20,38,SSD1306_WHITE);
        x2 = 3*cos(millis()/200);
        int8_t y2 = 3*((millis()/400)%2);
        display.drawLine(15,activeMenu.coords.y1+15,15+x2,activeMenu.coords.y1+15+y2,SSD1306_BLACK);
    }
    else{
        display.fillRect(0,activeMenu.coords.y1,32,screenHeight-activeMenu.coords.y1,SSD1306_BLACK);
        drawPendulum(16,activeMenu.coords.y1+23,26,90);
        display.fillRect(10,activeMenu.coords.y1+23,12,10,SSD1306_BLACK);
        display.drawBitmap(6,activeMenu.coords.y1+5,clock_1_bmp,20,38,SSD1306_WHITE);
        display.drawLine(15,activeMenu.coords.y1+15,15,activeMenu.coords.y1+13,SSD1306_BLACK);
    }
}

//returns the most negative value the swing curve can add to the clock before the clock delay is 0
float minimumwingOffset(){
  return MicroSperTimeStep;
}

void clockMenu(){
  float tVal = micros();
  float angle = 1;
  uint8_t cursor = 0;
  while(true){
    display.clearDisplay();
    //if you're on the swing menu, no need to draw the seq
    if(cursor != 2)
      drawSeq(false,false,false,false,false);
    activeMenu.displayClockMenu(angle,cursor);
    display.display();

    //for the clock pendulum
    if(swung)
      angle += 10*(micros()-tVal)/(MicroSperTimeStep)+(swingVal/2000)*sin(PI*int(angle)/45);
    else
      angle += 10*(micros()-tVal)/(MicroSperTimeStep);
    tVal = micros();
    // angle+=180/24;

    readButtons();
    readJoystick();
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        break;
      }
      if(play){
        togglePlayMode();
        lastTime = millis();
      }
      if(x != 0){
        if(x == 1 && cursor>0){
          cursor--;
          if(cursor == 2)
            cursor--;
          lastTime = millis();
        }
        else if(x == -1 && cursor<3){
          cursor++;
          if(cursor == 2)
            cursor++;
          lastTime = millis();
        }
      }
      if(sel){
        if(cursor == 1 || cursor == 2){
          swung = !swung;
          lastTime = millis();
        }
        else if(cursor == 3){
          clockSource = !clockSource;
          lastTime = millis();
        }
      }
    }
    while(counterA != 0){
      if(counterA >= 1){
        switch(cursor){
          //bpm
          case 0:
            if(!shift)
              setBpm(bpm+10);
            else
              setBpm(bpm+1);
            break;
          case 2:
            cursor = 1;
            break;
          //swing val
          //(complicated checking bc you're scaling it
          // and it needs to switch between division and mult. when
          //if goes from + to -)
          case 1:
            if(!swung)
              break;
            if(!shift){
              swingVal+=MicroSperTimeStep/10;
            }
            else{
              swingVal+=MicroSperTimeStep/100;
            }
            if(abs(swingVal)>MicroSperTimeStep)
              swingVal = swingVal<0?-MicroSperTimeStep:MicroSperTimeStep;
            break;
          //source
          case 3:
            clockSource = !clockSource;
            break;
        }
      }
      else if(counterA <= -1){
        switch(cursor){
          //bpm
          case 0:
            if(!shift)
              setBpm(bpm-10);
            else
              setBpm(bpm-1);
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
            if(!swung)
              break;
            if(!shift){
              swingVal-=MicroSperTimeStep/10;
            }
            else{
              swingVal-=MicroSperTimeStep/100;
            }
            if(abs(swingVal)>MicroSperTimeStep)
              swingVal = swingVal<0?-MicroSperTimeStep:MicroSperTimeStep;
            break;
          //source
          case 3:
            clockSource = !clockSource;
            break;
        }
      }
      counterA += counterA<0?1:-1;
    }
    while(counterB != 0){
      if(counterB >= 1){
        switch(cursor){
          case 1:
            cursor = 2;
            break;
          //swing subdiv
          case 2:
            if(!swung)
              break;
            if(shift && swingSubDiv<768)
              swingSubDiv++;
            else if(swingSubDiv%24)
              swingSubDiv+=24-swingSubDiv%24;
            else if(swingSubDiv<=384)
              swingSubDiv *= 2;
            break;
        }
      }
      else if(counterB <= -1){
        switch(cursor){
          case 1:
            cursor = 2;
            break;
          //swing subdiv
          case 2:
            if(!swung)
              break;
            if(shift && swingSubDiv>1)
              swingSubDiv--;
            else if(swingSubDiv%24){
              swingSubDiv-=swingSubDiv%24;
              if(swingSubDiv == 0)
                swingSubDiv = 1;
            }
            else if(swingSubDiv >= 2)
              swingSubDiv /= 2;
            break;
        }
      }
      counterB += counterB<0?1:-1;
    }
  }
  slideMenuOut(0,16);
  // menuIsActive = false;
  activeMenu.highlight = 10;
  constructMenu("MENU");
}

void tapBpm(){
  int activeLED = 0;
  bool leds[8] = {0,0,0,0,0,0,0,0};
  long t1;
  long timeE;
  long timeL;
  bool time1 = false;
  display.fillRect(0,8,screenWidth,30,SSD1306_BLACK);
  display.drawFastHLine(0,8,screenWidth,SSD1306_WHITE);
  display.drawFastHLine(0,40,screenWidth,SSD1306_WHITE);
  display.setCursor(46,12);
  display.print("bpm");
  display.setFont(&FreeSerifItalic9pt7b);
  display.setCursor(64-stringify(int(bpm)).length()*4,35);
  print7SegNumber(64,35,bpm,true);
  // display.print(int(bpm));
  display.setFont();
  display.display();
  t1 = millis();
  while(true){
    //checking if it's been a 1/4 note
    timeE = micros()-timeL;
    if(timeE  >= MicroSperTimeStep*24){
      timeL = micros();
      leds[activeLED] = 0;
      activeLED += 1;
      activeLED %= 8;
      leds[activeLED] = 1;
      writeLEDs(leds);
    }
    readButtons_MPX();
    readJoystick();
    if(menu_Press){
      lastTime = millis();
      return;
    }
    if(itsbeen(75)){
      if(step_buttons[0]||step_buttons[1]||step_buttons[2]||step_buttons[3]||step_buttons[4]||step_buttons[5]||step_buttons[6]||step_buttons[7]){
        setBpm(float(60000)/float(millis()-t1));
        t1 = millis();
        display.fillRect(0,8,screenWidth,30,SSD1306_BLACK);
        display.drawFastHLine(0,8,screenWidth,SSD1306_WHITE);
        display.drawFastHLine(0,40,screenWidth,SSD1306_WHITE);
        display.setCursor(46,12);
        display.print("bpm");
        display.setFont(&FreeSerifItalic9pt7b);
        display.setCursor(64-stringify(int(bpm)).length()*4,35);
        display.print(bpm);
        display.setFont();
        display.display();
        clearButtons();
        lastTime = millis();
      }
    }
    while(counterA != 0){
      if(counterA >= 1){
        if(shift){
          setBpm(bpm+1);
        }
        else{
          setBpm(bpm+10);
        }
      }
      if(counterA <= -1){
        if(shift){
          setBpm(bpm-1);
        }
        else{
          setBpm(bpm-10);
        }
      }
    }
  }
}

void setBpm(int newBpm) {
  if(newBpm<=0){
    newBpm = 1;
  }
  else if(newBpm>999){
    newBpm = 999;
  }
  bpm = newBpm;
  MicroSperTimeStep = round(2500000/(bpm));
  if(abs(swingVal)>MicroSperTimeStep)
    swingVal = swingVal<0?-MicroSperTimeStep:MicroSperTimeStep;
}