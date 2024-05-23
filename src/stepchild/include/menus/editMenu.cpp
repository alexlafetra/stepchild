//sets cursor to the visually nearest note
//steps to pixels = steps*scale
//for a note to be "visually" closer, it needs to have a smaller pixel
//distance from the cursor than another note
//compare trackDistance * trackHeight to stepDistance * scale
float getDistanceFromNoteToCursor(Note note,uint8_t track){
  //if the start of the note is closer than the end
  return sqrt(pow(sequence.activeTrack - track,2)+pow(((abs(note.startPos-sequence.cursorPos)<abs(note.endPos-sequence.cursorPos))?(note.startPos-sequence.cursorPos):(note.endPos-sequence.cursorPos)),2));
}

void setCursorToNearestNote(){
  const float maxPossibleDist = sequence.sequenceLength*sequence.viewScale+sequence.trackData.size()*trackHeight;
  float minDist = maxPossibleDist;
  int minTrack;
  int minNote;
  for(int track = 0; track<sequence.noteData.size(); track++){
    for(int note = 1; note<sequence.noteData[track].size(); note++){
      // //Serial.println("checking n:"+stringify(note)+" t:"+stringify(track));
      // Serial.flush();
      float distance = getDistanceFromNoteToCursor(sequence.noteData[track][note],track);
      if(distance<minDist){
        minTrack = track;
        minNote = note;
        minDist = distance;
        if(minDist == 0)
          break;
      }
    }
    if(minDist == 0)
      break;
  }
  // //Serial.println("setting cursor...");
  // Serial.flush();
  if(minDist != maxPossibleDist){
    setCursor((sequence.noteData[minTrack][minNote].startPos<sequence.cursorPos)?sequence.noteData[minTrack][minNote].startPos:sequence.noteData[minTrack][minNote].endPos-1);
    setActiveTrack(minTrack,false);
  }
}

void fxListControls(uint8_t* currentQuickFunction){
  if(utils.itsbeen(200)){
    if(controls.LOOP() || controls.MENU()){
      activeMenu.page = 0;
      lastTime = millis();
    }
    if(controls.joystickY != 0){
      if(controls.joystickY == 1 && (*currentQuickFunction)<5){
        (*currentQuickFunction)++;
        lastTime = millis();
      }
      else if(controls.joystickY == -1 && (*currentQuickFunction)>0){
        (*currentQuickFunction)--;
        lastTime = millis();
      }
    }
    //selecting an FX
    if(controls.SELECT() ){
      lastTime = millis();
      controls.setSELECT(false);
      fxApplicationFunctions[*currentQuickFunction]();
    }
  }
}

