struct SequenceRenderSettings{
    uint16_t start;
    uint16_t end;
    uint8_t startHeight;
    bool onlyWithinLoop;
    bool drawLoopFlags;
    bool drawLoopPoints;

    bool trackLabels;
    bool topLabels;
    bool loopPoints;
    // bool menus;
    bool trackSelection = false;
    bool shadeOutsideLoop = false;

    bool shrinkTopDisplay = false;
    bool drawPram = true;
    bool drawCursor = true;
    bool displayingVel = true;
    bool drawTrackChannel = false;
    bool drawSuperposition = false;

    SequenceRenderSettings(){
        shadeOutsideLoop = false;
        onlyWithinLoop = false;
        start = sequence.viewStart;
        end = sequence.viewEnd;
        trackLabels = true;
        topLabels = true;
        drawLoopPoints = true;
        // menus = false;
        trackSelection = false;
        if(!maxTracksShown==5){//this should change so that shrinkTop controls maxTracksShown, not the other way around
          shrinkTopDisplay = true;
          startHeight = 8;
          drawLoopFlags = false;
        }
        else{
          startHeight = headerHeight;
          drawLoopFlags = true;
        }
        drawTrackChannel = controls.SHIFT();
        makeViewInBounds();
    }

    void makeViewInBounds(){
        if(end>sequence.sequenceLength){
            end = sequence.sequenceLength;
        }
        if(start>end){
            start = 0;
        }
    }
    uint16_t getViewLength(){
        return end-start;
    }
};

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

