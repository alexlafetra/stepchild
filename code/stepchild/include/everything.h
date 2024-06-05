
//slides a menu in from the top,right,bottom, or left
void slideMenuIn(int fromWhere, int8_t speed){
  //sliding in from the right
  if(fromWhere == 1){
    //store original coords
    CoordinatePair targetCoords = activeMenu.coords;
    //then, offset the menu coordinates
    int16_t offset = screenWidth-activeMenu.coords.start.x;
    activeMenu.coords.start.x = screenWidth;
    activeMenu.coords.end.x += offset;
    //continuously move the menu coords and display it, until it reaches original position
    while(activeMenu.coords.start.x>targetCoords.start.x){
      activeMenu.coords.end.x -= speed;
      activeMenu.coords.start.x -= speed;
      if(activeMenu.coords.start.x<targetCoords.start.x){
        activeMenu.coords = targetCoords;
        break;
      }
      displaySeq();
      // delay(100);
    }
    activeMenu.coords = targetCoords;
  }
  //from the bottom
  else if(fromWhere == 0){
    //store original coords
    CoordinatePair targetCoords = activeMenu.coords;
    //then, offset the menu coordinates
    int16_t offset = screenHeight-activeMenu.coords.start.y;
    activeMenu.coords.start.y += offset;
    activeMenu.coords.end.y += offset;
    //continuously move the menu coords and display it, until it reaches original position
    while(activeMenu.coords.start.y>targetCoords.start.y){
      activeMenu.coords.start.y-= speed;
      activeMenu.coords.end.y-= speed;
      if(activeMenu.coords.start.y<targetCoords.start.y){
        activeMenu.coords = targetCoords;
      }
      displaySeq();
      // delay(20);
    }
    activeMenu.coords = targetCoords;
  }
}

//same thang, but in reverse
void slideMenuOut(int toWhere, int8_t speed){
  if(toWhere == 1){//sliding out to the left side
    while(activeMenu.coords.start.x<screenWidth){
      activeMenu.coords.start.x+=speed;
      activeMenu.coords.end.x+=speed;
      //make sure x bounds don't glitch out/overflow  (don't think this is necessary, leaving it for legacy/in case you find a menu that bugs)
      //Using this makes some menus slide a lil' ugly
      // if(activeMenu.coords.end.x>screenWidth){
      //   activeMenu.coords.end.x = screenWidth;
      // }
      displaySeq();
      graphics.drawPram(5,0);
    }
  }
  //to the bottom
  else if(toWhere == 0){
    while(activeMenu.coords.start.y<screenHeight){
      activeMenu.coords.start.y+=speed;
      activeMenu.coords.end.y+=speed;
      //make sure y bounds don't glitch out
      if(activeMenu.coords.end.y>screenHeight){
        activeMenu.coords.end.y = screenHeight;
      }
      displaySeq();
      graphics.drawPram(5,0);
    }
  }
}

//draws pram, other icons (not loop points tho)
void drawTopIcons(){
  // if it's recording and waiting for a note
  if(recording && waitingToReceiveANote){
    if(millis()%1000>500){
    }
    else{
    }
  }

  //music symbol while receiving notes
  if(isReceivingOrSending()){
    if(!((animOffset/10)%2)){
      display.drawChar(21,0,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
    }
    else{
      display.drawChar(21,1,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
    }
  }

  //note presence indicator(if notes are offscreen)
  if(areThereMoreNotes(true)){
    uint8_t y1 = (maxTracksShown>5&&!menuIsActive)?8:headerHeight;
    if(!((animOffset/10)%2)){
      display.fillTriangle(trackDisplay-7,y1+3,trackDisplay-3,y1+3,trackDisplay-5,y1+1, SSD1306_WHITE);
    }
    else{
      display.fillTriangle(trackDisplay-7,y1+2,trackDisplay-3,y1+2,trackDisplay-5,y1, SSD1306_WHITE);
    }
  }
  if(areThereMoreNotes(false)){
    if(!((animOffset/10)%2)){
      display.fillTriangle(trackDisplay-7,screenHeight-5,trackDisplay-3,screenHeight-5,trackDisplay-5,screenHeight-3, SSD1306_WHITE);
    }
    else{
      display.fillTriangle(trackDisplay-7,screenHeight-4,trackDisplay-3,screenHeight-4,trackDisplay-5,screenHeight-2, SSD1306_WHITE);
    }
  }

  uint8_t x1 = 32;
  //rec/play icon
  if(recording){
    if(clockSource == EXTERNAL_CLOCK && !gotClock){
      if(waitingToReceiveANote){
        if(millis()%1000>500){
          display.drawCircle(trackDisplay+3,3,3,SSD1306_WHITE);
        }
      }
      else
        display.drawCircle(trackDisplay+3,3,3,SSD1306_WHITE);
    }
    else if((clockSource == EXTERNAL_CLOCK && gotClock) || clockSource == INTERNAL_CLOCK){
      if(waitingToReceiveANote){
        if(millis()%1000>500){
          display.fillCircle(trackDisplay+3,3,3,SSD1306_WHITE);
        }
      }
      else
        display.fillCircle(trackDisplay+3,3,3,SSD1306_WHITE);
    }
    x1+=9;
    switch(recMode){
      //if one-shot rec
      case 0:
        // display.drawBitmap(x1+((millis()/200)%2),0,oneShot_rec,7,7,SSD1306_WHITE);
        printSmall(x1,1,"1",1);
        x1+=4;
        if((millis()/10)%100>50)
          display.drawBitmap(x1,1,oneshot_bmp,3,5,SSD1306_WHITE);
        x1+=4;
        break;
      //if continuous recording
      case 1:
        display.drawBitmap(x1,1,continuous_bmp,9,5,SSD1306_WHITE);
        x1+=10;
        break;
    }
    if(overwriteRecording){
      display.drawBitmap(x1,0,((millis()/10)%100>50)?overwrite_1:overwrite_2,7,7,SSD1306_WHITE);
      x1+=8;
    }
  }
  else if(playing){
    graphics.drawPlayIcon(trackDisplay+((millis()/200)%2)+1,0);
    x1 += 10;
    switch(sequence.isLooping){
      //if not looping
      case 0:
        if((millis()/10)%100>50)
          display.drawFastVLine(x1,0,7,1);
        printSmall(x1+3,1,"1",1);
        x1+=10;
        break;
      //if looping
      case 1:
        if(millis()%1000>500){
          display.drawBitmap(x1,0,toploop_arrow1,7,7,SSD1306_WHITE);
        }
        else{
          display.drawBitmap(x1,0,toploop_arrow2,7,7,SSD1306_WHITE);
        }
        x1+=10;
        break;
    }

  }
  //Data track icon
  if(sequence.autotrackData.size()>0){
    if(millis()%1600>800)
      // display.drawBitmap(x1,1,sine_small_bmp,6,4,SSD1306_WHITE);
      display.drawBitmap(x1,0,autotrack1,10,7,SSD1306_WHITE);
    else{
      // display.drawBitmap(x1,1,sine_small_reverse_bmp,6,4,SSD1306_WHITE);
      display.drawBitmap(x1,0,autotrack2,10,7,SSD1306_WHITE);
    }
    x1+=12;
  }

  //swing icon
  if(sequenceClock.isSwinging){
    graphics.drawPendulum(x1+2,0,7,millis()/2,1);
    x1+=10;
  }
  //fragment gem
  if(isFragmenting){
    drawTetra(x1,5,10+sin(float(millis())/float(500)),10+sin(float(millis())/float(500)),6+sin(float(millis())/float(500)),1+sin(float(millis())/float(500)),0,SSD1306_WHITE);
    x1+=8;
  }

  //velocity/chance indicator while shifting
  if(controls.SHIFT()){
    if(displayingVel){
      if(sequence.IDAtCursor() == 0){
        String vel = stringify(sequence.defaultVel);
        printSmall(x1,1,"v:"+vel,SSD1306_WHITE);
        x1+=vel.length()*4+8;;
      }
      else{
        String vel = stringify(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].velocity);
        printSmall(x1,1,"v:"+vel,SSD1306_WHITE);
        x1+=vel.length()*4+8;
      }
    }
    else{
      if(sequence.IDAtCursor() == 0){
        printSmall(x1,1,"c:"+stringify(100),SSD1306_WHITE);
        x1+=20;
      }
      else{
        printSmall(x1,1,"c:"+stringify(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].chance),SSD1306_WHITE);
        x1+=20;
      }
    }
  }
  else{
    //power/battery indicator
    graphics.drawPower(screenWidth-10,0);
  }
  if(arp.isActive){
    display.drawPixel(x1,3+2*sin(float(millis())/float(200)),1);
    display.drawPixel(x1+2,3+2*sin(float(millis())/float(200)+100),1);
    display.drawPixel(x1+4,3+2*sin(float(millis())/float(200)+200),1);
    // printSmall(x1+6,1,"a",1);
    display.drawBitmap(x1+6,2,tiny_arp_bmp,9,3,1);
    // x1+=8;
    x1+=17;
  }
  if(controls.SHIFT()){
    graphics.drawSequenceMemoryBar(screenWidth-30,0,30);
  }
  else{
    //draw menu text
    printSmall(x1,1,menuText,SSD1306_WHITE);
    x1+=menuText.length()*4+2;
  }
}

//same function, but doesn't clear or display the screen
void drawSeq(){
  drawSeq(true,true,true,true,false);
}
//sends data to screen
//move through rows/columns, printing out data
void displaySeq(){
  display.clearDisplay();
  drawSeq(true,true,true,true,false,false,sequence.viewStart,sequence.viewEnd);
  display.display();
}

