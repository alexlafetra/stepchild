class NoteEditMenu:public StepchildMenu{
  public:
    int8_t fnWindowStart = 0;
    //value determining how many subDivs are skipped when stencilling notes
    uint8_t stencil = 1;
    bool editingNote = false;
    uint8_t currentQuickFunction = 0;
    uint8_t page = 0;
    NoteEditMenu(){
      coords = CoordinatePair(trackDisplay-7,0,screenWidth,headerHeight);
    }
    void displayMenu();
    bool fxListControls();
    bool editMenuControls_editing();
    bool editMenuControls_normal();
    bool editMenuControls();
};

void NoteEditMenu::displayMenu(){

  //sliding the function window in and out
  if(page>0 && fnWindowStart<38){
    fnWindowStart+=12;
    if(fnWindowStart>38)
      fnWindowStart = 38;
  }
  else if(page == 0 && fnWindowStart>0){
    fnWindowStart-=16;
    if(fnWindowStart<0)
      fnWindowStart = 0;
  }

  //drawing the sequence
  SequenceRenderSettings settings;
  bool doneMoving = (coords.start.x == trackDisplay-7);
  settings.trackLabels = editingNote||!doneMoving;
  settings.topLabels = !doneMoving;
  settings.drawLoopFlags = !doneMoving;
  settings.drawPram = !doneMoving;
  settings.drawCursor = false;
  settings.displayingVel = cursor != 3;//show chance when cursor is on chance edit icon

  display.clearDisplay();
  drawSeq(settings);

  unsigned short int menuHeight = abs(coords.end.y-coords.start.y);

  if(!editingNote){
    //pencil
    if(controls.SHIFT()){
      display.fillRoundRect(-2,coords.start.x-32,31,30,3,1);
      display.drawBitmap(coords.start.x-20,coords.start.y+1+((millis()/200)%2),pen_bmp,15,15,0);
      printSmall(coords.start.x-6,coords.start.y+((millis()/200)%2)+12,stringify(stencil),0);
    }
    else{
      //back rect for stencil icon
      display.fillRoundRect(-2,coords.start.x-32,31,30,3,0);
      display.drawRoundRect(-2,coords.start.x-32,31,30,3,1);
      display.drawBitmap(coords.start.x-20,coords.start.y+1+((millis()/200)%2),pen_bmp,15,15,SSD1306_WHITE);
      printSmall(coords.start.x-6,coords.start.y+((millis()/200)%2)+12,stringify(stencil),SSD1306_WHITE);
    }

    //drawing menu box
    display.fillRect(coords.start.x,coords.start.y-2, coords.end.x-coords.start.x+3, coords.end.y-coords.start.y, SSD1306_BLACK);
    display.drawRoundRect(coords.start.x,coords.start.y-2, coords.end.x-coords.start.x+3, coords.end.y-coords.start.y, 3, SSD1306_WHITE);


    //draw edit icons if the tool bar is onscreen
    if(coords.start.x<screenWidth){
      //move
      drawMoveIcon(coords.start.x+3,1,cursor==0);
      //change length
      drawLengthIcon(coords.start.x+17,4,10,3,cursor == 1);
      //change vel
      drawVelIcon(coords.start.x+33,1,11,cursor == 2);
      //change chance
      drawChanceIcon(coords.start.x+47,1,11,cursor == 3);
      //quantize
      drawQuantIcon(coords.start.x+61,1,11,cursor == 4);
      //humanize
      drawHumanizeIcon(coords.start.x+75,1,10,cursor == 5);
      //warp
      drawQuickFunctionIcon(coords.start.x+90,1,11,cursor == 6);
    }

    //arrow highlight
    String txt;
    uint8_t xLoc = 0;
    if(coords.start.x<screenWidth){
      switch(cursor){
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
    if(fnWindowStart > 0){
      display.fillRect(screenWidth-fnWindowStart,0,fnWindowStart,screenHeight,0);
      display.drawRect(screenWidth-fnWindowStart,-1,fnWindowStart,screenHeight+2,1);
      display.fillRect(screenWidth-fnWindowStart,(currentQuickFunction)*7+1,fnWindowStart,7,1);
      //printing labels
      for(uint8_t i = 0; i<25; i++){
        printSmall(screenWidth-fnWindowStart+2,i*7+2,fxApplicationTexts[i],2);
      }
      display.drawBitmap(screenWidth-fnWindowStart-7,(currentQuickFunction)*7+6,mouse_cursor_fill_bmp,9,15,0);
      display.drawBitmap(screenWidth-fnWindowStart-7,(currentQuickFunction)*7+6,mouse_cursor_outline_bmp,9,15,1);
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
    //back rect for icon
    display.fillRoundRect(-2,coords.start.x-32,31,30,3,0);
    display.drawRoundRect(-2,coords.start.x-32,31,30,3,1);
    //drawing menu box
    display.fillRoundRect(coords.start.x,coords.start.y-2, coords.end.x-coords.start.x+3, coords.end.y-coords.start.y, 3, SSD1306_BLACK);
    display.drawRoundRect(coords.start.x,coords.start.y-2, coords.end.x-coords.start.x+3, coords.end.y-coords.start.y, 3, SSD1306_WHITE);

    String txt;
    switch(cursor){
      //moving notes
      case 0:
        if(millis()%1000 >= 500){
          display.drawBitmap(3,1,arrow_1_bmp,16,16,SSD1306_WHITE);
        }
        else{
          display.drawBitmap(3,1,arrow_3_bmp,16,16,SSD1306_WHITE);
        }
        if(sequence.IDAtCursor() != 0){
          //location points
          printSmall(56,3,stepsToPosition(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].startPos,true)+","+stepsToPosition(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos,true),SSD1306_WHITE);
        }
        txt = "MOVE";
        break;
      //changing length
      case 1:
        drawLengthIcon(2,6,16,6,true);
        if(sequence.IDAtCursor() != 0){
          graphics.printFraction(66,3,stepsToMeasures(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos-sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].startPos));
        }
        txt = "LENGTH";
        break;
      //vel
      case 2:
        if(sequence.IDAtCursor() != 0){
          printSmall(53,3,"v:"+stringify(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].velocity),SSD1306_WHITE);
          fillSquareDiagonally(4,2,15,sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].velocity,127);
          printSmall(10,7,"v",2);
        }
        else{
          fillSquareDiagonally(4,2,15,0);
          printSmall(10,7,"v",2);
        }
        printSmall(78,3,"^/&+[sh]",1);

        txt = "VEL";
        break;
      //chance
      case 3:
        if(sequence.IDAtCursor() != 0){
          printSmall(64,3,stringify(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].chance)+"%",SSD1306_WHITE);
          fillSquareDiagonally(4,2,15,sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].chance,100);
          printSmall(10,7,"%",2);
        }
        else{
          fillSquareDiagonally(4,2,15,0);
          printSmall(10,7,"%",2);
        }
        //control tooltip
        printSmall(80,3,"^/&+[sh]",1);
        txt = "CHANCE";
        break;
      //quantize
      case 4:
        //title
        drawBox(9-(millis()/500)%4,1+(millis()/500)%4,8,8,3,3,4);
        drawBox(2+(millis()/500)%4,6-(millis()/500)%4,8,8,3,3,0);
        txt = "QUANT";
        //amount
        printSmall(70,3,stringify(quantizeAmount)+"%",1);
        graphics.printFraction(60,3,stepsToMeasures(sequence.subDivision));
        printSmall(88,3,"[sh]+L",1);
        break;
      //draw humanize wiggly arc
      case 5:
        txt = "HUMANIZE";
        //data
        display.fillRoundRect(-4,20,38,25,3,0);
        display.drawRoundRect(-4,20,38,25,3,1);
        printSmall(sin(millis()/300),23,"tmng:"+stringify(humanizerParameters.timingAmount)+"%",1);
        printSmall(sin(millis()/300+1),30,"vel:"+stringify(humanizerParameters.velocityAmount)+"%",1);
        printSmall(sin(millis()/300+2),37,"chnc:"+stringify(humanizerParameters.chanceAmount)+"%",1);
        for(uint8_t i = 0; i<4; i++){
          if(i<(millis()/200)%4){
            display.drawRect(4+3*i,8-3*i,8,8,1);
          }
        }
        printSmall(78,3,"[sh]+L",1);
        break;
      case 6:
        txt = fxApplicationTexts[currentQuickFunction];
        break;
    }
    //drawing edit param info
    display.fillRoundRect(32,2,txt.length()*4+5,7,3,SSD1306_WHITE);
    printSmall(35,3,txt,SSD1306_BLACK);
  }
  //draw cursor bracket
  if(sequence.IDAtCursor() != 0)
    drawNoteBracket(sequence.noteData[sequence.activeTrack][sequence.lookupTable[sequence.activeTrack][sequence.cursorPos]],sequence.activeTrack,settings);
  //or draw brackets around the selection
  if(sequence.selectionCount > 0)
    drawSelectionBracket(settings);
  display.display();
}