void editMenuControls_editing(uint8_t* currentQuickFunction){
  while(controls.counterB != 0){
    //changing vel
    if(activeMenu.highlight == 2){
      if(controls.counterB<0)
        sequence.changeVel(-1);
      else{
        sequence.changeVel(1);
      }
    }
    //changing chance
    else if(activeMenu.highlight == 3){
      if(controls.counterB<0)
          sequence.changeChance(-1);
      else{
          sequence.changeChance(1);
      }
    }
    controls.counterB += controls.counterB<0?1:-1;;
  }

  //joystick
  if(utils.itsbeen(100)){
    if (controls.joystickY == 1){
      //highlight = 0 ==> moving notes, 1==> changing vel, 2==> changing chance, 3==> changing length
      if(activeMenu.highlight == 0){
        if(sequence.moveNotes(0,1)){
          setActiveTrack(sequence.activeTrack + 1, true);
        }
      }
      //vel
      else if(activeMenu.highlight == 2){
        if(controls.SHIFT())
          sequence.changeVel(-8);
        else
          moveToNextNote_inTrack(true);
      }
      //chance
      else if(activeMenu.highlight == 3){
        if(controls.SHIFT())
            sequence.changeChance(-5);
        else
          moveToNextNote_inTrack(true);
      }
      //quant
      else if(activeMenu.highlight == 4){
        if(controls.SHIFT() && quantizeAmount>0){
          quantizeAmount--;
        }
        else{
          moveToNextNote_inTrack(true);
        }
      }
      //humanize
      else if(activeMenu.highlight == 5){
        moveToNextNote_inTrack(true);
      }
      drawingNote = false;
      lastTime = millis();
    }
    if (controls.joystickY == -1){
      if(activeMenu.highlight == 0){
        if(sequence.moveNotes(0,-1))
          setActiveTrack(sequence.activeTrack - 1, true);
      }
      else if(activeMenu.highlight == 2){
        if(controls.SHIFT())
          sequence.changeVel(8);
        else
          moveToNextNote_inTrack(false);
      }
      else if(activeMenu.highlight == 3){
        if(controls.SHIFT())
            sequence.changeChance(5);
        else
          moveToNextNote_inTrack(false);
      }
      //quant
      else if(activeMenu.highlight == 4){
        if(controls.SHIFT() && quantizeAmount<100){
          quantizeAmount++;
        }
        else{
          moveToNextNote_inTrack(false);
        }
      }
      //humanize
      else if(activeMenu.highlight == 5){
        moveToNextNote_inTrack(false);
      }
      drawingNote = false;
      lastTime = millis();
    }
  }
  if(utils.itsbeen(100)){
    if (controls.joystickX == 1){
      if(!controls.SHIFT()){
        if(activeMenu.highlight == 1){
          //if it's not on a subDiv
          if(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos%sequence.subDivision)
            changeNoteLength_jumpToEnds(-(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos%sequence.subDivision));
          //if it is
          else
            changeNoteLength_jumpToEnds(-sequence.subDivision);
          lastTime = millis();
        }
        else if(activeMenu.highlight == 0){
          //if it's not on a subDiv
          if(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].startPos%sequence.subDivision){
            if(sequence.moveNotes(-(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].startPos%sequence.subDivision),0)){
              moveCursor(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].startPos-sequence.cursorPos);
              lastTime = millis();
            }
          }
          else{
            if(sequence.moveNotes(-sequence.subDivision,0)){
              moveCursor(-sequence.subDivision);
              lastTime = millis();
            }
          }
        }
        else{
          moveToNextNote(false,false);
          lastTime = millis();
        }
      }
      else{
        if(activeMenu.highlight == 0){
          if(sequence.moveNotes(-1,0)){
            moveCursor(-1);
            lastTime = millis();
          }
        }
        else if(activeMenu.highlight == 1){
          changeNoteLength_jumpToEnds(-1);
          lastTime = millis();
        }
        else{
          moveToNextNote(false,false);
          lastTime = millis();
        }
      }
    }
    if (controls.joystickX == -1){
      if(!controls.SHIFT()){  
        //special moves (while editing notes) 
        //if it's not on a subDiv
        if(activeMenu.highlight == 1){
          if(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos%sequence.subDivision){     
            changeNoteLength_jumpToEnds(sequence.subDivision-(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos%sequence.subDivision));
            lastTime = millis();
          }
          else{
            changeNoteLength_jumpToEnds(sequence.subDivision);
            lastTime = millis();
          }
        }
        else if(activeMenu.highlight == 0){
          if(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].startPos%sequence.subDivision){     
            if(sequence.moveNotes(sequence.subDivision-sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].startPos%sequence.subDivision,0)){
              moveCursor(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].startPos-sequence.cursorPos);
              lastTime = millis();
            }
          }
          else{
            if(sequence.moveNotes(sequence.subDivision,0)){
              moveCursor(sequence.subDivision);
              lastTime = millis();
            }
          }
        }
        else{
          moveToNextNote(true,false);
          lastTime = millis();
        }
      }
      else{
        if(activeMenu.highlight == 0){
          if(sequence.moveNotes(1,0)){
            moveCursor(1);
            lastTime = millis();
          }
        }
        else if(activeMenu.highlight == 1){
          changeNoteLength_jumpToEnds(1);
          lastTime = millis();
        }
        else{
          moveToNextNote(true,false);
          lastTime = millis();
        }
      }
    }
  }
  if(utils.itsbeen(200)){
    if(controls.MENU() || controls.B()){
      editingNote = false;
      activeMenu.coords.start.x = trackDisplay-5;
      activeMenu.coords.start.y = 0;
      activeMenu.coords.end.x = screenWidth;
      activeMenu.coords.end.y = headerHeight;
      slideMenuIn(1,48);
      lastTime = millis();
      return;
    }
    if(controls.LOOP()){
      switch(activeMenu.highlight){
        //in move/length/vel/chance mode, "loop" just toggles editing
        case 0:
        case 1:
        case 2:
        case 3:
          activeMenu.coords.start.x = trackDisplay-5;
          activeMenu.coords.start.y = 0;
          activeMenu.coords.end.x = screenWidth;
          activeMenu.coords.end.y = headerHeight;
          slideMenuIn(1,48);
          editingNote = false;
          lastTime = millis();
          return;
        //quantize
        case 4:
          quantize(true,false);
          lastTime = millis();
          break;
        //humanize
        case 5:
          humanize(true);
          lastTime = millis();
          break;
        case 6:
          lastTime = millis();
          controls.setLOOP(false);
          fxApplicationFunctions[*currentQuickFunction]();
          break;
      }
    }
  }
}

