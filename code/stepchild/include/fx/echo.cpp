bool echoMenuControls(uint8_t* cursor);
void drawEchoMenu(uint8_t cursor);

void drawEchoPreview(unsigned short int midX, unsigned short int yStart){
  SequenceRenderSettings settings;
  const uint8_t noteLength = 24;
  const uint8_t length = ((echoData.delay*echoData.repeats)+noteLength)*sequence.viewScale;
  const uint8_t xStart = 64 - length/2;
  for(uint8_t rep = 0; rep<echoData.repeats+1; rep++){
    uint8_t vel = 127.0 * pow(float(echoData.decay)/100.0,rep);
    uint16_t x1 = xStart+float(echoData.delay)*float(rep);
    NoteCoords nC;
    nC.x1 = x1*sequence.viewScale;
    nC.y1 = yStart;
    nC.y2 = trackHeight;
    nC.length = noteLength*sequence.viewScale;
    drawNoteSprite(nC,getVelShade(vel));
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
    echoNote.velocity *= pow(float(decay)/100.0,i+1);
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
  const uint8_t yCoord_0 = 22;
  display.drawBitmap(xCoord_0,yCoord_0-triOffset_0,time_bmp,25,6,SSD1306_WHITE);
  display.drawBitmap(xCoord_0,yCoord_0+24+triOffset_0,time_inverse_bmp,25,6,SSD1306_WHITE);
  display.drawFastHLine(xCoord_0,yCoord_0+25,24,SSD1306_BLACK);
  display.drawFastHLine(xCoord_0,yCoord_0+27,24,SSD1306_BLACK);
  display.drawFastHLine(xCoord_0,yCoord_0+29,24,SSD1306_BLACK);
  String text = stepsToMeasures(echoData.delay);
  graphics.printFraction_small_centered(xCoord_0+12,yCoord_0+12,text);

  //reps
  const uint8_t xCoord_1 = 29;
  const uint8_t yCoord_1 = 27;
  display.drawBitmap(xCoord_1,yCoord_1-triOffset_1,repetitions_bmp,66,11,SSD1306_WHITE);
  display.drawBitmap(xCoord_1,yCoord_1+20+triOffset_1,repetitions_inverse_bmp,66,11,SSD1306_WHITE);
  display.drawFastHLine(xCoord_1,yCoord_1+25,66,SSD1306_BLACK);
  display.drawFastHLine(xCoord_1,yCoord_1+27,66,SSD1306_BLACK);
  display.drawFastHLine(xCoord_1,yCoord_1+29,66,SSD1306_BLACK);
  printSmall(xCoord_1+35-stringify(echoData.repeats).length()*2,yCoord_1+12,stringify(echoData.repeats),SSD1306_WHITE);

  //decay
  const uint8_t xCoord_2 = 98;
  const uint8_t yCoord_2 = 22;
  display.drawBitmap(xCoord_2,yCoord_2-triOffset_2,decay_bmp,30,12,SSD1306_WHITE);
  display.drawBitmap(xCoord_2,yCoord_2+20+triOffset_2,decay_inverse_bmp,30,12,SSD1306_WHITE);
  display.drawFastHLine(xCoord_2,yCoord_2+25,30,SSD1306_BLACK);
  display.drawFastHLine(xCoord_2,yCoord_2+27,30,SSD1306_BLACK);
  display.drawFastHLine(xCoord_2,yCoord_2+29,30,SSD1306_BLACK);
  printSmall(xCoord_2+15-(stringify(echoData.decay).length()+1)*2,yCoord_2+12,stringify(echoData.decay)+"%",SSD1306_WHITE);
  
  drawEchoPreview(64,8);

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
    uint8_t reps = (animOffset-yCoord)/spacing+1;
    if(reps>maxReps){
      reps = maxReps;
    }
    for(uint8_t i = 0; i<reps; i++){
      if(animOffset/3-spacing*i+sin(animOffset)*(i%2)<(screenWidth+16))
        graphics.drawEllipse(xCoord, yCoord, animOffset/3-spacing*i+sin(animOffset)*(i%2), animOffset/8-spacing*i/3,0, SSD1306_WHITE);

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

vector<Note> echoNote(Note& n){
  vector<Note> newNotes = {};
  for(uint8_t i = echoData.repeats; i > 0; i--){
    Note newNote = n;
    newNote.setSelected(false);
    uint16_t offset = i*echoData.delay;
    if(newNote.endPos + offset > sequence.sequenceLength)
      continue;
    newNote.shift(offset);
    newNote.velocity *= pow(float(echoData.decay)/100.0,i);
    //if the note isn't so quiet it's muted
    if(newNote.velocity)
      newNotes.push_back(newNote);
  }
  return newNotes;
}

vector<NoteTrackPair> getEchoNotes(vector<NoteTrackPair> targetNotes){
  vector<NoteTrackPair> temp = {};
  //add the original notes first
  for(NoteTrackPair n:targetNotes){
    temp.push_back(NoteTrackPair(n.note,n.trackID));
  }
  for(NoteTrackPair n:targetNotes){
    vector<Note> newNotes = echoNote(n.note);
    for(Note newN:newNotes){
      temp.push_back(NoteTrackPair(newN,n.trackID));
    }
  }
  return temp;
}

#define MAX_ECHO_REPEATS 16

bool echo(){
  //set up render settings
  SequenceRenderSettings settings;
  settings.topLabels = false;
  settings.drawPram = false;
  settings.drawLoopFlags = false;
  settings.shrinkTopDisplay = false;

  //list of notes to apply fx to
  vector<NoteID> targetNoteIDs = {};
  vector<NoteTrackPair> targetNotes = {};
  vector<NoteTrackPair> previewNotes = {};
  if(sequence.selectionCount){
    targetNoteIDs = getSelectedNoteIDs();
  }
  else if(sequence.IDAtCursor()){
    targetNoteIDs.push_back(NoteID(sequence.activeTrack,sequence.IDAtCursor()));
  }
  //grab note objects
  for(NoteID n:targetNoteIDs){
    targetNotes.push_back(NoteTrackPair(n.getNote(),n.track));
  }

  //delete the targeted notes
  sequence.deleteNotes_byID(targetNoteIDs);

  bool changed = true;

  //gen
  while(true){
    controls.readInputs();
    //change repeats
    while(controls.counterA){
      if(controls.counterA > 0 && echoData.repeats < MAX_ECHO_REPEATS){
        echoData.repeats++;
        changed = true;
      }
      //changing subdivint
      else if(controls.counterA < 0 && echoData.repeats > 0){
        echoData.repeats--;
        changed = true;
      }
      controls.countDownA();
    }
    //change subdivision
    while(controls.counterB != 0){
      //if shifting, toggle between 1/3 and 1/4 mode
      if(controls.SHIFT()){
        sequence.toggleTriplets();
      }
      else if(controls.counterB >= 1){
        sequence.changeSubDivInt(true);
      }
      //changing subdivint
      else if(controls.counterB <= -1){
        sequence.changeSubDivInt(false);
      }
      controls.countDownB();
    }
    if(utils.itsbeen(100)){
      if(controls.UP() && echoData.decay < 100){
        lastTime = millis();
        changed = true;
        echoData.decay += 5;
      }
      else if(controls.DOWN() && echoData.decay > 0){
        lastTime = millis();
        changed = true;
        echoData.decay -= 5;
      }
    }
    if(utils.itsbeen(200)){
      //changing delay by adding/subbing a subdivision using the joystick
      if(controls.RIGHT()){
        if(echoData.delay < 192-sequence.subDivision){
          echoData.delay+=sequence.subDivision;
        }
        else{
          echoData.delay = 192;
        }
        lastTime = millis();
        changed = true;
      }
      if(controls.LEFT()){
        if(echoData.delay > sequence.subDivision){
          echoData.delay-=sequence.subDivision;
        }
        else{
          echoData.delay = 0;
        }
        lastTime = millis();
        changed = true;
      }
      //cancelling
      if(controls.MENU()){
        lastTime = millis();
        for(NoteTrackPair n:targetNotes){
          sequence.makeNote(n.note,n.trackID,false);
        }
        return false;
      }
      //committing
      if(controls.NEW()){
        lastTime = millis();
        for(NoteTrackPair n:previewNotes){
          sequence.makeNote(n.note,n.trackID,false);
        }
        return true;
      }
    }
    if(changed){
      previewNotes = getEchoNotes(targetNotes);
      display.clearDisplay();
      printSmall(49,1,"$: "+stepsToMeasures(echoData.delay),1);
      graphics.drawButton(32,0,"X/B",1);
  
      graphics.drawButton(81,0,"M",1);
      printSmall(90,1,"to quit",1);
  
      printSmall(41,9,stringify(echoData.decay)+"%",1);
      graphics.drawButton(32,8,"Y",1);

      printSmall(66,9,"#:"+stringify(echoData.repeats),1);
      graphics.drawButton(57,8,"A",1);

      graphics.drawEchoIcon(8,0,12,true);

      drawSeq(settings);
      for(NoteTrackPair n:previewNotes){
        drawNote(n.note,n.trackID,settings);
      }
      display.display();
    }
  }
  return false;
}
