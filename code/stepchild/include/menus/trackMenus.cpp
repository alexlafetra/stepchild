//Functions that all 3 track menus use:
void drawTrackMenuTopInfo(uint8_t topL);

vector<uint8_t> selectMultipleTracks(String text){
  //clearing out the selected tracks
  for(uint8_t i = 0; i<sequence.trackData.size(); i++){
    sequence.trackData[i].setSelected(false);
  }
  //to hold the id's and return
  vector<uint8_t> selection = {};
  while(true){
    display.clearDisplay();
    SequenceRenderSettings settings;
    settings.trackSelection = true;
    settings.drawPram = false;
    settings.shrinkTopDisplay = true;
    settings.topLabels = false;
    drawSeq(settings);
    printSmall(screenWidth-text.length()*4,0,text,1);

    printSmall(2,2,"Trk:"+stringify(sequence.activeTrack+1),1);

    display.display();
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(100)){
      if(controls.joystickY == 1){
        sequence.setActiveTrack(sequence.activeTrack+1,true);
        lastTime = millis();
      }
      if(controls.joystickY == -1){
        sequence.setActiveTrack(sequence.activeTrack-1,true);
        lastTime = millis();
      }
    }
    if(utils.itsbeen(200)){
      if(controls.SELECT()  && !controls.SHIFT()){
        controls.setSELECT(false);
        lastTime = millis();
        sequence.trackData[sequence.activeTrack].setSelected(!sequence.trackData[sequence.activeTrack].isSelected());
      }
      //toggle the selection on all of them
      if(controls.SHIFT() && controls.SELECT() ){
        controls.setNEW(false);
        lastTime = millis();
        sequence.trackData[sequence.activeTrack].setSelected(!sequence.trackData[sequence.activeTrack].isSelected());
        for(int i = 0; i<sequence.trackData.size(); i++){
          sequence.trackData[i].setSelected(sequence.trackData[sequence.activeTrack].isSelected());
        }
      }
      if(controls.NEW() && !controls.SHIFT()){
        controls.setNEW(false);
        lastTime = millis();
        //adding all the selected tracks to the list
        for(int i = 0; i<sequence.trackData.size(); i++){
          if(sequence.trackData[i].isSelected()){
            selection.push_back(i);
            sequence.trackData[i].setSelected(false);
          }
        }
        return selection;
      }
      if(controls.DELETE() || controls.MENU()){
        lastTime = millis();
        for(uint8_t track = 0; track<sequence.trackData.size(); track++){
          sequence.trackData[track].setSelected(false);
        }
        selection.clear();
        break;
      }
    }
  }
  return selection;
}

vector<uint8_t> selectMultipleTracks(){
  return selectMultipleTracks("");
}

void tuneTracksToScale(){
  selectKeysAnimation(true);
  vector<uint8_t> pitches = selectKeys(0);
  selectKeysAnimation(false);
  if(pitches.size() == 0){
    return;
  }
  vector<uint8_t> tracks = selectMultipleTracks("select tracks to tune");
  if(!tracks.size())
    return;
  bool allowDuplicates = binarySelectionBox(64,32,"no","ye","allow duplicate pitches?",drawSeq);
  for(uint8_t track = 0; track < tracks.size(); track++){
    sequence.setTrackToNearestPitch(pitches, tracks[track], allowDuplicates);
  }
}


