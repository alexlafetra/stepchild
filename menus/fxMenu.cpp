bool fxMenuControls(){
  if(itsbeen(100)){
    if(x != 0){
      if(x == -1){
        //if it's not divisible by four (in which case this would be 0)
        if((activeMenu.highlight+1)%4)
          activeMenu.highlight++;
        lastTime = millis();
      }
      else if(x == 1){
        if((activeMenu.highlight)%4)
          activeMenu.highlight--;
        lastTime = millis();
      }
    }
    if(y != 0){
      if(y == 1){
        //first row
        if(activeMenu.highlight < 20)
          activeMenu.highlight += 4;
        lastTime = millis();
      }
      else if(y == -1){
        //second row
        if(activeMenu.highlight > 3)
          activeMenu.highlight -= 4;
        lastTime = millis();
      }
      activeMenu.page = activeMenu.highlight/12;
    }
  }
  if(itsbeen(200)){
    if(menu_Press){
      lastTime = millis();
      slideMenuOut(0,20);
      return false;
    }
    if(sel){
      sel = false;
      lastTime = millis();
      switch(activeMenu.highlight){
        //chord
        case 0:
          quantizeMenu();
          break;
        //echo
        case 1:
          humanizeMenu();
          break;
        //humanize
        case 2:
          strumMenu();
          break;
        case 3:
          echoMenu();
          break;
        case 4:
          chordBuilder();
          break;
        //reverse
        case 5:
          slideMenuOut(0,20);
          reverse();
          activeMenu.coords.x1 = 25;
          activeMenu.coords.y1 = 1;
          activeMenu.coords.x2 = 93;
          activeMenu.coords.y2 = 64;
          slideMenuIn(0,30);
          break;
        case 6:
          warp();
          break;
        //scramble
        case 7:
          break;
        //split
        case 8:
          break;
        //strum
        case 9:
          break;
      }
    }
  }
  return true;
}

void drawFxLabel(){
  String text = "NAH";
  switch(activeMenu.highlight){
    case 0:
      text = "QUANTIZE";
      break;
    case 1:
      text = "HUMANIZE";
      break;
    case 2:
      text = "STRUM";
      break;
    case 3:
      text = "ECHO";
      break;
    case 4:
      text = "CHORD";
      break;
    case 5:
      text = "REVERSE";
      break;
    case 6:
      // text = "RANDOM";
      break;
    case 7:
      // text = "SCRAMBLE";
      break;
    case 8:
      // text = "SPLIT";
      break;
    case 9:
      // text = "WARP";
      break;
    case 10:
      // text = "CHOP";
      break;
    case 11:
      // text = "MAYHEM";
      break;
  }
  printChunky(activeMenu.coords.x1+activeMenu.coords.y1+38,4,text,SSD1306_WHITE);
}

void fxMenu(){
  int angleX = 0;
  int angleY = 0;
  int angleZ = 0;

  int angle2X = 0;
  int angle2Y = 0;
  int angle2Z = 0;

  WireFrame icon = makeGyro(angleX,angleY,angleZ,
                  angle2X,angle2Y,angle2Z);
  while(menuIsActive && activeMenu.menuTitle == "FX"){
    display.clearDisplay();
    drawSeq(true,false,false,false,false);
    drawPram(5,0);
    display.fillCircle(111,13,23,0);
    display.drawCircle(111,13,23,1);
    activeMenu.displayFxMenu();
    icon.render();
    printItalic(103,13,"FX",1);
    display.display();
    joyRead();
    readButtons();
    if(!fxMenuControls()){
      break;
    }
    angleX+=2;
    angleY-=2;
    // angleZ-=2;
    angleX%=360;
    angleY%=360;
    angleZ%=360;
    angle2X+=2;
    // angle2Y+=2;
    angle2Z+=2;
    angle2X%=360;
    angle2Y%=360;
    angle2Z%=360;
    icon = makeGyro(angleX,angleY,angleZ,
                    angle2X,angle2Y,angle2Z);
  }
  constructMenu("MENU");
  activeMenu.highlight = 5;//so the 3d icon doesn't flash
  // mainMenu();
}

void Menu::displayFxMenu(){
  display.fillRect(coords.x1,coords.y1+13, coords.x2-coords.x1, coords.y2-coords.y1, SSD1306_BLACK);
  display.drawRect(coords.x1,coords.y1+12, coords.x2-coords.x1, coords.y2-coords.y1-12, SSD1306_WHITE);

  display.setFont(&FreeSerifItalic9pt7b);
  display.setCursor(coords.x1+coords.y1-2,11);
  display.print("Fx");
  display.setFont();
  
  drawFxLabel();
  //printing page number/arrows
  printSmall(coords.x1+coords.y1+21,2,stringify(page+1)+"/2",SSD1306_WHITE);
  if(page == 0){
    drawArrow(coords.x1+coords.y1+26,11+sin(millis()/200),2,3,true);
  }
  else{
    drawArrow(coords.x1+coords.y1+26,9+sin(millis()/200),2,2,true);
  }

  const uint8_t width = 16;
  uint8_t count = 0;
  for(uint8_t j = 0; j<3; j++){
    for(uint8_t i = 0; i<4; i++){
      display.drawBitmap(coords.x1+4+width*i,coords.y1+j*(width-1)+17+sin(millis()/200+count),fxMenu_icons[page*12+count],12,12,SSD1306_WHITE);
      if(i+4*j == highlight-12*page){
        display.drawRoundRect(coords.x1+2+width*i,coords.y1+j*(width-1)+15+sin(millis()/200+count),width,width,3,SSD1306_WHITE);
      }
      count++;
    }
  }
}
