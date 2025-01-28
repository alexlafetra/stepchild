void quantizeSelectedNotes(bool deleteNote){
  if(sequence.selectionCount>0){
    for(uint8_t track = 0; track<sequence.noteData.size(); track++){
      for(uint16_t note = 1; note<sequence.noteData[track].size(); note++){
        if(sequence.noteData[track][note].isSelected()){
          //if a note was deld (when quantize fails)
          if(!quantizeNote(track,note,deleteNote)){
            note = 1;
          }
        }
      }
    }
  }
}
void quantize(bool move_the_cursor,bool deleteNote){
  //quantizing selected notes
  if(sequence.selectionCount>0){
    if(!sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].isSelected()){
      quantizeNote(sequence.activeTrack,sequence.IDAtCursor(),deleteNote);
    }
    for(uint8_t track = 0; track<sequence.noteData.size(); track++){
      for(uint16_t note = 1; note<sequence.noteData[track].size(); note++){
        if(sequence.noteData[track][note].isSelected()){
          //if a note was deld (when quantize fails)
          if(!quantizeNote(track,note,deleteNote)){
            note = 1;
          }
        }
      }
    }
  }
  //quantizing the note at the cursor
  else if(sequence.IDAtCursor() != 0){
    uint16_t id = sequence.IDAtCursor();
    quantizeNote(sequence.activeTrack,sequence.IDAtCursor(),move_the_cursor,deleteNote);
  }
}

bool quantizeNote(uint8_t track, uint16_t id, bool deleteNote){
  return quantizeNote(track,id, false, deleteNote);
}

//will attempt to quantize a note, or del it if it can't be quantized
bool quantizeNote(uint8_t track, uint16_t id, bool move, bool deleteNote){
  if(id == 0){
    return false;
  }
  uint32_t d1 = sequence.noteData[track][id].startPos%sequence.subDivision;
  uint32_t d2 = sequence.subDivision-d1;
  uint16_t distance;
  //move to the left
  if(d1<=d2){
    distance = d1*(float(quantizeAmount)/float(100));
    if(!sequence.moveNote(id,track,track,sequence.noteData[track][id].startPos-distance)){
      //if you can't move it, del it
      if(deleteNote)
        sequence.deleteNote_byID(track,id);
      return false;
    }
    else if(move){
      sequence.moveCursor(-distance);
    }
  }
  //move to the right
  else{
    distance = d2*(float(quantizeAmount)/float(100));
    if(!sequence.moveNote(id,track,track,sequence.noteData[track][id].startPos+distance)){
      //if you can't move it, del it
      if(deleteNote)
        sequence.deleteNote_byID(track,id);
      // //Serial.println("controls.DELETE()eting note: "+stringify(id));
      return false;
    }
    else if(move){
      sequence.moveCursor(distance);
    }
  }
  return true;
}

void drawSubDivBackground(){
  for(int16_t i = 0; i<96; i+=sequence.subDivision){
    display.drawFastVLine(float(i)/float(96)*screenWidth,16,32,1);
  }
}
void drawQuantCubes(uint8_t distance){
  if(distance == 1){
    distance++;
  }
  const uint8_t width = 15;
  // uint8_t hOffset = 2*sin(millis()/150);
  const uint8_t hOffset = 2;
  if(distance == 0){
    graphics.drawBox(47+distance/2,hOffset+16-distance/2,width+1,width+1,12,12,4);
    graphics.drawBox(48+distance/2,hOffset+17-distance/2,width,width,11,11,3);
  }
  else{
    //bottomL
    // graphics.drawBox(48-distance/2,hOffset+12+distance/2,width,width,11,11,2);
    graphics.drawBox(48-distance/2,hOffset+16+distance/2,width,width,12,12,7);

    //inverse wirewrame (topR)
    graphics.drawBox(48+distance/2,hOffset+16-distance/2,width,width,12,12,7);

    //inner lines
    // graphics.drawBox(48-distance/2,hOffset+12+distance/2,width,width,12,12,6);

    //top line
    graphics.drawDottedLineDiagonal(48-distance/2,hOffset+16+distance/2,48+distance/2,hOffset+16-distance/2,2);
    //bottom line
    graphics.drawDottedLineDiagonal(48-distance/2+width+14,hOffset+16+distance/2+width+10,48+distance/2+width+13,hOffset+16-distance/2+width+11,2);
  }
}

void drawLittleQuantCubes(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  graphics.drawBox(13-(millis()/500)%4,(millis()/500)%4,8,8,3,3,4);
  graphics.drawBox(6+(millis()/500)%4,5-(millis()/500)%4,8,8,3,3,0);
}

