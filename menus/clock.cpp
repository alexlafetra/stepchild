void drawSmallStepchild(uint8_t x1, uint8_t y1){
  //aspect ratio is ~11:9
  display.fillRoundRect(x1, y1, 22, 18, 4, SSD1306_WHITE);
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

void drawSwingCurve(int xPos, int yPos){
  display.fillRect(xPos,-1,64,66,SSD1306_BLACK);
  display.drawRect(xPos,-1,64,66,SSD1306_WHITE);
  if(!swung){
    printSmall(xPos+(screenWidth-xPos)/2-6,40,"off",SSD1306_WHITE);
    if(millis()%1000>500){
      printSmall(xPos+(screenWidth-xPos)/2-10,50,"[sel]",1);
    }
    display.drawFastVLine(xPos,16,48,SSD1306_WHITE);
    return;
  }
  //starting point is zero, end point is 96
  float sc = 96/32;
  float oldPoint = 0;
  uint16_t mid = (yPos);
  for(float i = 0; i<32; i+=0.5){
    float y1 = swingOffset(i*sc)/float(1000);
    // drawDottedLineDiagonal(xPos+i*2,yPos+1+y1,xPos+i*2,yPos+1,3);
    if(y1<0)
      shadeLineV(xPos+i*2,mid+ceil(y1),-ceil(y1),2);
    else
      shadeLineV(xPos+i*2,mid,ceil(y1),2);
    // display.drawPixel(xPos+i*2,y+yPos+1,SSD1306_WHITE);
    if(i == 0)
        display.drawLine(xPos,oldPoint+yPos+1,xPos+i*2,y1+yPos+1,SSD1306_WHITE);
    else
      display.drawLine(xPos+(i-0.5)*2,oldPoint+yPos+1,xPos+i*2,y1+yPos+1,SSD1306_WHITE);
    oldPoint = y1;
  }
  printSmall(screenWidth-8,18,"wv",1);
  printSmall(screenWidth-6,24,"%",1);
  //playhead
  if(playing || recording){
    display.drawFastVLine(xPos + ((playing ? playheadPos:recheadPos)%32)*sc,16,screenHeight-16,SSD1306_WHITE);
  }
  display.fillRect(activeMenu.topL[1]+70,0,58,16,SSD1306_BLACK);
}

void Menu::displayClockMenu(float tVal){
  //lines
  display.drawFastHLine(trackDisplay,debugHeight,screenWidth-trackDisplay,SSD1306_WHITE);
  uint8_t x1;
  uint8_t x2;
  uint8_t x3;
  switch(page){
    //bpm
    case 0:
      {
      //bpm label
      display.fillRoundRect(topL[1]+screenWidth-52,36,70,34,6,SSD1306_BLACK);
      display.drawRoundRect(topL[1]+screenWidth-52,36,70,34,6,SSD1306_WHITE);
      String b = stringify(bpm);
      while(b.length()<3){
        b = "0"+b;
      }
      if((millis()%1000)>500)
        print7SegString(topL[1]+screenWidth-48,40,b,true);
      else
        print7SegString(topL[1]+screenWidth-48,40,b,false);
      printCursive(topL[1]+76,3,"bpm",SSD1306_WHITE);
      x1 = 10+sin(millis()/100);
      drawArrow(42+topL[1],13+19+x1+2,3,2,false);
      }
      break;
    //swing amplitude
    case 1:
      {
      x2 = 10+sin(millis()/100);
      drawArrow(54+topL[1],13+27+x2+2,3,2,false);
      drawSwingCurve(topL[1]+66,40);
      display.drawFastHLine(topL[1]+70,16,58,SSD1306_WHITE);
      printSmall(109,4,stringify(swingVal/400)+"%",SSD1306_WHITE);
      printCursive(topL[1]+76,3,"swing",SSD1306_WHITE);
      }
      break;
    //swing sub div
    case 2:
      {
      x2 = 10+sin(millis()/100);
      drawArrow(54+topL[1],13+27+x2+2,3,2,false);
      drawSwingCurve(topL[1]+66,40);
      display.drawFastHLine(topL[1]+70,16,58,SSD1306_WHITE);
      printFraction_small(109,4,stepsToMeasures(swingSubDiv));
      printCursive(topL[1]+76,3,"swing",SSD1306_WHITE);
      }
      break;
    //internal/external
    case 3:
      {
      x3 = 10+sin(millis()/100);
      drawArrow(66+topL[1],13+31+x3+2,3,2,false);
      if(externalClock){
        drawBanner(84,52,"external");
        drawSmallStepchild(88,30+2*sin(millis()/200));
        display.drawBitmap(95,15+sin(millis()/200),down_arrow,9,12,SSD1306_WHITE);
        // display.drawBitmap(91,19+2*sin(millis()/200),MIDI_no_outline_bmp,17,17,SSD1306_WHITE);
        // display.drawBitmap(95,37+sin(millis()/200),down_arrow,9,12,SSD1306_WHITE);
      }
      else{
        drawBanner(84,52,"internal");
        drawSmallStepchild(88,30+2*sin(millis()/200));
        display.drawBitmap(95,15+sin(millis()/200),up_arrow,9,12,SSD1306_WHITE);
      }
      printCursive(topL[1]+76, 3, "source",SSD1306_WHITE);
      }
      break;
  }

  //bpm
  display.fillRoundRect(38+topL[1],13,9,19+x1,3,SSD1306_BLACK);
  display.drawRoundRect(38+topL[1],13,9,19+x1,3,SSD1306_WHITE);
  display.setRotation(1);
  printSmall(screenHeight-29-x1,topL[1]+40,"bpm",SSD1306_WHITE);
  display.setRotation(2);

  //swing
  display.fillRoundRect(50+topL[1],13,9,27+x2,3,SSD1306_BLACK);
  display.drawRoundRect(50+topL[1],13,9,27+x2,3,SSD1306_WHITE);
  display.setRotation(1);
  printSmall(screenHeight-37-x2,topL[1]+52,"swing",SSD1306_WHITE);
  display.setRotation(2);

  //source
  display.fillRoundRect(62+topL[1],13,9,31+x3,3,SSD1306_BLACK);
  display.drawRoundRect(62+topL[1],13,9,31+x3,3,SSD1306_WHITE);
  display.setRotation(1);
  printSmall(screenHeight-41-x3,topL[1]+64,"source",SSD1306_WHITE);
  display.setRotation(2);

  //title
  display.fillRect(34+topL[1]-2,0,39,16,SSD1306_BLACK);
  display.setCursor(34+topL[1]-2,5);
  display.setFont(&FreeSerifItalic9pt7b);
  display.print("Clock");
  display.setFont();

    //clock animation
    if(!externalClock){
        display.fillRect(0,activeMenu.topL[1],32,screenHeight-activeMenu.topL[1],SSD1306_BLACK);
        drawPendulum(16,activeMenu.topL[1]+23,26,tVal);
        display.fillRect(10,activeMenu.topL[1]+23,12,10,SSD1306_BLACK);
        display.drawBitmap(6,activeMenu.topL[1]+5,clock_1_bmp,20,38,SSD1306_WHITE);
        x2 = 3*cos(millis()/200);
        int8_t y2 = 3*sin(millis()/200);
        display.drawLine(15,activeMenu.topL[1]+15,15+x2,activeMenu.topL[1]+15+y2,SSD1306_BLACK);
    }
    else{
        display.fillRect(0,activeMenu.topL[1],32,screenHeight-activeMenu.topL[1],SSD1306_BLACK);
        drawPendulum(16,activeMenu.topL[1]+23,26,90);
        display.fillRect(10,activeMenu.topL[1]+23,12,10,SSD1306_BLACK);
        display.drawBitmap(6,activeMenu.topL[1]+5,clock_1_bmp,20,38,SSD1306_WHITE);
        display.drawLine(15,activeMenu.topL[1]+15,15,activeMenu.topL[1]+13,SSD1306_BLACK);
    }
}

void clockMenu(){
  float tVal = micros();
  float angle = 1;
  while(true){
    display.clearDisplay();
    drawSeq(false,false,false,false,false);
    activeMenu.displayClockMenu(angle);
    display.display();

    //for the clock pendulum
    if(swung)
      angle += 10*(micros()-tVal)/(MicroSperTimeStep)+(swingVal/2000)*sin(PI*int(angle)/45);
    else
      angle += 10*(micros()-tVal)/(MicroSperTimeStep);
    tVal = micros();
    // angle+=180/24;

    readButtons();
    joyRead();
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
        if(x == 1 && activeMenu.page>0){
          activeMenu.page--;
          if(activeMenu.page == 2)
            activeMenu.page--;
          lastTime = millis();
        }
        else if(x == -1 && activeMenu.page<3){
          activeMenu.page++;
          if(activeMenu.page == 2)
            activeMenu.page++;
          lastTime = millis();
        }
      }
      if(sel){
        if(activeMenu.page == 1 || activeMenu.page == 2){
          swung = !swung;
          lastTime = millis();
        }
        else if(activeMenu.page == 3){
          externalClock = !externalClock;
          lastTime = millis();
        }
      }
    }
    while(counterA != 0){
      if(counterA >= 1){
        switch(activeMenu.page){
          //bpm
          case 0:
            if(!shift)
              setBpm(bpm+10);
            else
              setBpm(bpm+1);
            break;
          case 2:
            activeMenu.page = 1;
            break;
          //swing val
          //(complicated checking bc you're scaling it
          // and it needs to switch between division and mult. when
          //if goes from + to -)
          case 1:
            if(!shift){
              if(swingVal>0){
                if(swingVal<43690)
                  swingVal *= 1.5;
              }
              else{
                if(abs(swingVal)<=1000)
                  swingVal = -swingVal;
                else{
                  swingVal /= 1.5;
                }
              }
            }
            else if(abs(swingVal)<=65435)
              swingVal += 100;
            break;
          //source
          case 3:
            externalClock = !externalClock;
            break;
        }
      }
      else if(counterA <= -1){
        switch(activeMenu.page){
          //bpm
          case 0:
            if(!shift)
              setBpm(bpm-10);
            else
              setBpm(bpm-1);
            lastTime = millis();
            break;
          case 2:
            activeMenu.page = 1;
            break;
          //swing val
          //(complicated checking bc you're scaling it
          // and it needs to switch between division and mult. when
          //if goes from + to -)
          case 1:
            if(!shift){
              if(swingVal<0){
                if(abs(swingVal)<43690)
                  swingVal *= 1.5;
              }
              else{
                if(abs(swingVal)<=1000)
                  swingVal = -swingVal;
                else{
                  swingVal /= 1.5;
                }
              }
            }
            else if(abs(swingVal)<=65435)
              swingVal -= 100;
            break;
          //source
          case 3:
            externalClock = !externalClock;
            break;
        }
      }
      counterA += counterA<0?1:-1;
    }
    while(counterB != 0){
      if(counterB >= 1){
        switch(activeMenu.page){
          case 1:
            activeMenu.page = 2;
            break;
          //swing subdiv
          case 2:
            if(shift && swingSubDiv<768)
              swingSubDiv++;
            else if(swingSubDiv<=384)
              swingSubDiv *= 2;
            break;
        }
      }
      else if(counterB <= -1){
        switch(activeMenu.page){
          case 1:
            activeMenu.page = 2;
            break;
          //swing subdiv
          case 2:
            if(shift && swingSubDiv>1)
              swingSubDiv--;
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
  constructMenu("MENU");
}

void Menu::displaySwingMenu(){
  //pendulum
  int a = 18;
  int h = 20;
  int k = 40;
  float x1;
  float y1;
  display.fillRect(topL[0],topL[1],bottomR[0]-topL[0],bottomR[1]-topL[1],SSD1306_BLACK);
  x1 = h + a * cos(radians(page));
  if (page > 180) {
    y1 = k - a * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
  }
  else {
    y1 = k + a * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
  }
  display.drawLine(x1,y1,h,k,SSD1306_WHITE);
  display.fillCircle(x1,y1,2,SSD1306_WHITE);

  drawSwingCurve(50,32);

  display.drawFastVLine(bottomR[0],topL[1],screenHeight,SSD1306_WHITE);
 //printing the menu
  int menuStart;
  if(activeMenu.highlight>1){
    menuStart = activeMenu.highlight - 1;
  }
  else if(activeMenu.highlight<=1){
    menuStart = 0;
  }
  for(int i = menuStart; i<menuStart+3; i++){
    if(i<activeMenu.options.size()){
      display.setCursor(topL[0]+4,topL[1]+2+(i-menuStart)*11);
      if(i == activeMenu.highlight){
        display.drawRoundRect(topL[0]+1,topL[1]+1+(i-menuStart)*11,activeMenu.options[i].length()*6+5,11,4,SSD1306_WHITE);
        display.print(activeMenu.options[i]);
      }
      else{
        display.print(activeMenu.options[i]);
      }
    }
  }
  if(options[highlight] == "amp"){
    printSmall(66,2,stringify(swingVal/100)+"%",SSD1306_WHITE);
  }
  if(options[highlight] == "int"){
    display.setCursor(66,2);
    printSmall(66,2,stepsToMeasures(swingSubDiv),SSD1306_WHITE);
  }
  if(options[highlight] == "on"){
    if(swung){
      printSmall(66,2,"swingin'",SSD1306_WHITE);
    }
    else{
      printSmall(66,2,"off",SSD1306_WHITE);
    }
  }
}