void drawSeq(bool trackLabels, bool topLabels, bool loopPoints, bool menus, bool trackSelection){
  drawSeq(trackLabels,topLabels,loopPoints,menus,trackSelection,false,sequence.viewStart,sequence.viewEnd);
}

//Start = step you're starting on, startheight is the y coord the sequence grid begins at
void drawSeqBackground(uint16_t start, uint16_t end, uint8_t startHeight, uint8_t height, bool onlyWithinLoop, bool loopFlags, bool loopPoints){
  //drawing the measure bars
  for (uint16_t step = start; step < end; step++) {
    unsigned short int x1 = trackDisplay+int((step-start)*sequence.viewScale);
    unsigned short int x2 = x1 + (step-start)*sequence.viewScale;

    //shade everything outside the loop
    if(onlyWithinLoop){
      if(step<sequence.loopData[sequence.activeLoop].start){
        graphics.shadeArea(x1,startHeight,(sequence.loopData[sequence.activeLoop].start-step)*sequence.viewScale,screenHeight-startHeight,3);
        step = sequence.loopData[sequence.activeLoop].start;
        //ok, step shouldn't ever be zero in this case, since that would mean it was LESS than zero to begin
        //with. But, just for thoroughnesses sake, make sure step doesn't overflow when you subtract from it
        if(step != 0){
          step--;
        }
        continue;
      }
      else if(step>sequence.loopData[sequence.activeLoop].end){
        graphics.shadeArea(x1,startHeight,(sequence.viewEnd-sequence.loopData[sequence.activeLoop].end)*sequence.viewScale,screenHeight-startHeight,3);
        break;
      }
    }

    //if the last track is showing
    if(endTrack == sequence.trackData.size()){
      //measure bars
      if (!(step % sequence.subDivision) && (step%96) && (sequence.subDivision*sequence.viewScale)>1) {
        graphics.drawDottedLineV(x1,startHeight,height,2);
      }
      if(!(step%96)){
        graphics.drawDottedLineV2(x1,startHeight,height,6);
      }
    }
    else{
      //measure bars
      if (!(step % sequence.subDivision) && (step%96) && (sequence.subDivision*sequence.viewScale)>1) {
        graphics.drawDottedLineV(x1,startHeight,height,2);
      }
      if(!(step%96)){
        graphics.drawDottedLineV2(x1,startHeight,height,6);
      }
    }

    //drawing loop points/flags
    if(loopPoints){//check
      if(step == sequence.loopData[sequence.activeLoop].start){
        if(loopFlags){
          if(movingLoop == -1 || movingLoop == 2){
            display.fillTriangle(trackDisplay+(step-start)*sequence.viewScale, startHeight-3-sin(millis()/50), trackDisplay+(step-start)*sequence.viewScale, startHeight-7-sin(millis()/50), trackDisplay+(step-start)*sequence.viewScale+4, startHeight-7-sin(millis()/50),SSD1306_WHITE);
            display.drawFastVLine(trackDisplay+(step-start)*sequence.viewScale,startHeight-3,3,SSD1306_WHITE);
          }
          else{
            if(sequence.cursorPos == step){
              display.fillTriangle(trackDisplay+(step-start)*sequence.viewScale, startHeight-3, trackDisplay+(step-start)*sequence.viewScale, startHeight-7, trackDisplay+(step-start)*sequence.viewScale+4, startHeight-7,SSD1306_WHITE);
              display.drawFastVLine(trackDisplay+(step-start)*sequence.viewScale,startHeight-3,3,SSD1306_WHITE);
            }
            else{
              display.fillTriangle(trackDisplay+(step-start)*sequence.viewScale, startHeight-1, trackDisplay+(step-start)*sequence.viewScale, startHeight-5, trackDisplay+(step-start)*sequence.viewScale+4, startHeight-5,SSD1306_WHITE);
            }
          }
        }
        else{
          display.drawPixel(trackDisplay+(sequence.loopData[sequence.activeLoop].start-start)*sequence.viewScale, startHeight-1,1);
        }
        if(!movingLoop || (movingLoop != 1 && (millis()/400)%2)){
          display.drawFastVLine(trackDisplay+(step-start)*sequence.viewScale,startHeight,screenHeight-startHeight-(endTrack == sequence.trackData.size()),SSD1306_WHITE);
          display.drawFastVLine(trackDisplay+(step-start)*sequence.viewScale-1,startHeight,screenHeight-startHeight-(endTrack == sequence.trackData.size()),SSD1306_WHITE);
        }
      }
      if(step == sequence.loopData[sequence.activeLoop].end-1){
        if(loopFlags){
          if(movingLoop == 1 || movingLoop == 2){
            display.drawTriangle(trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale, startHeight-3-sin(millis()/50), trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale-4, startHeight-7-sin(millis()/50), trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale, startHeight-7-sin(millis()/50),SSD1306_WHITE);
            display.drawFastVLine(trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale,startHeight-3,3,SSD1306_WHITE);
          }
          else{
            if(sequence.cursorPos == step+1){
              display.drawTriangle(trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale, startHeight-3, trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale-4, startHeight-7, trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale, startHeight-7,SSD1306_WHITE);
              display.drawFastVLine(trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale,startHeight-3,3,SSD1306_WHITE);
            }
            else{
              display.drawTriangle(trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale, startHeight-1, trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale-4, startHeight-5, trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale, startHeight-5,SSD1306_WHITE);
            }
          }
        }
        else{
          display.drawPixel(trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale, startHeight-1,1);
        }
        if(!movingLoop || (movingLoop != -1 && (millis()/400)%2)){
          display.drawFastVLine(trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale+1,startHeight,screenHeight-startHeight-(endTrack == sequence.trackData.size()),SSD1306_WHITE);
          display.drawFastVLine(trackDisplay+(sequence.loopData[sequence.activeLoop].end-start)*sequence.viewScale+2,startHeight,screenHeight-startHeight-(endTrack == sequence.trackData.size()),SSD1306_WHITE);
        }
      }
      if(movingLoop == 2){
        if(step>sequence.loopData[sequence.activeLoop].start && step<sequence.loopData[sequence.activeLoop].end && step%2){
          display.drawPixel(trackDisplay+(step-start)*sequence.viewScale, startHeight-7-sin(millis()/50),SSD1306_WHITE);
        }
      }
      if(loopFlags && (step == sequence.loopData[sequence.activeLoop].start+(sequence.loopData[sequence.activeLoop].end-sequence.loopData[sequence.activeLoop].start)/2))
        printSmall(trackDisplay+(step-start)*sequence.viewScale-1,startHeight-7,stringify(sequence.activeLoop),SSD1306_WHITE);
    }
  }
}

