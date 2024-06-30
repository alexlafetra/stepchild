class MainMenu:public StepchildMenu{
  public:
    WireFrame icon;
    uint8_t wireFrameID;
    bool renderWireframes = false;
    MainMenu(){
      coords = CoordinatePair(25,1,93,64);
      icon = makeGraphBox(5);
      icon.xPos = 112;
      icon.yPos = 16;
      graphAnimation(&icon);
    }
    void displayMenu();
    bool mainMenuControls();
    void updateMainMenuWireFrame();
    void animateMainMenuWireFrame();
    void drawMainMenuLabel();
};

void MainMenu::updateMainMenuWireFrame(){
  if(wireFrameID == cursor)
    return;
  switch(cursor){
    //autotracks
    case 0:
      icon = makeGraphBox(5);
      break;
    //loops
    case 1:
      icon = makeMobius();
      icon.rotate(90,0);
      icon.rotate(30,2);
      icon.rotate(random(0,360),1);
      break;
    //cassette icon
    case 2:
      icon = makeCassette();
      break;
    //wrench
    case 3:
      icon = makeWrench();
      break;
    //quicksave
    case 4:
      icon = makeCD();
      break;
    //fx
    case 5:
      icon = makeHammer();
      break;
    //rec
    case 6:
      icon = genRandMenuObjects(0,0,10,1);
      break;
    //PC
    case 7:
      icon = makeMonitor();
      icon.rotate(-30,1);
      icon.rotate(-10,0);
      break;
    //midi
    case 8:
      icon = makeMIDI();
      break;
    //files
    case 9:
      icon = makeFolder(30);
      break;
    //clock
    case 10:
      icon = makeMetronome(0);
      break;
    //arp
    case 11:
      icon = makeArpBoxes(millis());
      break;
  }
  icon.xPos = 112;
  if(cursor == 7)
    icon.yPos = 11;
  else
    icon.yPos = 16;
  wireFrameID = cursor;
}

void MainMenu::animateMainMenuWireFrame(){
  switch(cursor){
    //autotracks
    case 0:
      graphAnimation(&icon);
      icon.rotate(3,1);
      break;
    //loop
    case 1:
      //cute idea to pause the loop icon, but you're never not looping!
      // if(sequence.isLooping)
      icon.rotate(3,1);
      break;
    //keys
    case 2:
      icon.rotate(3,1);
      break;
    //settings
    case 3:
      icon.rotate(3,1);
      break;
    //disc
    case 4:
      icon.rotate(3,1);
      break;
    //fx
    case 5:
      icon.rotate(3,1);
      break;
    //die
    case 6:
      icon.rotate(1,0);
      icon.rotate(1,1);
      break;
    //monitor
    case 7:
      animateMonitor(icon,3.0,14);
      break;
    //midi
    case 8:
      icon.rotate(3,1);
      break;
    //files
    case 9:
      folderAnimation(icon);
      icon.rotate(3,1);
      break;
    //clock
    case 10:
      metAnimation(&icon);
      icon.yPos = 16 + 2*sin(float(millis())/400);
      break;
    //arp
    case 11:
      icon = makeArpBoxes(millis());
      icon.yPos = 16;
      icon.xPos = 112;
      break;
  }
}