bool quantizeMenuControls(uint8_t &whichParam, bool &deleteNote){
  if(utils.itsbeen(200)){
    if(controls.MENU()){
      lastTime = millis();
      return false;
    }
    if(controls.NEW()){
      lastTime = millis();
      while(true){
        if(selectNotes("quantize",drawLittleQuantCubes)){
          quantizeSelectedNotes(deleteNote);
        }
        else{
          break;
        }
      }
    }
    if(controls.SELECT()){
      switch(whichParam){
        //amount
        case 0:
          break;
        //grid
        case 1:
          break;
        //delete on/off
        case 2:
          deleteNote = !deleteNote;
          lastTime = millis();
          break;
      }
    }
  }
  //changing params
  while(controls.counterA != 0){
    switch(whichParam){
      //amount
      case 0:
        if(controls.counterA >= 1){
          if(quantizeAmount < 100){
            if(controls.SHIFT())
              quantizeAmount++;
            else{
              quantizeAmount+=5;
            }
          }
          if(quantizeAmount>100){
            quantizeAmount = 100;
          }
        }
        if(controls.counterA <= -1){
          if(quantizeAmount>0){
            if(controls.SHIFT())
              quantizeAmount--;
            else{
              quantizeAmount-=5;
            }
          }
          if(quantizeAmount<0){
            quantizeAmount = 0;
          }
        }
        break;
      //changing grid
      case 2:
        if(controls.counterA >= 1 && !controls.SHIFT()){
          sequence.changeSubDivInt(true);
        }
        //changing subdivint
        else if(controls.counterA <= -1 && !controls.SHIFT()){
          sequence.changeSubDivInt(false);
        }
        //if shifting, toggle between 1/3 and 1/4 mode
        else if(controls.SHIFT()){
          sequence.toggleTriplets();
        }
        break;
      //delete collisions
      case 1:
        deleteNote = !deleteNote;
        break;
    }
    controls.counterA += controls.counterA<0?1:-1;;
  }
  if(utils.itsbeen(60)){
    if(controls.joystickY != 0){
      switch(whichParam){
        case 2:
        case 1:
          if(controls.joystickY == 1){
            lastTime = millis();
            whichParam = 0;
          }
          break;
        case 0:
          if(controls.joystickY == -1){
            lastTime = millis();
            whichParam = 1;
          }
          break;

      }
    }
    if(controls.joystickX !=0 ){
      switch(whichParam){
        //editing quantize amount
        case 0:
          if(controls.SHIFT()){
            if(controls.joystickX == 1){
              if(quantizeAmount>=10){
                quantizeAmount-=10;
                lastTime = millis();
              }
              else{
                quantizeAmount = 0;
                lastTime = millis();
              }
            }
            else if(controls.joystickX == -1){
              if(quantizeAmount<=90){
                quantizeAmount+=10;
                lastTime = millis();
              }
              else{
                quantizeAmount = 100;
                lastTime = millis();
              }
            }
          }
          //move to grid editing
          else{
            if(controls.joystickX == -1){
              lastTime = millis();
              whichParam = 2;
            }
          }
          break;
        //changing grid
        case 2:
          if(controls.SHIFT()){
            if(controls.joystickX == -1){
              sequence.changeSubDivInt(true);
              lastTime = millis();
            }
            else if(controls.joystickX == 1){
              sequence.changeSubDivInt(false);
              lastTime = millis();
            }
          }
          //move cursor to collisions
          else{
            if(controls.joystickX == -1){
              lastTime = millis();
              whichParam = 1;
            }
          }
          break;
        case 1:          
          if(controls.SHIFT()){
            lastTime = millis();
            deleteNote = !deleteNote;
          }
          else{
            if(controls.joystickX == 1)
              whichParam = 2;
          }
          break;
      }
    }
  }
  return true;
}

bool quantizeMenu(){
  bool atLeastOnce = false;
  uint8_t whichParam = 0;
  bool deleteNote = true;
  while(true){
    display.clearDisplay();

    //amount
    switch(whichParam){
      //amount
      case 0:
        graphics.drawArrow(96+16+((millis()/400)%2),56,2,1,true);
        display.fillRoundRect(63,52,47,9,3,1);
        break;
      //collisions
      case 1:
        graphics.drawArrow(96,((millis()/400)%2)+18,2,2,true);
        break;
      //grid
      case 2:
        graphics.drawArrow(14+((millis()/400)%2),8,2,0,true);
        display.fillRoundRect(16,4,36,9,3,1);
        drawSubDivBackground();
        break;
    }

    //draw quant amount
    String q = stringify(quantizeAmount);
    while(q.length()<3){
      q = "0"+q;
    }
    print7SegSmall(90,53,q+"%",2);
    printSmall(65,54,"amount",2);
    
    //draw subDiv
    q = stepsToMeasures(sequence.subDivision);
    printSmall(18,6,q,2);
    printSmall(33,6," grid",2);

    //collision options
    graphics.drawLabel(100,1,deleteNote?"delete":"leave",1);
    printSmall_centered(100,8,"collisions",1);

    printSmall(0,54,"[N] to apply",1);

    //graphic
    drawQuantCubes((100-quantizeAmount)/(5));

    display.display();

    controls.readJoystick();
    controls.readButtons();
    if(!quantizeMenuControls(whichParam,deleteNote)){
      break;
    }
  }
  return atLeastOnce;
}