void sortTracks(){
  //0 = ascending, 1 = descending
  uint8_t sortType = 0;
  //0 = track pitch, 1 = track channel, 2 =  number of notes (wip)
  int8_t sortTarget = 0;
  Coordinate coords(screenWidth-44,8);
  uint8_t cursor = 0;
  menuText = "sort tracks";
  while(true){
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(200)){
      if(controls.MENU()){
        lastTime = millis();
        break;
      }
      if(controls.NEW() || (controls.SELECT() && cursor == 2)){
        sequence.sortTrackData(sortType,sortTarget);
        menuText = "sorted!";
        lastTime = millis();
        return;
      }
      if(controls.joystickY == 1 && cursor<3){
        cursor++;
        lastTime = millis();
      }
      if(controls.joystickY == -1 && cursor>0){
        cursor--;
        lastTime = millis();
      }
      if(controls.joystickX){
        switch(cursor){
          case 0:
            sortTarget+=controls.joystickX;
            if(sortTarget<0)
              sortTarget = 2;
            if(sortTarget == 3)
              sortTarget = 0;
            break;
          case 1:
            sortType = !sortType;
            break;
        }
        lastTime = millis();
      }
    }
    while(controls.counterA != 0){
      switch(cursor){
        case 0:
          sortTarget+=controls.counterA<0?-1:1;
          if(sortTarget<0)
            sortTarget = 2;
          if(sortTarget == 3)
            sortTarget = 0;
          break;
        case 1:
          sortType = !sortType;
          break;
      }
      controls.counterA+=controls.counterA<0?1:-1;
    }
    while(controls.counterB != 0){
      switch(cursor){
        case 0:
          sortTarget+=controls.counterB<0?-1:1;
          if(sortTarget<0)
            sortTarget = 2;
          if(sortTarget == 3)
            sortTarget = 0;
          break;
        case 1:
          sortType = !sortType;
          break;
      }
      controls.counterB+=controls.counterB<0?1:-1;
    }
    display.clearDisplay();
    SequenceRenderSettings settings;
    settings.drawTrackChannel = true;
    settings.shrinkTopDisplay = false;
    drawSeq(settings);
    display.fillRoundRect(coords.x,coords.y,46,40,3,SSD1306_BLACK);
    display.drawRoundRect(coords.x,coords.y,46,40,3,SSD1306_WHITE);
    // display.fillRoundRect(x1,y1,85,43,3,0);
    // display.drawRoundRect(x1,y1,85,43,3,1);
    // printSmall(x1+42,y1+3,"sort by:",1,ALIGN_CENTER);
    String target;
    switch(sortTarget){
      case 0:
        target = "pitch";
        break;
      case 1:
        target = "channel";
        break;
      case 2:
        target = "note count";
        break;
    }

    //sort by
    printSmall(coords.x+2,coords.y+2,"sort by:",1);
    if(cursor == 0){
      display.fillRoundRect(coords.x+1,coords.y+9,getSmallTextLength(target)+2,7,2,1);
      graphics.drawArrow(coords.x+millis()/200%2,coords.y+12,3,0,0);
    }
    printSmall(coords.x+2,coords.y+10,target,2);

    //order
    printSmall(coords.x+2,coords.y+17,"order:",1);
    String order = sortType?"descending":"ascending";
    if(cursor == 1){
      display.fillRoundRect(coords.x+1,coords.y+24,getSmallTextLength(order)+2,7,2,1);
      graphics.drawArrow(coords.x+millis()/200%2,coords.y+27,3,0,0);
    }
    printSmall(coords.x+2,coords.y+25,order,2);

    //commit
    if(cursor == 2){
      display.fillRoundRect(coords.x+11,coords.y+31,22,7,2,1);
      graphics.drawArrow(coords.x+millis()/200%2+10,coords.y+34,3,0,0);
    }
    printSmall(coords.x+12,coords.y+32,"sort!",2);
    display.display();
  }
}
void swapTracks(){
  // slideMenuOut(1,7);
  unsigned short int track1 = sequence.activeTrack;
  controls.setSELECT(false);
  while(true){
    if(utils.itsbeen(100)){
      if(controls.joystickY == 1){
        sequence.swapTracks(track1,sequence.activeTrack+1);
        sequence.setActiveTrack(sequence.activeTrack+1,true);
        track1 = sequence.activeTrack;
        lastTime = millis();
      }
      if(controls.joystickY == -1){
        sequence.swapTracks(track1,sequence.activeTrack-1);
        sequence.setActiveTrack(sequence.activeTrack-1,true);
        track1 = sequence.activeTrack;
        lastTime = millis();
      }
    }
    if(utils.itsbeen(200)){
      if(controls.NEW()){
        lastTime = millis();
        sortTracks();
      }
      if(controls.SELECT()  || controls.MENU()){
        controls.setSELECT(false);
        controls.setMENU(false) ;
        lastTime = millis();
        break;
      }
    }
    controls.readJoystick();
    controls.readButtons();
    display.clearDisplay();
    SequenceRenderSettings settings;
    settings.topLabels = false;
    settings.drawLoopPoints = false;
    drawSeq(settings);
    display.fillRect(70,0,16,16,0);
    if(millis()%1000 >= 500){
      display.drawBitmap(70,0,arrow_1_bmp,16,16,SSD1306_WHITE);
    }
    else{
      display.drawBitmap(70,0,arrow_3_bmp,16,16,SSD1306_WHITE);
    }
    display.fillRoundRect(86,-2,44,9,3,0);
    display.drawRoundRect(86,-2,44,9,3,1);
    printSmall(89,0,"[n] to sort",1);
    display.display();
  }
}

/*
--------------------------------
      Track Menu
--------------------------------
*/

class TrackMenu:public StepchildMenu{
  public:
    uint8_t xCursor = 0;
    TrackMenu(){
      coords = CoordinatePair(94,0,129,65);
    }
    bool trackMenuControls();
    void displayMenu();
    void displayMenuSidebar();
    String getMenuText();
};

void trackUtils(TrackMenu* t);