bool MainMenu::mainMenuControls(){
  controls.readJoystick();
  controls.readButtons();
  if(utils.itsbeen(100)){
    if(controls.joystickX != 0){
      if(controls.joystickX == -1){
        //if it's not divisible by four (in which case this would be 0)
        if((cursor+1)%4)
          cursor++;
        lastTime = millis();
      }
      else if(controls.joystickX == 1){
        if((cursor)%4)
          cursor--;
        lastTime = millis();
      }
    }
    if(controls.joystickY != 0){
      if(controls.joystickY == 1){
        //first row
        if(cursor < 8)
          cursor += 4;
        lastTime = millis();
      }
      else if(controls.joystickY == -1){
        //second row
        if(cursor > 3)
          cursor -= 4;
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    if(controls.MENU()){
      lastTime = millis();
      return false;
    }
    if(controls.LOOP()){
      lastTime = millis();
      loopMenu();
    }
    if(controls.NEW()){
      lastTime = millis();
      slideOut(OUT_FROM_BOTTOM,20);
      fxMenu();
      slideIn(IN_FROM_BOTTOM,20);
    }
    if(controls.SELECT() ){
      controls.setSELECT(false);
      lastTime = millis();
      switch(cursor){
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
          if(controls.SHIFT()){
            loadBackup();
          }
          else{
            quickSave();
          }
          break;
        //fx
        case 5:
          slideOut(OUT_FROM_BOTTOM,20);
          fxMenu();
          slideIn(IN_FROM_BOTTOM,20);
          break;
        //rec
        case 6:
          randMenu();
          break;
        //console
        case 7:
          PCEditor();
          break;
        //midi
        case 8:
          midiMenu();
          break;
        //files
        case 9:
          slideOut(OUT_FROM_BOTTOM,20);
          fileMenu();
          slideIn(IN_FROM_BOTTOM,20);
          break;
        //clock
        case 10:
          slideOut(OUT_FROM_BOTTOM,20);
          clockMenu();
          slideIn(IN_FROM_BOTTOM,20);
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

void MainMenu::drawMainMenuLabel(){
  String text;
  switch(cursor){
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
      text = "APPS";
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
  graphics.drawLabel(112,34,text,true);
}

void MainMenu::displayMenu(){
  display.clearDisplay();
  SequenceRenderSettings settings;
  settings.trackLabels = true;
  settings.topLabels = false;
  settings.loopPoints = true;
  settings.shrinkTopDisplay = false;
  drawSeq(settings);

  if(renderWireframes){
    display.fillCircle(111,15,23,0);
    display.drawCircle(111,15,23,1);
  }

  //drawing menu box (+16 so the title is transparent)
  display.fillRect(coords.start.x,coords.start.y+12, coords.end.x-coords.start.x, coords.end.y-coords.start.y, SSD1306_BLACK);
  display.drawRect(coords.start.x,coords.start.y+12, coords.end.x-coords.start.x, coords.end.y-coords.start.y-12, SSD1306_WHITE);

  //if the title will be on screen
  if(coords.start.x+coords.start.y-1<screenWidth){
    display.fillRect(coords.start.x+coords.start.y-1,0,40,13,0);
    display.setCursor(coords.start.x+coords.start.y-1,5);
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
        display.drawBitmap(coords.start.x+4+width*i,coords.start.y+j*(width-1)+17+sin(millis()/200+count),mainMenu_icons[count],12,12,SSD1306_WHITE);
        graphics.drawCircleRadian(coords.start.x+4+width*i+6,coords.start.y+j*(width-1)+17+sin(millis()/200+count)+6,4,millis()/10,0);
        graphics.drawCircleRadian(coords.start.x+4+width*i+6,coords.start.y+j*(width-1)+17+sin(millis()/200+count)+6,3,millis()/100,0);
      }
      else
        display.drawBitmap(coords.start.x+4+width*i,coords.start.y+j*(width-1)+17+sin(millis()/200+count),mainMenu_icons[count],12,12,SSD1306_WHITE);
      if(i+4*j == cursor){
        display.drawRoundRect(coords.start.x+2+width*i,coords.start.y+j*(width-1)+15+sin(millis()/200+count),width,width,3,SSD1306_WHITE);
      }
      count++;
    }
  }
  if(renderWireframes){
    animateMainMenuWireFrame();
    if(cursor == 6)
      icon.renderDie();
    else
      icon.render();
    drawMainMenuLabel();
  }
  display.display();
}

void mainMenu(){
  MainMenu mainMenu;
  mainMenu.slideIn(IN_FROM_BOTTOM,30);
  mainMenu.renderWireframes = true;
  while(true){
    if(!mainMenu.mainMenuControls())
      break;
    mainMenu.updateMainMenuWireFrame();
    mainMenu.displayMenu();
  }
  mainMenu.renderWireframes = false;
  mainMenu.slideOut(OUT_FROM_BOTTOM,20);
}