//this function is a mess!
void drawSeq(bool trackLabels, bool topLabels, bool loopPoints, bool menus, bool trackSelection, bool shadeOutsideLoop, uint16_t start, uint16_t end){
  if(!screenSaverActive){
    //handling the note view
    if(end>sequence.sequenceLength){
      end = sequence.sequenceLength;
    }
    if(start>end){
      start = 0;
    }
    uint16_t viewLength = end - start;

    uint8_t startHeight = 0;
    bool loopFlags = loopPoints;

    if(maxTracksShown == 6 && !menuIsActive){
      startHeight = 8;
      trackHeight = (screenHeight-startHeight)/maxTracksShown;
      loopFlags = false;
    }
    else{
      trackHeight = (screenHeight-headerHeight)/maxTracksShown;
      startHeight = headerHeight;
    }

    if(sequence.trackData.size()>maxTracksShown){
      endTrack = startTrack + maxTracksShown;
    }
    else{
      endTrack = startTrack + sequence.trackData.size();
    }
    while(sequence.activeTrack>=endTrack && sequence.trackData.size()>maxTracksShown){
      startTrack++;
      endTrack++;
    }
    while(sequence.activeTrack<startTrack && sequence.trackData.size()>maxTracksShown){
      startTrack--;
      endTrack--;
    }

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    // display.setTextWrap(false);

    //drawing selection box, since it needs to overlay stepSeq data
    if(selBox.begun){
      selBox.render();
    }
    uint8_t height;
    if(endTrack == sequence.trackData.size())
      height = startHeight+trackHeight*maxTracksShown;
    else if(sequence.trackData.size()>maxTracksShown)
      height = startHeight+trackHeight*(maxTracksShown+1);
    else
      height = startHeight+trackHeight*sequence.trackData.size();

    //drawing measure bars, loop points
    // drawSeqBackground(start, end, startHeight, height, shadeOutsideLoop, loopFlags, loopPoints);
    drawSeqBackground(start, end, startHeight, height, shadeOutsideLoop, sequence.isLooping, loopPoints);

    //top and bottom bounds
    if(startTrack == 0){
      display.drawFastHLine(trackDisplay,startHeight,screenWidth,SSD1306_WHITE);
    }
    //if the bottom is in view
    if(endTrack == sequence.trackData.size()){
      display.drawFastHLine(trackDisplay,startHeight+trackHeight*maxTracksShown,screenWidth,SSD1306_WHITE);
    }
   else if(endTrack< sequence.trackData.size())
        endTrack++;
    //drawin all da steps
    //---------------------------------------------------
    for (int track = startTrack; track < endTrack; track++) {
      unsigned short int y1 = (track-startTrack) * trackHeight + startHeight;
      unsigned short int y2 = y1 + trackHeight;
      uint8_t xCoord;
      //track info display
      if(sequence.activeTrack == track){
        xCoord = 9;
        // display.setCursor(9, y1+trackHeight/2-2);
        if(trackLabels)
          graphics.drawArrow(6+((millis()/400)%2),y1+trackHeight/2+1,2,0,true);
      }
      else{
        xCoord = 5;
        // display.setCursor(5, y1+trackHeight/2-2);
      }
      if(trackLabels){
        if(!isShrunk){
          //printing note names
          if(pitchesOrNumbers){
            printTrackPitch(xCoord, y1+trackHeight/2-2,track,false,true,SSD1306_WHITE);
          }
          //just printing pitch numbers
          else{
            display.setCursor(xCoord,y1+2);
            display.print(sequence.trackData[track].pitch);
          }
          // else{
          //   if(track == sequence.activeTrack && isModulated(sequence.trackData[track].channel)){
          //     if(playing)
          //       display.drawBitmap(trackDisplay-7,y1+3+((millis()/200)%2),sine_small_bmp,6,4,SSD1306_WHITE);
          //     else
          //       display.drawBitmap(trackDisplay-7,y1+2,sine_small_bmp,6,4,SSD1306_WHITE);
          //   }
          // }
        }
        //if it's shrunk, draw it small
        else{
          String pitch = sequence.trackData[track].getPitchAndOctave();
          if(track%2){
            printSmall(18, y1, pitchToString(sequence.trackData[track].pitch,true,true), SSD1306_WHITE);
          }
          else
            printSmall(2, y1, pitchToString(sequence.trackData[track].pitch,true,true), SSD1306_WHITE);
          if(sequence.trackData[track].noteLastSent != 255){
            display.drawRect(0,y1,trackDisplay,trackHeight+2,SSD1306_WHITE);
          }
        }
      }
      //if you're only drawing selected tracks, and this track ISN'T selected, shade it
      if(trackSelection && !sequence.trackData[track].isSelected){
        display.fillRect(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,0);
        graphics.shadeArea(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,3);
      }
      //if the track is muted, just hatch it out (don't draw any notes)
      //if it's solo'd and muted, draw it normal (solo overrules mute)
      else if(sequence.trackData[track].isMuted && !sequence.trackData[track].isSolo){
        // display.fillRect(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,0);
        graphics.shadeArea(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,9);
        continue;
      }
      else{
        //highlight for solo'd tracks
        if(sequence.trackData[track].isSolo)
          graphics.drawNoteBracket(trackDisplay+3,y1-1,screenWidth-trackDisplay-5,trackHeight+2,true);
        // display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
        for (int step = start; step < end; step++) {
          //if you only want to draw what's within the loop
          if(shadeOutsideLoop){
            //if you're less than the loop, jump to the start of the loop
            //(this could also be in the initial for loop condition!)
            if(step<sequence.loopData[sequence.activeLoop].start){
              step = sequence.loopData[sequence.activeLoop].start;
            }
            //if you're past the loop end, break out of the for loop
            else if(step>=sequence.loopData[sequence.activeLoop].end){
              break;
            }
          }
          int id = sequence.lookupTable[track][step];
          unsigned short int x1 = trackDisplay+int((step-start)*sequence.viewScale);
          unsigned short int x2 = x1 + (step-start)*sequence.viewScale;
          //drawing note
          if (id != 0){
            if(step == sequence.noteData[track][id].startPos){
              uint16_t length = (sequence.noteData[track][id].endPos - sequence.noteData[track][id].startPos)*sequence.viewScale;
              if(displayingVel)
                drawNote_vel(id, track, x1,y1,length,trackHeight,sequence.noteData[track][id].velocity,sequence.noteData[track][id].isSelected,sequence.noteData[track][id].muted);
              else
                drawNote_chance(id, track, x1,y1,length,trackHeight,sequence.noteData[track][id].chance,sequence.noteData[track][id].isSelected,sequence.noteData[track][id].muted);
              if(sequence.noteData[track][id].isSelected){
                display.drawRect(x1,y1,length+1,trackHeight,SSD1306_WHITE);
                display.drawRect(x1+1,y1+1,length-1,trackHeight-2,SSD1306_BLACK);
              }
            }
            else if(!isInView(sequence.noteData[track][id].startPos) && step == start){
              unsigned short int length = (sequence.noteData[track][id].endPos - start)*sequence.viewScale;
              if(displayingVel)
                drawNote_vel(id, track, x1,y1,length,trackHeight,sequence.noteData[track][id].velocity,sequence.noteData[track][id].isSelected,sequence.noteData[track][id].muted);
              else
                drawNote_chance(id, track, x1,y1,length,trackHeight,sequence.noteData[track][id].chance,sequence.noteData[track][id].isSelected,sequence.noteData[track][id].muted);
            }
          }
        }
      }
    }
    //---------------------------------------------------
    //drawing cursor
    if(!editingNote && !(menuIsActive && activeMenu.menuTitle == "EDIT" && activeMenu.page == 1)){
      uint8_t cPos = trackDisplay+int((sequence.cursorPos-start)*sequence.viewScale);
      if(cPos>127)
        cPos = 126;
      if(endTrack == sequence.trackData.size()){
        display.drawFastVLine(cPos, startHeight, (endTrack-startTrack)*trackHeight, SSD1306_WHITE);
        display.drawFastVLine(cPos+1, startHeight, (endTrack-startTrack)*trackHeight, SSD1306_WHITE);
        // display.drawRect(trackDisplay+int((sequence.cursorPos-start)*sequence.viewScale), startHeight, 2, (endTrack-startTrack)*trackHeight, SSD1306_WHITE);
      }
      else{
        display.drawFastVLine(cPos, startHeight, screenHeight-startHeight, SSD1306_WHITE);
        display.drawFastVLine(cPos+1, startHeight, screenHeight-startHeight, SSD1306_WHITE);
        // display.drawRect(trackDisplay+int((sequence.cursorPos-start)*sequence.viewScale), startHeight, 2, screenHeight-startHeight, SSD1306_WHITE);
      }
    }
    //carriage bitmap/title
    if(topLabels){
      graphics.drawPram();
      if(startHeight == 16){
        drawTopIcons();
      }
    }
    //playhead/rechead
    if(playing && isInView(playheadPos))
      display.drawRoundRect(trackDisplay+(playheadPos-start)*sequence.viewScale,startHeight,3, screenHeight-startHeight, 3, SSD1306_WHITE);
    if(recording && isInView(recheadPos))
      display.drawRoundRect(trackDisplay+(recheadPos-start)*sequence.viewScale,startHeight,3, screenHeight-startHeight, 3, SSD1306_WHITE);
    
    int cursorX = trackDisplay+int((sequence.cursorPos-start)*sequence.viewScale)-8;
    if(!playing && !recording){
      cursorX = 32;
    }
    else{
      //making sure it doesn't print over the subdiv info
      cursorX = 42;
    }
    //drawing active track highlight
    unsigned short int x1 = trackDisplay;
    unsigned short int y1 = (sequence.activeTrack-startTrack) * trackHeight + startHeight;
    unsigned short int x2 = x1+screenWidth-trackDisplay;
    unsigned short int y2 = y1 + trackHeight;
    // display.drawRect(x1, y1, screenWidth, trackHeight, SSD1306_WHITE);
    display.drawFastHLine(x1,y1,screenWidth-x1,1);
    display.drawFastHLine(x1,y1+trackHeight-1,screenWidth-x1,1);
  }
  if(menus && menuIsActive){
    activeMenu.displayMenu(true,true);
  }

  //anim offset (for the pram)
  if(!menuIsActive){
    animOffset++;
    if(animOffset>100)
      animOffset = 0;
  }

  if(playing || recording)
    sequence.updateLEDs();
}


//for no title
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, void (*drawingFunction)()){
  return binarySelectionBox(x1,y1,op1,op2,"",drawingFunction);
}
//centered on x1 and y1
//returns -1 (no answer/exit), 0 (no) or 1 (yes)
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title, void (*drawingFunction)()){
  //bool for breaking from the loop
  bool notChosenYet = true;
  //storing the state
  bool state = false;
  lastTime = millis();
  
  while(true){
    display.clearDisplay();
    drawingFunction();
    graphics.drawBinarySelectionBox(x1,y1,op1,op2,title,state);
    display.display();
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(200)){
      //x to select option
      if(controls.joystickX != 0){
        if(controls.joystickX == 1 && state){
          state = !state;
          lastTime = millis();
        }
        else if(controls.joystickX == -1 && !state){
          state = !state;
          lastTime = millis();
        }
      }
      //menu/del to cancel
      if(controls.MENU() || controls.DELETE()){
        lastTime = millis();
        return -1;
      }
      //choose option
      else if(controls.NEW() || controls.SELECT() ){
        lastTime = millis();
          controls.setNEW(false);
        controls.setSELECT(false);
        return state;
      }
    }
  }
  return false;
}
//Specialized functions for drawing geometries
unsigned short int horzSelectionBox(String caption, vector<String> options, unsigned short int x1, unsigned short int y1, unsigned short int width, unsigned short int height){
  long int time = millis();
  unsigned short int select = 0;
  bool selected = false;
  while(!selected){
    display.fillRect(x1,y1,width,height,SSD1306_BLACK);
    display.drawRect(x1,y1,width,height,SSD1306_WHITE);
    display.setCursor(x1+20, y1+10);
    printSmall(x1+2,y1+2,caption,SSD1306_WHITE);
    for(int i = 0; i<options.size(); i++){
      display.setCursor(x1+40+i*20,y1+30);
      if(i == select)
        display.drawRect(x1+i*20-2,y1+8,8,options[i].length()*4+2,SSD1306_WHITE);
      printSmall(x1+i*20,y1+10,options[i],SSD1306_WHITE);
    }
    display.display();
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(200)){
      if(controls.joystickX != 0 || controls.joystickY != 0){
        if(controls.joystickX == -1 && select<options.size()-1){
          select++;
          time = millis();
        }
        if(controls.joystickX == 1 && select>0){
          select--;
          time = millis();
        }
        if(select<0)
          select = 0;
        if(select>options.size()-1)
          select = options.size()-1;
      }
      if(controls.SELECT() ){
        selected = true;
        time = millis();
      }
    }
  }
  controls.setSELECT(false);
  lastTime = millis();
  display.invertDisplay(false);
  return select;
}
uint16_t getNoteCount(){
  uint16_t count = 0;
  for(uint8_t track = 0; track<sequence.noteData.size(); track++){
    count+=sequence.noteData[track].size()-1;
  }
  return count;
}