bool TrackMenu::trackMenuControls(){
  controls.readButtons();
  controls.readJoystick();
  //moving menu cursor
  if(!controls.SHIFT()){
    if(utils.itsbeen(100)){
      if(controls.joystickY != 0){
        if(controls.joystickY == 1){
          if(cursor<9 && cursor != 6){
            cursor++;
            lastTime = millis();
          }
        }
        if(controls.joystickY == -1){
          if(cursor == 7){
            cursor = 3;
            lastTime = millis();
          }
          else if(cursor>0){
            cursor--;
            lastTime = millis();
          }
        }
      }
    }
  }
  if(utils.itsbeen(100)){
    if(controls.SHIFT() && controls.joystickY != 0){
      if(controls.joystickY == -1){
        sequence.setActiveTrack(sequence.activeTrack-1,true);
        lastTime = millis();
      }
      if(controls.joystickY == 1){
        sequence.setActiveTrack(sequence.activeTrack+1,true);
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    if(controls.PLAY()){
      sequence.trackData[sequence.activeTrack].setPrimed(!sequence.trackData[sequence.activeTrack].isPrimed());
      lastTime = millis();
    }
    if(controls.SELECT() ){
      lastTime = millis();
      switch(cursor){
        //tune
        case 0:
          tuneTracksToScale();
          lastTime = millis();
          break;
        //midi route/channel
        case 2:
//          routeMenu();
          lastTime = millis();
          break;
        case 3:
          sequence.trackData[sequence.activeTrack].setPrimed(!sequence.trackData[sequence.activeTrack].isPrimed());
          if(controls.SHIFT()){
            for(uint8_t track = 0; track<sequence.trackData.size(); track++){
              sequence.trackData[track].setPrimed(sequence.trackData[sequence.activeTrack].isPrimed());
            }
          }
          break;
        //edit
        case 4:
          trackUtils(this);
          lastTime = millis();
          break;
        //mute
        case 5:
          if(controls.SHIFT()){
            sequence.muteMultipleTracks(selectMultipleTracks(stringify("Mute which?")));
          }
          else{
           sequence.toggleMute(sequence.activeTrack);
          }
          lastTime = millis();
          break;
        //move
        case 6:
          swapTracks();
          lastTime = millis();
          break;
        //erase
        case 7:
          if(controls.SHIFT()){
            sequence.eraseMultipleTracks(selectMultipleTracks(stringify("Erase which?")));
          }
          else{
            sequence.eraseTrack(sequence.activeTrack);
          }
          lastTime = millis();
          break;
        //solo
        case 8:
        sequence.toggleSolo(sequence.activeTrack);
          lastTime = millis();
          break;
        //latch
        case 9:
          sequence.trackData[sequence.activeTrack].setLatched(!sequence.trackData[sequence.activeTrack].isLatched());
          //latch all tracks
          if(controls.SHIFT()){
            for(int track = 0; track<sequence.trackData.size(); track++){
              sequence.trackData[track].setLatched(sequence.trackData[sequence.activeTrack].isLatched());
            }
          }
          lastTime = millis();
          break;
      }
    }
    if(controls.DELETE()){
      if(!controls.SHIFT()){
        lastTime = millis();
        sequence.deleteTrack(sequence.activeTrack);
      }
      else if(controls.SHIFT()){
        lastTime = millis();
        sequence.deleteEmptyTracks();
      }
    }
    if(controls.NEW()){
      if(controls.SHIFT())
        sequence.dupeTrack(sequence.activeTrack);
      else
        sequence.addTrack(sequence.trackData[sequence.activeTrack].pitch+1,sequence.defaultChannel,true);
      lastTime = millis();
    }
  }
  if(utils.itsbeen(200)){
    if(controls.A() || controls.MENU() || controls.B()){
      lastTime = millis();
      return false;
    }
  }
  //menu navigation/incrementing values
  if(utils.itsbeen(100)){
    if(controls.joystickX == -1){
      if(cursor >= 4 && cursor <=6){
        cursor+=3;
        lastTime = millis();
      }
    }
    else if(controls.joystickX == 1){
      if(cursor >= 7 && cursor <= 9){
        cursor-=3;
        lastTime = millis();
      }
    }
    if(cursor<3){
      while(controls.counterA != 0){
        if(controls.counterA >= 1 || controls.joystickX == -1){
          //pitch
          if(cursor == 0 && sequence.trackData[sequence.activeTrack].pitch<120){
            if(controls.SHIFT())
              sequence.transposeAllPitches(1);
            else
              sequence.setTrackPitch(sequence.activeTrack,sequence.trackData[sequence.activeTrack].pitch+1,true);
            lastTime = millis();
          }
          //octave
          else if(cursor == 1 && sequence.trackData[sequence.activeTrack].pitch<108){
            if(controls.SHIFT())
              sequence.transposeAllPitches(12);
            else
              sequence.setTrackPitch(sequence.activeTrack,sequence.trackData[sequence.activeTrack].pitch+12,true);
            lastTime = millis();
          }
          //channel
          else if(cursor == 2 && sequence.trackData[sequence.activeTrack].channel<16){
            if(controls.SHIFT())
              sequence.transposeAllChannels(1);
            else
              sequence.setTrackChannel(sequence.activeTrack,sequence.trackData[sequence.activeTrack].channel+1,true);
              lastTime = millis();
          }
          controls.counterA += controls.counterA<0?1:-1;;
        }
        if(controls.counterA <= -1 || controls.joystickX == 1){
          //pitch
          if(cursor == 0 && sequence.trackData[sequence.activeTrack].pitch>0){
            if(controls.SHIFT())
              sequence.transposeAllPitches(-1);
            else
              sequence.setTrackPitch(sequence.activeTrack,sequence.trackData[sequence.activeTrack].pitch-1,true);
            lastTime = millis();
          }
          //octave
          else if(cursor == 1 && sequence.trackData[sequence.activeTrack].pitch>11){
            if(controls.SHIFT())
              sequence.transposeAllPitches(-12);
            else
              sequence.setTrackPitch(sequence.activeTrack,sequence.trackData[sequence.activeTrack].pitch-12,true);
            lastTime = millis();
          }
          //channel
          else if(cursor == 2 && sequence.trackData[sequence.activeTrack].channel>1){
            if(controls.SHIFT())
              sequence.transposeAllChannels(-1);
            else
              sequence.setTrackChannel(sequence.activeTrack,sequence.trackData[sequence.activeTrack].channel-1,true);
            lastTime = millis();
          }
          controls.counterA += controls.counterA<0?1:-1;;
        }
      }
    }
    while(controls.counterB != 0){
      if(controls.counterB <= -1){
        sequence.setActiveTrack(sequence.activeTrack+1,true);
        lastTime = millis();
      }
      if(controls.counterB >= 1){
        sequence.setActiveTrack(sequence.activeTrack-1,true);
        lastTime = millis();
      }
      controls.counterB += controls.counterB<0?1:-1;;
    }
  }
  return true;
}

void TrackMenu::displayMenu(){
  SequenceRenderSettings settings;
  settings.shrinkTopDisplay = false;
  settings.drawLoopFlags = false;
  settings.drawPram = false;
  settings.topLabels = false;
  settings.drawTrackChannel = true;
  display.clearDisplay();
  drawSeq(settings);
  displayMenuSidebar();
  display.display();
}

String TrackMenu::getMenuText(){
  const vector<String> texts = {"change pitch","change octave","change channel","prime 4 rec","edit","mute","move","erase","solo","latch"};
  switch(cursor){
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
  }
  return texts[cursor];
}

void TrackMenu::displayMenuSidebar(){

  //title
  display.setCursor((screenWidth-coords.start.x)-34,7);
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.print("Trk");
  display.print(stringify(sequence.activeTrack+1));
  display.setFont();

  //drawing menu box
  display.fillRoundRect(coords.start.x,coords.start.y+12,coords.end.x-coords.start.x+1,coords.end.y-coords.start.y-11,3,SSD1306_BLACK);
  display.drawRoundRect(coords.start.x,coords.start.y+12,coords.end.x-coords.start.x+1,coords.end.y-coords.start.y-11,3,SSD1306_WHITE);
  //top labels 
  String text = getMenuText();
  // printChunky(coords.end.x-text.length()*6,coords.start.y+5,text,1);
  // printChunky(coords.end.x-6,coords.start.y+5,stringify(cursor),1);
  printSmall(coords.end.x-text.length()*4,coords.start.y+6,text,1);
  printSmall(coords.end.x-60,coords.start.y,"[n]->new|[del]->del",1);
  drawTrackMenuTopInfo(coords.start.x);

  //drawing menu options, and the highlight
  const uint8_t textHeight = 8;
  const uint8_t yPos = 12;
  //printing out the menu
  //pitch
  String p = pitchToString(sequence.trackData[sequence.activeTrack].pitch,false,true);
  if(cursor == 0){
    display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+2,8+p.length()*4+12,9,4,SSD1306_WHITE);
    display.drawChar(coords.start.x+6,yPos+coords.start.y+3,0x0E,SSD1306_BLACK,SSD1306_WHITE,1);
  }
  else
    display.drawChar(coords.start.x+6,yPos+coords.start.y+3,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
  printSmall(coords.start.x+12,yPos+coords.start.y+3,"$",2);//printing all the text inverse, so it turns black when highlighted
  printSmall(coords.start.x+14,yPos+coords.start.y+4," = "+p,2);
  //oct
  p = stringify(getOctave(sequence.trackData[sequence.activeTrack].pitch));
  if(cursor == 1)
    display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+3+textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(coords.start.x+6,yPos+coords.start.y+4+textHeight,"/8 = "+p,2);
  //channel
  p = stringify(sequence.trackData[sequence.activeTrack].channel);
  if(cursor == 2)
    display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+2+2*textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(coords.start.x+6,yPos+coords.start.y+3+2*textHeight,"ch = "+p,2);
  
  const uint8_t col1X = 5;
  const uint8_t col2X = 19;

  //prime icon
  // printSmall(coords.start.x+col1X,37,"rec",1);
  display.drawBitmap(coords.start.x+col1X,38,rec_tiny,11,4,1,0);
  if(sequence.trackData[sequence.activeTrack].isPrimed()){
    // printSmall(coords.start.x+2,37,"prime",1);
    if(millis()%1000<500)
      display.fillCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
    else
      display.drawCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
  }
  else{
    display.drawCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
  }
  
  //edit
  display.drawBitmap(coords.start.x+col1X,43,track_pencil,11,5,SSD1306_WHITE);
  //erase
  display.drawBitmap(coords.start.x+col2X,43,track_eraser,11,5,SSD1306_WHITE);
  //mute
  if(sequence.trackData[sequence.activeTrack].isMuted())
    display.drawBitmap(coords.start.x+col1X,50,track_mute,11,5,SSD1306_WHITE);
  else
    display.drawBitmap(coords.start.x+col1X,50,track_unmute,11,5,SSD1306_WHITE);
  //solo
  display.drawBitmap(coords.start.x+col2X,50,track_solo,11,5,SSD1306_WHITE);
  //move
  display.drawBitmap(coords.start.x+col1X,57,track_arrows,11,5,SSD1306_WHITE);
  //latch
  if(sequence.trackData[sequence.activeTrack].isLatched())
    display.drawBitmap(coords.start.x+col2X+3,57,track_latch,5,5,SSD1306_WHITE);
  else
    display.drawBitmap(coords.start.x+col2X+3,57,track_unlatch,5,5,SSD1306_WHITE);
  
  //menu highlight
  if(cursor == 3)
    graphics.drawArrow(coords.start.x+25+((millis()/200)%2),39,2,1,true);
  //first column
  else if(cursor>=4 && cursor<=6)
    graphics.drawRectWithMissingCorners(coords.start.x+3,34+7*(cursor-3),15,9,SSD1306_WHITE);
  else if(cursor>=7)
    graphics.drawRectWithMissingCorners(coords.start.x+17,34+7*(cursor-6),15,9,SSD1306_WHITE);
}

void trackMenu(){
  TrackMenu trackMenu;
  trackMenu.slideIn(IN_FROM_RIGHT,MENU_SLIDE_SLOW);
  while(trackMenu.trackMenuControls()){
    trackMenu.displayMenu();
  }
  trackMenu.slideOut(OUT_FROM_RIGHT,MENU_SLIDE_SLOW);
}

/*
--------------------------------
      Track Edit Menu
--------------------------------
*/

class TrackEditMenu:public StepchildMenu{
  public:
    //for which param to edit
    uint8_t xCursor = 0;
    //params are: track select, note, oct, channel, prime, latch, mute group
    //for which track to edit
    uint8_t yCursor = 0;
    TrackEditMenu(){
      coords = CoordinatePair(94,0,129,65);
    }
    void displayMenu();
    void drawTrackInfo();
    bool trackEditMenuControls();
};

void TrackEditMenu::drawTrackInfo(){
  const uint8_t sideWidth = 18;
  //track scrolling
  sequence.endTrack = sequence.startTrack + sequence.trackData.size();
  trackHeight = (screenHeight - headerHeight) / sequence.trackData.size();
  if(sequence.trackData.size()>5){
    sequence.endTrack = sequence.startTrack + 5;
    trackHeight = (screenHeight-headerHeight)/5;
  }
  while(sequence.activeTrack>sequence.endTrack-1 && sequence.trackData.size()>5){
    sequence.startTrack++;
    sequence.endTrack++;
  }
  while(sequence.activeTrack<sequence.startTrack && sequence.trackData.size()>5){
    sequence.startTrack--;
    sequence.endTrack--;
  }
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  //sideWidth border
  graphics.drawDottedLineV2(sideWidth,headerHeight+1,64,6);

  //top and bottom bounds
  display.drawFastHLine(0,headerHeight-1,screenWidth,SSD1306_WHITE);

  //tracks
  for(uint8_t track = sequence.startTrack; track<sequence.startTrack+5; track++){
    unsigned short int y1 = (track-sequence.startTrack) * trackHeight + headerHeight-1;
    if(sequence.trackData[track].isSelected()){
      //double digit
      if((track+1)>=10){
        display.setCursor(1-((millis()/200)%2), y1+4);
        display.print("{");
        display.setCursor(13+((millis()/200)%2), y1+4);
        display.print("}");
      }
      else{
        display.setCursor(3-((millis()/200)%2), y1+4);
        display.print("{");
        display.setCursor(13+((millis()/200)%2), y1+4);
        display.print("}");
      }
    }
    //track info display
    //single digit
    if((track+1)<10){
      printSmall(9,y1+5,stringify(track+1),SSD1306_WHITE);
    }
    else{
      printSmall(17-stringify(track+1).length()*6,y1+5,stringify(track+1),SSD1306_WHITE);
    }
    //track cursor
    if(track == sequence.activeTrack){
      //track
      if(xCursor == 0)
        graphics.drawArrow(3+((millis()/200)%2),y1+7,2,0,true);
      //note
      else if(xCursor == 1){
        if(sequence.trackData[track].getPitch().length()>1)
          display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 15, trackHeight+2, 3, SSD1306_WHITE);
        else
          display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 10, trackHeight+2, 3, SSD1306_WHITE);
      }
      //oct
      else if(xCursor == 2){
        if(sequence.trackData[track].pitch>=120){
          display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 13, trackHeight+2, 3, SSD1306_WHITE);
        }
        else
          display.drawRoundRect(sideWidth+(xCursor-1)*10+6, y1+2, 10, trackHeight+2, 3, SSD1306_WHITE);
      }
      //channel
      else if(xCursor == 3){
        if(sequence.trackData[track].channel>=10)
          display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 13, trackHeight+2, 3, SSD1306_WHITE);
        else
          display.drawRoundRect(sideWidth+(xCursor-1)*10+6, y1+2, 10, trackHeight+2, 3, SSD1306_WHITE);
      }
      //rec
      else if(xCursor == 4){
        display.drawCircle(sideWidth+(xCursor-1)*10+10, y1+7, 5, SSD1306_WHITE);
      }
      //mute group
      else if(xCursor == 6){
        if(sequence.trackData[track].muteGroup == 0)
          display.drawRoundRect(sideWidth+56, y1+2, 19, trackHeight+2, 3, SSD1306_WHITE);
        else
          display.drawRoundRect(sideWidth+56, y1+2, 5+4*stringify(sequence.trackData[track].muteGroup).length(), trackHeight+2, 3, SSD1306_WHITE);
      }
      else if(xCursor != 5)
        display.drawRoundRect(sideWidth+(xCursor-1)*10+6, y1+1, 10, trackHeight+2, 3, SSD1306_WHITE);
    }

    //all the track info...
    //pitch, oct, and channel
    printSmall(sideWidth+6,  y1+5, sequence.trackData[track].getPitch(), SSD1306_WHITE);//pitch

    if(getOctave(sequence.trackData[track].pitch)>=10)
      printSmall(sideWidth+16, y1+5, stringify(getOctave(sequence.trackData[track].pitch)), SSD1306_WHITE);//octave
    else
      printSmall(sideWidth+20, y1+5, stringify(getOctave(sequence.trackData[track].pitch)), SSD1306_WHITE);//octave

    if(sequence.trackData[track].channel>=10)
        printSmall(sideWidth+26, y1+5, stringify(sequence.trackData[track].channel), SSD1306_WHITE);//channel
    else
      printSmall(sideWidth+30, y1+5, stringify(sequence.trackData[track].channel), SSD1306_WHITE);//channel
    
    //primed rec symbol
    if(sequence.trackData[track].isPrimed()){
      if(millis()%1000>500){
        display.drawCircle(sideWidth+40, y1+7, 3, SSD1306_WHITE);
      }
      else{
        display.fillCircle(sideWidth+40, y1+7, 3, SSD1306_WHITE);
      }
    }

    //latch
    graphics.drawCheckbox(sideWidth+48,y1+4,sequence.trackData[track].isLatched(),xCursor == 5 && sequence.activeTrack == track);

    //mute group
    if(sequence.trackData[track].muteGroup == 0){
      printSmall(sideWidth+58, y1+5, "none",SSD1306_WHITE);
    }
    else{
      printSmall(sideWidth+59, y1+5, stringify(sequence.trackData[track].muteGroup),SSD1306_WHITE);
    }
  }
}
void TrackEditMenu::displayMenu(){
  display.clearDisplay();
  drawTrackInfo();
  //corner logo
  display.setCursor((screenWidth-coords.start.x)-34,7);
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.print("Trk");
  display.print(stringify(sequence.activeTrack+1));
  display.setFont();

  //track info
  drawTrackMenuTopInfo(coords.start.x);

  //drawing menu box
  display.fillRect(coords.start.x,coords.start.y, coords.end.x-coords.start.x, coords.end.y-coords.start.y, SSD1306_BLACK);
  display.drawRoundRect(coords.start.x,coords.start.y+12, coords.end.x-coords.start.x+1, coords.end.y-coords.start.y-11, 3,SSD1306_WHITE);
  String s;
  switch(cursor){
    case 0:
      s = "pitch";
      break;
    case 1:
      s = "octave";
      break;
    case 2:
      s = "chnnl";
      break;
    case 3:
      s = "prime";
      break;
    case 4:
      s = "utils";
      break;
    case 5:
      s = "mute";
      break;
    case 6:
      s = "move";
      break;
    case 7:
      s = "erase";
      break;
    case 8:
      s = "solo";
      break;
    case 9:
      s = "latch";
      break;
  }
  printChunky(coords.end.x-s.length()*6,coords.start.y+5,s,SSD1306_WHITE);

  //drawing menu options, and the highlight
  const uint8_t textHeight = 8;
  const uint8_t yPos = 12;
  //printing out the menu
  //pitch
  String p = pitchToString(sequence.trackData[sequence.activeTrack].pitch,false,true);
  if(cursor == 0){
    display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+2,8+p.length()*4+12,9,4,SSD1306_WHITE);
    display.drawChar(coords.start.x+6,yPos+coords.start.y+3,0x0E,SSD1306_BLACK,SSD1306_WHITE,1);
  }
  else
    display.drawChar(coords.start.x+6,yPos+coords.start.y+3,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
  printSmall(coords.start.x+12,yPos+coords.start.y+3,"$",2);//printing all the text inverse, so it turns black when highlighted
  printSmall(coords.start.x+14,yPos+coords.start.y+4," = "+p,2);
  //oct
  p = stringify(getOctave(sequence.trackData[sequence.activeTrack].pitch));
  if(cursor == 1)
    display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+3+textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(coords.start.x+6,yPos+coords.start.y+4+textHeight,"/8 = "+p,2);
  //channel
  p = stringify(sequence.trackData[sequence.activeTrack].channel);
  if(cursor == 2)
    display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+2+2*textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(coords.start.x+6,yPos+coords.start.y+3+2*textHeight,"ch = "+p,2);
  
  const uint8_t col1X = 5;
  const uint8_t col2X = 19;

  //prime icon
  // printSmall(coords.start.x+col1X,37,"rec",1);
  display.drawBitmap(coords.start.x+col1X,38,rec_tiny,11,4,1,0);
  if(sequence.trackData[sequence.activeTrack].isPrimed()){
    // printSmall(coords.start.x+2,37,"prime",1);
    if(millis()%1000<500)
      display.fillCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
    else
      display.drawCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
  }
  else{
    display.drawCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
  }
  //edit
  display.drawBitmap(coords.start.x+col1X,43,track_pencil,11,5,SSD1306_WHITE);
  //erase
  display.drawBitmap(coords.start.x+col2X,43,track_eraser,11,5,SSD1306_WHITE);
  //mute
  if(sequence.trackData[sequence.activeTrack].isMuted())
    display.drawBitmap(coords.start.x+col1X,50,track_mute,11,5,SSD1306_WHITE);
  else
    display.drawBitmap(coords.start.x+col1X,50,track_unmute,11,5,SSD1306_WHITE);
  //solo
  display.drawBitmap(coords.start.x+col2X,50,track_solo,11,5,SSD1306_WHITE);
  //move
  display.drawBitmap(coords.start.x+col1X,57,track_arrows,11,5,SSD1306_WHITE);
  //latch
  if(sequence.trackData[sequence.activeTrack].isLatched())
    display.drawBitmap(coords.start.x+col2X+3,57,track_latch,5,5,SSD1306_WHITE);
  else
    display.drawBitmap(coords.start.x+col2X+3,57,track_unlatch,5,5,SSD1306_WHITE);
  
  //menu highlight
  if(cursor == 3)
    graphics.drawArrow(coords.start.x+28+((millis()/200)%2),39,2,1,true);
  //first column
  else if(cursor>=4 && cursor<=6){
    graphics.drawRectWithMissingCorners(coords.start.x+3,34+7*(cursor-3),15,9,SSD1306_WHITE);
  }
  else if(cursor>=7){
    graphics.drawRectWithMissingCorners(coords.start.x+17,34+7*(cursor-6),15,9,SSD1306_WHITE);
  }
  display.display();
}

