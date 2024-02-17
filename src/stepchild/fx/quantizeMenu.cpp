void quantizeSelectedNotes(bool deleteNote){
  if(selectionCount>0){
    for(uint8_t track = 0; track<seqData.size(); track++){
      for(uint16_t note = 1; note<seqData[track].size(); note++){
        if(seqData[track][note].isSelected){
          //if a note was deleted (when quantize fails)
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
  if(selectionCount>0){
    if(!seqData[activeTrack][lookupData[activeTrack][cursorPos]].isSelected){
      quantizeNote(activeTrack,lookupData[activeTrack][cursorPos],deleteNote);
    }
    for(uint8_t track = 0; track<seqData.size(); track++){
      for(uint16_t note = 1; note<seqData[track].size(); note++){
        if(seqData[track][note].isSelected){
          //if a note was deleted (when quantize fails)
          if(!quantizeNote(track,note,deleteNote)){
            note = 1;
          }
        }
      }
    }
  }
  //quantizing the note at the cursor
  else if(lookupData[activeTrack][cursorPos] != 0){
    uint16_t id = lookupData[activeTrack][cursorPos];
    quantizeNote(activeTrack,lookupData[activeTrack][cursorPos],move_the_cursor,deleteNote);
  }
}

bool quantizeNote(uint8_t track, uint16_t id, bool deleteNote){
  return quantizeNote(track,id, false, deleteNote);
}

//will attempt to quantize a note, or delete it if it can't be quantized
bool quantizeNote(uint8_t track, uint16_t id, bool move, bool deleteNote){
  if(id == 0){
    return false;
  }
  uint32_t d1 = seqData[track][id].startPos%subDivInt;
  uint32_t d2 = subDivInt-d1;
  uint16_t distance;
  //move to the left
  if(d1<=d2){
    distance = d1*(float(quantizeAmount)/float(100));
    if(!moveNote(id,track,track,seqData[track][id].startPos-distance)){
      //if you can't move it, delete it
      if(deleteNote)
        deleteNote_byID(track,id);
      return false;
    }
    else if(move){
      moveCursor(-distance);
    }
  }
  //move to the right
  else{
    distance = d2*(float(quantizeAmount)/float(100));
    if(!moveNote(id,track,track,seqData[track][id].startPos+distance)){
      //if you can't move it, delete it
      if(deleteNote)
        deleteNote_byID(track,id);
      // //Serial.println("deleting note: "+stringify(id));
      return false;
    }
    else if(move){
      moveCursor(distance);
    }
  }
  return true;
}

void drawSubDivBackground(){
  for(int16_t i = 0; i<96; i+=subDivInt){
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
    drawBox(47+distance/2,hOffset+16-distance/2,width+1,width+1,12,12,4);
    drawBox(48+distance/2,hOffset+17-distance/2,width,width,11,11,3);
  }
  else{
    //bottomL
    // drawBox(48-distance/2,hOffset+12+distance/2,width,width,11,11,2);
    drawBox(48-distance/2,hOffset+16+distance/2,width,width,12,12,7);

    //inverse wirewrame (topR)
    drawBox(48+distance/2,hOffset+16-distance/2,width,width,12,12,7);

    //inner lines
    // drawBox(48-distance/2,hOffset+12+distance/2,width,width,12,12,6);

    //top line
    graphics.drawDottedLineDiagonal(48-distance/2,hOffset+16+distance/2,48+distance/2,hOffset+16-distance/2,2);
    //bottom line
    graphics.drawDottedLineDiagonal(48-distance/2+width+14,hOffset+16+distance/2+width+10,48+distance/2+width+13,hOffset+16-distance/2+width+11,2);
  }
}

void drawLittleQuantCubes(uint8_t x1, uint8_t y1, uint8_t w, bool anim){
  drawBox(13-(millis()/500)%4,(millis()/500)%4,8,8,3,3,4);
  drawBox(6+(millis()/500)%4,5-(millis()/500)%4,8,8,3,3,0);
}

bool quantizeMenuControls(uint8_t* whichParam, bool* deleteNote){
  if(itsbeen(200)){
    if(menu_Press){
      lastTime = millis();
      return false;
    }
    if(n){
      lastTime = millis();
      while(true){
        if(selectNotes("quantize",drawLittleQuantCubes)){
          quantizeSelectedNotes(*deleteNote);
        }
        else{
          break;
        }
      }
    }
    if(sel){
      lastTime = millis();
      (*deleteNote) = !(*deleteNote);
    }
  }
  //changing subDivInt
  while(counterB != 0){
    if(counterB >= 1 && !shift){
      changeSubDivInt(true);
    }
    //changing subdivint
    if(counterB <= -1 && !shift){
      changeSubDivInt(false);
    }
    //if shifting, toggle between 1/3 and 1/4 mode
    else while(counterB != 0 && shift){
      toggleTriplets();
    }
    counterB += counterB<0?1:-1;;
    if(*whichParam != 1){
      (*whichParam) = 1;
    }
  }
  //changing quantize amount
  while(counterA != 0){
    if(counterA >= 1){
      if(quantizeAmount < 100){
        if(shift)
          quantizeAmount++;
        else{
          quantizeAmount+=5;
        }
      }
      if(quantizeAmount>100){
        quantizeAmount = 100;
      }
    }
    if(counterA <= -1){
      if(quantizeAmount>0){
        if(shift)
          quantizeAmount--;
        else{
          quantizeAmount-=5;
        }
      }
      if(quantizeAmount<0){
        quantizeAmount = 0;
      }
    }
    if(*whichParam != 0){
      (*whichParam) = 0;
    }
    counterA += counterA<0?1:-1;;
  }
  if(itsbeen(60)){
    if(y != 0){
      if(y == 1 && (*whichParam) == 1){
        *whichParam = 0;
        lastTime = millis();
      }
      else if(y == -1 && (*whichParam) == 0){
        *whichParam = 1;
        lastTime = millis();
      }
    }
    if(x!=0){
      switch(*whichParam){
        //editing quantize amount
        case 0:
          if(shift){
            if(x == 1 && quantizeAmount>0){
              quantizeAmount--;
              lastTime = millis();
            }
            else if(x == -1 && quantizeAmount<100){
              quantizeAmount++;
              lastTime = millis();
            }
          }
          else{
            if(x == 1){
              if(quantizeAmount>=10){
                quantizeAmount-=10;
                lastTime = millis();
              }
              else{
                quantizeAmount = 0;
                lastTime = millis();
              }
            }
            else if(x == -1){
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
          break;
        //changing subDivInt
        case 1:
          if(shift){
            toggleTriplets();
            lastTime = millis();
          }
          else{
            if(x == -1){
              changeSubDivInt(true);
              lastTime = millis();
            }
            else if(x == 1){
              changeSubDivInt(false);
              lastTime = millis();
            }
          }
          break;
      }
    }
  }
  return true;
}

void quantizeMenu(){
  uint8_t whichParam = 0;
  bool deleteNote = true;
  while(true){
    display.clearDisplay();

    //draw quant amount
    String q = stringify(quantizeAmount);
    while(q.length()<3){
      q = "0"+q;
    }
    print7SegSmall(90,53,q+"%",1);
    //draw subDiv
    q = stepsToMeasures(subDivInt);
    printSmall(28,6,q,1);

    //draw swing indicator
    // printSmall(90,6,deleteNote?"delete":"leave",1);
    drawLabel(100,1,deleteNote?"delete":"leave",1);
    printSmall_centered(100,8,"collisions",1);
    printSmall(0,53,"quantize",1);

    if(whichParam == 0){
      drawArrow(96+16+2*((millis()/400)%2),56,2,1,true);
      printSmall(90,46,"amount",1);
    }
    else{
      drawArrow(24+2*((millis()/400)%2),8,2,0,true);
      printSmall(0,6," grid",1);
      //bracket
      drawSubDivBackground();
    }

    //graphic
    drawQuantCubes((100-quantizeAmount)/(5));

    display.display();

    readJoystick();
    readButtons();
    if(!quantizeMenuControls(&whichParam,&deleteNote)){
      break;
    }
  }
}