float getNoteDensity(int timestep){
  float density = 0;
  for(int track = 0; track<sequence.trackData.size(); track++){
    if(sequence.lookupTable[track][timestep] != 0){
      density++;
    }
  }
  return density/float(sequence.trackData.size());
}
float getNoteDensity(int start, int end){
  float density;
  for(int i = start; i<= end; i++){
    density+=getNoteDensity(i);
  }
  return density/float(end-start+1);
}

void handleInternalCC(uint8_t ccNumber, uint8_t val, uint8_t channel, uint8_t yPosition){
  switch(ccNumber){
    //velocity
    case 0:
      globalModifiers.velocity[0] = channel;
      globalModifiers.velocity[1] = val-63;
      break;
    //chance
    case 1:
      globalModifiers.chance[0] = channel;
      globalModifiers.chance[1] = float(val)/float(127) * 100 - 50;
      break;
    break;
    //track pitch
    case 2:
      globalModifiers.pitch[0] = channel;
      //this can at MOST change the pitch by 2 octaves up or down, so a span of 48 notes
      globalModifiers.pitch[1] = float(val)/float(127) * 48 - 24;
      break;
    //sequenceClock.BPM
    case 3:
      sequenceClock.setBPM(float(val)*2);
      break;
    //swing amount
    case 4:
      sequenceClock.swingAmplitude += val-63;
    //track channel
    case 5:
      break;
  }
}

// //gets a note from data, returns a note object
// Note getNote(int track, int timestep) {
//   unsigned short int noteID;
//   if (sequence.lookupTable[track][timestep] != 0) {
//     noteID = sequence.lookupTable[track][timestep];
//     Note targetNote = sequence.noteData[track][noteID];
//     return targetNote;
//   }
//   else {
//     return Note();
//   }
// }

//counts notes
uint16_t countNotesInRange(uint16_t start, uint16_t end){
  uint16_t count = 0;
  for(uint8_t t = 0; t<sequence.trackData.size(); t++){
    //if there are no notes, ignore it
    if(sequence.noteData[t].size() == 1)
      continue;
    else{
      //move over each note
      for(uint16_t i = 1; i<sequence.noteData.size(); i++){
        if(sequence.noteData[t][i].startPos>=start && sequence.noteData[t][i].startPos<end)
          count++;
      }
    }
  }
  return count;
}

//changes which track is active, changing only to valid tracks
void setActiveTrack(uint8_t newActiveTrack, bool loudly) {
  if (newActiveTrack >= 0 && newActiveTrack < sequence.trackData.size()) {
    if(sequence.activeTrack == 4 && newActiveTrack == 5 && maxTracksShown == 5){
      maxTracksShown = 6;
    }
    else if(sequence.activeTrack == 1 && newActiveTrack == 0 && maxTracksShown == 6){
      maxTracksShown = 5;
    }
    sequence.activeTrack = newActiveTrack;
    if (loudly) {
      MIDI.noteOn(sequence.trackData[sequence.activeTrack].pitch, sequence.defaultVel, sequence.trackData[sequence.activeTrack].channel);
      MIDI.noteOff(sequence.trackData[sequence.activeTrack].pitch, 0, sequence.trackData[sequence.activeTrack].channel);
      if(sequence.trackData[sequence.activeTrack].isLatched){
        MIDI.noteOn(sequence.trackData[sequence.activeTrack].pitch, sequence.defaultVel, sequence.trackData[sequence.activeTrack].channel);
        MIDI.noteOff(sequence.trackData[sequence.activeTrack].pitch, 0, sequence.trackData[sequence.activeTrack].channel);
      }
    }
  }
  sequence.updateLEDs();
  menuText = pitchToString(sequence.trackData[sequence.activeTrack].pitch,true,true);
}

void changeTrackChannel(int id, int newChannel){
  if(newChannel>=0 && newChannel<=16){
    sequence.trackData[id].channel = newChannel;
  }
}

void changeAllTrackChannels(int newChannel){
  for(int track = 0; track<sequence.trackData.size(); track++){
    changeTrackChannel(track, newChannel);
  }
}

void moveToNextNote_inTrack(bool up){
  uint8_t track = sequence.activeTrack;
  uint16_t currentID = sequence.IDAtCursor();
  bool foundTrack = false;
  //moving the track up/down until it hits a track with notes
  //and checking bounds
  if(up){
    while(track<sequence.trackData.size()-1){
      track++;
      if(sequence.noteData[track].size()>1){
        foundTrack = true;
        break;
      }
    }
  }
  else{
    while(track>0){
      track--;
      if(sequence.noteData[track].size()>1){
        foundTrack = true;
        break;
      }
    }
  }
  //if you couldn't find a track with a note on it, just return
  if(!foundTrack){
    return;
  }
  for(uint16_t dist = 0; dist<sequence.sequenceLength; dist++){
    bool stillValid = false;
    //if the new position is in bounds
    if(sequence.cursorPos+dist<=sequence.sequenceLength){
      stillValid = true;
      //and if there's something there!
      if(sequence.lookupTable[track][sequence.cursorPos+dist] != 0){
        //move to it
        moveCursor(dist);
        setActiveTrack(track,false);
        return;
      }
    }
    if(sequence.cursorPos>=dist){
      stillValid = true;
      if(sequence.lookupTable[track][sequence.cursorPos-dist] != 0){
        moveCursor(-dist);
        setActiveTrack(track,false);
        return;
      }
    }
    //if it's reached the bounds
    if(!stillValid){
      return;
    }
  }
}

//moves thru each step, forward or backward, and moves the cursor to the first note it finds
void moveToNextNote(bool forward,bool endSnap){
  //if there's a note on this track at all
  if(sequence.noteData[sequence.activeTrack].size()>1){
    unsigned short int id = sequence.IDAtCursor();
    if(forward){
      for(int i = sequence.cursorPos; i<sequence.sequenceLength; i++){
        if(sequence.lookupTable[sequence.activeTrack][i] !=id && sequence.lookupTable[sequence.activeTrack][i] != 0){
          moveCursor(sequence.noteData[sequence.activeTrack][sequence.lookupTable[sequence.activeTrack][i]].startPos-sequence.cursorPos);
          return;
        }
      }
      if(endSnap){
        moveCursor(sequence.sequenceLength-sequence.cursorPos);
      }
      return;
    }
    else{
      for(int i = sequence.cursorPos; i>0; i--){
        if(sequence.lookupTable[sequence.activeTrack][i] !=id && sequence.lookupTable[sequence.activeTrack][i] != 0){
          moveCursor(sequence.noteData[sequence.activeTrack][sequence.lookupTable[sequence.activeTrack][i]].startPos-sequence.cursorPos);
          return;
        }
      }
      if(endSnap){
        moveCursor(-sequence.cursorPos);
      }
      return;
    }
  }
}

void moveToNextNote(bool forward){
  moveToNextNote(forward, false);
}

//View ------------------------------------------------------------------
//view start is inclusive, starts at 0
//view end is inclusive, 127
//handles making sure the view is correct
void moveView(int16_t val) {
  int oldViewLength = sequence.viewEnd-sequence.viewStart;
  if((sequence.viewStart+val)<0){
    sequence.viewStart = 0;
    sequence.viewEnd = sequence.viewStart+oldViewLength;
  }
  if(sequence.viewEnd+val>sequence.sequenceLength){
    sequence.viewEnd = sequence.sequenceLength;
    sequence.viewStart = sequence.viewEnd - oldViewLength;
  }
  if(sequence.viewEnd+val<=sequence.sequenceLength && sequence.viewStart+val>=0){
    sequence.viewStart += val;
    sequence.viewEnd += val;
  }
}

//moving the cursor around
int16_t moveCursor(int moveAmount){
  int16_t amt;
  //if you're trying to move back at the start
  if(sequence.cursorPos == 0 && moveAmount < 0){
    return 0;
  }
  if(moveAmount<0 && sequence.cursorPos+moveAmount<0){
    amt = sequence.cursorPos;
    sequence.cursorPos = 0;
  }
  else{
    sequence.cursorPos += moveAmount;
    amt = moveAmount;
  }
  if(sequence.cursorPos > sequence.sequenceLength) {
    amt += (sequence.sequenceLength-sequence.cursorPos);
    sequence.cursorPos = sequence.sequenceLength;
  }
  //extend the note if one is being drawn (and if you're moving forward)
  if(drawingNote && moveAmount>0){
    if(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].endPos<sequence.cursorPos)
      changeNoteLength(sequence.cursorPos-sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].endPos,sequence.activeTrack,sequence.noteData[sequence.activeTrack].size()-1);
  }
  //Move the view along with the cursor
  if(sequence.cursorPos<sequence.viewStart+sequence.subDivision && sequence.viewStart>0){
    moveView(sequence.cursorPos - (sequence.viewStart+sequence.subDivision));
  }
  else if(sequence.cursorPos > sequence.viewEnd-sequence.subDivision && sequence.viewEnd<sequence.sequenceLength){
    moveView(sequence.cursorPos - (sequence.viewEnd-sequence.subDivision));
  }
  //update the LEDs
  sequence.updateLEDs();
  menuText = ((moveAmount>0)?(stepsToPosition(sequence.cursorPos,true)+">>"):("<<"+stepsToPosition(sequence.cursorPos,true)));
  return amt;
}