bool NoteEditMenu::fxListControls(){
  if(utils.itsbeen(200)){
    if(controls.MENU()){
      page = 0;
      lastTime = millis();
    }
    if(controls.joystickY != 0){
      if(controls.joystickY == 1 && (currentQuickFunction)<5){
        (currentQuickFunction)++;
        lastTime = millis();
      }
      else if(controls.joystickY == -1 && (currentQuickFunction)>0){
        (currentQuickFunction)--;
        lastTime = millis();
      }
    }
    //selecting an FX
    if(controls.SELECT() || controls.LOOP()){
      lastTime = millis();
      controls.setSELECT(false);
      controls.setLOOP(false);
      fxApplicationFunctions[currentQuickFunction]();
    }
  }
  return true;
}

bool NoteEditMenu::editMenuControls_editing(){
  while(controls.counterB != 0){
    //changing vel
    if(cursor == 2){
      if(controls.counterB<0)
        sequence.changeVel(-1);
      else{
        sequence.changeVel(1);
      }
    }
    //changing chance
    else if(cursor == 3){
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
      if(cursor == 0){
        if(sequence.moveNotes(0,1)){
          setActiveTrack(sequence.activeTrack + 1, true);
        }
      }
      //vel
      else if(cursor == 2){
        if(controls.SHIFT())
          sequence.changeVel(-8);
        else
          moveToNextNote_inTrack(true);
      }
      //chance
      else if(cursor == 3){
        if(controls.SHIFT())
            sequence.changeChance(-5);
        else
          moveToNextNote_inTrack(true);
      }
      //quant
      else if(cursor == 4){
        if(controls.SHIFT() && quantizeAmount>0){
          quantizeAmount--;
        }
        else{
          moveToNextNote_inTrack(true);
        }
      }
      //humanize
      else if(cursor == 5){
        moveToNextNote_inTrack(true);
      }
      drawingNote = false;
      lastTime = millis();
    }
    if (controls.joystickY == -1){
      if(cursor == 0){
        if(sequence.moveNotes(0,-1))
          setActiveTrack(sequence.activeTrack - 1, true);
      }
      else if(cursor == 2){
        if(controls.SHIFT())
          sequence.changeVel(8);
        else
          moveToNextNote_inTrack(false);
      }
      else if(cursor == 3){
        if(controls.SHIFT())
            sequence.changeChance(5);
        else
          moveToNextNote_inTrack(false);
      }
      //quant
      else if(cursor == 4){
        if(controls.SHIFT() && quantizeAmount<100){
          quantizeAmount++;
        }
        else{
          moveToNextNote_inTrack(false);
        }
      }
      //humanize
      else if(cursor == 5){
        moveToNextNote_inTrack(false);
      }
      drawingNote = false;
      lastTime = millis();
    }
  }
  if(utils.itsbeen(100)){
    if (controls.joystickX == 1){
      if(!controls.SHIFT()){
        if(cursor == 1){
          //if it's not on a subDiv
          if(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos%sequence.subDivision)
            changeNoteLength_jumpToEnds(-(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos%sequence.subDivision));
          //if it is
          else
            changeNoteLength_jumpToEnds(-sequence.subDivision);
          lastTime = millis();
        }
        else if(cursor == 0){
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
        if(cursor == 0){
          if(sequence.moveNotes(-1,0)){
            moveCursor(-1);
            lastTime = millis();
          }
        }
        else if(cursor == 1){
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
        if(cursor == 1){
          if(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos%sequence.subDivision){     
            changeNoteLength_jumpToEnds(sequence.subDivision-(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos%sequence.subDivision));
            lastTime = millis();
          }
          else{
            changeNoteLength_jumpToEnds(sequence.subDivision);
            lastTime = millis();
          }
        }
        else if(cursor == 0){
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
        if(cursor == 0){
          if(sequence.moveNotes(1,0)){
            moveCursor(1);
            lastTime = millis();
          }
        }
        else if(cursor == 1){
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
      lastTime = millis();
      return true;
    }
    if(controls.LOOP()){
      switch(cursor){
        //in move/length/vel/chance mode, "loop" just toggles editing
        case 0:
        case 1:
        case 2:
        case 3:
          editingNote = false;
          lastTime = millis();
          return true;
        //quantize
        case 4:
          if(controls.SHIFT())
            quantizeMenu();
          else
            quantize(true,false);
          lastTime = millis();
          break;
        //humanize
        case 5:
          if(controls.SHIFT())
            humanizeMenu();
          else
            humanize(true);
          lastTime = millis();
          break;
        case 6:
          lastTime = millis();
          controls.setLOOP(false);
          fxApplicationFunctions[currentQuickFunction]();
          break;
      }
    }
  }
  return true;
}

bool NoteEditMenu::editMenuControls_normal(){
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
      if(controls.counterA >= 1 && (stencil)<16){
        (stencil)++;
      }
      else if(controls.counterA <= -1 && (stencil)>1){
        (stencil)--;
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
      else if(cursor>0){
        cursor--;
        lastTime = millis();
      }
    }
    if (controls.joystickX == -1){
      if(!controls.SHIFT()){  
        moveToNextNote(true,false);
        lastTime = millis();
      }
      else if(cursor<6){
        cursor++;
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    //this is the 'move and place' key (might make sense to have it be a diff key)
    if(controls.MENU()){
      lastTime = millis();
      return false;
    }
    if(controls.B()){
      controls.setB(false);
      lastTime = millis();
      return false;
    }
    if(controls.A()){
      controls.setA(false);
      lastTime = millis();
      return false;
    }
    //moving menu cursor
    if(controls.joystickY != 0 && controls.SHIFT()){
      if(controls.joystickY == 1){
        if(cursor>0){
          cursor--;
          lastTime = millis();
        }
        else if(cursor == 0){
          cursor = 6;
          lastTime = millis();
        }
      }
      else if(controls.joystickY == -1){
        if(cursor<6){
          cursor++;
          lastTime = millis();
        }
        else if(cursor == 6){
          cursor = 0;
          lastTime = millis();
        }
      }
    }
    if(controls.LOOP()){
      if(controls.SHIFT()){
        switch(cursor){
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
            page = 1;
            lastTime = millis();
            controls.setLOOP(false);
            break;
        }
      }
      else{
        //if it's a quickFX
        if(cursor == 6){
          //trigger fx
          controls.setLOOP(false);
          lastTime = millis();
          page = 1;
        }
        //if there are ANY notes jump into edit mode
        else if(areThereAnyNotes()){
          if(sequence.IDAtCursor() == 0){
            setCursorToNearestNote();
          }
          editingNote = true;
          lastTime = millis();
        }
      }
    }
    if(controls.NEW() && !drawingNote && !controls.SELECT() ){
      if(controls.SHIFT()){
        lastTime = millis();
        sequence.stencilNotes(stencil);
      }
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
  mainSequencerStepButtons();
  return true;
}

bool NoteEditMenu::editMenuControls(){
  controls.readJoystick();
  controls.readButtons();
  if(page != 0)
    return fxListControls();
  else if(editingNote)
    return editMenuControls_editing();
  else
    return editMenuControls_normal();
}

void editMenu(){
  NoteEditMenu noteEditMenu;
  noteEditMenu.slideIn(IN_FROM_RIGHT,48);
  while(noteEditMenu.editMenuControls()){
    //draw seq without top info, side info, or menus
    noteEditMenu.displayMenu();
  }
  noteEditMenu.slideOut(OUT_FROM_RIGHT,48);
}