bool TrackEditMenu::trackEditMenuControls(){
  controls.readJoystick();
  controls.readButtons();
  if(utils.itsbeen(150)){
    //moving thru tracks
    if(controls.joystickY != 0){
      if (controls.joystickY == 1) {
        sequence.setActiveTrack(sequence.activeTrack + 1, true);
        lastTime = millis();
      }
      if (controls.joystickY == -1) {
        sequence.setActiveTrack(sequence.activeTrack - 1, true);
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    //moving thru params
    if(controls.joystickX != 0){
      //changing params
      if(controls.SHIFT()){
        if(controls.joystickX == 1){
          switch(xCursor){
            //note
            case 1:
              if(sequence.trackData[sequence.activeTrack].pitch>0){
                sequence.trackData[sequence.activeTrack].pitch--;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                  if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].pitch>0)
                    sequence.trackData[track].pitch--;
                }
              }
              break;
            //octave
            case 2:
              if(sequence.trackData[sequence.activeTrack].pitch>11){
                sequence.trackData[sequence.activeTrack].pitch-=12;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                  if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].pitch>11)
                    sequence.trackData[track].pitch-=12;
                }
              }
              break;
            //channel
            case 3:
              if(sequence.trackData[sequence.activeTrack].channel>1){
                sequence.trackData[sequence.activeTrack].channel--;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                  if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].channel>0)
                    sequence.trackData[track].channel--;
                }
              }
              break;
            //mute group
            case 6:
              if(sequence.trackData[sequence.activeTrack].muteGroup>0){
                sequence.trackData[sequence.activeTrack].muteGroup--;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                  if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].muteGroup>0)
                    sequence.trackData[track].muteGroup--;
                }
              }
              break;
          }
        }
        else if(controls.joystickX == -1){
          switch(xCursor){
            //note
            case 1:
              if(sequence.trackData[sequence.activeTrack].pitch<127){
                sequence.trackData[sequence.activeTrack].pitch++;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                  if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].pitch<127)
                    sequence.trackData[track].pitch++;
                }
              }
              break;
            //octave
            case 2:
              if(sequence.trackData[sequence.activeTrack].pitch<=115){
                sequence.trackData[sequence.activeTrack].pitch+=12;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                  if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].pitch<=115)
                    sequence.trackData[track].pitch+=12;
                }
              }
              break;
            //channel
            case 3:
              if(sequence.trackData[sequence.activeTrack].channel<16){
                sequence.trackData[sequence.activeTrack].channel++;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                  if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].channel<16)
                    sequence.trackData[track].channel++;
                }
              }
              break;
            //mute group
            case 6:
              if(sequence.trackData[sequence.activeTrack].muteGroup<127){
                sequence.trackData[sequence.activeTrack].muteGroup++;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                  if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].muteGroup<127)
                    sequence.trackData[track].muteGroup++;
                }
              }
              break;
          }
        }
      }
      else{
        if(controls.joystickX == 1 && xCursor > 0){
          xCursor--;
          lastTime = millis();
        }
        else if(controls.joystickX == -1 && xCursor<6){
          xCursor++;
          lastTime = millis();
        }
      }
    }
    if(controls.NEW()){
      sequence.addTrack(sequence.trackData[sequence.activeTrack].pitch,sequence.defaultChannel,false);
      lastTime = millis();
    }
    while(controls.counterA != 0){
      if(controls.counterA >= 1){
        switch(xCursor){
          //note
          case 1:
            if(sequence.trackData[sequence.activeTrack].pitch<127){
              sequence.trackData[sequence.activeTrack].pitch++;
              lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].pitch<127)
                  sequence.trackData[track].pitch++;
              }
            }
            break;
          //octave
          case 2:
            if(sequence.trackData[sequence.activeTrack].pitch<=115){
              sequence.trackData[sequence.activeTrack].pitch+=12;
              lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].pitch<=115)
                  sequence.trackData[track].pitch+=12;
              }
            }
            break;
          //channel
          case 3:
            if(sequence.trackData[sequence.activeTrack].channel<16){
              sequence.trackData[sequence.activeTrack].channel++;
              lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].channel<16)
                  sequence.trackData[track].channel++;
              }
            }
            break;
          //mute group
          case 6:
            if(sequence.trackData[sequence.activeTrack].muteGroup<127){
              sequence.trackData[sequence.activeTrack].muteGroup++;
              lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].muteGroup<127)
                  sequence.trackData[track].muteGroup++;
              }
            }
            break;
        }
      }
      else{
        switch(xCursor){
          //note
          case 1:
            if(sequence.trackData[sequence.activeTrack].pitch>0){
              sequence.trackData[sequence.activeTrack].pitch--;
              lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].pitch>0)
                  sequence.trackData[track].pitch--;
              }
            }
            break;
          //octave
          case 2:
            if(sequence.trackData[sequence.activeTrack].pitch>11){
              sequence.trackData[sequence.activeTrack].pitch-=12;
              lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].pitch>11)
                  sequence.trackData[track].pitch-=12;
              }
            }
            break;
          //channel
          case 3:
            if(sequence.trackData[sequence.activeTrack].channel>1){
              sequence.trackData[sequence.activeTrack].channel--;
              lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].channel>0)
                  sequence.trackData[track].channel--;
              }
            }
            break;
          //mute group
          case 6:
            if(sequence.trackData[sequence.activeTrack].muteGroup>0){
              sequence.trackData[sequence.activeTrack].muteGroup--;
              lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<sequence.trackData.size(); track++){
                if(track != sequence.activeTrack && sequence.trackData[track].isSelected() && sequence.trackData[track].muteGroup>0)
                  sequence.trackData[track].muteGroup--;
              }
            }
            break;
        }
      }
      controls.counterA += controls.counterA<0?1:-1;
    }
    if(controls.SELECT() ){
      switch(xCursor){
        //if it's on track, select
        case 0:
          //toggles selection on all
          if(controls.SHIFT()){
            sequence.trackData[sequence.activeTrack].setSelected(!sequence.trackData[sequence.activeTrack].isSelected());
            for(uint8_t track = 0; track<sequence.trackData.size(); track++){
              sequence.trackData[track].setSelected(sequence.trackData[sequence.activeTrack].isSelected());
            }
          }
          //normal selection toggle
          else
            sequence.trackData[sequence.activeTrack].setSelected(!sequence.trackData[sequence.activeTrack].isSelected());
          
          lastTime = millis();
          break;
        //if it's on primed
        case 4:
          //toggles prime on all
          if(controls.SHIFT()){
            sequence.trackData[sequence.activeTrack].setPrimed(!sequence.trackData[sequence.activeTrack].isPrimed());
            for(uint8_t track = 0; track<sequence.trackData.size(); track++){
              sequence.trackData[track].setPrimed(sequence.trackData[sequence.activeTrack].isPrimed());
            }
          }
          //normal selection toggle
          else{
            sequence.trackData[sequence.activeTrack].setPrimed(!sequence.trackData[sequence.activeTrack].isPrimed());
            for(uint8_t track = 0; track<sequence.trackData.size(); track++){
              if(track != sequence.activeTrack && sequence.trackData[track].isSelected())
                sequence.trackData[track].setPrimed(sequence.trackData[sequence.activeTrack].isPrimed());
            }
          }
          lastTime = millis();
          break;
        //if it's on latch
        case 5:
          //toggles prime on all
          if(controls.SHIFT()){
            sequence.trackData[sequence.activeTrack].setLatched(!sequence.trackData[sequence.activeTrack].isLatched());
            for(uint8_t track = 0; track<sequence.trackData.size(); track++){
              sequence.trackData[track].setLatched(sequence.trackData[sequence.activeTrack].isLatched());
            }
          }
          //normal selection toggle
          else{
            sequence.trackData[sequence.activeTrack].setLatched(!sequence.trackData[sequence.activeTrack].isLatched());
            for(uint8_t track = 0; track<sequence.trackData.size(); track++){
              if(track != sequence.activeTrack && sequence.trackData[track].isSelected())
                sequence.trackData[track].setLatched(sequence.trackData[sequence.activeTrack].isLatched());
            }
          }
          lastTime = millis();
          break;
      }
    }
    if(controls.MENU() || controls.A()){
      for(uint8_t track = 0; track<sequence.trackData.size(); track++){
        sequence.trackData[track].setSelected(false);
      }
      lastTime = millis();
      controls.setMENU(false) ;
      controls.setA(false);;
      return false;
    }
  }
  return true;
}