void setCursor(uint16_t loc){
  moveCursor(loc-sequence.cursorPos);
}

void moveCursorIntoView(){
  if (sequence.cursorPos < 0) {
    sequence.cursorPos = 0;
  }
  if (sequence.cursorPos > sequence.sequenceLength-1) {
    sequence.cursorPos = sequence.sequenceLength-1;
  }
  if (sequence.cursorPos < sequence.viewStart) {
    moveView(sequence.cursorPos-sequence.viewStart);
  }
  if (sequence.cursorPos >= sequence.viewEnd) {//doin' it this way so the last column of pixels is drawn, but you don't interact with it
    moveView(sequence.cursorPos-sequence.viewEnd);
  }
}

unsigned short int getNoteLength(unsigned short int track, unsigned short int id){
  return sequence.noteData[track][id].getLength();
}

int16_t changeNoteLength(int val, unsigned short int track, unsigned short int id){
  if(id!=0){
    int newEnd;
    //if it's 1 step long and being increased by sequence.subDivision, make it sequence.subDivision steps long instead of sequence.subDivision+1 steps
    //(just to make editing more intuitive)
    if(sequence.noteData[track][id].endPos - sequence.noteData[track][id].startPos == 1 && val == sequence.subDivision){
      newEnd = sequence.noteData[track][id].endPos + val - 1;
    }
    else{
      newEnd = sequence.noteData[track][id].endPos + val;
    }
    //check and see if there's a different note there
    //if there is, set the new end to be 
    for(uint16_t step = 1; step<=val; step++){
      if(sequence.lookupTable[track][step+sequence.noteData[track][id].endPos] != 0 && sequence.lookupTable[track][step+sequence.noteData[track][id].endPos] != id){
        //if it's the first step, just fail to save time
        if(step == 1)
          return 0;
        //if it's not the first step, set the new end to right before the other note
        else{
          newEnd = sequence.noteData[track][id].endPos+step;
          break;
        }
      }
    }
    //if the new end is before/at the start, don't do anything
    if(newEnd<=sequence.noteData[track][id].startPos)
      return 0;
      // newEnd = sequence.noteData[track][id].startPos+1;
    //if the new end is past/at the end of the seq
    if(newEnd>sequence.sequenceLength){
      newEnd = sequence.sequenceLength;
    }
    Note note = sequence.noteData[track][id];
    int16_t amount = newEnd-note.endPos;
    note.endPos = newEnd;
    sequence.deleteNote_byID(track, id);
    sequence.makeNote(note, track, false);
    return amount;
  }
  return 0;
}


void changeNoteLengthSelected(int amount){
  for(int track = 0; track<sequence.trackData.size(); track++){
    for(int note = 1; note <= sequence.noteData[track].size()-1; note++){
      if(sequence.noteData[track][note].isSelected){
        changeNoteLength(amount, track, note);
      }
    }
  }
}

int16_t changeNoteLength(int amount){
  if(sequence.selectionCount > 0){
    changeNoteLengthSelected(amount);
    vector<uint16_t> bounds = getSelectedNotesBoundingBox();
    //move the cursor to the end/beginning of the selection box
    if(amount>0)
      setCursor(bounds[2]);
    else
      setCursor(bounds[0]);
    return 0;
  }
  else{
    return changeNoteLength(amount, sequence.activeTrack, sequence.IDAtCursor());
  }
}

//this one jumps the cursor to the end or start of the note
void changeNoteLength_jumpToEnds(int16_t amount){
  if(sequence.selectionCount > 0){
    changeNoteLengthSelected(amount);
    vector<uint16_t> bounds = getSelectedNotesBoundingBox();
    //move the cursor to the end/beginning of the selection box
    if(amount>0)
      setCursor(bounds[2]);
    else
      // setCursor(bounds[0]);
      setCursor(bounds[2]-sequence.subDivision);//testing this
  }
  else{
    //if the note was changed
    if(changeNoteLength(amount, sequence.activeTrack, sequence.IDAtCursor()) != 0){
      //if you're shrinking the note
      if(amount<0){
        setCursor(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].endPos-sequence.subDivision);//testing this
        // setCursor(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].startPos);
        //if it's out of view
        // else
          // setCursor(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].endPos+amount);
      }
      //if you're growing it
      else
        // setCursor(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].endPos-sequence.subDivision);
        setCursor(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].endPos-1);//testing this

        // setCursor(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos - amount);
    }
  }
}

//gets the closest pitch of a scale
uint8_t getNearestPitch(int pitch, int scale){
  uint8_t closestPitch;
  return closestPitch;
}

//------------------------------------------------------------------------------------------------------------------------------
bool isInView(int target){
  if(target>=sequence.viewStart && target<=sequence.viewEnd)
    return true;
  else
    return false;
}

uint16_t changeSubDiv(bool direction, uint8_t subDiv, bool allowZero){
  //down
  if(!direction){
    if(subDiv == 1 && allowZero)
      subDiv = 0;
    else if(subDiv>3)
      subDiv /= 2;
    else if(subDiv == 3)
      subDiv = 1;
  }
  else{
    if(subDiv == 0)
      subDiv = 1;
    else if(subDiv == 1)//if it's one, set it to 3
      subDiv = 3;
    else if(subDiv !=  96 && subDiv != 32){
      //if triplet mode
      if(!(subDiv%2))
        subDiv *= 2;
      else if(!(subDiv%3))
        subDiv *=2;
    }
  }
  return subDiv;
}

void changeSubDivInt(bool down){
  changeSubDivInt(down,false);
}

void changeSubDivInt(bool down, bool limitToView){
  if(down){
    if(sequence.subDivision>3 && (!limitToView || (sequence.subDivision*sequence.viewScale)>2))
      sequence.subDivision /= 2;
    else if(sequence.subDivision == 3)
      sequence.subDivision = 1;
  }
  else{
    if(sequence.subDivision == 1)//if it's one, set it to 3
      sequence.subDivision = 3;
    else if(sequence.subDivision !=  96 && sequence.subDivision != 32){
      //if triplet mode
      if(!(sequence.subDivision%2))
        sequence.subDivision *= 2;
      else if(!(sequence.subDivision%3))
        sequence.subDivision *=2;
    }
  }
  menuText = "~"+stepsToMeasures(sequence.subDivision);
}

uint16_t toggleTriplets(uint16_t subDiv){
  //this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
  if(subDiv == 192){
    subDiv = 32;
  }
  else if(!(subDiv%3)){//if it's in 1/4 mode...
    subDiv = 2*subDiv/3;//set it to triplet mode
  }
  else if(!(subDiv%2)){//if it was in triplet mode...
    subDiv = 3*subDiv/2;//set it to 1/4 mode
  }
  menuText = stepsToMeasures(sequence.subDivision);
  return subDiv;
}

void toggleTriplets(){
  //this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
  if(sequence.subDivision == 192){
    sequence.subDivision = 32;
  }
  else if(!(sequence.subDivision%3)){//if it's in 1/4 mode...
    sequence.subDivision = 2*sequence.subDivision/3;//set it to triplet mode
  }
  else if(!(sequence.subDivision%2)){//if it was in triplet mode...
    sequence.subDivision = 3*sequence.subDivision/2;//set it to 1/4 mode
  }
  menuText = stepsToMeasures(sequence.subDivision);
}

//makes sure scale/viewend line up with the display
void checkView(){
  if(sequence.viewEnd>sequence.sequenceLength){
    sequence.viewScale = float(96)/float(sequence.sequenceLength);
    sequence.viewEnd = sequence.sequenceLength+1;
  }
}
//zooms in/out
void zoom(bool in){
  uint16_t viewLength = sequence.viewEnd-sequence.viewStart;
  if(!in && viewLength<sequence.sequenceLength){
    sequence.viewScale /= 2;
  }
  else if(in && viewLength/2>1){
    sequence.viewScale *= 2;
  }  
  sequence.viewStart = 0;
  sequence.viewEnd = 96/sequence.viewScale;
  checkView();
  changeSubDivInt(in);
  moveCursorIntoView();
  sequence.updateLEDs();
  menuText = stepsToMeasures(sequence.viewStart)+"<-->"+stepsToMeasures(sequence.viewEnd)+"(~"+stepsToMeasures(sequence.subDivision)+")";
}
bool areThereAnyNotes(){
  for(uint8_t t = 0; t<sequence.noteData.size(); t++){
    if(sequence.noteData[t].size()>1){
      return true;
    }
  }
  return false;
}
//checks for notes above or below a track
bool areThereMoreNotes(bool above){
  if(sequence.trackData.size()>maxTracksShown){
    if(!above){
      for(int track = endTrack+1; track<sequence.trackData.size();track++){
        if(sequence.noteData[track].size()-1>0)
        return true;
      }
    }
    else if(above){
      for(int track = startTrack-1; track>=0; track--){
        if(sequence.noteData[track].size()-1>0)
        return true;
      }
    }
  }
  return false;
}
//checks to see if a channel is currently being modulated by an autotrack
bool isModulated(uint8_t ch){
  for(uint8_t dt = 0; dt<sequence.autotrackData.size(); dt++){
    if(sequence.autotrackData[dt].parameterType == 2){
      if(sequence.autotrackData[dt].channel == ch && sequence.autotrackData[dt].isActive)
        return true;
    }
  }
  return false;
}
//sort function for sorting tracks by channel
bool compareChannels(Track t1, Track t2){
  return t1.channel>t2.channel;
}