//draws pram, other icons (not loop points tho)
void drawTopIcons(SequenceRenderSettings& settings){
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
      display.drawBitmap(x1,0,autotrack1,10,7,SSD1306_WHITE);
    else{
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
    if(settings.displayingVel){
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
        printSmall(x1,1,"c:"+stringify(sequence.defaultChance),SSD1306_WHITE);
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
    display.drawBitmap(x1+6,2,tiny_arp_bmp,9,3,1);
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
void drawSeq(SequenceRenderSettings& settings){

    uint16_t viewLength = settings.end - settings.start;

    trackHeight = (screenHeight-settings.startHeight)/maxTracksShown;

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

    //drawing selection box, since it needs to overlay stepSeq data
    if(selBox.begun){
        selBox.render();
    }
    uint8_t height;
    if(endTrack == sequence.trackData.size())
        height = settings.startHeight+trackHeight*maxTracksShown;
    else if(sequence.trackData.size()>maxTracksShown)
        height = settings.startHeight+trackHeight*(maxTracksShown+1);
    else
        height = settings.startHeight+trackHeight*sequence.trackData.size();

    //drawing measure bars, loop points
    drawSeqBackground(settings.start, settings.end, settings.startHeight, height, settings.shadeOutsideLoop, settings.drawLoopFlags, settings.drawLoopPoints);

    //top and bottom bounds
    if(startTrack == 0){
        display.drawFastHLine(trackDisplay,settings.startHeight,screenWidth,SSD1306_WHITE);
    }
    //if the bottom is in view
    if(endTrack == sequence.trackData.size()){
        display.drawFastHLine(trackDisplay,settings.startHeight+trackHeight*maxTracksShown,screenWidth,SSD1306_WHITE);
    }
    else if(endTrack< sequence.trackData.size())
        endTrack++;
    //drawin all da steps
    //---------------------------------------------------
    for (uint8_t track = startTrack; track < endTrack; track++) {
        unsigned short int y1 = (track-startTrack) * trackHeight + settings.startHeight;
        unsigned short int y2 = y1 + trackHeight;
        uint8_t xCoord;
        //track info display
        if(sequence.activeTrack == track){
            xCoord = 9;
            if(settings.trackLabels)
                graphics.drawArrow(6+((millis()/400)%2),y1+trackHeight/2+1,2,0,true);
        }
        else{
            xCoord = 5;
        }
        if(settings.trackLabels){
            if(!isShrunk){
                //printing note names
                if(pitchesOrNumbers){
                    printTrackPitch(xCoord, y1+trackHeight/2-2,track,false,settings.drawTrackChannel,SSD1306_WHITE);
                }
                //just printing pitch numbers
                else{
                    display.setCursor(xCoord,y1+2);
                    display.print(sequence.trackData[track].pitch);
                }
            }
            //if it's shrunk, draw it small
            else{
                String pitch = sequence.trackData[track].getPitchAndOctave();
                if(track%2)
                    printSmall(18, y1, pitchToString(sequence.trackData[track].pitch,true,true), SSD1306_WHITE);
                else
                    printSmall(2, y1, pitchToString(sequence.trackData[track].pitch,true,true), SSD1306_WHITE);
                if(sequence.trackData[track].noteLastSent != 255){
                    display.drawRect(0,y1,trackDisplay,trackHeight+2,SSD1306_WHITE);
                }
            }
        }
        //if you're drawing selected tracks highlight
        if(settings.trackSelection){
          //if this track isn't selected, shade it
          if(!sequence.trackData[track].isSelected()){
            display.fillRect(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,0);
            graphics.shadeArea(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,3);
            continue;
          }
          else{
            display.fillRect(0,y1,trackDisplay,trackHeight,2);
          }
        }
        //if the track is muted, just hatch it out (don't draw any notes)
        //if it's solo'd and muted, draw it normal (solo overrules mute)
        else if(sequence.trackData[track].isMuted() && !sequence.trackData[track].isSolo()){
            graphics.shadeArea(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,9);
            continue;
        }
        else{
            //highlight for solo'd tracks
            if(sequence.trackData[track].isSolo())
                graphics.drawNoteBracket(trackDisplay+3,y1-1,screenWidth-trackDisplay-5,trackHeight+2,true);
            //Check to see if you only want to render the region in the active loop, and shade everything else!
            for (uint16_t step = settings.shadeOutsideLoop?sequence.loopData[sequence.activeLoop].start:settings.start; step < (settings.shadeOutsideLoop?sequence.loopData[sequence.activeLoop].end:settings.end); step++) {
                uint16_t id = sequence.lookupTable[track][step];
                unsigned short int x1 = trackDisplay+int8_t((step-settings.start)*sequence.viewScale);
                unsigned short int x2 = x1 + (step-settings.start)*sequence.viewScale;
                //drawing note
                if (id != 0){
                    if(step == sequence.noteData[track][id].startPos){
                        uint16_t length = (sequence.noteData[track][id].endPos - sequence.noteData[track][id].startPos)*sequence.viewScale;
                        if(settings.displayingVel)
                            drawNote_vel(id, track, x1,y1,length,trackHeight,sequence.noteData[track][id].velocity,sequence.noteData[track][id].isSelected(),sequence.noteData[track][id].isMuted());
                        else
                            drawNote_chance(id, track, x1,y1,length,trackHeight,sequence.noteData[track][id].chance,sequence.noteData[track][id].isSelected(),sequence.noteData[track][id].isMuted());
                        if(sequence.noteData[track][id].isSelected()){
                            display.drawRect(x1,y1,length+1,trackHeight,SSD1306_WHITE);
                            display.drawRect(x1+1,y1+1,length-1,trackHeight-2,SSD1306_BLACK);
                        }
                    }
                    else if(!isInView(sequence.noteData[track][id].startPos) && step == settings.start){
                        unsigned short int length = (sequence.noteData[track][id].endPos - settings.start)*sequence.viewScale;
                        if(settings.displayingVel)
                            drawNote_vel(id, track, x1,y1,length,trackHeight,sequence.noteData[track][id].velocity,sequence.noteData[track][id].isSelected(),sequence.noteData[track][id].isMuted());
                        else
                            drawNote_chance(id, track, x1,y1,length,trackHeight,sequence.noteData[track][id].chance,sequence.noteData[track][id].isSelected(),sequence.noteData[track][id].isMuted());
                    }
                }
            }
        }
    }
    //---------------------------------------------------
    //drawing cursor
    if(settings.drawCursor){
        uint8_t cPos = trackDisplay+int((sequence.cursorPos-settings.start)*sequence.viewScale);
        if(cPos>127)
            cPos = 126;
        if(endTrack == sequence.trackData.size()){
            display.drawFastVLine(cPos, settings.startHeight, (endTrack-startTrack)*trackHeight, SSD1306_WHITE);
            display.drawFastVLine(cPos+1, settings.startHeight, (endTrack-startTrack)*trackHeight, SSD1306_WHITE);
        }
        else{
            display.drawFastVLine(cPos, settings.startHeight, screenHeight-settings.startHeight, SSD1306_WHITE);
            display.drawFastVLine(cPos+1, settings.startHeight, screenHeight-settings.startHeight, SSD1306_WHITE);
        }
    }
    //all the top icons/tooltips
    if(settings.topLabels){
        drawTopIcons(settings);
    }
    //drawing big or small pram in the corner
    if(settings.drawPram){
        if(settings.shrinkTopDisplay){
            graphics.drawTinyPram();
        }
        else{
            graphics.drawBigPram();
        }
    }
    //playhead/rechead
    if(playing && isInView(playheadPos))
        display.drawRoundRect(trackDisplay+(playheadPos-settings.start)*sequence.viewScale,settings.startHeight,3, screenHeight-settings.startHeight, 3, SSD1306_WHITE);
    if(recording && isInView(recheadPos))
        display.drawRoundRect(trackDisplay+(recheadPos-settings.start)*sequence.viewScale,settings.startHeight,3, screenHeight-settings.startHeight, 3, SSD1306_WHITE);

    int cursorX = trackDisplay+int((sequence.cursorPos-settings.start)*sequence.viewScale)-8;
    if(!playing && !recording){
        cursorX = 32;
    }
    else{
        //making sure it doesn't print over the subdiv info
        cursorX = 42;
    }
    //drawing active track highlight
    uint8_t y1 = (sequence.activeTrack-startTrack) * trackHeight + settings.startHeight;
    // display.drawRect(x1, y1, screenWidth, trackHeight, SSD1306_WHITE);
    display.drawFastHLine(trackDisplay,y1,screenWidth-trackDisplay,1);
    display.drawFastHLine(trackDisplay,y1+trackHeight-1,screenWidth-trackDisplay,1);

    //anim offset (for the pram)
    if(!menuIsActive){
        animOffset++;
        animOffset%=100;
    }
    //it's ok to call this in here bc the LB checks to make sure it doesn't redundantly write
    sequence.displayMainSequenceLEDs();
}

void drawSeq(bool trackLabels, bool topLabels, bool loopPoints, bool menus, bool trackSelection, bool shadeOutsideLoop, uint16_t start, uint16_t end){
    SequenceRenderSettings settings;
    settings.trackLabels = trackLabels;
    settings.topLabels = topLabels;
    settings.drawLoopPoints = loopPoints;
    // settings.menus = menus;
    settings.trackSelection = trackSelection;
    settings.trackLabels = trackLabels;
    settings.shadeOutsideLoop = shadeOutsideLoop;
    settings.start = start;
    settings.end = end;
    drawSeq(settings);
}

void drawSeq(bool trackLabels, bool topLabels, bool loopPoints, bool menus, bool trackSelection){
  drawSeq(trackLabels,topLabels,loopPoints,menus,trackSelection,false,sequence.viewStart,sequence.viewEnd);
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

