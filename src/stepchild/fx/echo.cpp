void drawEcho(unsigned short int xStart, unsigned short int yStart, short unsigned int time, short unsigned int decay, short unsigned int repeats){
  unsigned short int previewLength = 12;
  // drawNote(xStart,yStart,previewLength,trackHeight,1,false,false);//pilot note
  float vel = 127;
  for(int rep = 1; rep<repeats+1; rep++){
    vel = vel * decay/100;
    short unsigned int x1 = xStart+time*rep;
    short unsigned int y1 = yStart;
    // drawNote_vel(x1,y1,previewLength,trackHeight,vel,false,false);
  }
}

void echoMenu(){
  animOffset = 0;
  uint8_t cursor = 0;
  // echoAnimation();
  while(true){
    readButtons();
    readJoystick();
    if(!echoMenuControls(&cursor)){
      return;
    }
    display.clearDisplay();
    drawEchoMenu(cursor);
    display.display();
  }
}

//we should also have a reverse echo, because why not
void echoNote(int track, int id){
  int delay = echoData.delay;
  int decay = echoData.decay;
  int repeats = echoData.repeats;
  Note targetNote = seqData[track][id];
  for(int i = 0; i<repeats; i++){
    Note echoNote = targetNote;
    echoNote.isSelected = false;
    uint16_t offset = (i+1)*delay;
    echoNote.startPos += offset;
    echoNote.endPos += offset;
    echoNote.velocity *= pow(float(decay)/float(100),i+1);
    if(echoNote.velocity>0)
      makeNote(echoNote,track);
    else{
      return;
    }
  }
}

void echoSelectedNotes(){
  //if no notes are selected, just return
  if(!selectionCount){
    return;
  }
  for(uint8_t track = 0; track<trackData.size(); track++){
    for(uint8_t note = 1; note<seqData[track].size(); note++){
      if(seqData[track][note].isSelected){
        echoNote(track,note);
      }
    }
  }
  clearSelection();
}

void drawEchoMenu(uint8_t cursor){
  int xCoord = 64;
  int yCoord = 32;
  display.setTextColor(SSD1306_WHITE);
  display.setFont();
  // spacing  = float(echoData.delay*10)/float(24);
  int spacing = echoData.delay;
  int maxReps = echoData.repeats;
  display.clearDisplay();

  int8_t triOffset = 2*((millis()/400)%2);
  switch(cursor){
    case 0:{
      display.drawBitmap(52,17-triOffset,time_bmp,25,6,SSD1306_WHITE);
      display.drawBitmap(52,41+triOffset,time_inverse_bmp,25,6,SSD1306_WHITE);
      display.drawFastHLine(52,42,24,SSD1306_BLACK);
      display.drawFastHLine(52,44,24,SSD1306_BLACK);
      display.drawFastHLine(52,46,24,SSD1306_BLACK);

      //time
      String text = stepsToMeasures(echoData.delay);
      graphics.printFraction_small_centered(64,29,text);

      //arrows
      graphics.drawArrow(78-((millis()/400)%2),31,3,0,false);
      }
      break;
    case 1:{
      display.drawBitmap(29,17-triOffset,repetitions_bmp,66,11,SSD1306_WHITE);
      display.drawBitmap(29,41+triOffset,repetitions_inverse_bmp,66,11,SSD1306_WHITE);
      display.drawFastHLine(29,42,66,SSD1306_BLACK);
      display.drawFastHLine(29,44,66,SSD1306_BLACK);
      display.drawFastHLine(29,46,66,SSD1306_BLACK);
      printSmall(64-stringify(echoData.repeats).length()*2,29,stringify(echoData.repeats),SSD1306_WHITE);
      
      graphics.drawArrow(78-((millis()/400)%2),31,3,0,false);
      graphics.drawArrow(50+((millis()/400)%2),31,3,1,false);
      }
      break;
    case 2:{
      display.drawBitmap(49,17-triOffset,decay_bmp,30,12,SSD1306_WHITE);
      display.drawBitmap(49,41+triOffset,decay_inverse_bmp,30,12,SSD1306_WHITE);
      display.drawFastHLine(49,42,30,SSD1306_BLACK);
      display.drawFastHLine(49,44,30,SSD1306_BLACK);
      display.drawFastHLine(49,46,30,SSD1306_BLACK);
      printSmall(64-(stringify(echoData.decay).length()+1)*2,29,stringify(echoData.decay)+"%",SSD1306_WHITE);
      
      graphics.drawArrow(50+((millis()/400)%2),31,3,1,false);
      }
      break;
  }

  if(animOffset<=32){
    if(animOffset>8){//drops and reflection
      display.drawCircle(xCoord, animOffset-8, 1+sin(animOffset), SSD1306_WHITE);
      display.drawCircle(xCoord, screenHeight-(animOffset-8), 1+sin(animOffset), SSD1306_WHITE);
    }
    display.drawCircle(xCoord, animOffset, 3+sin(animOffset), SSD1306_WHITE);
    display.drawCircle(xCoord, screenHeight-animOffset, 3+sin(animOffset), SSD1306_WHITE);
  }
  else if(animOffset>yCoord){
    int reps = (animOffset-yCoord)/spacing+1;
    if(reps>maxReps){
      reps = maxReps;
    }
    for(int i = 0; i<reps; i++){
      if(animOffset/3-spacing*i+sin(animOffset)*(i%2)<(screenWidth+16))
        graphics.drawEllipse(xCoord, yCoord, animOffset/3-spacing*i+sin(animOffset)*(i%2), animOffset/8-spacing*i/3,SSD1306_WHITE);
    }
  }
  if(animOffset<yCoord){
    animOffset+=5;
  }
  else
    animOffset+=6;
  if(animOffset>=8*spacing*maxReps/3+8*32+20){
    animOffset = 0;
  }
}

