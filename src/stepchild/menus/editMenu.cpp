void fxListControls(uint8_t* currentQuickFunction){
  if(itsbeen(200)){
    if(loop_Press || menu_Press){
      activeMenu.page = 0;
      lastTime = millis();
    }
    if(y != 0){
      if(y == 1 && (*currentQuickFunction)<5){
        (*currentQuickFunction)++;
        lastTime = millis();
      }
      else if(y == -1 && (*currentQuickFunction)>0){
        (*currentQuickFunction)--;
        lastTime = millis();
      }
    }
    //selecting an FX
    if(sel){
      lastTime = millis();
      sel = false;
      fxApplicationFunctions[*currentQuickFunction]();
    }
  }
}

void editMenuControls_editing(uint8_t* currentQuickFunction){
  while(counterB != 0){
    //changing vel
    if(activeMenu.highlight == 2){
      if(counterB<0)
        changeVel(-1);
      else{
        changeVel(1);
      }
    }
    //changing chance
    else if(activeMenu.highlight == 3){
      if(counterB<0)
        changeChance(-1);
      else{
        changeChance(1);
      }
    }
    counterB += counterB<0?1:-1;;
  }

  //joystick
  if(itsbeen(100)){
    if (y == 1){
      //highlight = 0 ==> moving notes, 1==> changing vel, 2==> changing chance, 3==> changing length
      if(activeMenu.highlight == 0){
        if(moveNotes(0,1)){
          setActiveTrack(activeTrack + 1, true);
        }
      }
      //vel
      else if(activeMenu.highlight == 2){
        if(shift)
          changeVel(-8);
        else
          moveToNextNote_inTrack(true);
      }
      //chance
      else if(activeMenu.highlight == 3){
        if(shift)
          changeChance(-5);
        else
          moveToNextNote_inTrack(true);
      }
      //quant
      else if(activeMenu.highlight == 4){
        if(shift && quantizeAmount>0){
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
    if (y == -1){
      if(activeMenu.highlight == 0){
        if(moveNotes(0,-1))
          setActiveTrack(activeTrack - 1, true);
      }
      else if(activeMenu.highlight == 2){
        if(shift)
          changeVel(8);
        else
          moveToNextNote_inTrack(false);
      }
      else if(activeMenu.highlight == 3){
        if(shift)
          changeChance(5);
        else
          moveToNextNote_inTrack(false);
      }
      //quant
      else if(activeMenu.highlight == 4){
        if(shift && quantizeAmount<100){
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
  if(itsbeen(100)){
    if (x == 1){
      if(!shift){
        if(activeMenu.highlight == 1){
          //if it's not on a subDiv
          if(seqData[activeTrack][lookupData[activeTrack][cursorPos]].endPos%subDivInt)
            changeNoteLength_jumpToEnds(-(seqData[activeTrack][lookupData[activeTrack][cursorPos]].endPos%subDivInt));
          //if it is
          else
            changeNoteLength_jumpToEnds(-subDivInt);
          lastTime = millis();
        }
        else if(activeMenu.highlight == 0){
          //if it's not on a subDiv
          if(seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos%subDivInt){
            if(moveNotes(-(seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos%subDivInt),0)){
              moveCursor(seqData[activeTrack][seqData[activeTrack].size()-1].startPos-cursorPos);
              lastTime = millis();
            }
          }
          else{
            if(moveNotes(-subDivInt,0)){
              moveCursor(-subDivInt);
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
          if(moveNotes(-1,0)){
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
    if (x == -1){
      if(!shift){  
        //special moves (while editing notes) 
        //if it's not on a subDiv
        if(activeMenu.highlight == 1){
          if(seqData[activeTrack][lookupData[activeTrack][cursorPos]].endPos%subDivInt){     
            changeNoteLength_jumpToEnds(subDivInt-(seqData[activeTrack][lookupData[activeTrack][cursorPos]].endPos%subDivInt));
            lastTime = millis();
          }
          else{
            changeNoteLength_jumpToEnds(subDivInt);
            lastTime = millis();
          }
        }
        else if(activeMenu.highlight == 0){
          if(seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos%subDivInt){     
            if(moveNotes(subDivInt-seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos%subDivInt,0)){
              moveCursor(seqData[activeTrack][seqData[activeTrack].size()-1].startPos-cursorPos);
              lastTime = millis();
            }
          }
          else{
            if(moveNotes(subDivInt,0)){
              moveCursor(subDivInt);
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
          if(moveNotes(1,0)){
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
  if(itsbeen(200)){
    if(menu_Press || note_Press){
      editingNote = false;
      activeMenu.coords.x1 = trackDisplay-5;
      activeMenu.coords.y1 = 0;
      activeMenu.coords.x2 = screenWidth;
      activeMenu.coords.y2 = debugHeight;
      slideMenuIn(1,48);
      lastTime = millis();
      return;
    }
    if(loop_Press){
      switch(activeMenu.highlight){
        //in move/length/vel/chance mode, "loop" just toggles editing
        case 0:
        case 1:
        case 2:
        case 3:
          activeMenu.coords.x1 = trackDisplay-5;
          activeMenu.coords.y1 = 0;
          activeMenu.coords.x2 = screenWidth;
          activeMenu.coords.y2 = debugHeight;
          slideMenuIn(1,48);
          editingNote = false;
          lastTime = millis();
          return;
        //quantize
        case 4:
          quantize(true);
          lastTime = millis();
          break;
        //humanize
        case 5:
          humanize(true);
          lastTime = millis();
          break;
        case 6:
          lastTime = millis();
          loop_Press = false;
          fxApplicationFunctions[*currentQuickFunction]();
          break;
      }
    }
  }
}

void editMenuControls_normal(uint8_t* stencil, bool editing, uint8_t* currentQuickFunction){
  //selectionBox
  //when sel is pressed and stick is moved, and there's no selection box
  if(sel && !selBox.begun && (x != 0 || y != 0)){
    selBox.begun = true;
    selBox.x1 = cursorPos;
    selBox.y1 = activeTrack;
  }
  //if sel is released, and there's a selection box
  if(!sel && selBox.begun){
    selBox.x2 = cursorPos;
    selBox.y2 = activeTrack;
    selectBox();
    selBox.begun = false;
  }
  if(!n){
    drawingNote = false;
  }
  //encoderA changes zoom AND +shift changes the stencil
  while(counterA != 0){
    if(!shift){
      //changing zoom
      if(counterA >= 1){
          zoom(true);
        }
      if(counterA <= -1){
        zoom(false);
      }
    }
    else{
      if(counterA >= 1 && (*stencil)<16){
        (*stencil)++;
      }
      else if(counterA <= -1 && (*stencil)>1){
        (*stencil)--;
      }
    }
    counterA += counterA<0?1:-1;;
  }
  //encoder B steps through menu options AND increments them by one (unlike joystick) when shifted
  while(counterB != 0){
    if(!shift){
      if(counterB >= 1 && !shift){
        changeSubDivInt(true);
      }
      //changing subdivint
      if(counterB <= -1 && !shift){
        changeSubDivInt(false);
      }
    }
    //if shifting, toggle between 1/3 and 1/4 mode
    else while(counterB != 0 && note_Press){
      toggleTriplets();
    }
    counterB += counterB<0?1:-1;;
  }
  //joystick
  if(itsbeen(100)){
    if (y == 1){
      if(shift){
        moveToNextNote_inTrack(true);
        drawingNote = false;
        lastTime = millis();
      }
      else{
        setActiveTrack(activeTrack+1,false);
        lastTime = millis();
      }
    }
    if (y == -1){
      if(shift){
        moveToNextNote_inTrack(false);
        drawingNote = false;
        lastTime = millis();
      }
      else{
        setActiveTrack(activeTrack-1,false);
        lastTime = millis();
      }
    }
  }
  if(itsbeen(100)){
    if (x == 1){
      if(!shift){
        moveToNextNote(false,false);
        lastTime = millis();
      }
      //shift
      else if(activeMenu.highlight>0){
        activeMenu.highlight--;
        lastTime = millis();
      }
    }
    if (x == -1){
      if(!shift){  
        // moveCursor(subDivInt);
        moveToNextNote(true,false);
        lastTime = millis();
      }
      else if(activeMenu.highlight<6){
        activeMenu.highlight++;
        lastTime = millis();
      }
    }
  }
  if(itsbeen(200)){
    //this is the 'move and place' key (might make sense to have it be a diff key)
    if(menu_Press){
      slideMenuOut(1,20);
      editingNote = false;
      menuIsActive = false;
      lastTime = millis();
      constructMenu("MENU");
    }
    if(note_Press){
      slideMenuOut(1,20);
      menuIsActive = false;
      lastTime = millis();
      note_Press = false;
      constructMenu("MENU");
    }
    if(track_Press){
      slideMenuOut(1,20);
      lastTime = millis();
      track_Press = false;
      constructMenu("TRK");
    }
    //moving menu cursor
    if(y != 0 && shift){
      if(y == 1){
        if(activeMenu.highlight>0){
          activeMenu.highlight--;
          lastTime = millis();
        }
        else if(activeMenu.highlight == 0){
          activeMenu.highlight = 6;
          lastTime = millis();
        }
      }
      else if(y == -1){
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
    if(loop_Press){
      if(shift){
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
            loop_Press = false;
            break;
        }
      }
      else{
        //if it's a quickFX
        if(activeMenu.highlight == 6){
          //trigger fx
          loop_Press = false;
          lastTime = millis();
          activeMenu.page = 1;
        }
        //if there are ANY notes jump into edit mode
        else if(areThereAnyNotes()){
          if(lookupData[activeTrack][cursorPos] == 0){
            setCursorToNearestNote();
          }
          editingNote = true;
          slideMenuOut(1,20);
          lastTime = millis();
        }
      }
    }
    if(n && !drawingNote && !sel){
      if(shift){
        lastTime = millis();
        stencilNotes(*stencil);
      }
      // if(lookupData[activeTrack][cursorPos] == 0 || cursorPos != seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos){
      //   makeNote(activeTrack,cursorPos,subDivInt,true);
      //   drawingNote = true;
      //   lastTime = millis();
      //   moveCursor(subDivInt);
      // }
    }
    if(sel && !selBox.begun){
      unsigned short int id;
      id = lookupData[activeTrack][cursorPos];
      //select all
      if(n){
        selectAll();
      }
      //select only one
      else if(shift){
        clearSelection();
        toggleSelectNote(activeTrack, id, false);
      }
      //normal select
      else{
        toggleSelectNote(activeTrack, id, true);          
      }
      lastTime = millis();
    }
    if(del){
      if(shift){
        muteNote(activeTrack, lookupData[activeTrack][cursorPos], true);
        lastTime = millis();
      }
      else{
        if(selectionCount>0){
          deleteSelected();
        }
        deleteNote();
      }
      lastTime = millis();
    }
    if(play){
      if(shift)
        toggleRecordingMode(waitForNote);
      else
        togglePlayMode();
      lastTime = millis();
    }
    if(copy_Press){
      lastTime = millis();
      if(shift)
        paste();
      else
        copy();
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
  drawDottedRect(x1,y1+size/2-1,size/2+1,size/2+1,2);
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
  drawDottedRect(x1,y1,size,size,2);
  drawRotatedRect(x1+size/2,y1+size/2,size-2,size-2,angle,SSD1306_WHITE);
}

void drawVelIcon(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  if(anim){
    display.drawRect(x1,y1,w,w,SSD1306_WHITE);
    shadeRect(x1+1,y1+1,w-2,w-2,(millis()/200)%5+1);
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
    shadeRect(x1+1,y1+1,w-2,w-2,(millis()/200)%5+1);
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
  drawDottedRect(x1,y1,w,w,2);
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
  activeMenu.coords.x1 = trackDisplay-7;
  activeMenu.coords.y1 = 0;
  activeMenu.coords.x2 = screenWidth;
  activeMenu.coords.y2 = debugHeight;
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
    readJoystick();
    readButtons();
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
  unsigned short int menuHeight = abs(coords.y2-coords.y1);

  //back rect for stencil icon
  display.drawRoundRect(-2,coords.x1-32,31,30,3,1);
  //drawing menu box
  display.fillRect(coords.x1,coords.y1-2, coords.x2-coords.x1+3, coords.y2-coords.y1, SSD1306_BLACK);
  display.drawRoundRect(coords.x1,coords.y1-2, coords.x2-coords.x1+3, coords.y2-coords.y1, 3, SSD1306_WHITE);
  
  //ensuring drawSeq draws chance-notes while editing chance
  if(activeMenu.highlight == 3)
    displayingVel = false;
  else
    displayingVel = true;

  if(!editingNote){
    //pencil
    display.drawBitmap(coords.x1-20,coords.y1+1+sin(millis()/100),pen_bmp,15,15,SSD1306_WHITE);
    //stencil, if it's greater than 1
    // if((*stencil)!=1){
    printSmall(coords.x1-6,coords.y1+sin(millis()/100)+12,stringify(*stencil),SSD1306_WHITE);
    // }

    //draw edit icons if the tool bar is onscreen
    if(coords.x1<screenWidth){
      //move
      drawMoveIcon(coords.x1+3,1,activeMenu.highlight==0);
      //change length
      drawLengthIcon(coords.x1+17,4,10,3,activeMenu.highlight == 1);
      //change vel
      drawVelIcon(coords.x1+33,1,11,activeMenu.highlight == 2);
      //change chance
      drawChanceIcon(coords.x1+47,1,11,activeMenu.highlight == 3);
      //quantize
      drawQuantIcon(coords.x1+61,1,11,activeMenu.highlight == 4);
      //humanize
      drawHumanizeIcon(coords.x1+75,1,10,activeMenu.highlight == 5);
      //warp
      drawQuickFunctionIcon(coords.x1+90,1,11,activeMenu.highlight == 6);
    }

    //arrow highlight
    String txt;
    if(coords.x1<screenWidth)
    switch(activeMenu.highlight){
      //moving notes
      case 0:
        txt = "MOVE";
        drawArrow(coords.x1+8,13+sin(millis()/200),4,2,false);
        break;
      //length
      case 1:
        txt = "LENGTH";
        drawArrow(coords.x1+23,13+sin(millis()/200),4,2,false);
        break;
      //vel
      case 2:
        txt = "VEL";
        drawArrow(coords.x1+38,13+sin(millis()/200),4,2,false);
        break;
      //chance
      case 3:
        txt = "CHANCE";
        drawArrow(coords.x1+52,13+sin(millis()/200),4,2,false);
        break;
      //quantize
      case 4:
        txt = "QUANT";
        drawArrow(coords.x1+66,13+sin(millis()/200),4,2,false);
        break;
      //humanize
      case 5:
        txt = "HUMAN";
        drawArrow(coords.x1+80,13+sin(millis()/200),4,2,false);
        break;
      //warp
      case 6:
        txt = fxApplicationTexts[currentQuickFunction];
        drawArrow(coords.x1+95,13+sin(millis()/200),4,2,false);
        break;
    }
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
    display.fillRect(0,coords.x1-2,31,screenHeight-coords.x1,0);
    // display.drawFastHLine(0,coords.x1-3,32,1);
    display.fillRoundRect(15-txt.length()*2-3,coords.x1-6,txt.length()*4+5,7,3,SSD1306_WHITE);
    printSmall(15-txt.length()*2,coords.x1-5,txt,SSD1306_BLACK);

    //draw note info when you're on a note
    if(lookupData[activeTrack][cursorPos] != 0){
      Note activeNote = seqData[activeTrack][lookupData[activeTrack][cursorPos]];
      //length
      display.fillCircle(6,coords.x1+9,3,1);
      printSmall(5,coords.x1+7,"L",2);
      printFractionCentered(18,coords.x1+7,stepsToMeasures(activeNote.endPos-activeNote.startPos));
      //vel
      display.fillCircle(6,coords.x1+20,3,1);
      printSmall(5,coords.x1+18,"V",2);
      // display.setCursor(sin(millis()/50+2)+11,coords.x1+18);
      // display.print(stringify(activeNote.velocity));
      print007SegSmall(11,coords.x1+17,stringify(activeNote.velocity),1);
      //chance
      display.fillCircle(6,coords.x1+31,3,1);
      printSmall(5,coords.x1+29,"%",2);
      // display.setCursor(sin(millis()/50+4)+11,coords.x1+30);
      // display.print(stringify(activeNote.chance));
      print007SegSmall(11,coords.x1+28,stringify(activeNote.chance),1);
            //indicator arrows
    }
    //if there's no note here...
    else{
      //note count display
      display.fillRoundRect(4,coords.x1+11,22,7,3,SSD1306_WHITE);
      display.fillRoundRect(4,coords.x1+26,22,7,3,SSD1306_WHITE);
      uint8_t noteCount = getNoteCount();
      printSmall(9,coords.x1+5,"seq",SSD1306_WHITE);
      printSmall(15-stringify(noteCount).length()*2,coords.x1+12,stringify(noteCount),SSD1306_BLACK);
      printSmall(9,coords.x1+20,"trk",SSD1306_WHITE);
      printSmall(15-stringify(seqData[activeTrack].size()-1).length()*2,coords.x1+27,stringify(seqData[activeTrack].size()-1),SSD1306_BLACK);
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
        if(lookupData[activeTrack][cursorPos] != 0){
          //location points
          printSmall(56,1,stepsToPosition(seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos,true)+","+stepsToPosition(seqData[activeTrack][lookupData[activeTrack][cursorPos]].endPos,true),SSD1306_WHITE);
        }
        txt = "MOVE";
        break;
      //changing length
      case 1:
        drawLengthIcon(4,6,16,6,true);
        if(lookupData[activeTrack][cursorPos] != 0){
          printFraction(66,3,stepsToMeasures(seqData[activeTrack][lookupData[activeTrack][cursorPos]].endPos-seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos));
        }
        txt = "LENGTH";
        break;
      //vel
      case 2:
        if(lookupData[activeTrack][cursorPos] != 0){
          printSmall(50,1,"v:"+stringify(seqData[activeTrack][lookupData[activeTrack][cursorPos]].velocity),SSD1306_WHITE);
          fillSquareDiagonally(8,0,15,float(seqData[activeTrack][lookupData[activeTrack][cursorPos]].velocity*100)/float(127));
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
        if(lookupData[activeTrack][cursorPos] != 0){
          printSmall(63,1,stringify(seqData[activeTrack][lookupData[activeTrack][cursorPos]].chance)+"%",SSD1306_WHITE);
          fillSquareDiagonally(8,0,15,float(seqData[activeTrack][lookupData[activeTrack][cursorPos]].chance));
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
        printFraction(60,3,stepsToMeasures(subDivInt));
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
    if(lookupData[activeTrack][cursorPos] != 0)
      drawNoteBracket(seqData[activeTrack][lookupData[activeTrack][cursorPos]],activeTrack);
    //or draw brackets around the selection
    if(selectionCount > 0)
      drawSelectionBracket();

    //drawing edit param info
    display.fillRoundRect(32,0,txt.length()*4+5,7,3,SSD1306_WHITE);
    printSmall(35,1,txt,SSD1306_BLACK);
  }
}