void editMenuControls_normal(uint8_t* stencil, bool editing, uint8_t* currentQuickFunction){
  //selectionBox
  //when controls.SELECT()  is pressed and stick is moved, and there's no selection box
  if(controls.SELECT()  && !selBox.begun && (controls.joystickX != 0 || controls.joystickY != 0)){
    selBox.begun = true;
    selBox.coords.start.x = sequence.cursorPos;
    selBox.coords.start.y = sequence.activeTrack;
  }
  //if controls.SELECT()  is released, and there's a selection box
  if(!controls.SELECT()  && selBox.begun){
    selBox.coords.end.x = sequence.cursorPos;
    selBox.coords.end.y = sequence.activeTrack;
    selectBox();
    selBox.begun = false;
  }
  if(!controls.NEW()){
    drawingNote = false;
  }
  //encoderA changes zoom AND +controls.SHIFT() changes the stencil
  while(controls.counterA != 0){
    if(!controls.SHIFT()){
      //changing zoom
      if(controls.counterA >= 1){
          zoom(true);
        }
      if(controls.counterA <= -1){
        zoom(false);
      }
    }
    else{
      if(controls.counterA >= 1 && (*stencil)<16){
        (*stencil)++;
      }
      else if(controls.counterA <= -1 && (*stencil)>1){
        (*stencil)--;
      }
    }
    controls.counterA += controls.counterA<0?1:-1;;
  }
  //encoder B steps through menu options AND increments them by one (unlike joystick) when shifted
  while(controls.counterB != 0){
    if(!controls.SHIFT()){
      if(controls.counterB >= 1 && !controls.SHIFT()){
        changeSubDivInt(true);
      }
      //changing subdivint
      if(controls.counterB <= -1 && !controls.SHIFT()){
        changeSubDivInt(false);
      }
    }
    //if shifting, toggle between 1/3 and 1/4 mode
    else while(controls.counterB != 0 && controls.B()){
      toggleTriplets();
    }
    controls.counterB += controls.counterB<0?1:-1;;
  }
  //joystick
  if(utils.itsbeen(100)){
    if (controls.joystickY == 1){
      if(controls.SHIFT()){
        moveToNextNote_inTrack(true);
        drawingNote = false;
        lastTime = millis();
      }
      else{
        setActiveTrack(sequence.activeTrack+1,false);
        lastTime = millis();
      }
    }
    if (controls.joystickY == -1){
      if(controls.SHIFT()){
        moveToNextNote_inTrack(false);
        drawingNote = false;
        lastTime = millis();
      }
      else{
        setActiveTrack(sequence.activeTrack-1,false);
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(100)){
    if (controls.joystickX == 1){
      if(!controls.SHIFT()){
        moveToNextNote(false,false);
        lastTime = millis();
      }
      //controls.SHIFT()
      else if(activeMenu.highlight>0){
        activeMenu.highlight--;
        lastTime = millis();
      }
    }
    if (controls.joystickX == -1){
      if(!controls.SHIFT()){  
        // moveCursor(sequence.subDivision);
        moveToNextNote(true,false);
        lastTime = millis();
      }
      else if(activeMenu.highlight<6){
        activeMenu.highlight++;
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    //this is the 'move and place' key (might make sense to have it be a diff key)
    if(controls.MENU()){
      slideMenuOut(1,20);
      editingNote = false;
      menuIsActive = false;
      lastTime = millis();
      constructMenu("MENU");
    }
    if(controls.B()){
      slideMenuOut(1,20);
      menuIsActive = false;
      lastTime = millis();
      controls.setB(false);
      constructMenu("MENU");
    }
    if(controls.A()){
      slideMenuOut(1,20);
      lastTime = millis();
      controls.setA(false);
      constructMenu("TRK");
    }
    //moving menu cursor
    if(controls.joystickY != 0 && controls.SHIFT()){
      if(controls.joystickY == 1){
        if(activeMenu.highlight>0){
          activeMenu.highlight--;
          lastTime = millis();
        }
        else if(activeMenu.highlight == 0){
          activeMenu.highlight = 6;
          lastTime = millis();
        }
      }
      else if(controls.joystickY == -1){
        if(activeMenu.highlight<6){
          activeMenu.highlight++;
          lastTime = millis();
        }
        else if(activeMenu.highlight == 6){
          activeMenu.highlight = 0;
          lastTime = millis();
        }
      }
    }
    if(controls.LOOP()){
      if(controls.SHIFT()){
        switch(activeMenu.highlight){
          //quantize
          case 4:
            quantizeMenu();
            break;
          //humanize
          case 5:
            humanizeMenu();
            break;
          //quick fx
          case 6:
            //trigger fx selection
            activeMenu.page = 1;
            lastTime = millis();
            controls.setLOOP(false);
            break;
        }
      }
      else{
        //if it's a quickFX
        if(activeMenu.highlight == 6){
          //trigger fx
          controls.setLOOP(false);
          lastTime = millis();
          activeMenu.page = 1;
        }
        //if there are ANY notes jump into edit mode
        else if(areThereAnyNotes()){
          if(sequence.IDAtCursor() == 0){
            setCursorToNearestNote();
          }
          editingNote = true;
          slideMenuOut(1,20);
          lastTime = millis();
        }
      }
    }
    if(controls.NEW() && !drawingNote && !controls.SELECT() ){
      if(controls.SHIFT()){
        lastTime = millis();
        sequence.stencilNotes(*stencil);
      }
      // if(sequence.IDAtCursor() == 0 || sequence.cursorPos != sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].startPos){
      //   sequence.makeNote(sequence.activeTrack,sequence.cursorPos,sequence.subDivision,true);
      //   drawingNote = true;
      //   lastTime = millis();
      //   moveCursor(sequence.subDivision);
      // }
    }
    if(controls.SELECT()  && !selBox.begun){
      unsigned short int id;
      id = sequence.IDAtCursor();
      //select all
      if(controls.NEW()){
        selectAll();
      }
      //select only one
      else if(controls.SHIFT()){
        clearSelection();
        toggleSelectNote(sequence.activeTrack, id, false);
      }
      //normal select
      else{
        toggleSelectNote(sequence.activeTrack, id, true);          
      }
      lastTime = millis();
    }
    if(controls.DELETE()){
      if(controls.SHIFT()){
          sequence.muteNote(sequence.activeTrack, sequence.IDAtCursor(), true);
        lastTime = millis();
      }
      else{
        if(sequence.selectionCount>0){
            sequence.deleteSelected();
        }
        sequence.deleteNote();
      }
      lastTime = millis();
    }
    if(controls.PLAY()){
      if(controls.SHIFT())
        toggleRecordingMode(waitForNoteBeforeRec);
      else
        togglePlayMode();
      lastTime = millis();
    }
    if(controls.COPY()){
      lastTime = millis();
      if(controls.SHIFT())
        clipboard.paste();
      else
        clipboard.copy();
    }
  }
}

void drawMoveIcon(uint8_t x1, uint8_t y1, bool anim){
  if(anim && (millis()%600) > 300){
    display.drawBitmap(x1+1,y1+1,arrow_small_bmp2,9,9,SSD1306_WHITE);
  }
  else{
    display.drawBitmap(x1,y1,arrow_small_bmp1,11,11,SSD1306_WHITE);
  }
}

void drawLengthIcon(uint8_t x1, uint8_t y1, uint8_t length, uint8_t animThing, bool anim){
  uint8_t offset=0;
  if(anim){
    offset+=(millis()/200)%(animThing);
  }
  //brackets
  printSmall(x1+offset,y1,"(",SSD1306_WHITE);
  printSmall(x1+length-offset,y1,")",SSD1306_WHITE);
  //rect
  display.fillRect(x1+offset+3,y1,length-2*offset-3,5,SSD1306_WHITE);
}

void drawFxIcon(uint8_t x1,uint8_t y1, uint8_t w, bool anim){
  display.drawRect(x1,y1,w,w,SSD1306_WHITE);
  printSmall(x1+2,y1+3,"fx",1);
}

//two blocks
void drawQuantIcon(uint8_t x1, uint8_t y1, uint8_t size, bool anim){
  //dotted
  graphics.drawDottedRect(x1,y1+size/2-1,size/2+1,size/2+1,2);
  //full
  if(anim){
    x1-=(millis()/200)%(size/2);
    y1+=(millis()/200)%(size/2);
  }
  display.fillRect(x1+size/2-1,y1,size/2+2,size/2+2,SSD1306_WHITE);
}

//one square, one rotated square
void drawHumanizeIcon(uint8_t x1, uint8_t y1, uint8_t size, bool anim){
  float angle = 15;
  if(anim){
    angle = (millis()/15)%90;
  }
  graphics.drawDottedRect(x1,y1,size,size,2);
  graphics.drawRotatedRect(x1+size/2,y1+size/2,size-2,size-2,angle,SSD1306_WHITE);
}

void drawVelIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  if(anim){
    display.drawRect(x1,y1,w,w,SSD1306_WHITE);
    graphics.shadeRect(x1+1,y1+1,w-2,w-2,(millis()/200)%5+1);
    display.drawRect(x1+1,y1+1,w-2,w-2,SSD1306_BLACK);
  }
  else{
    display.fillRect(x1,y1,w,w,SSD1306_WHITE);
    display.drawRect(x1+1,y1+1,w-2,w-2,SSD1306_BLACK);
    printSmall(x1+w/2-1,y1+3,"v",2);
  }
}

void drawChanceIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  if(anim){
    display.drawRect(x1,y1,w,w,SSD1306_WHITE);
    graphics.shadeRect(x1+1,y1+1,w-2,w-2,(millis()/200)%5+1);
    display.drawRect(x1+1,y1+1,w-2,w-2,SSD1306_BLACK);
  }
  else{
    display.fillRect(x1,y1,w,w,SSD1306_WHITE);
    display.drawRect(x1+1,y1+1,w-2,w-2,SSD1306_BLACK);
    printSmall(x1+w/2-1,y1+3,"%",2);
  }
}

void drawWarpIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  w--;
  graphics.drawDottedRect(x1,y1,w,w,2);
  if(anim)
    display.fillRect(x1,y1,(millis()/100)%(w)+2,(millis()/100)%(w)+2,SSD1306_WHITE);
  else
    display.fillRect(x1,y1,w/2,w/2,SSD1306_WHITE);
}

//inverting square
void drawReverseIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  display.drawRect(x1,y1,w,w,1);
  display.fillRect(x1+2,y1+2,w-4,w-4,1);
  display.fillRect(x1+4,y1+4,w-8,w-8,0);
  if(anim){
    if(millis()%400>200){
      display.fillRect(x1+1,y1+1,w-2,w-2,2);
    }
  }
}

