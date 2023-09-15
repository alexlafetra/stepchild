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
    joyRead();
    readButtons();
    if(!trackMenuControls()){
      break;
    }
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
  display.setCursor((screenWidth-topL[0])-34,7);
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.print("Trk");
  display.print(stringify(activeTrack+1));
  display.setFont();

  //drawing menu box
  display.fillRoundRect(topL[0],topL[1]+12,bottomR[0]-topL[0]+1,bottomR[1]-topL[1]-11,3,SSD1306_BLACK);
  display.drawRoundRect(topL[0],topL[1]+12,bottomR[0]-topL[0]+1,bottomR[1]-topL[1]-11,3,SSD1306_WHITE);
  //top labels 
  const vector<String> texts = {"track","pitch","octave","channel","prime","latch","mute group"};
  if(xCursor != 6)
    printChunky(bottomR[0]-texts[xCursor].length()*6,topL[1]+5,texts[xCursor],1);
  else{
    printChunky(bottomR[0]-30,topL[1],"mute",1);
    printChunky(bottomR[0]-30,topL[1]+7,"group",1);
  }
  drawTrackMenuTopInfo(topL[0]);

  //drawing menu options, and the highlight
  const uint8_t textWidth = 20;
  const uint8_t textHeight = 8;
  const uint8_t yPos = 12;
  //printing out the menu
  //pitch
  String p = pitchToString(trackData[activeTrack].pitch,false,true);
  if(highlight == 0){
    display.fillRoundRect(topL[0]+4,yPos+topL[1]+2,8+p.length()*4+12,9,4,SSD1306_WHITE);
    display.drawChar(topL[0]+6,yPos+topL[1]+3,0x0E,SSD1306_BLACK,SSD1306_WHITE,1);
  }
  else
    display.drawChar(topL[0]+6,yPos+topL[1]+3,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
  printSmall(topL[0]+12,yPos+topL[1]+3,"$",2);//printing all the text inverse, so it turns black when highlighted
  printSmall(topL[0]+14,yPos+topL[1]+4," = "+p,2);
  //oct
  p = stringify(getOctave(trackData[activeTrack].pitch));
  if(highlight == 1)
    display.fillRoundRect(topL[0]+4,yPos+topL[1]+3+textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(topL[0]+6,yPos+topL[1]+4+textHeight,"/8 = "+p,2);
  //channel
  p = stringify(trackData[activeTrack].channel);
  if(highlight == 2)
    display.fillRoundRect(topL[0]+4,yPos+topL[1]+2+2*textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(topL[0]+6,yPos+topL[1]+3+2*textHeight,"ch = "+p,2);
  
  const uint8_t col1X = 5;
  const uint8_t col2X = 19;

  //prime icon
  // printSmall(topL[0]+col1X,37,"rec",1);
  display.drawBitmap(topL[0]+col1X,38,rec_tiny,11,4,1,0);
  if(trackData[activeTrack].isPrimed){
    // printSmall(topL[0]+2,37,"prime",1);
    if(millis()%1000<500)
      display.fillCircle(topL[0]+col2X+4,39,2,SSD1306_WHITE);
    else
      display.drawCircle(topL[0]+col2X+4,39,2,SSD1306_WHITE);
  }
  else{
    display.drawCircle(topL[0]+col2X+4,39,2,SSD1306_WHITE);
  }
  
  //edit
  display.drawBitmap(topL[0]+col1X,43,track_pencil,11,5,SSD1306_WHITE);
  //erase
  display.drawBitmap(topL[0]+col2X,43,track_eraser,11,5,SSD1306_WHITE);
  //mute
  if(trackData[activeTrack].isMuted)
    display.drawBitmap(topL[0]+col1X,50,track_mute,11,5,SSD1306_WHITE);
  else
    display.drawBitmap(topL[0]+col1X,50,track_unmute,11,5,SSD1306_WHITE);
  //solo
  display.drawBitmap(topL[0]+col2X,50,track_solo,11,5,SSD1306_WHITE);
  //move
  display.drawBitmap(topL[0]+col1X,57,track_arrows,11,5,SSD1306_WHITE);
  //latch
  if(trackData[activeTrack].isLatched)
    display.drawBitmap(topL[0]+col2X+3,57,track_latch,5,5,SSD1306_WHITE);
  else
    display.drawBitmap(topL[0]+col2X+3,57,track_unlatch,5,5,SSD1306_WHITE);
  
  //menu highlight
  if(highlight == 3)
    drawArrow(topL[0]+25+sin(millis()/100),39,2,1,true);
  //first column
  else if(highlight>=4 && highlight<=6)
    display.drawRoundRect(topL[0]+3,34+7*(highlight-3),15,9,2,SSD1306_WHITE);
  else if(highlight>=7)
    display.drawRoundRect(topL[0]+17,34+7*(highlight-6),15,9,2,SSD1306_WHITE);
}

void Menu::displayTrackMenu(){
  //corner logo
  display.setCursor((screenWidth-topL[0])-34,7);
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.print("Trk");
  display.print(stringify(activeTrack+1));
  display.setFont();
  // display.drawFastHLine(0,16,32,SSD1306_WHITE);

  //track info
  drawTrackMenuTopInfo(topL[0]);

  //drawing menu box
  display.fillRect(topL[0],topL[1], bottomR[0]-topL[0], bottomR[1]-topL[1], SSD1306_BLACK);
  display.drawRoundRect(topL[0],topL[1]+12, bottomR[0]-topL[0]+1, bottomR[1]-topL[1]-11, 3,SSD1306_WHITE);
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
  printChunky(bottomR[0]-s.length()*6,topL[1]+5,s,SSD1306_WHITE);

  //drawing menu options, and the highlight
  const uint8_t textWidth = 20;
  const uint8_t textHeight = 8;
  const uint8_t yPos = 12;
  //printing out the menu
  //pitch
  String p = pitchToString(trackData[activeTrack].pitch,false,true);
  if(highlight == 0){
    display.fillRoundRect(topL[0]+4,yPos+topL[1]+2,8+p.length()*4+12,9,4,SSD1306_WHITE);
    display.drawChar(topL[0]+6,yPos+topL[1]+3,0x0E,SSD1306_BLACK,SSD1306_WHITE,1);
  }
  else
    display.drawChar(topL[0]+6,yPos+topL[1]+3,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
  printSmall(topL[0]+12,yPos+topL[1]+3,"$",2);//printing all the text inverse, so it turns black when highlighted
  printSmall(topL[0]+14,yPos+topL[1]+4," = "+p,2);
  //oct
  p = stringify(getOctave(trackData[activeTrack].pitch));
  if(highlight == 1)
    display.fillRoundRect(topL[0]+4,yPos+topL[1]+3+textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(topL[0]+6,yPos+topL[1]+4+textHeight,"/8 = "+p,2);
  //channel
  p = stringify(trackData[activeTrack].channel);
  if(highlight == 2)
    display.fillRoundRect(topL[0]+4,yPos+topL[1]+2+2*textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  printSmall(topL[0]+6,yPos+topL[1]+3+2*textHeight,"ch = "+p,2);
  
  const uint8_t col1X = 5;
  const uint8_t col2X = 19;

  //prime icon
  // printSmall(topL[0]+col1X,37,"rec",1);
  display.drawBitmap(topL[0]+col1X,38,rec_tiny,11,4,1,0);
  if(trackData[activeTrack].isPrimed){
    // printSmall(topL[0]+2,37,"prime",1);
    if(millis()%1000<500)
      display.fillCircle(topL[0]+col2X+4,39,2,SSD1306_WHITE);
    else
      display.drawCircle(topL[0]+col2X+4,39,2,SSD1306_WHITE);
  }
  else{
    display.drawCircle(topL[0]+col2X+4,39,2,SSD1306_WHITE);
  }

  // display.drawRoundRect(topL[0]+2,42,screenWidth-topL[0]-2,27,3,SSD1306_WHITE);
  //edit
  display.drawBitmap(topL[0]+col1X,43,track_pencil,11,5,SSD1306_WHITE);
  //erase
  display.drawBitmap(topL[0]+col2X,43,track_eraser,11,5,SSD1306_WHITE);
  //mute
  if(trackData[activeTrack].isMuted)
    display.drawBitmap(topL[0]+col1X,50,track_mute,11,5,SSD1306_WHITE);
  else
    display.drawBitmap(topL[0]+col1X,50,track_unmute,11,5,SSD1306_WHITE);
  //solo
  display.drawBitmap(topL[0]+col2X,50,track_solo,11,5,SSD1306_WHITE);
  //move
  display.drawBitmap(topL[0]+col1X,57,track_arrows,11,5,SSD1306_WHITE);
  //latch
  if(trackData[activeTrack].isLatched)
    display.drawBitmap(topL[0]+col2X+3,57,track_latch,5,5,SSD1306_WHITE);
  else
    display.drawBitmap(topL[0]+col2X+3,57,track_unlatch,5,5,SSD1306_WHITE);
  
  //menu highlight
  if(highlight == 3)
    drawArrow(topL[0]+28+sin(millis()/100),39,2,1,true);
  //first column
  else if(highlight>=4 && highlight<=6){
    display.drawRoundRect(topL[0]+3,34+7*(highlight-3),15,9,2,SSD1306_WHITE);
  }
  else if(highlight>=7){
    display.drawRoundRect(topL[0]+17,34+7*(highlight-6),15,9,2,SSD1306_WHITE);
  }
}
