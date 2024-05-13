void trackUtils();

bool trackMenuControls(){
  //moving menu cursor
  if(!shift){
    if(utils.itsbeen(100)){
      if(controls.joystickY != 0){
        if(controls.joystickY == 1){
          if(activeMenu.highlight<9 && activeMenu.highlight != 6){
            activeMenu.highlight++;
            lastTime = millis();
          }
        }
        if(controls.joystickY == -1){
          if(activeMenu.highlight == 7){
            activeMenu.highlight = 3;
            lastTime = millis();
          }
          else if(activeMenu.highlight>0){
            activeMenu.highlight--;
            lastTime = millis();
          }
        }
      }
    }
  }
  if(utils.itsbeen(100)){
    if(shift && controls.joystickY != 0){
      if(controls.joystickY == -1){
        setActiveTrack(activeTrack-1,true);
        lastTime = millis();
      }
      if(controls.joystickY == 1){
        setActiveTrack(activeTrack+1,true);
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    if(play){
      trackData[activeTrack].isPrimed = !trackData[activeTrack].isPrimed;
      lastTime = millis();
    }
    if(sel){
      sel = false;
      lastTime = millis();
      switch(activeMenu.highlight){
        //tune
        case 0:
          tuneTracksToScale();
          break;
        //midi route/channel
        case 2:
          routeMenu();
          break;
        case 3:
          trackData[activeTrack].isPrimed = !trackData[activeTrack].isPrimed;
          if(shift){
            for(uint8_t track = 0; track<trackData.size(); track++){
              trackData[track].isPrimed = trackData[activeTrack].isPrimed;
            }
          }
          break;
        //edit
        case 4:
          trackUtils();
          break;
        //mute
        case 5:
          if(shift){
            muteMultipleTracks(selectMultipleTracks(stringify("Mute which?")));
          }
          else{
            toggleMute(activeTrack);
          }
          break;
        //move
        case 6:
          swapTracks();
          break;
        //erase
        case 7:
          if(shift){
            eraseMultipleTracks(selectMultipleTracks(stringify("Erase which?")));
          }
          else{
            eraseTrack(activeTrack);
          }
          break;
        //solo
        case 8:
          toggleSolo(activeTrack);
          break;
        //latch
        case 9:
          trackData[activeTrack].isLatched = !trackData[activeTrack].isLatched;
          //latch all tracks
          if(shift){
            for(int track = 0; track<trackData.size(); track++){
              trackData[track].isLatched = trackData[activeTrack].isLatched;
            }
          }
          break;
      }
    }
    if(del){
      if(!shift){
        lastTime = millis();
        deleteTrack(activeTrack);
      }
      else if(shift){
        lastTime = millis();
        deleteEmptyTracks();
      }
    }
    if(n){
      if(shift)
        dupeTrack(activeTrack);
      else
        addTrack(trackData[activeTrack].pitch+1,true);
      lastTime = millis();
    }
  }
  if(utils.itsbeen(200)){
    if(controls.A() || menu_Press){
      slideMenuOut(1,10);
      menuIsActive = false;
      constructMenu("MENU");
      lastTime = millis();
      return false;
    }
    if(controls.B()){
      slideMenuOut(1,10);
      lastTime = millis();
      constructMenu("EDIT");
      return false;
    }
  }
  //menu navigation/incrementing values
  if(utils.itsbeen(100)){
    if(controls.joystickX == -1){
      if(activeMenu.highlight >= 4 && activeMenu.highlight <=6){
        activeMenu.highlight+=3;
        lastTime = millis();
      }
    }
    else if(controls.joystickX == 1){
      if(activeMenu.highlight >= 7 && activeMenu.highlight <= 9){
        activeMenu.highlight-=3;
        lastTime = millis();
      }
    }
    if(activeMenu.highlight<3){
      while(counterA != 0){
        if(counterA >= 1 || controls.joystickX == -1){
          //pitch
          if(activeMenu.highlight == 0 && trackData[activeTrack].pitch<120){
            if(shift)
              transposeAllPitches(1);
            else
              setTrackPitch(activeTrack,trackData[activeTrack].pitch+1,true);
            lastTime = millis();
          }
          //octave
          else if(activeMenu.highlight == 1 && trackData[activeTrack].pitch<108){
            if(shift)
              transposeAllPitches(12);
            else
              setTrackPitch(activeTrack,trackData[activeTrack].pitch+12,true);
            lastTime = millis();
          }
          //channel
          else if(activeMenu.highlight == 2 && trackData[activeTrack].channel<16){
            if(shift)
              transposeAllChannels(1);
            else
              setTrackChannel(activeTrack,trackData[activeTrack].channel+1,true);
            lastTime = millis();
          }
          counterA += counterA<0?1:-1;;
        }
        if(counterA <= -1 || controls.joystickX == 1){
          //pitch
          if(activeMenu.highlight == 0 && trackData[activeTrack].pitch>0){
            if(shift)
              transposeAllPitches(-1);
            else
              setTrackPitch(activeTrack,trackData[activeTrack].pitch-1,true);
            lastTime = millis();
          }
          //octave
          else if(activeMenu.highlight == 1 && trackData[activeTrack].pitch>11){
            if(shift)
              transposeAllPitches(-12);
            else
              setTrackPitch(activeTrack,trackData[activeTrack].pitch-12,true);
            lastTime = millis();
          }
          //channel
          else if(activeMenu.highlight == 2 && trackData[activeTrack].channel>1){
            if(shift)
              transposeAllChannels(-1);
            else
              setTrackChannel(activeTrack,trackData[activeTrack].channel-1,true);
            lastTime = millis();
          }
          counterA += counterA<0?1:-1;;
        }
      }
    }
    while(counterB != 0){
      if(counterB <= -1){
        setActiveTrack(activeTrack+1,true);
        lastTime = millis();
      }
      if(counterB >= 1){
        setActiveTrack(activeTrack-1,true);
        lastTime = millis();
      }
      counterB += counterB<0?1:-1;;
    }
  }
  return true;
}

void trackMenu(){
  while(true){
    display.clearDisplay();
    drawSeq(true,false,false,false,false);
    if(activeMenu.page == 0)
      activeMenu.displayTrackMenu();
    display.display();
    controls.readJoystick();
    readButtons();
    if(!trackMenuControls()){
      break;
    }
  }
}

//shows options for edit, tune tracks, delete empty tracks, and disarm tracks w/ notes
void drawTrackUtils(uint8_t cursor,vector<String> options){
  const uint8_t x1 = 0;
  const uint8_t y1 = headerHeight;
  const uint8_t length = 95;
  display.fillRect(x1,y1,length,screenHeight-y1,0);
  display.drawRect(x1,y1,length,screenHeight-y1,1);
  for(uint8_t i = 0; i<options.size(); i++){
    if(i == cursor){
      display.fillRect(x1+1,y1+i*6+1,length-2,7,1);
    }
    printSmall(x1+2,y1+i*6+2,options[i],2);
  }
}

void trackUtils(){
  uint8_t cursor = 0;
  vector<String> options = {"edit tracks","tune 2 scale","delete empty tracks","disarm tracks w notes","sort tracks"};
  while(true){
    controls.readJoystick();
    readButtons();
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
      if(menu_Press){
        lastTime = millis();
        break;
      }
      if(sel){
        lastTime = millis();
        switch(cursor){
          case 0:
            trackEditMenu();
            break;
          case 1:
            tuneTracksToScale();
            break;
          case 2:
            deleteEmptyTracks();
            break;
          case 3:
            disarmTracksWithNotes();
            break;
          case 4:
            sortTracks();
            break;
        }
      }
    }
    display.clearDisplay();
    drawTrackUtils(cursor,options);
    activeMenu.displayTrackMenu();
    display.display();
  }
}

//primed, pitch, latch, mute info
void drawTrackMenuTopInfo(uint8_t topL){
  //track info
  uint8_t x1 = 3+(screenWidth-topL)+uint8_t(log10(activeTrack+1))*9;
  String p = pitchToString(trackData[activeTrack].pitch,true,true);
  printSmall(x1,0,p,1);
  printSmall(x1+4+p.length()*4,0,"("+stringify(trackData[activeTrack].pitch)+")",1);
  //primed
  if(trackData[activeTrack].isPrimed){
    if(millis()%1000>500){
      display.drawCircle(x1+3,10,3,1);
    }
    else{
      display.fillCircle(x1+3,10,3,1);
    }
  }
  //latch
  if(trackData[activeTrack].isLatched){
    display.drawBitmap(x1+9,7,latch_big,7,7,1,0);
    display.fillRect(x1+10,9,5,4,1);
  }
  else
    display.drawBitmap(x1+9,7,latch_big,7,7,1,0);
  //mute
  display.drawBitmap(x1+18,7,trackData[activeTrack].isMuted?muted:unmuted,7,7,1,0);
  //solo'd
  if(trackData[activeTrack].isSolo)
    printItalic(x1+27,7,"S",1);
  //drum icon
  drawDrumIcon(x1+35,2,trackData[activeTrack].pitch);
}


void Menu::displayTrackMenu_trackEdit(uint8_t xCursor){
  //title
  display.setCursor((screenWidth-coords.start.x)-34,7);
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.print("Trk");
  display.print(stringify(activeTrack+1));
  display.setFont();

  //drawing menu box
  display.fillRoundRect(coords.start.x,coords.start.y+12,coords.end.x-coords.start.x+1,coords.end.y-coords.start.y-11,3,SSD1306_BLACK);
  display.drawRoundRect(coords.start.x,coords.start.y+12,coords.end.x-coords.start.x+1,coords.end.y-coords.start.y-11,3,SSD1306_WHITE);
  //top labels 
  const vector<String> texts = {"track","pitch","octave","channel","prime","latch","mute"};
  printChunky(coords.end.x-texts[xCursor].length()*6,coords.start.y+5,texts[xCursor],1);
  drawTrackMenuTopInfo(coords.start.x);

  //drawing menu options, and the highlight
  const uint8_t textWidth = 20;
  const uint8_t textHeight = 8;
  const uint8_t yPos = 12;
  //printing out the menu
  //pitch
  String p = pitchToString(trackData[activeTrack].pitch,false,true);
  if(highlight == 0){
    display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+2,8+p.length()*4+12,9,4,SSD1306_WHITE);
    display.drawChar(coords.start.x+6,yPos+coords.start.y+3,0x0E,SSD1306_BLACK,SSD1306_WHITE,1);
  }
  else
    display.drawChar(coords.start.x+6,yPos+coords.start.y+3,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
  printSmall(coords.start.x+12,yPos+coords.start.y+3,"$",2);//printing all the text inverse, so it turns black when highlighted
  printSmall(coords.start.x+14,yPos+coords.start.y+4," = "+p,2);
  //oct
  p = stringify(getOctave(trackData[activeTrack].pitch));
  if(highlight == 1)
    display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+3+textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(coords.start.x+6,yPos+coords.start.y+4+textHeight,"/8 = "+p,2);
  //channel
  p = stringify(trackData[activeTrack].channel);
  if(highlight == 2)
    display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+2+2*textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(coords.start.x+6,yPos+coords.start.y+3+2*textHeight,"ch = "+p,2);
  
  const uint8_t col1X = 5;
  const uint8_t col2X = 19;

  //prime icon
  // printSmall(coords.start.x+col1X,37,"rec",1);
  display.drawBitmap(coords.start.x+col1X,38,rec_tiny,11,4,1,0);
  if(trackData[activeTrack].isPrimed){
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
  if(trackData[activeTrack].isMuted)
    display.drawBitmap(coords.start.x+col1X,50,track_mute,11,5,SSD1306_WHITE);
  else
    display.drawBitmap(coords.start.x+col1X,50,track_unmute,11,5,SSD1306_WHITE);
  //solo
  display.drawBitmap(coords.start.x+col2X,50,track_solo,11,5,SSD1306_WHITE);
  //move
  display.drawBitmap(coords.start.x+col1X,57,track_arrows,11,5,SSD1306_WHITE);
  //latch
  if(trackData[activeTrack].isLatched)
    display.drawBitmap(coords.start.x+col2X+3,57,track_latch,5,5,SSD1306_WHITE);
  else
    display.drawBitmap(coords.start.x+col2X+3,57,track_unlatch,5,5,SSD1306_WHITE);
  
  //menu highlight
  if(highlight == 3)
    graphics.drawArrow(coords.start.x+25+((millis()/200)%2),39,2,1,true);
  //first column
  else if(highlight>=4 && highlight<=6)
    display.drawRoundRect(coords.start.x+3,34+7*(highlight-3),15,9,2,SSD1306_WHITE);
  else if(highlight>=7)
    display.drawRoundRect(coords.start.x+17,34+7*(highlight-6),15,9,2,SSD1306_WHITE);
}

void Menu::displayTrackMenu(){
  //corner logo
  display.setCursor((screenWidth-coords.start.x)-34,7);
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.print("Trk");
  display.print(stringify(activeTrack+1));
  display.setFont();
  // display.drawFastHLine(0,16,32,SSD1306_WHITE);

  //track info
  drawTrackMenuTopInfo(coords.start.x);

  //drawing menu box
  display.fillRect(coords.start.x,coords.start.y, coords.end.x-coords.start.x, coords.end.y-coords.start.y, SSD1306_BLACK);
  display.drawRoundRect(coords.start.x,coords.start.y+12, coords.end.x-coords.start.x+1, coords.end.y-coords.start.y-11, 3,SSD1306_WHITE);
  String s;
  switch(highlight){
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
  const uint8_t textWidth = 20;
  const uint8_t textHeight = 8;
  const uint8_t yPos = 12;
  //printing out the menu
  //pitch
  String p = pitchToString(trackData[activeTrack].pitch,false,true);
  if(highlight == 0){
    display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+2,8+p.length()*4+12,9,4,SSD1306_WHITE);
    display.drawChar(coords.start.x+6,yPos+coords.start.y+3,0x0E,SSD1306_BLACK,SSD1306_WHITE,1);
  }
  else
    display.drawChar(coords.start.x+6,yPos+coords.start.y+3,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
  printSmall(coords.start.x+12,yPos+coords.start.y+3,"$",2);//printing all the text inverse, so it turns black when highlighted
  printSmall(coords.start.x+14,yPos+coords.start.y+4," = "+p,2);
  //oct
  p = stringify(getOctave(trackData[activeTrack].pitch));
  if(highlight == 1)
    display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+3+textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(coords.start.x+6,yPos+coords.start.y+4+textHeight,"/8 = "+p,2);
  //channel
  p = stringify(trackData[activeTrack].channel);
  if(highlight == 2)
    display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+2+2*textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(coords.start.x+6,yPos+coords.start.y+3+2*textHeight,"ch = "+p,2);
  
  const uint8_t col1X = 5;
  const uint8_t col2X = 19;

  //prime icon
  // printSmall(coords.start.x+col1X,37,"rec",1);
  display.drawBitmap(coords.start.x+col1X,38,rec_tiny,11,4,1,0);
  if(trackData[activeTrack].isPrimed){
    // printSmall(coords.start.x+2,37,"prime",1);
    if(millis()%1000<500)
      display.fillCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
    else
      display.drawCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
  }
  else{
    display.drawCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
  }

  // display.drawRoundRect(coords.start.x+2,42,screenWidth-coords.start.x-2,27,3,SSD1306_WHITE);
  //edit
  display.drawBitmap(coords.start.x+col1X,43,track_pencil,11,5,SSD1306_WHITE);
  //erase
  display.drawBitmap(coords.start.x+col2X,43,track_eraser,11,5,SSD1306_WHITE);
  //mute
  if(trackData[activeTrack].isMuted)
    display.drawBitmap(coords.start.x+col1X,50,track_mute,11,5,SSD1306_WHITE);
  else
    display.drawBitmap(coords.start.x+col1X,50,track_unmute,11,5,SSD1306_WHITE);
  //solo
  display.drawBitmap(coords.start.x+col2X,50,track_solo,11,5,SSD1306_WHITE);
  //move
  display.drawBitmap(coords.start.x+col1X,57,track_arrows,11,5,SSD1306_WHITE);
  //latch
  if(trackData[activeTrack].isLatched)
    display.drawBitmap(coords.start.x+col2X+3,57,track_latch,5,5,SSD1306_WHITE);
  else
    display.drawBitmap(coords.start.x+col2X+3,57,track_unlatch,5,5,SSD1306_WHITE);
  
  //menu highlight
  if(highlight == 3)
    graphics.drawArrow(coords.start.x+28+((millis()/200)%2),39,2,1,true);
  //first column
  else if(highlight>=4 && highlight<=6){
    display.drawRoundRect(coords.start.x+3,34+7*(highlight-3),15,9,2,SSD1306_WHITE);
  }
  else if(highlight>=7){
    display.drawRoundRect(coords.start.x+17,34+7*(highlight-6),15,9,2,SSD1306_WHITE);
  }
}

//gen midi numbers taken from https://usermanuals.finalemusic.com/SongWriter2012Win/Content/PercussionMaps.htm
void drawDrumIcon(uint8_t x1, uint8_t y1, uint8_t note){
  int8_t which = -1;
  switch(note){
    //"bass drum" => 808
    case 35:
      which = 0;
      break;
    //"kick"
    case 36:
      which = 1;
      break;
    //cowbell
    case 34:
    case 56:
      which = 2;
      break;
    //clap
    case 39:
      which = 3;
      break;
    //crash cymbal
    case 49:
      which = 4;
      break;
    //"laser" ==> gun (sfx)
    case 27:
    case 28:
    case 29:
    case 30:
      which = 5;
      break;
    //closing hat pedal
    case 44:
      which = 6;
      break;
    //open hat
    case 46:
      which = 7;
      break;
    //closed hat
    case 42:
      which = 8;
      break;
    //rim
    case 31:
    case 32:
    case 37:
      which = 9;
      break;
    //shaker
    case 82:
      which = 10;
      break;
    //snare
    case 38:
    case 40:
      which = 11;
      break;
    //tomL
    case 45:
      which = 12;
      break;
    //tomM
    case 47:
    case 48:
      which = 13;
      break;
    //tomS
    case 50:
      which = 14;
      break;
    //triangle
    case 80:
    case 81:
      which = 15;
      break;
  }
  if(which != -1){
    display.drawBitmap(x1,y1,drum_icons[which],16,8,1,0);
  }
}

void drawTrackInfo(uint8_t xCursor){
  const uint8_t sideWidth = 18;
  //track scrolling
  endTrack = startTrack + trackData.size();
  trackHeight = (screenHeight - headerHeight) / trackData.size();
  if(trackData.size()>5){
    endTrack = startTrack + 5;
    trackHeight = (screenHeight-headerHeight)/5;
  }
  while(activeTrack>endTrack-1 && trackData.size()>5){
    startTrack++;
    endTrack++;
  }
  while(activeTrack<startTrack && trackData.size()>5){
    startTrack--;
    endTrack--;
  }
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  //sideWidth border
  graphics.drawDottedLineV2(sideWidth,headerHeight+1,64,6);

  //top and bottom bounds
  display.drawFastHLine(0,headerHeight-1,screenWidth,SSD1306_WHITE);

  //tracks
  for(uint8_t track = startTrack; track<startTrack+5; track++){
    unsigned short int y1 = (track-startTrack) * trackHeight + headerHeight-1;
    unsigned short int y2 = y1 + trackHeight;
    if(trackData[track].isSelected){
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
    if(track == activeTrack){
      //track
      if(xCursor == 0)
        graphics.drawArrow(3+((millis()/200)%2),y1+7,2,0,true);
      //note
      else if(xCursor == 1){
        if(trackData[track].getPitch().length()>1)
          display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 15, trackHeight+2, 3, SSD1306_WHITE);
        else
          display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 10, trackHeight+2, 3, SSD1306_WHITE);
      }
      //oct
      else if(xCursor == 2){
        if(trackData[track].pitch>=120){
          display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 13, trackHeight+2, 3, SSD1306_WHITE);
        }
        else
          display.drawRoundRect(sideWidth+(xCursor-1)*10+6, y1+2, 10, trackHeight+2, 3, SSD1306_WHITE);
      }
      //channel
      else if(xCursor == 3){
        if(trackData[track].channel>=10)
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
        if(trackData[track].muteGroup == 0)
          display.drawRoundRect(sideWidth+56, y1+2, 19, trackHeight+2, 3, SSD1306_WHITE);
        else
          display.drawRoundRect(sideWidth+56, y1+2, 5+4*stringify(trackData[track].muteGroup).length(), trackHeight+2, 3, SSD1306_WHITE);
      }
      else if(xCursor != 5)
        display.drawRoundRect(sideWidth+(xCursor-1)*10+6, y1+1, 10, trackHeight+2, 3, SSD1306_WHITE);
    }

    //all the track info...
    //pitch, oct, and channel
    printSmall(sideWidth+6,  y1+5, trackData[track].getPitch(), SSD1306_WHITE);//pitch

    if(getOctave(trackData[track].pitch)>=10)
      printSmall(sideWidth+16, y1+5, stringify(getOctave(trackData[track].pitch)), SSD1306_WHITE);//octave
    else
      printSmall(sideWidth+20, y1+5, stringify(getOctave(trackData[track].pitch)), SSD1306_WHITE);//octave

    if(trackData[track].channel>=10)
        printSmall(sideWidth+26, y1+5, stringify(trackData[track].channel), SSD1306_WHITE);//channel
    else
      printSmall(sideWidth+30, y1+5, stringify(trackData[track].channel), SSD1306_WHITE);//channel
    
    //primed rec symbol
    if(trackData[track].isPrimed){
      if(millis()%1000>500){
        display.drawCircle(sideWidth+40, y1+7, 3, SSD1306_WHITE);
      }
      else{
        display.fillCircle(sideWidth+40, y1+7, 3, SSD1306_WHITE);
      }
    }

    //latch
    graphics.drawCheckbox(sideWidth+48,y1+4,trackData[track].isLatched,xCursor == 5 && activeTrack == track);

    //mute group
    if(trackData[track].muteGroup == 0){
      printSmall(sideWidth+58, y1+5, "none",SSD1306_WHITE);
    }
    else{
      printSmall(sideWidth+59, y1+5, stringify(trackData[track].muteGroup),SSD1306_WHITE);
    }
  }
}

void trackEditMenu(){
  //deselecting all the tracks
  for(uint8_t track = 0; track<trackData.size(); track++){
    trackData[track].isSelected = false;
  }
  //for which param to edit
  uint8_t xCursor = 0;
  //params are: track select, note, oct, channel, prime, latch, mute group
  //for which track to edit
  uint8_t yCursor = 0;
  String text1;
  String text2;
  vector<uint8_t> selectedTracks;
  while(true){
    controls.readJoystick();
    readButtons();
    if(utils.itsbeen(150)){
      //moving thru tracks
      if(controls.joystickY != 0){
        if (controls.joystickY == 1) {
          setActiveTrack(activeTrack + 1, true);
          lastTime = millis();
        }
        if (controls.joystickY == -1) {
          setActiveTrack(activeTrack - 1, true);
          lastTime = millis();
        }
      }
    }
    if(utils.itsbeen(200)){
      //moving thru params
      if(controls.joystickX != 0){
        //changing params
        if(shift){
          if(controls.joystickX == 1){
            switch(xCursor){
              //note
              case 1:
                if(trackData[activeTrack].pitch>0){
                  trackData[activeTrack].pitch--;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch>0)
                      trackData[track].pitch--;
                  }
                }
                break;
              //octave
              case 2:
                if(trackData[activeTrack].pitch>11){
                  trackData[activeTrack].pitch-=12;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch>11)
                      trackData[track].pitch-=12;
                  }
                }
                break;
              //channel
              case 3:
                if(trackData[activeTrack].channel>1){
                  trackData[activeTrack].channel--;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].channel>0)
                      trackData[track].channel--;
                  }
                }
                break;
              //mute group
              case 6:
                if(trackData[activeTrack].muteGroup>0){
                  trackData[activeTrack].muteGroup--;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].muteGroup>0)
                      trackData[track].muteGroup--;
                  }
                }
                break;
            }
          }
          else if(controls.joystickX == -1){
            switch(xCursor){
              //note
              case 1:
                if(trackData[activeTrack].pitch<127){
                  trackData[activeTrack].pitch++;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch<127)
                      trackData[track].pitch++;
                  }
                }
                break;
              //octave
              case 2:
                if(trackData[activeTrack].pitch<=115){
                  trackData[activeTrack].pitch+=12;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch<=115)
                      trackData[track].pitch+=12;
                  }
                }
                break;
              //channel
              case 3:
                if(trackData[activeTrack].channel<16){
                  trackData[activeTrack].channel++;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].channel<16)
                      trackData[track].channel++;
                  }
                }
                break;
              //mute group
              case 6:
                if(trackData[activeTrack].muteGroup<127){
                  trackData[activeTrack].muteGroup++;
                  lastTime = millis();
                  //checking all selected tracks
                  for(uint8_t track = 0; track<trackData.size(); track++){
                    if(track != activeTrack && trackData[track].isSelected && trackData[track].muteGroup<127)
                      trackData[track].muteGroup++;
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
      if(n){
        addTrack(trackData[activeTrack].pitch);
        setActiveTrack(trackData.size()-1,false);
        lastTime = millis();
      }
      while(counterA != 0){
        if(counterA >= 1){
          switch(xCursor){
            //note
            case 1:
              if(trackData[activeTrack].pitch<127){
                trackData[activeTrack].pitch++;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch<127)
                    trackData[track].pitch++;
                }
              }
              break;
            //octave
            case 2:
              if(trackData[activeTrack].pitch<=115){
                trackData[activeTrack].pitch+=12;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch<=115)
                    trackData[track].pitch+=12;
                }
              }
              break;
            //channel
            case 3:
              if(trackData[activeTrack].channel<16){
                trackData[activeTrack].channel++;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].channel<16)
                    trackData[track].channel++;
                }
              }
              break;
            //mute group
            case 6:
              if(trackData[activeTrack].muteGroup<127){
                trackData[activeTrack].muteGroup++;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].muteGroup<127)
                    trackData[track].muteGroup++;
                }
              }
              break;
          }
        }
        else{
          switch(xCursor){
            //note
            case 1:
              if(trackData[activeTrack].pitch>0){
                trackData[activeTrack].pitch--;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch>0)
                    trackData[track].pitch--;
                }
              }
              break;
            //octave
            case 2:
              if(trackData[activeTrack].pitch>11){
                trackData[activeTrack].pitch-=12;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].pitch>11)
                    trackData[track].pitch-=12;
                }
              }
              break;
            //channel
            case 3:
              if(trackData[activeTrack].channel>1){
                trackData[activeTrack].channel--;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].channel>0)
                    trackData[track].channel--;
                }
              }
              break;
            //mute group
            case 6:
              if(trackData[activeTrack].muteGroup>0){
                trackData[activeTrack].muteGroup--;
                lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<trackData.size(); track++){
                  if(track != activeTrack && trackData[track].isSelected && trackData[track].muteGroup>0)
                    trackData[track].muteGroup--;
                }
              }
              break;
          }
        }
        counterA += counterA<0?1:-1;
      }
      if(sel){
        switch(xCursor){
          //if it's on track, select
          case 0:
            //toggles selection on all
            if(shift){
              trackData[activeTrack].isSelected = !trackData[activeTrack].isSelected;
              for(uint8_t track = 0; track<trackData.size(); track++){
                trackData[track].isSelected = trackData[activeTrack].isSelected;
              }
            }
            //normal selection toggle
            else
              trackData[activeTrack].isSelected = !trackData[activeTrack].isSelected;
            
            lastTime = millis();
            break;
          //if it's on primed
          case 4:
            //toggles prime on all
            if(shift){
              trackData[activeTrack].isPrimed = !trackData[activeTrack].isPrimed;
              for(uint8_t track = 0; track<trackData.size(); track++){
                trackData[track].isPrimed = trackData[activeTrack].isPrimed;
              }
            }
            //normal selection toggle
            else{
              trackData[activeTrack].isPrimed = !trackData[activeTrack].isPrimed;
              for(uint8_t track = 0; track<trackData.size(); track++){
                if(track != activeTrack && trackData[track].isSelected)
                  trackData[track].isPrimed = trackData[activeTrack].isPrimed;
              }
            }
            lastTime = millis();
            break;
          //if it's on latch
          case 5:
            //toggles prime on all
            if(shift){
              trackData[activeTrack].isLatched = !trackData[activeTrack].isLatched;
              for(uint8_t track = 0; track<trackData.size(); track++){
                trackData[track].isLatched = trackData[activeTrack].isLatched;
              }
            }
            //normal selection toggle
            else{
              trackData[activeTrack].isLatched = !trackData[activeTrack].isLatched;
              for(uint8_t track = 0; track<trackData.size(); track++){
                if(track != activeTrack && trackData[track].isSelected)
                  trackData[track].isLatched = trackData[activeTrack].isLatched;
              }
            }
            lastTime = millis();
            break;
        }
      }
      if(menu_Press || controls.A()){
        // menuIsActive = false;
        // constructMenu("MENU");
        for(uint8_t track = 0; track<trackData.size(); track++){
          trackData[track].isSelected = false;
        }
        lastTime = millis();
        menu_Press = false;
        controls.setA(false);;
        return;
      }
    }
    display.clearDisplay();
    drawTrackInfo(xCursor);
    activeMenu.displayTrackMenu_trackEdit(xCursor);
    display.display();
  }
}