//concentric circles
void drawEchoIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  // display.drawRect(x1,y1,w,w,1);
  if(!anim){
    display.drawCircle(x1+w/2,y1+w/2,w-6,1);
    // display.drawCircle(x1+w/2,y1+w/2,w-7,1);
    display.drawCircle(x1+w/2,y1+w/2,w-8,1);
    display.drawPixel(x1+w/2,y1+w/2,1);
  }
  else{
    uint8_t r;
    if(millis()%800>600){
      r = w - 6;
      display.drawCircle(x1+w/2,y1+w/2,r,1);
    }
    if(millis()%800>400){
      r = w - 8;
      display.drawCircle(x1+w/2,y1+w/2,r,1);
    }
    if(millis()%800>200){
      r = 0;
      display.drawCircle(x1+w/2,y1+w/2,r,1);
    }
  }
}

//die
void drawRandomIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  display.fillRect(x1,y1,w,w,1);
  display.drawRect(x1+1,y1+1,w-2,w-2,0);
  uint8_t dots = 6;
  if(anim){
    dots = (millis()%1200)/200+1;
  }
  switch(dots){
    case 1:
      display.drawPixel(x1+5,y1+5,0);
      break;
    case 2:
      display.drawPixel(x1+3,y1+5,0);
      display.drawPixel(x1+7,y1+5,0);
      break;
    case 3:
      display.drawPixel(x1+3,y1+3,0);
      display.drawPixel(x1+5,y1+5,0);
      display.drawPixel(x1+7,y1+7,0);
      break;
    case 4:
      display.drawPixel(x1+3,y1+3,0);
      display.drawPixel(x1+3,y1+7,0);
      display.drawPixel(x1+7,y1+3,0);
      display.drawPixel(x1+7,y1+7,0);
      break;
    case 5:
      display.drawPixel(x1+3,y1+3,0);
      display.drawPixel(x1+3,y1+7,0);
      display.drawPixel(x1+7,y1+3,0);
      display.drawPixel(x1+7,y1+7,0);
      display.drawPixel(x1+5,y1+5,0);
      break;
    case 6:
      display.drawPixel(x1+3,y1+3,0);
      display.drawPixel(x1+3,y1+5,0);
      display.drawPixel(x1+3,y1+7,0);

      display.drawPixel(x1+7,y1+3,0);
      display.drawPixel(x1+7,y1+5,0);
      display.drawPixel(x1+7,y1+7,0);
      break;
  }
}

void drawQuickFunctionIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  drawFxIcon(x1,y1,w,anim);
}

void drawQuantBrackets(uint8_t x1, uint8_t y1){
  const uint8_t width = 24;
  const uint8_t height = 16;
  display.drawFastVLine(x1,y1,height,1);
  display.drawFastVLine(x1+1,y1,height,1);
  display.drawFastHLine(x1+2,y1,4,1);
  display.drawFastHLine(x1+2,y1+1,4,1);
  display.drawFastHLine(x1+2,y1+height-1,4,1);
  display.drawFastHLine(x1+2,y1+height,4,1);

  display.drawFastVLine(x1+width,y1,height,1);
  display.drawFastVLine(x1+width-1,y1,height,1);
  display.drawFastHLine(x1+width-5,y1+height,4,1);
  display.drawFastHLine(x1+width-5,y1+1+height,4,1);
}

void editMenu(){
  activeMenu.coords.start.x = trackDisplay-7;
  activeMenu.coords.start.y = 0;
  activeMenu.coords.end.x = screenWidth;
  activeMenu.coords.end.y = headerHeight;
  uint8_t fnWindowStart = 0;
  //value determining how many subDivs are skipped when stencilling notes
  uint8_t stencil = 1;
  bool editing = false;
  uint8_t currentQuickFunction = 0;
  while(menuIsActive){
    display.clearDisplay();
    //draw seq without top info, side info, or menus
    drawSeq(editingNote,false,true,false,false);
    activeMenu.displayEditMenu(&stencil,fnWindowStart,currentQuickFunction);
    display.display();
    controls.readJoystick();
    controls.readButtons();
    if(activeMenu.page != 0)
      fxListControls(&currentQuickFunction);
    else if(editingNote)
      editMenuControls_editing(&currentQuickFunction);
    else
      editMenuControls_normal(&stencil,editing,&currentQuickFunction);
    //sliding the window in and out
    if(activeMenu.page>0 && fnWindowStart<40){
      fnWindowStart+=8;
    }
    else if(activeMenu.page == 0 && fnWindowStart>0){
      fnWindowStart-=8;
    }
  }
}


