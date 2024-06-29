struct SequenceRenderSettings{
    uint16_t start;
    uint16_t end;
    uint8_t startHeight;
    uint8_t maxTracksShown = 5;
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

        trackSelection = false;
        shrinkTopDisplay = true;
        startHeight = headerHeight;
        drawLoopFlags = true;
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
void drawSeqBackground(SequenceRenderSettings& settings, uint8_t height){
  //drawing the measure bars
  for (uint16_t step = settings.start; step < settings.end; step++) {
    unsigned short int x1 = trackDisplay+int((step-settings.start)*sequence.viewScale);
    unsigned short int x2 = x1 + (step-settings.start)*sequence.viewScale;

    //shade everything outside the loop
    if(settings.shadeOutsideLoop){
      if(step<sequence.loopData[sequence.activeLoop].start){
        graphics.shadeArea(x1,settings.startHeight,(sequence.loopData[sequence.activeLoop].start-step)*sequence.viewScale,screenHeight-settings.startHeight,3);
        step = sequence.loopData[sequence.activeLoop].start;
        //ok, step shouldn't ever be zero in this case, since that would mean it was LESS than zero to begin
        //with. But, just for thoroughnesses sake, make sure step doesn't overflow when you subtract from it
        if(step != 0){
          step--;
        }
        continue;
      }
      else if(step>sequence.loopData[sequence.activeLoop].end){
        graphics.shadeArea(x1,settings.startHeight,(sequence.viewEnd-sequence.loopData[sequence.activeLoop].end)*sequence.viewScale,screenHeight-settings.startHeight,3);
        break;
      }
    }

    //if the last track is showing
    if(endTrack == sequence.trackData.size()){
      //measure bars
      if (!(step % sequence.subDivision) && (step%96) && (sequence.subDivision*sequence.viewScale)>1) {
        graphics.drawDottedLineV(x1,settings.startHeight,height,2);
      }
      if(!(step%96)){
        graphics.drawDottedLineV2(x1,settings.startHeight,height,6);
      }
    }
    else{
      //measure bars
      if (!(step % sequence.subDivision) && (step%96) && (sequence.subDivision*sequence.viewScale)>1) {
        graphics.drawDottedLineV(x1,settings.startHeight,height,2);
      }
      if(!(step%96)){
        graphics.drawDottedLineV2(x1,settings.startHeight,height,6);
      }
    }

    //drawing loop points/flags
    if(settings.drawLoopPoints){//check
      if(step == sequence.loopData[sequence.activeLoop].start){
        if(settings.drawLoopFlags){
          if(movingLoop == -1 || movingLoop == 2){
            display.fillTriangle(trackDisplay+(step-settings.start)*sequence.viewScale, settings.startHeight-3-sin(millis()/50), trackDisplay+(step-settings.start)*sequence.viewScale, settings.startHeight-7-sin(millis()/50), trackDisplay+(step-settings.start)*sequence.viewScale+4, settings.startHeight-7-sin(millis()/50),SSD1306_WHITE);
            display.drawFastVLine(trackDisplay+(step-settings.start)*sequence.viewScale,settings.startHeight-3,3,SSD1306_WHITE);
          }
          else{
            if(sequence.cursorPos == step){
              display.fillTriangle(trackDisplay+(step-settings.start)*sequence.viewScale, settings.startHeight-3, trackDisplay+(step-settings.start)*sequence.viewScale, settings.startHeight-7, trackDisplay+(step-settings.start)*sequence.viewScale+4, settings.startHeight-7,SSD1306_WHITE);
              display.drawFastVLine(trackDisplay+(step-settings.start)*sequence.viewScale,settings.startHeight-3,3,SSD1306_WHITE);
            }
            else{
              display.fillTriangle(trackDisplay+(step-settings.start)*sequence.viewScale, settings.startHeight-1, trackDisplay+(step-settings.start)*sequence.viewScale, settings.startHeight-5, trackDisplay+(step-settings.start)*sequence.viewScale+4, settings.startHeight-5,SSD1306_WHITE);
            }
          }
        }
        else{
          display.drawPixel(trackDisplay+(sequence.loopData[sequence.activeLoop].start-settings.start)*sequence.viewScale, settings.startHeight-1,1);
        }
        if(!movingLoop || (movingLoop != 1 && (millis()/400)%2)){
          display.drawFastVLine(trackDisplay+(step-settings.start)*sequence.viewScale,settings.startHeight,screenHeight-settings.startHeight-(endTrack == sequence.trackData.size()),SSD1306_WHITE);
          display.drawFastVLine(trackDisplay+(step-settings.start)*sequence.viewScale-1,settings.startHeight,screenHeight-settings.startHeight-(endTrack == sequence.trackData.size()),SSD1306_WHITE);
        }
      }
      if(step == sequence.loopData[sequence.activeLoop].end-1){
        if(settings.drawLoopFlags){
          if(movingLoop == 1 || movingLoop == 2){
            display.drawTriangle(trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale, settings.startHeight-3-sin(millis()/50), trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale-4, settings.startHeight-7-sin(millis()/50), trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale, settings.startHeight-7-sin(millis()/50),SSD1306_WHITE);
            display.drawFastVLine(trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale,settings.startHeight-3,3,SSD1306_WHITE);
          }
          else{
            if(sequence.cursorPos == step+1){
              display.drawTriangle(trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale, settings.startHeight-3, trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale-4, settings.startHeight-7, trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale, settings.startHeight-7,SSD1306_WHITE);
              display.drawFastVLine(trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale,settings.startHeight-3,3,SSD1306_WHITE);
            }
            else{
              display.drawTriangle(trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale, settings.startHeight-1, trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale-4, settings.startHeight-5, trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale, settings.startHeight-5,SSD1306_WHITE);
            }
          }
        }
        else{
          display.drawPixel(trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale, settings.startHeight-1,1);
        }
        if(!movingLoop || (movingLoop != -1 && (millis()/400)%2)){
          display.drawFastVLine(trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale+1,settings.startHeight,screenHeight-settings.startHeight-(endTrack == sequence.trackData.size()),SSD1306_WHITE);
          display.drawFastVLine(trackDisplay+(sequence.loopData[sequence.activeLoop].end-settings.start)*sequence.viewScale+2,settings.startHeight,screenHeight-settings.startHeight-(endTrack == sequence.trackData.size()),SSD1306_WHITE);
        }
      }
      if(movingLoop == 2){
        if(step>sequence.loopData[sequence.activeLoop].start && step<sequence.loopData[sequence.activeLoop].end && step%2){
          display.drawPixel(trackDisplay+(step-settings.start)*sequence.viewScale, settings.startHeight-7-sin(millis()/50),SSD1306_WHITE);
        }
      }
      if(settings.drawLoopFlags && (step == sequence.loopData[sequence.activeLoop].start+(sequence.loopData[sequence.activeLoop].end-sequence.loopData[sequence.activeLoop].start)/2))
        printSmall(trackDisplay+(step-settings.start)*sequence.viewScale-1,settings.startHeight-7,stringify(sequence.activeLoop),SSD1306_WHITE);
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
    uint8_t y1 = (settings.maxTracksShown>5&&!menuIsActive)?8:headerHeight;
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

uint8_t getVelShade(uint8_t vel){
  int8_t shade = 13-(vel/10);
  if(shade<=0)
    return 1;
  else
    return shade;
}

int8_t getChanceShade(uint8_t odds){
  int8_t shade = 11-(odds/10);
  if(shade<=0)
    return 1;
  else
    return shade;
}

NoteCoords getNoteCoords(Note& note, uint8_t track, SequenceRenderSettings& settings){
  NoteCoords nCoords;
  if(note.startPos>settings.start){
    nCoords.length = (note.endPos - note.startPos)*sequence.viewScale;
    nCoords.x1 = trackDisplay+int16_t((note.startPos-settings.start)*sequence.viewScale);
  }
  else{
    nCoords.length = (note.endPos - settings.start)*sequence.viewScale+1;
    nCoords.x1 = trackDisplay-1;
  }
  nCoords.y1 = (track-startTrack) * trackHeight + settings.startHeight;
  nCoords.y2 = nCoords.y1+trackHeight;
  return nCoords;
}

NoteCoords getNoteCoords(Note& note, uint8_t track){
  SequenceRenderSettings settings;
  return getNoteCoords(note,track,settings);
}

void drawNote(Note& note, uint8_t track, NoteCoords noteCoords, SequenceRenderSettings& settings){
  //if the note is currently superpositioned, draw it where it should be, but not if it's out of view
  if(note.isSuperpositioned()){
    noteCoords.offsetY(note.superposition.pitch - sequence.trackData[track].pitch);
  }
  //if it's not actively in superposition, BUT it has one and the cursor is over it, draw a rounded rect behind it
  else if(note.superposition.pitch != 255 && sequence.cursorPos<note.endPos && sequence.cursorPos >= note.startPos && sequence.activeTrack == track){
    display.fillRoundRect(noteCoords.x1+3, noteCoords.y1-2, noteCoords.length-1, trackHeight, 2, SSD1306_BLACK);
    display.drawRoundRect(noteCoords.x1+3, noteCoords.y1-2, noteCoords.length-1, trackHeight, 2, SSD1306_WHITE);
  }
  //if the noteCoords.length is less than 3, don't worry about shading it
  if(noteCoords.length<3){
    display.fillRect(noteCoords.x1, noteCoords.y1+1, noteCoords.length+2, trackHeight-2, SSD1306_WHITE);
  }
  else{
    if(note.isMuted()){
      display.fillRect(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, trackHeight-2, SSD1306_BLACK);
      display.drawRect(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, trackHeight-2, SSD1306_WHITE);
      display.drawLine(noteCoords.x1+1,noteCoords.y1+1, noteCoords.x1+noteCoords.length-1, noteCoords.y1+trackHeight-2,SSD1306_WHITE);
      display.drawLine(noteCoords.x1+1,noteCoords.y1+trackHeight-2,noteCoords.x1+noteCoords.length-1,noteCoords.y1+1,SSD1306_WHITE);
      display.drawFastVLine(noteCoords.x1+noteCoords.length,noteCoords.y1+1,trackHeight-2,SSD1306_BLACK);
    }
    else{
      uint8_t shade = settings.displayingVel?getVelShade(note.velocity):getChanceShade(note.chance);
      if(shade != 1){//so it does this faster
        display.fillRect(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, trackHeight-2, SSD1306_BLACK);//clearing out the note area
        for(uint8_t j = 1; j<trackHeight-2; j++){//shading the note...
          for(uint8_t i = noteCoords.x1+1;i+j%shade<noteCoords.x1+noteCoords.length-1; i+=shade){
            display.drawPixel(i+j%shade,noteCoords.y1+j,SSD1306_WHITE);
          }
        }
        display.drawRect(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, trackHeight-2, SSD1306_WHITE);
      }
      //if it's a solid note, fill it quickly
      else{
        display.fillRect(noteCoords.x1+1, noteCoords.y1+1, noteCoords.length-1, trackHeight-2, SSD1306_WHITE);
      }
      //line at the end, if there's something at the end
      if(sequence.lookupTable[track][note.endPos] != 0)
        display.drawFastVLine(noteCoords.x1+noteCoords.length,noteCoords.y1+1,trackHeight-2,SSD1306_BLACK);
    }
    if(note.isSelected()){
      display.drawRect(noteCoords.x1,noteCoords.y1+1,noteCoords.length,trackHeight-2,SSD1306_BLACK);
      display.drawRect(noteCoords.x1+2,noteCoords.y1+2,noteCoords.length-3,trackHeight-4,SSD1306_WHITE);
      display.drawRect(noteCoords.x1,noteCoords.y1,noteCoords.length+1,trackHeight,SSD1306_WHITE);
      display.drawRect(noteCoords.x1+1,noteCoords.y1+1,noteCoords.length-1,trackHeight-2,SSD1306_BLACK);
    }
  }
}
void drawNote(Note& note, uint8_t track, SequenceRenderSettings& settings){
  drawNote(note, track, getNoteCoords(note,track,settings), settings);
}

//this function is a mess!
void drawSeq(SequenceRenderSettings& settings){

    if(settings.shrinkTopDisplay){
      settings.maxTracksShown = maxTracksShown;
      if(!(settings.maxTracksShown==5)){//this should change so that shrinkTop controls maxTracksShown, not the other way around
        settings.startHeight = 8;
        settings.drawLoopFlags = false;
      }
    }

    trackHeight = (screenHeight-settings.startHeight)/settings.maxTracksShown;//calc track height

    if(sequence.trackData.size()>settings.maxTracksShown){
      endTrack = startTrack + settings.maxTracksShown;
      if(sequence.activeTrack>=endTrack){
        endTrack = sequence.activeTrack+1;
        startTrack = endTrack-settings.maxTracksShown;
      }
      else if(sequence.activeTrack<startTrack){
        startTrack = sequence.activeTrack;
        endTrack = startTrack+settings.maxTracksShown;
      }
    }
    else{
        endTrack = startTrack + sequence.trackData.size();
    }
    //drawing selection box, since it needs to overlay stepSeq data
    if(selBox.begun){
        selBox.render();
    }
    uint8_t height;
    if(endTrack == sequence.trackData.size())
        height = settings.startHeight+trackHeight*settings.maxTracksShown;
    else if(sequence.trackData.size()>settings.maxTracksShown)
        height = settings.startHeight+trackHeight*(settings.maxTracksShown+1);
    else
        height = settings.startHeight+trackHeight*sequence.trackData.size();

    //drawing measure bars, loop points
    drawSeqBackground(settings, height);

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

    //drawing active track highlight
    uint8_t y1 = (sequence.activeTrack-startTrack) * trackHeight + settings.startHeight;
    // display.drawRect(x1, y1, screenWidth, trackHeight, SSD1306_WHITE);
    display.drawFastHLine(trackDisplay,y1,screenWidth-trackDisplay,1);
    display.drawFastHLine(trackDisplay,y1+trackHeight-1,screenWidth-trackDisplay,1);

    //top and bottom bounds
    if(startTrack == 0){
        display.drawFastHLine(trackDisplay,settings.startHeight,screenWidth,SSD1306_WHITE);
    }
    //if the bottom is in view
    if(endTrack == sequence.trackData.size()){
        display.drawFastHLine(trackDisplay,settings.startHeight+trackHeight*settings.maxTracksShown,screenWidth,SSD1306_WHITE);
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
            // if(sequence.trackData[track].isSolo())
            //     graphics.drawNoteBracket(trackDisplay+3,y1-1,screenWidth-trackDisplay-5,trackHeight+2,true);
            //Check to see if you only want to render the region in the active loop, and shade everything else!
            for (uint16_t step = settings.shadeOutsideLoop?sequence.loopData[sequence.activeLoop].start:settings.start; step < (settings.shadeOutsideLoop?sequence.loopData[sequence.activeLoop].end:settings.end); step++) {
                uint16_t id = sequence.lookupTable[track][step];
                //drawing note
                if (id != 0){
                  drawNote(sequence.noteData[track][id],track,settings);
                  step = sequence.noteData[track][id].endPos;//skip to the end of the note
                }
            }
        }
    }
    //all the top icons/tooltips
    if(settings.topLabels){
        drawTopIcons(settings);
    }
    //drawing big or small pram in the corner
    if(settings.drawPram){
        if(settings.maxTracksShown != 5){
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

