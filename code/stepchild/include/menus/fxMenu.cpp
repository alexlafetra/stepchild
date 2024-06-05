
bool fxMenuControls(){
  if(utils.itsbeen(100)){
    if(controls.joystickX != 0){
      if(controls.joystickX == -1){
        //if it's not divisible by four (in which case this would be 0)
        if((activeMenu.highlight+1)%4)
          activeMenu.highlight++;
        lastTime = millis();
      }
      else if(controls.joystickX == 1){
        if((activeMenu.highlight)%4)
          activeMenu.highlight--;
        lastTime = millis();
      }
    }
    if(controls.joystickY != 0){
      if(controls.joystickY == 1){
        //first row
        if(activeMenu.highlight < 20)
          activeMenu.highlight += 4;
        lastTime = millis();
      }
      else if(controls.joystickY == -1){
        //second row
        if(activeMenu.highlight > 3)
          activeMenu.highlight -= 4;
        lastTime = millis();
      }
      activeMenu.page = activeMenu.highlight/12;
    }
  }
  if(utils.itsbeen(200)){
    if(controls.MENU()){
      lastTime = millis();
      slideMenuOut(0,20);
      return false;
    }
    if(controls.SELECT() ){
      controls.setSELECT(false);
      lastTime = millis();
      fxApplicationFunctions[activeMenu.highlight]();
    }
  }
  return true;
}

void drawFxLabel(){
  String text = fxApplicationTexts[activeMenu.highlight];
  printChunky(activeMenu.coords.start.x+activeMenu.coords.start.y+38,4,text,SSD1306_WHITE);
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
    drawSeq(true,false,true,false,false);
    graphics.drawPram(5,0);
    display.fillCircle(111,13,23,0);
    display.drawCircle(111,13,23,1);
    activeMenu.displayFxMenu();
    icon.render();
    printItalic(103,13,"FX",1);
    display.display();
    controls.readJoystick();
    controls.readButtons();
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
  if(menuIsActive)
    constructMenu("MENU");
}

void Menu::displayFxMenu(){
  //drawing menu box (+16 so the title is transparent)
  display.fillRect(coords.start.x,coords.start.y+13, coords.end.x-coords.start.x, coords.end.y-coords.start.y, SSD1306_BLACK);
  display.drawRect(coords.start.x,coords.start.y+12, coords.end.x-coords.start.x, coords.end.y-coords.start.y-12, SSD1306_WHITE);

  //if the title will be on screen
  if(coords.start.x+coords.start.y-1<screenWidth){
    display.fillRect(coords.start.x+coords.start.y-1,0,18,13,0);
    display.setCursor(coords.start.x+coords.start.y-1,5);
    display.setFont(&FreeSerifItalic9pt7b);
    display.print("Fx");
    display.setFont();
  }
  
  drawFxLabel();
  //printing page number/arrows
  printSmall(coords.start.x+coords.start.y+21,2,stringify(page+1)+"/2",SSD1306_WHITE);
  if(page == 0){
    graphics.drawArrow(coords.start.x+coords.start.y+26,11+((millis()/400)%2),2,3,true);
  }
  else{
    graphics.drawArrow(coords.start.x+coords.start.y+26,9+((millis()/400)%2),2,2,true);
  }

  const uint8_t width = 16;
  uint8_t count = 0;
  for(uint8_t j = 0; j<3; j++){
    for(uint8_t i = 0; i<4; i++){
      display.drawBitmap(coords.start.x+4+width*i,coords.start.y+j*(width-1)+17+sin(millis()/200+count),fxApplicationIcons[page*12+count],12,12,SSD1306_WHITE);
      if(i+4*j == highlight-12*page){
        display.drawRoundRect(coords.start.x+2+width*i,coords.start.y+j*(width-1)+15+sin(millis()/200+count),width,width,3,SSD1306_WHITE);
      }
      count++;
    }
  }
}