void Menu::displayEditMenu(){
  uint8_t temp = 1;
  displayEditMenu(&temp,0,0);
}
void Menu::displayEditMenu(uint8_t* stencil,uint8_t windowStart,uint8_t currentQuickFunction){
  unsigned short int menuHeight = abs(coords.end.y-coords.start.y);

  //back rect for stencil icon
  display.drawRoundRect(-2,coords.start.x-32,31,30,3,1);
  //drawing menu box
  display.fillRect(coords.start.x,coords.start.y-2, coords.end.x-coords.start.x+3, coords.end.y-coords.start.y, SSD1306_BLACK);
  display.drawRoundRect(coords.start.x,coords.start.y-2, coords.end.x-coords.start.x+3, coords.end.y-coords.start.y, 3, SSD1306_WHITE);
  
  //ensuring drawSeq draws chance-notes while editing chance
  if(activeMenu.highlight == 3)
    displayingVel = false;
  else
    displayingVel = true;

  if(!editingNote){
    //pencil
    display.drawBitmap(coords.start.x-20,coords.start.y+1+((millis()/200)%2),pen_bmp,15,15,SSD1306_WHITE);
    //stencil, if it's greater than 1
    // if((*stencil)!=1){
    printSmall(coords.start.x-6,coords.start.y+((millis()/200)%2)+12,stringify(*stencil),SSD1306_WHITE);
    // }

    //draw edit icons if the tool bar is onscreen
    if(coords.start.x<screenWidth){
      //move
      drawMoveIcon(coords.start.x+3,1,activeMenu.highlight==0);
      //change length
      drawLengthIcon(coords.start.x+17,4,10,3,activeMenu.highlight == 1);
      //change vel
      drawVelIcon(coords.start.x+33,1,11,activeMenu.highlight == 2);
      //change chance
      drawChanceIcon(coords.start.x+47,1,11,activeMenu.highlight == 3);
      //quantize
      drawQuantIcon(coords.start.x+61,1,11,activeMenu.highlight == 4);
      //humanize
      drawHumanizeIcon(coords.start.x+75,1,10,activeMenu.highlight == 5);
      //warp
      drawQuickFunctionIcon(coords.start.x+90,1,11,activeMenu.highlight == 6);
    }

    //arrow highlight
    String txt;
    uint8_t xLoc = 0;
    if(coords.start.x<screenWidth){
      switch(activeMenu.highlight){
        //moving notes
        case 0:
          txt = "MOVE";
          xLoc = 8;
          break;
        //length
        case 1:
          txt = "LENGTH";
          xLoc = 23;
          break;
        //vel
        case 2:
          txt = "VEL";
          xLoc = 38;
          break;
        //chance
        case 3:
          txt = "CHANCE";
          xLoc = 52;
          break;
        //quantize
        case 4:
          txt = "QUANT";
          xLoc = 66;
          break;
        //humanize
        case 5:
          txt = "HUMAN";
          xLoc = 80;
          break;
        //warp
        case 6:
          txt = "FX";
          xLoc = 95;
          break;
      }
    }
    graphics.drawArrow(coords.start.x+xLoc,13+((millis()/400)%2),controls.SHIFT()?3:4,2,controls.SHIFT());
    //drawing quick function list
    if(windowStart > 0){
      display.fillRect(screenWidth-windowStart,0,windowStart,screenHeight,0);
      display.drawRect(screenWidth-windowStart,-1,windowStart,screenHeight+2,1);
      display.fillRect(screenWidth-windowStart,(currentQuickFunction)*7+1,windowStart,7,1);
      //printing labels
      for(uint8_t i = 0; i<25; i++){
        printSmall(screenWidth-windowStart+2,i*7+2,fxApplicationTexts[i],2);
      }
      display.drawBitmap(screenWidth-windowStart-7,(currentQuickFunction)*7+6,mouse_cursor_fill_bmp,9,15,0);
      display.drawBitmap(screenWidth-windowStart-7,(currentQuickFunction)*7+6,mouse_cursor_outline_bmp,9,15,1);
    }
    //target parameter text (just shows what param you're gonna edit)
    display.fillRect(0,coords.start.x-2,31,screenHeight-coords.start.x,0);
    // display.drawFastHLine(0,coords.start.x-3,32,1);
    display.fillRoundRect(15-txt.length()*2-3,coords.start.x-6,txt.length()*4+5,7,3,SSD1306_WHITE);
    printSmall(15-txt.length()*2,coords.start.x-5,txt,SSD1306_BLACK);

    //draw note info when you're on a note
    if(sequence.IDAtCursor() != 0){
      Note activeNote = sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()];
      //length
      display.fillCircle(6,coords.start.x+9,3,1);
      printSmall(5,coords.start.x+7,"L",2);
      graphics.printFractionCentered(18,coords.start.x+7,stepsToMeasures(activeNote.endPos-activeNote.startPos));
      //vel
      display.fillCircle(6,coords.start.x+20,3,1);
      printSmall(5,coords.start.x+18,"V",2);
      print007SegSmall(11,coords.start.x+17,stringify(activeNote.velocity),1);
      //chance
      display.fillCircle(6,coords.start.x+31,3,1);
      printSmall(5,coords.start.x+29,"%",2);
      print007SegSmall(11,coords.start.x+28,stringify(activeNote.chance),1);
    }
    //if there's no note here...
    else{
      //note count display
      display.fillRoundRect(4,coords.start.x+11,22,7,3,SSD1306_WHITE);
      display.fillRoundRect(4,coords.start.x+26,22,7,3,SSD1306_WHITE);
      uint8_t noteCount = getNoteCount();
      printSmall(9,coords.start.x+5,"seq",SSD1306_WHITE);
      printSmall(15-stringify(noteCount).length()*2,coords.start.x+12,stringify(noteCount),SSD1306_BLACK);
      printSmall(9,coords.start.x+20,"trk",SSD1306_WHITE);
      printSmall(15-stringify(sequence.noteData[sequence.activeTrack].size()-1).length()*2,coords.start.x+27,stringify(sequence.noteData[sequence.activeTrack].size()-1),SSD1306_BLACK);
    }
  }
  //editing icons that appear when the note is being edited
  //"while editing note"
  else{
    String txt;
    switch(activeMenu.highlight){
      //moving notes
      case 0:
        if(millis()%1000 >= 500){
          display.drawBitmap(6,0,arrow_1_bmp,16,16,SSD1306_WHITE);
        }
        else{
          display.drawBitmap(6,0,arrow_3_bmp,16,16,SSD1306_WHITE);
        }
        if(sequence.IDAtCursor() != 0){
          //location points
          printSmall(56,1,stepsToPosition(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].startPos,true)+","+stepsToPosition(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos,true),SSD1306_WHITE);
        }
        txt = "MOVE";
        break;
      //changing length
      case 1:
        drawLengthIcon(4,6,16,6,true);
        if(sequence.IDAtCursor() != 0){
          graphics.printFraction(66,3,stepsToMeasures(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos-sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].startPos));
        }
        txt = "LENGTH";
        break;
      //vel
      case 2:
        if(sequence.IDAtCursor() != 0){
          printSmall(50,1,"v:"+stringify(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].velocity),SSD1306_WHITE);
          fillSquareDiagonally(8,0,15,float(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].velocity*100)/float(127));
          printSmall(14,5,"v",2);
        }
        else{
          fillSquareDiagonally(8,0,15,0);
          printSmall(14,5,"v",2);
        }
        printSmall(98,0,"^/&+[sh]",1);

        txt = "VEL";
        break;
      //chance
      case 3:
        if(sequence.IDAtCursor() != 0){
          printSmall(63,1,stringify(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].chance)+"%",SSD1306_WHITE);
          fillSquareDiagonally(8,0,15,float(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].chance));
          printSmall(14,5,"%",2);
        }
        else{
          fillSquareDiagonally(8,0,15,0);
          printSmall(14,5,"%",2);
        }
        //control tooltip
        printSmall(98,0,"^/&+[sh]",1);
        txt = "CHANCE";
        break;
      //quantize
      case 4:
        //title
        drawBox(13-(millis()/500)%4,(millis()/500)%4,8,8,3,3,4);
        drawBox(6+(millis()/500)%4,5-(millis()/500)%4,8,8,3,3,0);
        txt = "QUANT";

        //amount
        printSmall(70,1,stringify(quantizeAmount)+"%",1);
        graphics.printFraction(60,3,stepsToMeasures(sequence.subDivision));
        printSmall(104,0,"[sh]+L",1);
        // printSmall(84,0,"[menu] back",1);
        break;
      //draw humanize wiggly arc
      case 5:
        txt = "HUMANIZE";
        //data
        display.fillRoundRect(0,18,33,27,3,0);
        display.drawRoundRect(-4,20,38,25,3,1);
        printSmall(sin(millis()/300),23,"tmng:"+stringify(humanizerParameters.timingAmount)+"%",1);
        printSmall(sin(millis()/300+1),30,"vel:"+stringify(humanizerParameters.velocityAmount)+"%",1);
        printSmall(sin(millis()/300+2),37,"chnc:"+stringify(humanizerParameters.chanceAmount)+"%",1);
        for(uint8_t i = 0; i<4; i++){
          if(i<(millis()/200)%4){
            display.drawRect(8+3*i,7-3*i,8,8,1);
          }
        }
        printSmall(104,0,"[sh]+L",1);
        break;
      case 6:
        txt = fxApplicationTexts[currentQuickFunction];
        break;
    }
    //draw moving brackets
    if(sequence.IDAtCursor() != 0)
      graphics.drawNoteBracket(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()],sequence.activeTrack);
    //or draw brackets around the selection
    if(sequence.selectionCount > 0)
      graphics.drawSelectionBracket();

    //drawing edit param info
    display.fillRoundRect(32,0,txt.length()*4+5,7,3,SSD1306_WHITE);
    printSmall(35,1,txt,SSD1306_BLACK);
  }
}