void trackEditMenu(){
  //deselecting all the tracks
  for(uint8_t track = 0; track<sequence.trackData.size(); track++){
    sequence.trackData[track].setSelected(false);
  }
  TrackEditMenu trackEditMenu;
  while(trackEditMenu.trackEditMenuControls()){
    trackEditMenu.displayMenu();
  }
  //deselecting all the tracks
  for(uint8_t track = 0; track<sequence.trackData.size(); track++){
    sequence.trackData[track].setSelected(false);
  }
}

/*
--------------------------------
      Track Utility Menu
--------------------------------
*/

class TrackUtilMenu:public StepchildMenu{
  public:
    //storing a ref to the track menu so you can call the display method
    TrackMenu* trackMenu;
    const vector<String> options = {"group edit tracks","tune 2 scale","delete empty tracks","disarm tracks w notes","sort tracks"};
    TrackUtilMenu(TrackMenu* t){trackMenu = t;
                                coords = CoordinatePair(0,16,128,64);}
    void drawTrackUtils();
    bool trackUtilMenuControls();
    void displayMenu();
};

void TrackUtilMenu::displayMenu(){
  display.clearDisplay();
  if(coords.start.y>16){
  SequenceRenderSettings settings;
    settings.shrinkTopDisplay = false;
    settings.drawLoopFlags = false;
    settings.drawPram = false;
    settings.topLabels = false;
    settings.drawTrackChannel = true;
    drawSeq(settings);
  }
  trackMenu->displayMenuSidebar();
  drawTrackUtils();
  display.display();
}

