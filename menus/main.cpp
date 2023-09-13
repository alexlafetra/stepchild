bool mainMenuControls(){
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
        if(activeMenu.highlight < 8)
          activeMenu.highlight += 4;
        lastTime = millis();
      }
      else if(y == -1){
        //second row
        if(activeMenu.highlight > 3)
          activeMenu.highlight -= 4;
        lastTime = millis();
      }
    }
  }
  if(itsbeen(200)){
    if(menu_Press){
      if(shift){
        slideMenuOut(0,20);
        lastTime = millis();
        constructMenu("DEBUG");
      }
      else{
        lastTime = millis();
        slideMenuOut(0,20);
        menuIsActive = false;
        return false;
      }
    }
    if(loop_Press){
      lastTime = millis();
      loopMenu();
    }
    if(n){
      lastTime = millis();
      slideMenuOut(0,20);
      constructMenu("FX");
      fxMenu();
    }
    if(sel){
      sel = false;
      lastTime = millis();
      switch(activeMenu.highlight){
        //datatracks
        case 0:
          dataTrackViewer();
          break;
        //loop
        case 1:
          loopMenu();
          break;
        //keys
        case 2:
          // toggleKeys();
          selectInstrumentMenu();
          break;
        //settings
        case 3:
          // slideMenuOut(0,20);
          // constructMenu("SYS");  
          constructMenu("SETTINGS");  
          break;
        //quicksave
        case 4:
          //if you're shifting, load most recent backup
          if(shift){
            loadBackup();
          }
          else{
            quickSave();
          }
          break;
        //fx
        case 5:
          slideMenuOut(0,20);
          constructMenu("FX");
          fxMenu();
          break;
        //rec
        case 6:
          // playBackMenu();
          randMenu();
          break;
        //console
        case 7:
          console();
          break;
        //midi
        case 8:
          midiMenu();
          break;
        //files
        case 9:
          constructMenu("FILES");
          break;
        //clock
        case 10:
          slideMenuOut(0,20);
          constructMenu("CLOCK");
          break;
        //arp
        case 11:
          arpMenu();
          keyboardAnimation(38,14,0,14,false);
          break;
      }
    }
  }
  return true;
}

void drawMainMenuLabel(){
  String text;
  switch(activeMenu.highlight){
    //DT
    case 0:
      text = "AUTO";
      break;
    //Loop
    case 1:
      text = "LOOP";
      break;
    //instruments
    case 2:
      text = "INST";
      break;
    //settings
    case 3:
      text = "PREFS";
      break;
    //quicksave
    case 4:
      text = "QIKSAVE";
      break;
    //clock
    case 5:
      text = "FX";
      break;
    //rec
    case 6:
      text = "RANDOM";
      break;
    //heart
    case 7:
      text = "CONSOLE";
      break;
    //midi
    case 8:
      text = "MIDI";
      break;
    //files
    case 9:
      text = "FILES";
      break;
    //clock
    case 10:
      text = "CLOCK";
      break;
    //arp menu
    case 11:
      text = "ARP";
      break;
  }
  drawLabel(112,34,text,true);
}
void mainMenu(){
  uint8_t activeWireFrame;
  WireFrame icon = getMenuWireFrame();
  icon.xPos = 112;
  icon.yPos = 16;
  while(true){
    //controls
    joyRead();
    readButtons();
    if(!mainMenuControls())
      break;
    
    //if the highlight changes, get the new wireFrame
    if(activeWireFrame != activeMenu.highlight){
      icon = getMenuWireFrame();
      icon.xPos = 112;
      if(activeMenu.highlight == 7)
        icon.yPos = 11;
      else
        icon.yPos = 16;
      activeWireFrame = activeMenu.highlight;
    }

    animateIcon(&icon);
    display.clearDisplay();
    drawSeq(true,false,false,false,false);
    drawPram(5,0);
    display.fillCircle(111,15,23,0);
    display.drawCircle(111,15,23,1);
    activeMenu.displayMainMenu();
    if(activeWireFrame == 6)
      icon.renderDie();
    else
      icon.render();
    drawMainMenuLabel();
    display.display();
  }
}

void Menu::displayMainMenu(){
  //drawing menu box (+16 so the title is transparent)
  display.fillRect(topL[0],topL[1]+13, bottomR[0]-topL[0], bottomR[1]-topL[1], SSD1306_BLACK);
  display.drawRect(topL[0],topL[1]+12, bottomR[0]-topL[0], bottomR[1]-topL[1]-12, SSD1306_WHITE);

  display.setCursor(topL[0]+topL[1]-2,5);
  display.setFont(&FreeSerifItalic9pt7b);
  display.print("Menu");
  display.setFont();

  const uint8_t width = 16;
  uint8_t count = 0;
  for(uint8_t j = 0; j<3; j++){
    for(uint8_t i = 0; i<4; i++){
      //animate rec icon 
      // if(count == 6){
      //   if(sin(millis()/200+count)<0)
      //     display.drawBitmap(topL[0]+4+width*i,topL[1]+j*(width-1)+17+sin(millis()/200+count),mainMenu_icons[count],12,12,SSD1306_WHITE);
      //   else
      //     display.drawBitmap(topL[0]+4+width*i,topL[1]+j*(width-1)+17+sin(millis()/200+count),playback2_bmp,12,12,SSD1306_WHITE);
      // }
      //animate clock hands
     if(count == 10){
        display.drawBitmap(topL[0]+4+width*i,topL[1]+j*(width-1)+17+sin(millis()/200+count),mainMenu_icons[count],12,12,SSD1306_WHITE);
        drawCircleRadian(topL[0]+4+width*i+6,topL[1]+j*(width-1)+17+sin(millis()/200+count)+6,4,millis()/10,0);
        drawCircleRadian(topL[0]+4+width*i+6,topL[1]+j*(width-1)+17+sin(millis()/200+count)+6,3,millis()/100,0);
      }
      else
        display.drawBitmap(topL[0]+4+width*i,topL[1]+j*(width-1)+17+sin(millis()/200+count),mainMenu_icons[count],12,12,SSD1306_WHITE);
      if(i+4*j == highlight){
        display.drawRoundRect(topL[0]+2+width*i,topL[1]+j*(width-1)+15+sin(millis()/200+count),width,width,3,SSD1306_WHITE);
      }
      count++;
    }
  }
}