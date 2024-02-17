WireFrame getMainMenuWireFrame(){
  WireFrame w;
  switch(activeMenu.highlight){
    //autotracks
    case 0:
      w = makeGraphBox(5);
      break;
    //loops
    case 1:
      // w = makeLoopArrows(0);
      w = makeMobius();
      w.rotate(90,0);
      w.rotate(30,2);
      w.rotate(random(0,360),1);
      break;
    //cassette icon
    case 2:
      w = makeCassette();
      break;
    //wrench
    case 3:
      w = makeWrench();
      break;
    //quicksave
    case 4:
      w = makeCD();
      break;
    //fx
    case 5:
      w = makeHammer();
      break;
    //rec
    case 6:
      w = genRandMenuObjects(0,0,10,1);
      break;
    //heart
    case 7:
      w = makePram();
      break;
    //midi
    case 8:
      w = makeMIDI();
      break;
    //files
    case 9:
      w = makeFolder(30);
      break;
    //clock
    case 10:
      w = makeMetronome(0);
      break;
    //arp
    case 11:
      w = makeHand_flat(0,0,0,0,0);
      w.yPos = 32;
      break;
  }
  return w;
}

void animateIcon(WireFrame* w){
  switch(activeMenu.highlight){
    //autotracks
    case 0:
      graphAnimation(w);
      w -> rotate(3,1);
      break;
    //loop
    case 1:
      // loopArrowAnimation(w);
      if(isLooping)
        w -> rotate(3,1);
      break;
    //keys
    case 2:
      w -> rotate(3,1);
      break;
    //settings
    case 3:
      w -> rotate(3,1);
      break;
    case 4:
      w -> rotate(3,1);
      break;
    //fx
    case 5:
      w -> rotate(3,1);
      break;
    //rec
    case 6:
      w -> rotate(1,0);
      w -> rotate(1,1);
      break;
    //heart
    case 7:
      w -> rotate(3,1);
      break;
    
    //midi
    case 8:
      w -> rotate(3,1);
      break;
    //files
    case 9:
      folderAnimation(w);
      w -> rotate(3,1);
      break;
    //clock
    case 10:
      metAnimation(w);
      w -> yPos = 16 + 2*sin(millis()/400);
      break;
    //arp
    case 11:
      handAnimation(w);
      w -> yPos=20+2*((millis()/400)%2);
      w -> rotate(4*sin(millis()/400),1);
      break;
  }
}

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
        //autotracks
        case 0:
          autotrackViewer();
          break;
        //loop
        case 1:
          loopMenu();
          break;
        //keys
        case 2:
          selectInstrumentMenu();
          break;
        //settings
        case 3:
          settingsMenu();
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
          // console();
          PCEditor();
          break;
        //midi
        case 8:
          midiMenu();
          break;
        //files
        case 9:
          slideMenuOut(0,20);
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
      text = "PRGCHG";
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
  WireFrame icon = getMainMenuWireFrame();
  icon.xPos = 112;
  icon.yPos = 16;
  while(true){
    //controls
    readJoystick();
    readButtons();
    if(!mainMenuControls())
      break;
    
    //if the highlight changes, get the new wireFrame
    if(activeWireFrame != activeMenu.highlight){
      icon = getMainMenuWireFrame();
      icon.xPos = 112;
      if(activeMenu.highlight == 7)
        icon.yPos = 11;
      else
        icon.yPos = 16;
      activeWireFrame = activeMenu.highlight;
    }

    animateIcon(&icon);
    display.clearDisplay();
    drawSeq(true,false,true,false,false);
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
  display.fillRect(coords.x1,coords.y1+12, coords.x2-coords.x1, coords.y2-coords.y1, SSD1306_BLACK);
  display.drawRect(coords.x1,coords.y1+12, coords.x2-coords.x1, coords.y2-coords.y1-12, SSD1306_WHITE);

  //if the title will be on screen
  if(coords.x1+coords.y1-1<screenWidth){
    display.fillRect(coords.x1+coords.y1-1,0,40,13,0);
    display.setCursor(coords.x1+coords.y1-1,5);
    display.setFont(&FreeSerifItalic9pt7b);
    display.print("Menu");
    display.setFont();
  }

  const uint8_t width = 16;
  uint8_t count = 0;
  for(uint8_t j = 0; j<3; j++){
    for(uint8_t i = 0; i<4; i++){
      //animate clock hands
     if(count == 10){
        display.drawBitmap(coords.x1+4+width*i,coords.y1+j*(width-1)+17+sin(millis()/200+count),mainMenu_icons[count],12,12,SSD1306_WHITE);
        graphics.drawCircleRadian(coords.x1+4+width*i+6,coords.y1+j*(width-1)+17+sin(millis()/200+count)+6,4,millis()/10,0);
        graphics.drawCircleRadian(coords.x1+4+width*i+6,coords.y1+j*(width-1)+17+sin(millis()/200+count)+6,3,millis()/100,0);
      }
      else
        display.drawBitmap(coords.x1+4+width*i,coords.y1+j*(width-1)+17+sin(millis()/200+count),mainMenu_icons[count],12,12,SSD1306_WHITE);
      if(i+4*j == highlight){
        display.drawRoundRect(coords.x1+2+width*i,coords.y1+j*(width-1)+15+sin(millis()/200+count),width,width,3,SSD1306_WHITE);
      }
      count++;
    }
  }
}