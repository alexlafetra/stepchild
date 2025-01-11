bool echoMenuControls(uint8_t* cursor);
void drawEchoMenu(uint8_t cursor);

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

bool echoMenu(){
  animOffset = 0;
  uint8_t cursor = 0;
  // echoAnimation();
  while(true){
    controls.readButtons();
    controls.readJoystick();
    if(!echoMenuControls(&cursor)){
      return false;
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
  Note targetNote = sequence.noteData[track][id];
  for(int i = 0; i<repeats; i++){
    Note echoNote = targetNote;
    echoNote.setSelected(false);
    uint16_t offset = (i+1)*delay;
    echoNote.startPos += offset;
    echoNote.endPos += offset;
    echoNote.velocity *= pow(float(decay)/float(100),i+1);
    if(echoNote.velocity>0)
      sequence.makeNote(echoNote,track);
    else{
      return;
    }
  }
}

void echoSelectedNotes(){
  //if no notes are selected, just return
  if(!sequence.selectionCount){
    return;
  }
  for(uint8_t track = 0; track<sequence.trackData.size(); track++){
    for(uint8_t note = 1; note<sequence.noteData[track].size(); note++){
      if(sequence.noteData[track][note].isSelected()){
        echoNote(track,note);
      }
    }
  }
  clearSelection();
}

void drawEchoMenu(uint8_t cursor){
  uint8_t xCoord = 64;
  uint8_t yCoord = 32;
  display.setTextColor(SSD1306_WHITE);
  display.setFont();
  // spacing  = float(echoData.delay*10)/float(24);
  int spacing = echoData.delay;
  int maxReps = echoData.repeats;
  display.clearDisplay();

  int8_t triOffset_0 = 2.0*sin(float(millis())/400.0);
  int8_t triOffset_1 = 2.0*sin(float(millis())/400.0+1);
  int8_t triOffset_2 = 2.0*sin(float(millis())/400.0+2);

  //time
  const uint8_t xCoord_0 = 0;
  const uint8_t yCoord_0 = 15;
  display.drawBitmap(xCoord_0,yCoord_0-triOffset_0,time_bmp,25,6,SSD1306_WHITE);
  display.drawBitmap(xCoord_0,yCoord_0+24+triOffset_0,time_inverse_bmp,25,6,SSD1306_WHITE);
  display.drawFastHLine(xCoord_0,yCoord_0+25,24,SSD1306_BLACK);
  display.drawFastHLine(xCoord_0,yCoord_0+27,24,SSD1306_BLACK);
  display.drawFastHLine(xCoord_0,yCoord_0+29,24,SSD1306_BLACK);
  String text = stepsToMeasures(echoData.delay);
  graphics.printFraction_small_centered(xCoord_0+12,yCoord_0+12,text);

  //reps
  const uint8_t xCoord_1 = 29;
  const uint8_t yCoord_1 = 20;
  display.drawBitmap(xCoord_1,yCoord_1-triOffset_1,repetitions_bmp,66,11,SSD1306_WHITE);
  display.drawBitmap(xCoord_1,yCoord_1+20+triOffset_1,repetitions_inverse_bmp,66,11,SSD1306_WHITE);
  display.drawFastHLine(xCoord_1,yCoord_1+25,66,SSD1306_BLACK);
  display.drawFastHLine(xCoord_1,yCoord_1+27,66,SSD1306_BLACK);
  display.drawFastHLine(xCoord_1,yCoord_1+29,66,SSD1306_BLACK);
  printSmall(xCoord_1+35-stringify(echoData.repeats).length()*2,yCoord_1+12,stringify(echoData.repeats),SSD1306_WHITE);

  //decay
  const uint8_t xCoord_2 = 98;
  const uint8_t yCoord_2 = 15;
  display.drawBitmap(xCoord_2,yCoord_2-triOffset_2,decay_bmp,30,12,SSD1306_WHITE);
  display.drawBitmap(xCoord_2,yCoord_2+20+triOffset_2,decay_inverse_bmp,30,12,SSD1306_WHITE);
  display.drawFastHLine(xCoord_2,yCoord_2+25,30,SSD1306_BLACK);
  display.drawFastHLine(xCoord_2,yCoord_2+27,30,SSD1306_BLACK);
  display.drawFastHLine(xCoord_2,yCoord_2+29,30,SSD1306_BLACK);
  printSmall(xCoord_2+15-(stringify(echoData.decay).length()+1)*2,yCoord_2+12,stringify(echoData.decay)+"%",SSD1306_WHITE);
  
  switch(cursor){
    case 0:{
        graphics.drawArrow(xCoord_0+12,yCoord_0+21-((millis()/400)%2),3,2,false);
      }
      break;
    case 1:{
        graphics.drawArrow(xCoord_1+34,yCoord_1+19-((millis()/400)%2),3,2,false);
      }
      break;
    case 2:{
        graphics.drawArrow(xCoord_2+14,yCoord_2+19-((millis()/400)%2),3,2,false);
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
    animOffset+=1;
  }
  else
    animOffset+=4;
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
      animFrame+=1;
    }
    else
      animFrame+=1;
    display.display();
  }
}


bool echoMenuControls(uint8_t* cursor){
  if(utils.itsbeen(200)){
    if(controls.joystickX == 1 && (*cursor) > 0){
      (*cursor)--;
      lastTime = millis();
    }
    if(controls.joystickX == -1 && (*cursor) < 2){
      (*cursor)++;
      lastTime = millis();
    }
  }
  if(utils.itsbeen(200)){
    if(controls.MENU()){
      lastTime = millis();
      return false;
    }
    if(controls.NEW()){
      lastTime = millis();
      while(true){
        //echo selected notes
        if(selectNotes("echo",[](uint8_t a, uint8_t b, uint8_t c, bool d){graphics.drawEchoIcon(a,b,c,d);})){//lambda function!
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
  while(controls.counterA != 0){//if there's data for this option
    if(controls.counterA >= 1){
      if((*cursor) == 0 && echoData.delay<96){
        if(controls.SHIFT())
          echoData.delay++;
        else
          echoData.delay*=2;
        if(echoData.delay>96)
          echoData.delay = 96;
      }
      else if((*cursor) == 1)
        echoData.repeats++;
      else if((*cursor) == 2){
        if(controls.SHIFT() && echoData.decay<100)
          echoData.decay++;
        else if(echoData.decay<=90)
          echoData.decay+=10;
      }
    }
    else if(controls.counterA <= -1){
      if((*cursor) == 0){
        if(controls.SHIFT() && echoData.delay>0)
          echoData.delay--;
        else if(echoData.delay>=2)
          echoData.delay/=2;
      }
      else if((*cursor) == 1 &&  echoData.repeats > 1)
        echoData.repeats--;
      else if((*cursor) == 2){
        if(controls.SHIFT() && echoData.decay>2)
          echoData.decay--;
        else if(echoData.decay>=11)
          echoData.decay-=10;
      }
    }
    controls.counterA += controls.counterA<0?1:-1;;
  }
  while(controls.counterB != 0){
    if(controls.counterB <= -1 && (*cursor)>0){
      (*cursor)--;
    }
    else if(controls.counterB >= 1 && (*cursor)<2){
      (*cursor)++;
    }
    controls.counterB += controls.counterB<0?1:-1;;
  }
  return true;
}
