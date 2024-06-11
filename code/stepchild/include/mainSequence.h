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
  //it's ok to call this in here bc the LB checks to make sure it doesn't redundantly write
  sequence.displayMainSequenceLEDs();
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
        sequence.setLoopPoint(sequence.cursorPos,true);
      }
      else if(movingLoop == 1){
        sequence.setLoopPoint(sequence.cursorPos,false);
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
        sequence.setLoopPoint(sequence.cursorPos,true);
      }
      else if(movingLoop == 1){
        sequence.setLoopPoint(sequence.cursorPos,false);
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
        sequence.setLoopPoint(sequence.cursorPos,true);
      else if(movingLoop == 1)
        sequence.setLoopPoint(sequence.cursorPos,false);
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
    //shift+loop pushes the loop to to a new loop
    else if(controls.LOOP()){
      lastTime = millis();
      pushToNewLoop();
      // sequence.isLooping = !sequence.isLooping;
      // lastTime = millis();
      // menuText = sequence.isLooping?"loop on":"loop off";
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

void stepButtons(){
  //don't need to time debounce the step buttons!
  //DJ loop selector
  if(controls.SHIFT()){
    if(controls.stepButton(15) && sequence.activeTrack){
      setActiveTrack(sequence.activeTrack-1,false);
    }
    else if(controls.stepButton(14)){
      setActiveTrack(sequence.activeTrack+1,false);
    }
  }
  else{
    //if it's in 1/4 mode
    if(!(sequence.subDivision%3)){
      for(int i = 0; i<16; i++){
        if(controls.stepButton(i)){
          uint16_t viewLength = sequence.viewEnd-sequence.viewStart;
          sequence.toggleNote(sequence.activeTrack, sequence.viewStart+i*viewLength/16,viewLength/8);
        }
      }
    }
    //if it's in 1/3 mode, last two buttons do nothing
    else if(!(sequence.subDivision%2)){
      for(int i = 0; i<12; i++){
        if(controls.stepButton(i)){
          uint16_t viewLength = sequence.viewEnd-sequence.viewStart;
          sequence.toggleNote(sequence.activeTrack,sequence.viewStart+i*viewLength/12,viewLength/6);
        }
      }
    }
  }
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

void mainSequence(){
    controls.readJoystick();
    controls.readButtons();
    mainSequencerButtons();
    stepButtons();
    mainSequencerEncoders();
    defaultJoystickControls(true);
    displaySeq();
}