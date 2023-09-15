void drawFxLabel(){
  String text = "NAH";
  switch(activeMenu.highlight){
    case 0:
      text = "CHORD";
      break;
    case 1:
      text = "ECHO";
      break;
    case 2:
      text = "HUMANIZE";
      break;
    case 3:
      text = "WARP";
      break;
    case 4:
      text = "QUANTIZE";
      break;
    case 5:
      text = "REVERSE";
      break;
    case 6:
      text = "RANDOM";
      break;
    case 7:
      text = "SCRAMBLE";
      break;
    case 8:
      text = "SPLIT";
      break;
    case 9:
      text = "STRUM";
      break;
    case 10:
      text = "CHOP";
      break;
    case 11:
      text = "MAYHEM";
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