//shows options for edit, tune tracks, del empty tracks, and disarm tracks w/ notes
void TrackUtilMenu::drawTrackUtils(){
  const uint8_t length = 95;
  display.fillRect(coords.start.x,coords.start.y,length,screenHeight-coords.start.y,0);
  display.drawRect(coords.start.x,coords.start.y,length,screenHeight-coords.start.y,1);
  for(uint8_t i = 0; i<options.size(); i++){
    if(i == cursor){
      display.fillRect(coords.start.x+1,coords.start.y+i*6+1,length-2,7,1);
    }
    printSmall(coords.start.x+2,coords.start.y+i*6+2,options[i],2);
  }
}

bool TrackUtilMenu::trackUtilMenuControls(){
  controls.readJoystick();
  controls.readButtons();
  if(utils.itsbeen(100)){
    if(controls.joystickY == -1 && cursor>0){
      cursor--;
      lastTime = millis();
    }
    else if(controls.joystickY == 1 && cursor<(options.size()-1)){
      cursor++;
      lastTime = millis();
    }
  }
  if(utils.itsbeen(200)){
    if(controls.MENU()){
      lastTime = millis();
      return false;
    }
    if(controls.SELECT() ){
      lastTime = millis();
      switch(cursor){
        case 0:
          trackEditMenu();
          break;
        case 1:
          tuneTracksToScale();
          break;
        case 2:
          sequence.deleteEmptyTracks();
          break;
        case 3:
          sequence.disarmTracksWithNotes();
          break;
        case 4:
          sortTracks();
          break;
      }
    }
  }
  return true;
}

