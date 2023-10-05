bool trackMenuControls(){
  //moving menu cursor
  if(!shift){
    if(itsbeen(100)){
      if(y != 0){
        if(y == 1){
          if(activeMenu.highlight<9 && activeMenu.highlight != 6){
            activeMenu.highlight++;
            lastTime = millis();
          }
        }
        if(y == -1){
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
  if(itsbeen(100)){
    if(shift && y != 0){
      if(y == -1){
        setActiveTrack(activeTrack-1,true);
        lastTime = millis();
      }
      if(y == 1){
        setActiveTrack(activeTrack+1,true);
        lastTime = millis();
      }
    }
  }
  if(itsbeen(200)){
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
          trackEditMenu();
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
  if(itsbeen(200)){
    if(track_Press || menu_Press){
      slideMenuOut(1,7);
      menuIsActive = false;
      constructMenu("MENU");
      lastTime = millis();
      return false;
    }
    if(note_Press){
      slideMenuOut(1,7);
      lastTime = millis();
      constructMenu("EDIT");
      return false;
    }
  }
  //menu navigation/incrementing values
  if(itsbeen(100)){
    if(x == -1){
      if(activeMenu.highlight >= 4 && activeMenu.highlight <=6){
        activeMenu.highlight+=3;
        lastTime = millis();
      }
    }
    else if(x == 1){
      if(activeMenu.highlight >= 7 && activeMenu.highlight <= 9){
        activeMenu.highlight-=3;
        lastTime = millis();
      }
    }
    if(activeMenu.highlight<3){
      while(counterA != 0){
        if(counterA >= 1 || x == -1){
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
        if(counterA <= -1 || x == 1){
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
    readJoystick();
    readButtons();
    if(!trackMenuControls()){
      break;
    }
  }
}

void drawTrackUtils(){
  
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
  display.setCursor((screenWidth-coords.x1)-34,7);
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.print("Trk");
  display.print(stringify(activeTrack+1));
  display.setFont();

  //drawing menu box
  display.fillRoundRect(coords.x1,coords.y1+12,coords.x2-coords.x1+1,coords.y2-coords.y1-11,3,SSD1306_BLACK);
  display.drawRoundRect(coords.x1,coords.y1+12,coords.x2-coords.x1+1,coords.y2-coords.y1-11,3,SSD1306_WHITE);
  //top labels 
  const vector<String> texts = {"track","pitch","octave","channel","prime","latch","mute group"};
  if(xCursor != 6)
    printChunky(coords.x2-texts[xCursor].length()*6,coords.y1+5,texts[xCursor],1);
  else{
    printChunky(coords.x2-30,coords.y1,"mute",1);
    printChunky(coords.x2-30,coords.y1+7,"group",1);
  }
  drawTrackMenuTopInfo(coords.x1);

  //drawing menu options, and the highlight
  const uint8_t textWidth = 20;
  const uint8_t textHeight = 8;
  const uint8_t yPos = 12;
  //printing out the menu
  //pitch
  String p = pitchToString(trackData[activeTrack].pitch,false,true);
  if(highlight == 0){
    display.fillRoundRect(coords.x1+4,yPos+coords.y1+2,8+p.length()*4+12,9,4,SSD1306_WHITE);
    display.drawChar(coords.x1+6,yPos+coords.y1+3,0x0E,SSD1306_BLACK,SSD1306_WHITE,1);
  }
  else
    display.drawChar(coords.x1+6,yPos+coords.y1+3,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
  printSmall(coords.x1+12,yPos+coords.y1+3,"$",2);//printing all the text inverse, so it turns black when highlighted
  printSmall(coords.x1+14,yPos+coords.y1+4," = "+p,2);
  //oct
  p = stringify(getOctave(trackData[activeTrack].pitch));
  if(highlight == 1)
    display.fillRoundRect(coords.x1+4,yPos+coords.y1+3+textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(coords.x1+6,yPos+coords.y1+4+textHeight,"/8 = "+p,2);
  //channel
  p = stringify(trackData[activeTrack].channel);
  if(highlight == 2)
    display.fillRoundRect(coords.x1+4,yPos+coords.y1+2+2*textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(coords.x1+6,yPos+coords.y1+3+2*textHeight,"ch = "+p,2);
  
  const uint8_t col1X = 5;
  const uint8_t col2X = 19;

  //prime icon
  // printSmall(coords.x1+col1X,37,"rec",1);
  display.drawBitmap(coords.x1+col1X,38,rec_tiny,11,4,1,0);
  if(trackData[activeTrack].isPrimed){
    // printSmall(coords.x1+2,37,"prime",1);
    if(millis()%1000<500)
      display.fillCircle(coords.x1+col2X+4,39,2,SSD1306_WHITE);
    else
      display.drawCircle(coords.x1+col2X+4,39,2,SSD1306_WHITE);
  }
  else{
    display.drawCircle(coords.x1+col2X+4,39,2,SSD1306_WHITE);
  }
  
  //edit
  display.drawBitmap(coords.x1+col1X,43,track_pencil,11,5,SSD1306_WHITE);
  //erase
  display.drawBitmap(coords.x1+col2X,43,track_eraser,11,5,SSD1306_WHITE);
  //mute
  if(trackData[activeTrack].isMuted)
    display.drawBitmap(coords.x1+col1X,50,track_mute,11,5,SSD1306_WHITE);
  else
    display.drawBitmap(coords.x1+col1X,50,track_unmute,11,5,SSD1306_WHITE);
  //solo
  display.drawBitmap(coords.x1+col2X,50,track_solo,11,5,SSD1306_WHITE);
  //move
  display.drawBitmap(coords.x1+col1X,57,track_arrows,11,5,SSD1306_WHITE);
  //latch
  if(trackData[activeTrack].isLatched)
    display.drawBitmap(coords.x1+col2X+3,57,track_latch,5,5,SSD1306_WHITE);
  else
    display.drawBitmap(coords.x1+col2X+3,57,track_unlatch,5,5,SSD1306_WHITE);
  
  //menu highlight
  if(highlight == 3)
    drawArrow(coords.x1+25+sin(millis()/100),39,2,1,true);
  //first column
  else if(highlight>=4 && highlight<=6)
    display.drawRoundRect(coords.x1+3,34+7*(highlight-3),15,9,2,SSD1306_WHITE);
  else if(highlight>=7)
    display.drawRoundRect(coords.x1+17,34+7*(highlight-6),15,9,2,SSD1306_WHITE);
}

void Menu::displayTrackMenu(){
  //corner logo
  display.setCursor((screenWidth-coords.x1)-34,7);
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.print("Trk");
  display.print(stringify(activeTrack+1));
  display.setFont();
  // display.drawFastHLine(0,16,32,SSD1306_WHITE);

  //track info
  drawTrackMenuTopInfo(coords.x1);

  //drawing menu box
  display.fillRect(coords.x1,coords.y1, coords.x2-coords.x1, coords.y2-coords.y1, SSD1306_BLACK);
  display.drawRoundRect(coords.x1,coords.y1+12, coords.x2-coords.x1+1, coords.y2-coords.y1-11, 3,SSD1306_WHITE);
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
      s = "edit";
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
  printChunky(coords.x2-s.length()*6,coords.y1+5,s,SSD1306_WHITE);

  //drawing menu options, and the highlight
  const uint8_t textWidth = 20;
  const uint8_t textHeight = 8;
  const uint8_t yPos = 12;
  //printing out the menu
  //pitch
  String p = pitchToString(trackData[activeTrack].pitch,false,true);
  if(highlight == 0){
    display.fillRoundRect(coords.x1+4,yPos+coords.y1+2,8+p.length()*4+12,9,4,SSD1306_WHITE);
    display.drawChar(coords.x1+6,yPos+coords.y1+3,0x0E,SSD1306_BLACK,SSD1306_WHITE,1);
  }
  else
    display.drawChar(coords.x1+6,yPos+coords.y1+3,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
  printSmall(coords.x1+12,yPos+coords.y1+3,"$",2);//printing all the text inverse, so it turns black when highlighted
  printSmall(coords.x1+14,yPos+coords.y1+4," = "+p,2);
  //oct
  p = stringify(getOctave(trackData[activeTrack].pitch));
  if(highlight == 1)
    display.fillRoundRect(coords.x1+4,yPos+coords.y1+3+textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(coords.x1+6,yPos+coords.y1+4+textHeight,"/8 = "+p,2);
  //channel
  p = stringify(trackData[activeTrack].channel);
  if(highlight == 2)
    display.fillRoundRect(coords.x1+4,yPos+coords.y1+2+2*textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(coords.x1+6,yPos+coords.y1+3+2*textHeight,"ch = "+p,2);
  
  const uint8_t col1X = 5;
  const uint8_t col2X = 19;

  //prime icon
  // printSmall(coords.x1+col1X,37,"rec",1);
  display.drawBitmap(coords.x1+col1X,38,rec_tiny,11,4,1,0);
  if(trackData[activeTrack].isPrimed){
    // printSmall(coords.x1+2,37,"prime",1);
    if(millis()%1000<500)
      display.fillCircle(coords.x1+col2X+4,39,2,SSD1306_WHITE);
    else
      display.drawCircle(coords.x1+col2X+4,39,2,SSD1306_WHITE);
  }
  else{
    display.drawCircle(coords.x1+col2X+4,39,2,SSD1306_WHITE);
  }

  // display.drawRoundRect(coords.x1+2,42,screenWidth-coords.x1-2,27,3,SSD1306_WHITE);
  //edit
  display.drawBitmap(coords.x1+col1X,43,track_pencil,11,5,SSD1306_WHITE);
  //erase
  display.drawBitmap(coords.x1+col2X,43,track_eraser,11,5,SSD1306_WHITE);
  //mute
  if(trackData[activeTrack].isMuted)
    display.drawBitmap(coords.x1+col1X,50,track_mute,11,5,SSD1306_WHITE);
  else
    display.drawBitmap(coords.x1+col1X,50,track_unmute,11,5,SSD1306_WHITE);
  //solo
  display.drawBitmap(coords.x1+col2X,50,track_solo,11,5,SSD1306_WHITE);
  //move
  display.drawBitmap(coords.x1+col1X,57,track_arrows,11,5,SSD1306_WHITE);
  //latch
  if(trackData[activeTrack].isLatched)
    display.drawBitmap(coords.x1+col2X+3,57,track_latch,5,5,SSD1306_WHITE);
  else
    display.drawBitmap(coords.x1+col2X+3,57,track_unlatch,5,5,SSD1306_WHITE);
  
  //menu highlight
  if(highlight == 3)
    drawArrow(coords.x1+28+sin(millis()/100),39,2,1,true);
  //first column
  else if(highlight>=4 && highlight<=6){
    display.drawRoundRect(coords.x1+3,34+7*(highlight-3),15,9,2,SSD1306_WHITE);
  }
  else if(highlight>=7){
    display.drawRoundRect(coords.x1+17,34+7*(highlight-6),15,9,2,SSD1306_WHITE);
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