uint8_t getLowestVal(vector<uint8_t> vec){
  uint8_t lowest = 255;
  for(uint8_t i = 0; i<vec.size(); i++){
    if(vec[i]<lowest)
      lowest = vec[i];
  }
  return lowest;
}

bool isInVector(int val, vector<uint8_t> vec){
  if(vec.size() == 0) 
    return false;
  for(int i = 0; i<vec.size(); i++){
    if(val == vec[i]){
      return true;
    }
  }
  return false;
}

void drawSmallChannelIcon(uint8_t x1, uint8_t y1, uint8_t ch){
  display.drawBitmap(x1,y1,ch_tiny,6,3,SSD1306_WHITE);
  printSmall(x1+7,y1,stringify(ch),1);
}

//true if Stepchild is sending or receiving notes
bool isReceiving(){
  for(uint8_t i = 0; i<sequence.trackData.size(); i++){
    if(sequence.trackData[i].noteLastSent != 255)
      return true;
  }
  return receivedNotes.notes.size();
}
bool isSending(){
  for(uint8_t i = 0; i<sequence.trackData.size(); i++){
    if(sequence.trackData[i].noteLastSent != 255)
      return true;
  }
  return sentNotes.notes.size();
}
bool isReceivingOrSending(){
  return (isReceiving() || isSending());
}
void filterOutUnisonNotes(vector<uint8_t>& notes){
  vector<uint8_t> uniqueNotes;
  for(uint8_t i = 0; i<notes.size(); i++){
    if(!uniqueNotes.size())
      uniqueNotes.push_back(notes[i]%12);
    else{
      if(find(uniqueNotes.begin(),uniqueNotes.end(),notes[i]%12) == uniqueNotes.end()){
        uniqueNotes.push_back(notes[i]%12);
      }
    }
  }
}
void togglePlayMode(){
  playing = !playing;
  //if it's looping, set the playhead to the sequence.activeLoop start
  if(sequence.isLooping)
    playheadPos = sequence.loopData[sequence.activeLoop].start;
  else
    playheadPos = 0;
  if(playing){
    if(recording){
      toggleRecordingMode(waitForNoteBeforeRec);
    }
    #ifndef HEADLESS
    MIDI1.setHandleNoteOn(handleNoteOn_Normal);
    MIDI1.setHandleNoteOff(handleNoteOff_Normal);
    MIDI1.setHandleClock(handleClock_playing);
    MIDI1.setHandleStart(handleStart_playing);
    MIDI1.setHandleStop(handleStop_playing);
    MIDI0.setHandleNoteOn(handleNoteOn_Normal);
    MIDI0.setHandleNoteOff(handleNoteOff_Normal);
    MIDI0.setHandleClock(handleClock_playing);
    MIDI0.setHandleStart(handleStart_playing);
    MIDI0.setHandleStop(handleStop_playing);
    #endif

    sequenceClock.startTime = micros();
    if(arp.isActive){
      arp.start();
    }
    MIDI.sendStart();
  }
  else if(!playing){
    stop();
    setNormalMode();
    MIDI.sendStop();
    globalModifiers.velocity[1] = 0;
    globalModifiers.chance[1] = 0;
    globalModifiers.pitch[1] = 0;
    CV.off();

  }
}
void setNormalMode(){
  stop();
  if(arp.isActive){
    arp.stop();
  }
  if(recordingToAutotrack){
    recordingToAutotrack = false;
    controls.counterA = 0;
    controls.counterB = 0;
  }
  #ifndef HEADLESS
  MIDI1.disconnectCallbackFromType(midi::Clock);
  MIDI1.disconnectCallbackFromType(midi::Start);
  MIDI1.disconnectCallbackFromType(midi::Stop);
  MIDI1.disconnectCallbackFromType(midi::NoteOn);
  MIDI1.disconnectCallbackFromType(midi::NoteOff);
  MIDI1.disconnectCallbackFromType(midi::ControlChange);

  MIDI0.disconnectCallbackFromType(midi::Clock);
  MIDI0.disconnectCallbackFromType(midi::Start);
  MIDI0.disconnectCallbackFromType(midi::Stop);
  MIDI0.disconnectCallbackFromType(midi::NoteOn);
  MIDI0.disconnectCallbackFromType(midi::NoteOff);
  MIDI0.disconnectCallbackFromType(midi::ControlChange);

  MIDI0.setHandleNoteOn(handleNoteOn_Normal);
  MIDI0.setHandleNoteOff(handleNoteOff_Normal);
  MIDI0.setHandleStart(handleStart_Normal);
  MIDI0.setHandleStop(handleStop_Normal);

  MIDI1.setHandleNoteOn(handleNoteOn_Normal);
  MIDI1.setHandleNoteOff(handleNoteOff_Normal);
  MIDI1.setHandleStart(handleStart_Normal);
  MIDI1.setHandleStop(handleStop_Normal);

  MIDI0.setHandleControlChange(handleCC_Normal);
  MIDI1.setHandleControlChange(handleCC_Normal);
  #endif
}

void toggleRecordingMode(bool butWait){
  recording = !recording;
  //if it stopped recording
  if(!recording)
    cleanupRecording(recheadPos);
  //if it's recording to the loop
  if(recMode == ONESHOT || recMode == LOOP_MODE)
    recheadPos = sequence.loopData[sequence.activeLoop].start;
  // else
  //   recheadPos = ONESHOT;
  if(butWait)
    waitingToReceiveANote = true;
  else
    waitingToReceiveANote = false;
  if(recording){
    if(playing){
      togglePlayMode();
    }
    stop();
    #ifndef HEADLESS
    //disconnecting all the midi callbacks!
    MIDI1.disconnectCallbackFromType(midi::NoteOn);
    MIDI1.disconnectCallbackFromType(midi::NoteOff);
    MIDI1.disconnectCallbackFromType(midi::Clock);
    MIDI1.disconnectCallbackFromType(midi::Start);
    MIDI1.disconnectCallbackFromType(midi::Stop);

    MIDI0.disconnectCallbackFromType(midi::NoteOn);
    MIDI0.disconnectCallbackFromType(midi::NoteOff);
    MIDI0.disconnectCallbackFromType(midi::Clock);
    MIDI0.disconnectCallbackFromType(midi::Start);
    MIDI0.disconnectCallbackFromType(midi::Stop);

    //reconnecting the midi callbacks
    MIDI1.setHandleNoteOn(handleNoteOn_Recording);
    MIDI1.setHandleNoteOff(handleNoteOff_Recording);
    MIDI1.setHandleClock(handleClock_recording);
    MIDI1.setHandleStart(handleStart_recording);
    MIDI1.setHandleStop(handleStop_recording);
    MIDI1.setHandleControlChange(handleCC_Recording);

    MIDI0.setHandleNoteOn(handleNoteOn_Recording);
    MIDI0.setHandleNoteOff(handleNoteOff_Recording);
    MIDI0.setHandleClock(handleClock_recording);
    MIDI0.setHandleStart(handleStart_recording);
    MIDI0.setHandleStop(handleStop_recording);
    MIDI0.setHandleControlChange(handleCC_Recording);
    #endif
    sequenceClock.startTime = micros();
  }
  else{//go back to normal mode
    setNormalMode();
  }
}

//moves the whole loop
void moveLoop(int16_t amount){
  uint16_t length = sequence.loopData[sequence.activeLoop].end-sequence.loopData[sequence.activeLoop].start;
  //if it's being moved back
  if(amount<0){
    //if amount is larger than start, meaning start would be moved before 0
    if(sequence.loopData[sequence.activeLoop].start<=amount)
      setLoopPoint(0,true);
    else
      setLoopPoint(sequence.loopData[sequence.activeLoop].start+amount,true);
    setLoopPoint(sequence.loopData[sequence.activeLoop].start+length,false);
  }
  //if it's being moved forward
  else{
    //if amount is larger than the gap between seqend and sequence.loopData[sequence.activeLoop].end
    if((sequence.sequenceLength-sequence.loopData[sequence.activeLoop].end)<=amount)
      setLoopPoint(sequence.sequenceLength,false);
    else
      setLoopPoint(sequence.loopData[sequence.activeLoop].end+amount,false);
    setLoopPoint(sequence.loopData[sequence.activeLoop].end - length,true);
  }
}
void toggleLoopMove(){
  switch(movingLoop){
    case 0:
      movingLoop = 1;
      moveCursor(sequence.loopData[sequence.activeLoop].start-sequence.cursorPos);
      break;
    case -1:
      movingLoop = 1;
      moveCursor(sequence.loopData[sequence.activeLoop].end-sequence.cursorPos);
      break;
    case 1:
      movingLoop = 2;
      break;
    case 2:
      movingLoop = 0;
      break;
  }
}

void yControls(){
  if(utils.itsbeen(100)){
    if (controls.joystickY == 1) {
      setActiveTrack(sequence.activeTrack + 1, false);
      drawingNote = false;
      lastTime = millis();
    }
    if (controls.joystickY == -1) {
      setActiveTrack(sequence.activeTrack - 1, false);
      drawingNote = false;
      lastTime = millis();
    }
  }
}