void trackUtils(TrackMenu* t){
  TrackUtilMenu trackUtilMenu(t);
  trackUtilMenu.slideIn(IN_FROM_BOTTOM,MENU_SLIDE_FAST);
  while(trackUtilMenu.trackUtilMenuControls()){
    trackUtilMenu.displayMenu();
  }
  trackUtilMenu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_FAST);
}

/*
    Common drawing functions:
*/


//primed, pitch, latch, mute info
void drawTrackMenuTopInfo(uint8_t topL){
  //track info
  uint8_t x1 = 3+(screenWidth-topL)+uint8_t(log10(sequence.activeTrack+1))*9;
  String p = pitchToString(sequence.trackData[sequence.activeTrack].pitch,true,true);
  printSmall(x1,0,p,1);
  printSmall(x1+4+p.length()*4,0,"("+stringify(sequence.trackData[sequence.activeTrack].pitch)+")",1);
  //primed
  if(sequence.trackData[sequence.activeTrack].isPrimed()){
    if(millis()%1000>500){
      display.drawCircle(x1+3,10,3,1);
    }
    else{
      display.fillCircle(x1+3,10,3,1);
    }
  }
  //latch
  if(sequence.trackData[sequence.activeTrack].isLatched()){
    display.drawBitmap(x1+9,7,latch_big,7,7,1,0);
    display.fillRect(x1+10,9,5,4,1);
  }
  else
    display.drawBitmap(x1+9,7,latch_big,7,7,1,0);
  //mute
  display.drawBitmap(x1+18,7,sequence.trackData[sequence.activeTrack].isMuted()?muted:unmuted,7,7,1,0);
  //solo'd
  if(sequence.trackData[sequence.activeTrack].isSolo())
    printItalic(x1+27,7,"S",1);
  //drum icon
  graphics.drawDrumIcon(x1+35,2,sequence.trackData[sequence.activeTrack].pitch);
}