void echoAnimation(){
  int maxReps = 1;//for how many rings
  int spacing = 10;//for the spacing
  int animFrame = 0;
  int xCoord = 64;
  int yCoord = 32;
  display.setTextColor(SSD1306_WHITE);
  display.setFont();
  while(animFrame<8*spacing*maxReps/3+8*32+30){
    spacing = echoData.delay;
    maxReps = 1;
    display.clearDisplay();
    // drawSeq();
    if(animFrame<=32){
      if(animFrame>8){//drops and reflection
        display.drawCircle(xCoord, animFrame-8, 1+sin(animFrame), SSD1306_WHITE);
        display.drawCircle(xCoord, screenHeight-(animFrame-8), 1+sin(animFrame), SSD1306_WHITE);
      }
      display.drawCircle(xCoord, animFrame, 3+sin(animFrame), SSD1306_WHITE);
      display.drawCircle(xCoord, screenHeight-animFrame, 3+sin(animFrame), SSD1306_WHITE);
    }
    else if(animFrame>yCoord){
      int reps = (animFrame-yCoord)/spacing+1;
      if(reps>maxReps){
        reps = maxReps;
      }
      for(int i = 0; i<reps; i++){
        graphics.drawEllipse(xCoord, yCoord, animFrame/3-spacing*i+sin(animFrame)*(i%2), animFrame/8-spacing*i/3, SSD1306_WHITE);
      }
    }
    if(animFrame<yCoord){
      animFrame+=5;
    }
    else
      animFrame+=6;
    display.display();
  }
}


bool echoMenuControls(uint8_t* cursor){
  if(itsbeen(200)){
    if(x == 1 && (*cursor) > 0){
      (*cursor)--;
      lastTime = millis();
    }
    if(x == -1 && (*cursor) < 2){
      (*cursor)++;
      lastTime = millis();
    }
  }
  if(itsbeen(200)){
    if(menu_Press){
      lastTime = millis();
      return false;
    }
    if(n){
      lastTime = millis();
      while(true){
        //echo selected notes
        if(selectNotes("echo",drawEchoIcon)){
          echoSelectedNotes();
          clearSelection();
        }
        //cancel
        else{
          break;
        }
      }
    }
  }
  while(counterA != 0){//if there's data for this option
    if(counterA >= 1){
      if((*cursor) == 0 && echoData.delay<96){
        if(shift)
          echoData.delay++;
        else
          echoData.delay*=2;
        if(echoData.delay>96)
          echoData.delay = 96;
      }
      else if((*cursor) == 1)
        echoData.repeats++;
      else if((*cursor) == 2){
        if(shift && echoData.decay<100)
          echoData.decay++;
        else if(echoData.decay<=90)
          echoData.decay+=10;
      }
    }
    else if(counterA <= -1){
      if((*cursor) == 0){
        if(shift && echoData.delay>0)
          echoData.delay--;
        else if(echoData.delay>=2)
          echoData.delay/=2;
      }
      else if((*cursor) == 1 &&  echoData.repeats > 1)
        echoData.repeats--;
      else if((*cursor) == 2){
        if(shift && echoData.decay>2)
          echoData.decay--;
        else if(echoData.decay>=11)
          echoData.decay-=10;
      }
    }
    counterA += counterA<0?1:-1;;
  }
  while(counterB != 0){
    if(counterB <= -1 && (*cursor)>0){
      (*cursor)--;
    }
    else if(counterB >= 1 && (*cursor)<2){
      (*cursor)++;
    }
    counterB += counterB<0?1:-1;;
  }
  return true;
}