//moving cursor, loop, and active track. Pass "true" to allow changing the velocity of notes
void defaultJoystickControls(bool velocityEditingAllowed){
  if (utils.itsbeen(100)) {
    if (controls.joystickX == 1 && !controls.SHIFT()) {
      //if cursor isn't on a measure marker, move it to the nearest one
      if(sequence.cursorPos%sequence.subDivision){
        moveCursor(-sequence.cursorPos%sequence.subDivision);
        lastTime = millis();
        //moving entire loop
        if(movingLoop == 2)
          moveLoop(-sequence.cursorPos%sequence.subDivision);
      }
      else{
        moveCursor(-sequence.subDivision);
        lastTime = millis();
        //moving entire loop
        if(movingLoop == 2)
          moveLoop(-sequence.subDivision);
      }
      //moving loop start/end
      if(movingLoop == -1){
        setLoopPoint(sequence.cursorPos,true);
      }
      else if(movingLoop == 1){
        setLoopPoint(sequence.cursorPos,false);
      }
    }
    if (controls.joystickX == -1 && !controls.SHIFT()) {
      if(sequence.cursorPos%sequence.subDivision){
        moveCursor(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
        lastTime = millis();
        if(movingLoop == 2)
          moveLoop(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
      }
      else{
        moveCursor(sequence.subDivision);
        lastTime = millis();
        if(movingLoop == 2)
          moveLoop(sequence.subDivision);
      }
      //moving loop start/end
      if(movingLoop == -1){
        setLoopPoint(sequence.cursorPos,true);
      }
      else if(movingLoop == 1){
        setLoopPoint(sequence.cursorPos,false);
      }
    }
  }
  if(utils.itsbeen(100)){
    if (controls.joystickY == 1 && !controls.SHIFT() && !controls.LOOP()) {
      if(recording)//if you're not in normal mode, you don't want it to be loud
        setActiveTrack(sequence.activeTrack + 1, false);
      else
        setActiveTrack(sequence.activeTrack + 1, false);
      drawingNote = false;
      lastTime = millis();
    }
    if (controls.joystickY == -1 && !controls.SHIFT() && !controls.LOOP()) {
      if(recording)//if you're not in normal mode, you don't want it to be loud
        setActiveTrack(sequence.activeTrack - 1, false);
      else
        setActiveTrack(sequence.activeTrack - 1, false);
      drawingNote = false;
      lastTime = millis();
    }
  }
  if (utils.itsbeen(50)) {
    //moving
    if (controls.joystickX == 1 && controls.SHIFT()) {
      moveCursor(-1);
      lastTime = millis();
      if(movingLoop == 2)
        moveLoop(-1);
      else if(movingLoop == -1)
        setLoopPoint(sequence.cursorPos,true);
      else if(movingLoop == 1)
        setLoopPoint(sequence.cursorPos,false);
    }
    if (controls.joystickX == -1 && controls.SHIFT()) {
      moveCursor(1);
      lastTime = millis();
      if(movingLoop == 2)
        moveLoop(1);
      else if(movingLoop == -1)
        sequence.loopData[sequence.activeLoop].start = sequence.cursorPos;
      else if(movingLoop == 1)
        sequence.loopData[sequence.activeLoop].end = sequence.cursorPos;
    }
    //changing vel
    if(velocityEditingAllowed){
      if (controls.joystickY == 1 && controls.SHIFT()) {
        sequence.changeVel(-10);
        lastTime = millis();
      }
      if (controls.joystickY == -1 && controls.SHIFT()) {
        sequence.changeVel(10);
        lastTime = millis();
      }

      if(sequence.IDAtCursor()==0){
        if(controls.joystickY == 1 && controls.SHIFT()){
          sequence.defaultVel-=10;
          if(sequence.defaultVel<1)
            sequence.defaultVel = 1;
          lastTime = millis();
        }
        if(controls.joystickY == -1 && controls.SHIFT()){
          sequence.defaultVel+=10;
          if(sequence.defaultVel>127)
            sequence.defaultVel = 127;
          lastTime = millis();
        }
      }
    }
  }
}

void defaultSelectBoxControls(){
  //when controls.SELECT()  is pressed and stick is moved, and there's no selection box yet, start one
  if(controls.SELECT()  && !selBox.begun && (controls.joystickX != 0 || controls.joystickY != 0)){
    selBox.begun = true;
    selBox.coords.start.x = sequence.cursorPos;
    selBox.coords.start.y = sequence.activeTrack;
  }
  //if controls.SELECT()  is released, and there's a selection box, end it and select what was in the box
  if(!controls.SELECT()  && selBox.begun){
    selBox.coords.end.x = sequence.cursorPos;
    selBox.coords.end.y = sequence.activeTrack;
    selectBox();
    selBox.begun = false;
  }
}

//default selection behavior
void defaultSelectControls(){
  if(controls.SELECT()  && !selBox.begun){
    uint16_t id = sequence.IDAtCursor();
    if(id == 0){
      clearSelection();
    }
    else{
      //select all
      if(controls.NEW()){
        selectAll();
      }
      //select only one
      else if(controls.SHIFT()){
        clearSelection();
        toggleSelectNote(sequence.activeTrack,id, false);
      }
      //normal select
      else{
        toggleSelectNote(sequence.activeTrack, id, true);          
      }
    }
    lastTime = millis();
  }
}

void defaultLoopControls(){
  if(controls.LOOP()){
      //if you're not moving a loop, start
      if(movingLoop == 0){
        //if you're on the start, move the start
        if(sequence.cursorPos == sequence.loopData[sequence.activeLoop].start){
          movingLoop = -1;
          menuText = "Moving Loop Start";
        }
        //if you're on the end
        else if(sequence.cursorPos == sequence.loopData[sequence.activeLoop].end){
          movingLoop = 1;
          menuText = "Moving Loop End";
        }
        //if you're not on either, move the whole loop
        else{
          movingLoop = 2;
          menuText = "Moving Loop";
        }
        lastTime = millis();
      }
      //if you were moving, stop
      else{
        movingLoop = 0;
        lastTime = millis();
      }
  }
}

//default copy/paste behavior
void defaultCopyControls(){
  if(controls.COPY()){
    if(controls.SHIFT())
      clipboard.paste();
    else{
      clipboard.copy();
    }
    lastTime = millis();
  }
}
void mainSequencerButtons(){
  defaultSelectBoxControls();
  if(!controls.NEW())
    drawingNote = false;

  //del happens a liitle faster (so you can draw/erase fast)
  if(utils.itsbeen(75)){
    //del
    if(controls.DELETE() && !controls.SHIFT()){
      if (sequence.selectionCount > 0){
        sequence.deleteSelected();
        lastTime = millis();
      }
      else if(sequence.IDAtCursor() != 0){
        sequence.deleteNote(sequence.activeTrack,sequence.cursorPos);
        sequence.updateLEDs();
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    //new
    if(controls.NEW() && !drawingNote && !controls.SELECT() ){
      if(controls.SHIFT()){
        //if there's no note starting here, make a new note 1/2 the subdiv
        if((sequence.IDAtCursor() == 0 || sequence.cursorPos != sequence.noteAtCursor().startPos)){
          sequence.makeNote(sequence.activeTrack,sequence.cursorPos,sequence.subDivision/2,true);
          moveCursor(sequence.subDivision/2);
          drawingNote = true;
          lastTime = millis();
        }
        //if there is a note here, cut it in half/thirds (depending on if you're in triplet mode or not)
        else{
          sequence.chopNote(sequence.activeTrack,sequence.cursorPos,sequence.isQuarterGrid()?2:3);
          lastTime = millis();
        }
      }
      else if(!controls.SHIFT() && (sequence.IDAtCursor() == 0 || sequence.cursorPos != sequence.noteAtCursor().startPos)){
        sequence.makeNote(sequence.activeTrack,sequence.cursorPos,sequence.subDivision,true);
        moveCursor(sequence.subDivision);
        drawingNote = true;
        lastTime = millis();
      }
    }
    defaultSelectControls();
    defaultCopyControls();
    if(!controls.SHIFT()){
      defaultLoopControls();
    }
    //special case for the main sequence; loop+controls.SHIFT() jumps into the loop menu
    else if(controls.LOOP()){
      sequence.isLooping = !sequence.isLooping;
      lastTime = millis();
      menuText = sequence.isLooping?"loop on":"loop off";
    }

    if(controls.DELETE() && controls.SHIFT()){
        sequence.muteNote(sequence.activeTrack, sequence.IDAtCursor(), true);
      if(sequence.selectionCount>0){
        sequence.muteSelectedNotes();
      }
      lastTime = millis();
    }

    //Modes: play, listen, and record
    if(controls.PLAY() && !controls.SHIFT() && !recording){
      togglePlayMode();
      lastTime = millis();
    }
    //if play+controls.SHIFT(), or if play and it's already recording
    if(controls.PLAY() && controls.SHIFT() || controls.PLAY() && recording){
      toggleRecordingMode(waitForNoteBeforeRec);
      lastTime = millis();
    }
 

    //menu press
    if(controls.MENU()){
      lastTime = millis();
      menuIsActive = true;
      constructMenu("MENU");
      mainMenu();
      return;
    }
    if(controls.A()){
      menuIsActive = true;
      lastTime = millis();
      // controls.setA(false);
      constructMenu(TRACK_MENU);
    }
    if(controls.B()){
      lastTime = millis();
      // controls.setB(false);;
      menuIsActive = true;
      constructMenu(EDIT_MENU);
    }
  }
}

//CHECK
//true if pitch is being sent or received
bool isBeingPlayed(uint8_t pitch){
  if(sentNotes.containsPitch(pitch) || receivedNotes.containsPitch(pitch)){
    return true;
  }
  return false;
}

void stepButtons(){
  if(utils.itsbeen(200)){
    //DJ loop selector
    if(controls.SHIFT()){
      for(uint8_t i = 0; i<sequence.loopData.size(); i++){
        if(controls.stepButton(i)){
          setActiveLoop(i);
          lastTime = millis();
          break;
        }
      }
    }
    else{
      bool atLeastOne = false;
      //if it's in 1/4 mode
      if(!(sequence.subDivision%3)){
        for(int i = 0; i<16; i++){
          if(controls.stepButton(i)){
            uint16_t viewLength = sequence.viewEnd-sequence.viewStart;
            sequence.toggleNote(sequence.activeTrack, sequence.viewStart+i*viewLength/16,viewLength/8);
            atLeastOne = true;
          }
        }
      }
      //if it's in 1/3 mode, last two buttons do nothing
      else if(!(sequence.subDivision%2)){
        for(int i = 0; i<12; i++){
          if(controls.stepButton(i)){
            uint16_t viewLength = sequence.viewEnd-sequence.viewStart;
            sequence.toggleNote(sequence.activeTrack,sequence.viewStart+i*viewLength/12,viewLength/6);
            atLeastOne = true;
          }
        }
      }
      if(atLeastOne){
        lastTime = millis();
        sequence.updateLEDs();
      }
    }
  }
}

void alert(String text, int time){
  unsigned short int len = text.length()*6;
  display.clearDisplay();
  display.setCursor(screenWidth/2-len/2, 29);
  display.print(text);
  display.display();
  delay(time);
}

void defaultEncoderControls(){
  while(controls.counterA != 0){
    //changing zoom
    if(controls.counterA >= 1){
      zoom(true);
    }
    if(controls.counterA <= -1){
      zoom(false);
    }
    controls.counterA += controls.counterA<0?1:-1;;
  }
  while(controls.counterB != 0){
    //if shifting, toggle between 1/3 and 1/4 mode
    if(controls.SHIFT()){
      toggleTriplets();
    }
    else if(controls.counterB >= 1){
      changeSubDivInt(true);
    }
    //changing subdivint
    else if(controls.counterB <= -1){
      changeSubDivInt(false);
    }
    controls.counterB += controls.counterB<0?1:-1;;
  }
}

void mainSequencerEncoders(){
  while(controls.counterA != 0){
    //changing pitch
    if(controls.SHIFT()){
      if(controls.counterA >= 1){
        setTrackPitch(sequence.activeTrack,sequence.trackData[sequence.activeTrack].pitch+1,true);
      }
      if(controls.counterA <= -1){
        setTrackPitch(sequence.activeTrack,sequence.trackData[sequence.activeTrack].pitch-1,true);
      }
    }
    //changing zoom
    else{
      if(controls.counterA >= 1 && !controls.A()){
        zoom(true);
      }
      if(controls.counterA <= -1 && !controls.A()){
        zoom(false);
      }
    }
    controls.counterA += controls.counterA<0?1:-1;;
  }
  while(controls.counterB != 0){
    if(controls.SHIFT()){
      toggleTriplets();
    }
    else if(controls.counterB >= 1){
      changeSubDivInt(true,true);
    }
    //changing subdivint
    else if(controls.counterB <= -1){
      changeSubDivInt(false,true);
    }
    controls.counterB += controls.counterB<0?1:-1;;
  }
}

String enterText(String title){
  bool done = false;
  int highlight = 0;
  int rows = 5;
  int columns = 8;
  int textWidth = 12;
  int textHeight = 8;
  String text = "";
  vector<String> alphabet = {};
  vector<String> alpha1 = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","_",".","!","0","1","2","3","4","5","6","7","8","9","Enter"};
  vector<String> alpha2 = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","_",".","!","0","1","2","3","4","5","6","7","8","9","Enter"};
  alphabet = alpha1;
  while(!done){
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(200)){
      if(controls.SHIFT()){
        if(alphabet[0] == "a")
          alphabet = alpha2;
        else 
          alphabet = alpha1;
        lastTime = millis();
      }

      if(controls.DELETE() && text.length()>0){
        controls.setDELETE(false);
        String newString = text.substring(0,text.length()-1);
        text = newString;
        lastTime = millis();
      }
      if(controls.SELECT() ){
        //adding character to text
        if(alphabet[highlight] != "Enter" && text.length()<10){
          text+=alphabet[highlight];
        }
        //or quitting
        else if(alphabet[highlight] == "Enter")
          done = true;
        lastTime = millis();
      }
      if(controls.MENU()){
        text = "";
        done = true;
        lastTime = millis();
      }
    }
    if(utils.itsbeen(100)){
      if(controls.joystickX == -1 && highlight<alphabet.size()-1){
        highlight++;
        lastTime = millis();
      }
      if(controls.joystickX == 1 && highlight>0){
        highlight--;
        lastTime = millis();
      }
      if(controls.joystickY == -1 && highlight>=columns){
        highlight-=columns;
        lastTime = millis();
      }
      if(controls.joystickY == 1 && highlight<alphabet.size()-columns){
        highlight+=columns;
        lastTime = millis();
      }
    }
    //title
    display.clearDisplay();
    display.setCursor(5,6);
    display.setFont(&FreeSerifItalic9pt7b);
    display.print(title);
    display.setFont();

    //text tooltip
    printSmall(88,1,"[SHF] caps",SSD1306_WHITE);
    printSmall(88,8,"[MNU] exit",SSD1306_WHITE);

    //input text
    display.setCursor(10,15);
    if(alphabet[highlight] != "Enter"){
      display.print(text+alphabet[highlight]);
      //cursor
      if(millis()%750>250){
        display.drawFastVLine(9+text.length()*6,15,10,SSD1306_WHITE);
      }
    }
    else
      display.print(text);
    

    //drawing alphabet
    int count = 0;
    for(int i = 0; i<rows; i++){
      for(int j = 0; j<columns; j++){
        if(count<alphabet.size()){
          display.setCursor(j*textWidth+10, i*textHeight+24);
          if(count == highlight){
            //cursor highlight
            if(millis()%750>250){
              display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
              display.fillRect(j*textWidth+9,i*textHeight+23,textWidth-5,textHeight+1,SSD1306_WHITE);
              // display.print(alphabet[count]);
              // display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
            }
            // else{
              display.print(alphabet[count]);
              display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
              // display.drawRect(j*textWidth+9,i*textHeight+23,textWidth-5,textHeight+1,SSD1306_WHITE);
            // }
          }
          else{
            display.print(alphabet[count]);
          }
          count++;
        }
      }
    }

    display.display();
  }
  if(text == ""){
    text = "default";
  }
  return text;
}

//pulses the onboard LED
void ledPulse(uint8_t speed){
  //use abs() so that it counts DOWN when millis() overflows into the negatives
  //Multiply by 4 so that it's 'saturated' for a while --> goes on, waits, then pulses
  analogWrite(ONBOARD_LED,4*abs(int8_t(millis()/speed)));
}

//runs while "playing" is true
void playingLoop(){
  //internal timing
  if(clockSource == INTERNAL_CLOCK){
    if(sequenceClock.hasItBeenEnoughTime(playheadPos)){
      MIDI.sendClock();
      playStep(playheadPos);
      playheadPos++;
      checkLoop();
    }
  }
  //external timing
  else if(clockSource == EXTERNAL_CLOCK){
    MIDI.read();
    if(gotClock && hasStarted){
      gotClock = false;
      playStep(playheadPos);
      playheadPos += 1;
      checkLoop();
      // checkFragment();
    }
  }
}

//this records CC to the activeDT (when you're in the )
void checkAutotracks(){
  if(recordingToAutotrack){
    int newVal = 64;
    switch(sequence.autotrackData[sequence.activeAutotrack].recordFrom){
      //recording externally, so get outta this loop!
      case 0:
        return;
      //rec from encoder A
      case 1:
        if(controls.counterA>127){
          controls.counterA = 127;
        }
        if(controls.counterA<0)
          controls.counterA = 0;
        newVal = controls.counterA;
        break;
      //rec from encoder B
      case 2:
        if(controls.counterB>127)
          controls.counterB = 127;
        if(controls.counterB<0)
          controls.counterB = 0;
        newVal = controls.counterB;
        break;
      //rec from joystick X
      case 3:
        newVal = getJoyX();
        if(newVal < 58 || newVal>68)
          waitingToReceiveANote = false;
        break;
      //rec from joystick Y
      case 4:
        newVal = getJoyY();
        if(newVal < 58 || newVal>68)
          waitingToReceiveANote = false;
        break;
    }
    //bounds checking the new value before we write it to the DT
    if(newVal>127)
      newVal = 127;
    else if(newVal<0)
      newVal = 0;
    if(waitingToReceiveANote){
      return;
    }
    recentCC.val = newVal;
    recentCC.cc = sequence.autotrackData[sequence.activeAutotrack].control;
    recentCC.channel = sequence.autotrackData[sequence.activeAutotrack].channel;
    sequence.autotrackData[sequence.activeAutotrack].data[recheadPos] = newVal;
  }
}

void defaultLoop(){
  playheadPos = sequence.loopData[sequence.activeLoop].start;
  recheadPos = sequence.loopData[sequence.activeLoop].start;
  // fragmentStep = 0;
  MIDI.read();
}

void arpLoop(){
  //if it was active, but hadn't started playing yet
  if(!arp.playing){
    switch(arp.source){
      case 0:
        if(receivedNotes.notes.size()>0)
          arp.start();
        break;
      case 1:
        if(sentNotes.notes.size()>0)
          arp.start();
        break;
    }
  }
  if(arp.playing){
    //if the arp isn't latched and there are no notes for it
    if(!arp.holding  && ((arp.source == 0 && !receivedNotes.notes.size()) || (arp.source == 1 && !sentNotes.notes.size()))){
      arp.stop();
    }
    //if it IS latched or there are notes for it, then continue
    else if(arp.hasItBeenEnoughTime()){
      arp.playstep();
    }
  }
}
