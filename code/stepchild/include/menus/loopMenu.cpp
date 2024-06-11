#define NORMAL 0
#define RANDOM 1
#define RANDOM_SAME 2
#define RETURN 3
#define INFINITE 4

void printLoopTitle(uint8_t, uint8_t);
void drawLoopBlocksVertically(int,int,int);
void drawLoopPreview(uint8_t x1, uint8_t y1, uint8_t loop);
void drawLoopInfo(uint8_t x1, uint8_t y1, uint8_t whichLoop);

void moveCursorWithinLoop(int amount, uint8_t whichLoop){
  //if the cursor is going to move before the loop
  if(amount<0 && (sequence.cursorPos + amount)<sequence.loopData[whichLoop].start)
    moveCursor(sequence.cursorPos - sequence.loopData[whichLoop].start);
  //if the cursor is going to move past the loop
  else if(amount>0 && (sequence.cursorPos+amount)>=sequence.loopData[whichLoop].end)
    moveCursor(sequence.loopData[whichLoop].end-sequence.cursorPos);
  //if not, then it's moving within the loop
  else
    moveCursor(amount);
}

bool isWithinLoop(int val, int loop, int amount, bool inclusive){
  if(!inclusive){
    if(val<(sequence.loopData[loop].end-amount) && val>(sequence.loopData[loop].start+amount)){
      return true;
    }
    else{
      return false;
    }
  }
  else{
    if(val<=(sequence.loopData[loop].end-amount) && val>=(sequence.loopData[loop].start+amount)){
      return true;
    }
    else{
      return false;
    }
  }
}

void insertLoop(int afterThis, Loop newLoop){
  vector<Loop> tempData;
  for(int i = 0; i<sequence.loopData.size(); i++){
    tempData.push_back(sequence.loopData[i]);
    if(i == afterThis)
      tempData.push_back(newLoop);
  }
  sequence.loopData.swap(tempData);
}

void dupeLoop(int loop){
  vector<Loop> tempData;
  for(int i = 0; i<sequence.loopData.size(); i++){
    tempData.push_back(sequence.loopData[i]);
    if(i == loop)
      tempData.push_back(sequence.loopData[i]);
  }
  sequence.loopData.swap(tempData);
}

uint16_t getLongestLoop(){
  uint16_t longestLength = 0;
  for(uint8_t i = 0; i<sequence.loopData.size(); i++){
    uint16_t l = sequence.loopData[i].end - sequence.loopData[i].start;
    if(l>longestLength)
      longestLength = l;
  }
  return longestLength;
}

//just your normal editing controls, but you can't move the view past the loop
bool viewLoopControls(uint8_t which){
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
  if(!controls.NEW())
    drawingNote = false;
  mainSequencerEncoders();
  if (utils.itsbeen(100)) {
    if (controls.joystickX == 1 && !controls.SHIFT()) {
      //if cursor isn't on a measure marker, move it to the nearest one
      if(sequence.cursorPos%sequence.subDivision){
        if(!movingLoop)
          moveCursorWithinLoop(-sequence.cursorPos%sequence.subDivision,which);
        else
          moveCursor(-sequence.cursorPos%sequence.subDivision);
        lastTime = millis();
        //moving entire loop
        if(movingLoop == 2)
          moveLoop(-sequence.cursorPos%sequence.subDivision);
      }
      else{
        if(!movingLoop)
          moveCursorWithinLoop(-sequence.subDivision,which);
        else
          moveCursor(-sequence.subDivision);
        lastTime = millis();
        //moving entire loop
        if(movingLoop == 2)
          moveLoop(-sequence.subDivision);
      }
      //moving loop start/end
      if(movingLoop == -1){
        sequence.setLoopPoint(sequence.cursorPos,true);
      }
      else if(movingLoop == 1){
        sequence.setLoopPoint(sequence.cursorPos,false);
      }
    }
    if (controls.joystickX == -1 && !controls.SHIFT()) {
      if(sequence.cursorPos%sequence.subDivision){
        if(!movingLoop)
          moveCursorWithinLoop(sequence.subDivision-sequence.cursorPos%sequence.subDivision,which);
        else
          moveCursor(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
        lastTime = millis();
        if(movingLoop == 2)
          moveLoop(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
      }
      else{
        if(!movingLoop)
          moveCursorWithinLoop(sequence.subDivision,which);
        else
          moveCursor(sequence.subDivision);
        lastTime = millis();
        if(movingLoop == 2)
          moveLoop(sequence.subDivision);
      }
      //moving loop start/end
      if(movingLoop == -1){
        sequence.setLoopPoint(sequence.cursorPos,true);
      }
      else if(movingLoop == 1){
        sequence.setLoopPoint(sequence.cursorPos,false);
      }
    }
  }
  if(utils.itsbeen(100)){
    if (controls.joystickY == 1 && !controls.SHIFT() && !controls.LOOP()) {
      if(recording)//if you're not in normal mode, you don't want it to be loud
        setActiveTrack(sequence.activeTrack + 1, false);
      else
        setActiveTrack(sequence.activeTrack + 1, true);
      drawingNote = false;
      lastTime = millis();
    }
    if (controls.joystickY == -1 && !controls.SHIFT() && !controls.LOOP()) {
      if(recording)//if you're not in normal mode, you don't want it to be loud
        setActiveTrack(sequence.activeTrack - 1, false);
      else
        setActiveTrack(sequence.activeTrack - 1, true);
      drawingNote = false;
      lastTime = millis();
    }
  }
  if (utils.itsbeen(50)) {
    //moving
    if (controls.joystickX == 1 && controls.SHIFT()) {
      if(!movingLoop)
        moveCursorWithinLoop(-1,which);
      else
        moveCursor(-1);
      lastTime = millis();
      if(movingLoop == 2)
        moveLoop(-1);
      else if(movingLoop == -1)
        sequence.setLoopPoint(sequence.cursorPos,true);
      else if(movingLoop == 1)
        sequence.setLoopPoint(sequence.cursorPos,false);
    }
    if (controls.joystickX == -1 && controls.SHIFT()) {
      if(!movingLoop)
        moveCursorWithinLoop(1,which);
      else
        moveCursor(1);
      lastTime = millis();
      if(movingLoop == 2)
        moveLoop(1);
      else if(movingLoop == -1)
        sequence.loopData[sequence.activeLoop].start = sequence.cursorPos;
      else if(movingLoop == 1)
        sequence.loopData[sequence.activeLoop].end = sequence.cursorPos;
    }
    //changing vel
    if (controls.joystickY == 1 && controls.SHIFT()) {
      sequence.changeVel(-10);
      lastTime = millis();
    }
    if (controls.joystickY == -1 && controls.SHIFT()) {
      sequence.changeVel(10);
      lastTime = millis();
    }

    if(sequence.IDAtCursor()==0){
      if(controls.joystickY == 1 && controls.SHIFT()){
        sequence.defaultVel-=10;
        if(sequence.defaultVel<1)
          sequence.defaultVel = 1;
        lastTime = millis();
      }
      if(controls.joystickY == -1 && controls.SHIFT()){
        sequence.defaultVel+=10;
        if(sequence.defaultVel>127)
          sequence.defaultVel = 127;
        lastTime = millis();
      }
    }
  }
  //del happens a liitle faster (so you can draw/erase fast)
  if(utils.itsbeen(75)){
    //del
    if(controls.DELETE() && !controls.SHIFT()){
      if (sequence.selectionCount > 0){
        sequence.deleteSelected();
        lastTime = millis();
      }
      else if(sequence.IDAtCursor() != 0){
        sequence.deleteNote(sequence.activeTrack,sequence.cursorPos);
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    //new
    if(controls.NEW() && !controls.A() && !drawingNote && !controls.SELECT() ){
      if((!controls.SHIFT())&&(sequence.IDAtCursor() == 0 || sequence.cursorPos != sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].startPos)){
        sequence.makeNote(sequence.activeTrack,sequence.cursorPos,sequence.subDivision,true);
        drawingNote = true;
        lastTime = millis();
        moveCursorWithinLoop(sequence.subDivision,which);
      }
      if(controls.SHIFT()){
        addTrack(sequence.defaultPitch, sequence.defaultChannel);
        lastTime = millis();
      }
    }
    //select
    if(controls.SELECT()  && !selBox.begun){
      uint16_t id = sequence.IDAtCursor();
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
    if(controls.DELETE() && controls.SHIFT()){
      sequence.muteNote(sequence.activeTrack, sequence.IDAtCursor(), true);
      lastTime = millis();
    }

    //Modes: play, listen, and record
    if(controls.PLAY() && !controls.SHIFT() && !recording){
      togglePlayMode();
      lastTime = millis();
    }
    //if play+controls.SHIFT(), or if play and it's already recording
    if((controls.PLAY() && controls.SHIFT()) || (controls.PLAY() && recording)){
      toggleRecordingMode(waitForNoteBeforeRec);
      lastTime = millis();
    }
 
    //loop
    if(controls.LOOP()){
      //if you're not moving a loop, start
      if(movingLoop == 0){
        //if you're on the start, move the start
        if(sequence.cursorPos == sequence.loopData[sequence.activeLoop].start){
          movingLoop = -1;
        }
        //if you're on the end
        else if(sequence.cursorPos == sequence.loopData[sequence.activeLoop].end){
          movingLoop = 1;
        }
        //if you're not on either, move the whole loop
        else{
          movingLoop = 2;
        }
        lastTime = millis();
      }
      //if you were moving, stop
      else{
        movingLoop = 0;
        lastTime = millis();
      }
    }
    //menu press
    if(controls.MENU()){
      if(controls.SHIFT()){
        lastTime = millis();
        return false;
      }
      else{
        lastTime = millis();
        controls.setMENU(false) ;
        return false;
      }
    }
    //copy/pasate
    if(controls.COPY()){
      if(controls.SHIFT())
        clipboard.paste();
      else{
        clipboard.copy();
      }
      lastTime = millis();
    }
  }
  return true;
}

void viewLoop(uint8_t which){
  setCursor(sequence.loopData[which].start);
  setActiveLoop(which);
  String tempText;
  while(true){
    controls.readJoystick();
    controls.readButtons();
    if(!viewLoopControls(which))
      return;
    tempText = menuText;
    menuText = "loop-"+stringify(which)+" "+menuText;
    display.clearDisplay();
    // drawSeq(true,true,true,false,false,true,sequence.loopData[which].start,sequence.loopData[which].end*scale);
    drawSeq(true,true,true,false,false,true,sequence.viewStart,sequence.viewEnd);
    // drawSeq(true,true,true,false,false,true,sequence.loopData[which].start>sequence.viewStart?sequence.loopData[which].start:sequence.viewStart,sequence.loopData[which].end<sequence.viewEnd?sequence.loopData[which].end:sequence.viewEnd);
    display.display();
    menuText = tempText;
  }
}
const uint8_t maxLoopsShown = 6;

void loopMenu(){
  menuText = "";
  uint8_t xStart = 32;
  uint8_t targetL = sequence.activeLoop;
  uint8_t dispStart = sequence.activeLoop;
  int blockZoom = getLongestLoop()/30;
  Loop storedLoop;
  storedLoop.start = sequence.loopData[targetL].start;
  storedLoop.end = sequence.loopData[targetL].end;
  storedLoop.reps = sequence.loopData[targetL].reps;
  storedLoop.type = sequence.loopData[targetL].type;
  bool loopStored = false;
  WireFrame w = makeLoopArrows(0);
  w.xPos = 64;
  w.yPos = 32;
  w.scale = 4.0;
  while(true){
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(1000)){
      menuText = "";
    }
    //changing the loop iterations
    while(controls.counterA != 0){
      if(controls.counterA>0){
        sequence.loopData[targetL].reps++;
      }
      else{
        if(sequence.loopData[targetL].reps != 0)
          sequence.loopData[targetL].reps--;
      }
      controls.counterA += controls.counterA<0?1:-1;
    }
    //changing the loop type
    while(controls.counterB != 0){
      if(controls.counterB>0){
        if(sequence.loopData[targetL].type<4)
          sequence.loopData[targetL].type++;
        else
          sequence.loopData[targetL].type = 0;
      }
      else{
        if(sequence.loopData[targetL].type>0)
          sequence.loopData[targetL].type--;
        else
          sequence.loopData[targetL].type = 4;
      }
      controls.counterB += controls.counterB<0?1:-1;
    }
    if(utils.itsbeen(100)){
      if(controls.joystickY != 0){
        if(controls.joystickY == 1){
          if(targetL<sequence.loopData.size()-1){
            targetL++;
          }
        }
        else if(controls.joystickY == -1){
          if(targetL>0){
            targetL--;
          }
        }
        lastTime = millis();
      }
    }
    if(utils.itsbeen(200)){
      // step buttons loop selecting
      for(uint8_t i = 0; i<sequence.loopData.size(); i++){
        if(controls.stepButton(i)){
          setActiveLoop(i);
          lastTime = millis();
          break;
        }
      }
      //changing loop type and iterations
      if(controls.joystickX != 0){
        //iterations
        if(!controls.SHIFT()){
          if(controls.joystickX == -1){
            sequence.loopData[targetL].reps++;
            lastTime = millis();
          }
          else if(controls.joystickX == 1 && sequence.loopData[targetL].reps>0){
            sequence.loopData[targetL].reps--;
            lastTime = millis();
          }
        }
        //loop type
        else{
          if(controls.joystickX == -1 && sequence.loopData[targetL].type<3){
            sequence.loopData[targetL].type++;
            lastTime = millis();
          }
          if(controls.joystickX == 1 && sequence.loopData[targetL].type>0){
            sequence.loopData[targetL].type--;
            lastTime = millis();
          }
        }
      }
      //copying a loop
      if(controls.COPY() && !controls.SHIFT()){
        loopStored = true;
        storedLoop.start = sequence.loopData[targetL].start;
        storedLoop.end = sequence.loopData[targetL].end;
        storedLoop.reps = sequence.loopData[targetL].reps;
        storedLoop.type = sequence.loopData[targetL].type;

        lastTime = millis();
      }
      //pasting loop
      if(controls.COPY() && controls.SHIFT() && loopStored){
        insertLoop(targetL,storedLoop);
        lastTime = millis();
      }
      //controls.DELETE()eting a loop
      if(controls.DELETE() && sequence.loopData.size()>1){
        deleteLoop(targetL);
        if(targetL>=sequence.loopData.size())
          targetL--;
        lastTime = millis();
      }
      //playing
      if(controls.PLAY()){
        setActiveLoop(targetL);
        togglePlayMode();
        lastTime = millis();
      }
      //new loop
      if(controls.NEW()){
        dupeLoop(targetL);
        targetL++;
        lastTime = millis();
      }
      //infinite loop toggle
      if(controls.LOOP()){
        if(!controls.SHIFT()){
          sequence.isLooping = !sequence.isLooping;
          lastTime = millis();
        }
        else{
          lastTime = millis();
          setLoopToInfinite(targetL);
        }
      }
      //select loop
      if(controls.SELECT() ){
        lastTime = millis();
        controls.setSELECT(false);
        uint8_t tempType = sequence.loopData[targetL].type;
        setLoopToInfinite(targetL);
        viewLoop(targetL);
        sequence.loopData[targetL].type = tempType;
      }
      if(controls.MENU()){
        lastTime = millis();
        controls.setMENU(false) ;
        menuText = "";
        return;
      }
    }

    //checking bounds for display
    //if targetL is more than 4 loops away from starting loop, then move menu
    while(targetL-dispStart > (maxLoopsShown-2)){
      dispStart++;
    }
    while(targetL<dispStart){
      dispStart--;
    }

    //rotating background arrows while looping is active
    if(sequence.isLooping){
      w.rotate(-1,2);
    }

    display.clearDisplay();

    //wireframe
    w.render();

    //draw the highlight of the active loop (so it's behind everything)
    display.fillRoundRect(11,4+(11)*(targetL-dispStart),106,16,3,0);
    display.drawRoundRect(11,4+(11)*(targetL-dispStart),106,16,3,1);
    display.fillRect(11,4+(11)*(targetL-dispStart),2,16,0);

    //title
    printLoopTitle(1,14);
    //side border line
    display.drawFastVLine(13,0,screenHeight,1);

    //loops
    drawLoopBlocksVertically(dispStart,targetL,blockZoom);

    //info
    drawLoopInfo(56,0,targetL);

    //top border line
    display.drawFastHLine(13,0,40,1);
    display.display();
  }
}

//prints loop information out
void drawLoopInfo(uint8_t x1, uint8_t y1, uint8_t whichLoop){
  //type of loop (random, random of same size)
  String playType;
  //style
  switch(sequence.loopData[whichLoop].type){
    case 0:
      playType = "next";
      break;
    case 1:
      playType = "rnd loop";
      break;
    case 2:
      playType = "rnd = length";
      break;
    case 3:
      playType = "return to 1st";
      break;
    case 4:
      playType = "inf repeat";
      break;
  }
  display.fillRoundRect(x1-3,y1-2,screenWidth-x1+5,9,3,0);
  display.drawRoundRect(x1-3,y1-2,screenWidth-x1+5,9,3,1);
  printSmall(x1,y1,"after:"+playType,1);
  String reps = stringify(sequence.loopData[whichLoop].reps+1)+(sequence.loopData[whichLoop].reps==0?"rep":"reps");
  display.setRotation(DISPLAY_SIDEWAYS_L);
  printSmall(screenHeight-8-reps.length()*4,screenWidth-5,reps,1);
  display.setRotation(DISPLAY_UPRIGHT);
}

//x1 is start of the first loop, y1 is the midpoint of the first loop height,
//l is length of the first loop, 
//and h is the distance between the two loop-block midpoints
void drawLoopArrow(uint8_t x1, uint8_t y1, uint8_t l, uint8_t h, uint8_t type, uint8_t number){
  //normal 'lead to next loop' loop
  switch(type){
    case 0:
      display.drawBitmap(x1+l,y1,semicircle_bmp,3,6,SSD1306_WHITE);
      display.drawLine(x1+l,y1+5,x1,y1+5,SSD1306_WHITE);
      display.setRotation(DISPLAY_UPSIDEDOWN);
      display.drawBitmap(screenWidth-x1,screenHeight-y1-11,semicircle_bmp,3,6,SSD1306_WHITE);
      display.setRotation(DISPLAY_UPRIGHT);
      break;
    //reflexive
    case 1:
      display.drawBitmap(x1+l,y1,semicircle_bmp,3,6,SSD1306_WHITE);
      display.drawLine(x1+l,y1+5,x1,y1+5,SSD1306_WHITE);
      display.setRotation(DISPLAY_UPSIDEDOWN);
      display.drawBitmap(screenWidth-x1,screenHeight-y1-6,semicircle_bmp,3,6,SSD1306_WHITE);
      display.setRotation(DISPLAY_UPRIGHT);
      break;
    //return
    case 2:
      display.drawBitmap(x1+l,y1,semicircle_bmp,3,6,SSD1306_WHITE);
      //flat
      display.drawLine(x1+l,y1+5,x1-2,y1+5,SSD1306_WHITE);
      //bottom elbow
      display.drawLine(x1-2,y1+5,x1-5,y1+2,SSD1306_WHITE);
      //if the first loop isn't on screen, draw a line to the top
      if(number>(2+maxLoopsShown))//+2 because of the top and bottom loops
        display.drawLine(x1-5,y1+2,x1-5,0,SSD1306_WHITE);
      else{
        display.drawLine(x1-5,y1+2,x1-5,y1+2-11*number,SSD1306_WHITE);
        display.drawLine(x1-5,y1+2-11*number,x1-2,y1-1-11*number,SSD1306_WHITE);
        display.drawLine(x1-2,y1-1-11*number,x1,y1-1-11*number,SSD1306_WHITE);
      }
      break;
    //rnd of same size
    case 3:
      display.drawBitmap(x1+l,y1-3,loop_arrow_special_bmp,6,6,SSD1306_WHITE);
      break;
    //totally random
    case 4:
      display.drawBitmap(x1+l,y1-3,loop_arrow_special_bmp,6,6,SSD1306_WHITE);
      break;
  }
}

void printLoopTitle(uint8_t x1, uint8_t y1){
  if(sequence.isLooping){
    display.setRotation(DISPLAY_SIDEWAYS_R);
    display.drawBitmap(screenHeight-y1-5,screenWidth-x1-9-((millis()/200)%2),loop_parenth,5,9,SSD1306_WHITE);
    display.setRotation(DISPLAY_SIDEWAYS_L);
    display.drawBitmap(y1+5,x1-sin(millis()/100+1),loop_L,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+12,x1-sin(millis()/100+2),loop_O,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+19,x1-sin(millis()/100+3),loop_O,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+26,x1-sin(millis()/100+4),loop_P,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+33,x1-sin(millis()/100+5),loop_parenth,5,9,SSD1306_WHITE);
    display.setRotation(DISPLAY_UPRIGHT);
  }
  else{
    display.setRotation(DISPLAY_SIDEWAYS_R);
    display.drawBitmap(screenHeight-y1-5,screenWidth-x1-9,loop_parenth,5,9,SSD1306_WHITE);
    display.setRotation(DISPLAY_SIDEWAYS_L);
    display.drawBitmap(y1+5,x1,loop_L,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+12,x1,loop_O,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+19,x1,loop_O,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+26,x1,loop_P,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+33,x1,loop_parenth,5,9,SSD1306_WHITE);
    display.setRotation(DISPLAY_UPRIGHT);
  }
}

vector<uint8_t> getTracksWithNotes(){
  vector<uint8_t> list;
  for(uint8_t track = 0; track<sequence.trackData.size(); track++){
    if(sequence.noteData[track].size()>1)
      list.push_back(track);
  }
  return list;
}

vector<uint8_t> getTracksWithNotesInLoop(uint8_t loop){
  vector<uint8_t> list1 = getTracksWithNotes();
  vector<uint8_t> list2;
  for(uint8_t track = 0; track<list1.size(); track++){
    for(uint16_t i = sequence.loopData[loop].start; i<sequence.loopData[loop].end; i++){
      if(sequence.lookupTable[list1[track]][i] != 0){
        list2.push_back(list1[track]);
        break;
      }
    }
  }
  return list2;
}

//draws mini sequence in a 32x16 grid
void drawLoopPreview(uint8_t x1, uint8_t y1, uint8_t loop){
  display.fillRoundRect(x1-2,y1-15,37,37,3,0);
  display.drawRoundRect(x1-2,y1-15,37,37,3,1);
  //scale
  float s = float(32)/float(sequence.loopData[loop].end-sequence.loopData[loop].start);

  //figure out how many tracks to draw
  vector<uint8_t> tracksWithNotes = getTracksWithNotesInLoop(loop);
  y1 = tracksWithNotes.size()==0?screenHeight-8:screenHeight-tracksWithNotes.size();

  printSmall(x1+9,y1-8,"view",1);
  //start
  display.fillTriangle(x1, y1-6, x1+4,y1-6,x1,y1-2,SSD1306_WHITE);
  //end
  display.drawTriangle(x1+32,y1-6,x1+28,y1-6,x1+32,y1-2,SSD1306_WHITE);

  //bounds
  if(tracksWithNotes.size()>0){
    display.drawFastVLine(x1+32,y1-2,2+tracksWithNotes.size(),SSD1306_WHITE);
    display.drawFastVLine(x1,y1-2,2+tracksWithNotes.size(),SSD1306_WHITE);
    display.drawFastHLine(x1,y1+tracksWithNotes.size(),3,SSD1306_WHITE);
    display.drawFastHLine(x1+30,y1+tracksWithNotes.size(),3,SSD1306_WHITE);
  }
  else{
    display.drawFastVLine(x1+32,y1-2,8,SSD1306_WHITE);
    display.drawFastVLine(x1,y1-2,8,SSD1306_WHITE);
    display.drawFastHLine(x1,y1+6,3,SSD1306_WHITE);
    display.drawFastHLine(x1+30,y1+6,3,SSD1306_WHITE);
  }


  //if there are notes in the loop
  if(tracksWithNotes.size()>0){
    //move through each step of the loop
    for(uint8_t tr = 0; tr<tracksWithNotes.size(); tr++){
      uint8_t t = tracksWithNotes[tr];
      for(uint16_t i = sequence.loopData[loop].start; i<sequence.loopData[loop].end; i++){
        //if i finds the start positino of a note
        if(sequence.lookupTable[t][i] != 0 && i == sequence.noteData[t][sequence.lookupTable[t][i]].startPos){
          //get scaled length
          uint16_t length = (sequence.noteData[t][sequence.lookupTable[t][i]].endPos - sequence.noteData[t][sequence.lookupTable[t][i]].startPos+1);
          //draw line representing note
          display.drawFastHLine(x1+float(i-sequence.loopData[loop].start)*s,y1+tr,length*s,SSD1306_WHITE);
          i = sequence.noteData[t][sequence.lookupTable[t][i]].endPos;
        }
      }
    }
    //drawing playhead, if it's in view
    if(playing && playheadPos>sequence.loopData[loop].start && playheadPos<sequence.loopData[loop].end){
      display.drawFastVLine(x1+(playheadPos-sequence.loopData[loop].start)*s,y1-1,tracksWithNotes.size()+2,SSD1306_WHITE);
    }
  }
  //if there are no notes
  else{
    printSmall(x1+3,y1,"[Empty]",SSD1306_WHITE);
  }

  graphics.printFraction_small_centered(x1+18,y1-18,stepsToMeasures(sequence.loopData[loop].end - sequence.loopData[loop].start));
  display.setRotation(DISPLAY_SIDEWAYS_L);
  printSmall(screenHeight-y1+8,x1,stepsToPosition(sequence.loopData[loop].start,true),1);
  printSmall(screenHeight-y1+8,x1+27,stepsToPosition(sequence.loopData[loop].end,true),1);
  display.setRotation(DISPLAY_UPRIGHT);
}

void drawLoopBlocksVertically(int firstLoop,int highlight, int z){
  const int yStart = 8;
  const int xStart = 32;
  const int loopHeight = 8;

  //drawing all the loops
  //starts from -1 so you draw one "before" loop (just for looks)
  for(int8_t loop = -1; loop<maxLoopsShown+1; loop++){
    if(loop+firstLoop<sequence.loopData.size()){
      //number
      printSmall(xStart-15,yStart+(loopHeight+3)*loop+2,stringify(loop+firstLoop+1),SSD1306_WHITE);

      //scale is 1:12, so every 1/4 note is 2px
      int length = (sequence.loopData[loop+firstLoop].end-sequence.loopData[loop+firstLoop].start)/z;
      int shade = (sequence.loopData[loop+firstLoop].start+sequence.loopData[loop+firstLoop].end+loop+firstLoop)%12+2;

      //clearing background
      display.fillRect(xStart, yStart+(loopHeight+3)*loop, length, loopHeight, 0);

      //play progress
      if(playing && loop+firstLoop == sequence.activeLoop)
        display.fillRect(xStart, yStart+(loopHeight+3)*loop, float(length)*float(playheadPos-sequence.loopData[sequence.activeLoop].start)/float(sequence.loopData[sequence.activeLoop].end-sequence.loopData[sequence.activeLoop].start), loopHeight, SSD1306_WHITE);

      //loop block
      display.drawRect(xStart, yStart+(loopHeight+3)*loop, length, loopHeight, 1);
      if(sequence.isLooping)
        graphics.shadeArea(xStart, yStart+(loopHeight+3)*loop, length, loopHeight,shade);
      
      uint8_t x1 = xStart+length+3;

      //active loop note
      if(loop+firstLoop == sequence.activeLoop){
        //highlight arrow
        if(highlight == sequence.activeLoop){
          graphics.drawArrow(xStart-9+((millis()/200)%2)+2, yStart+(loopHeight+3)*loop+4,2,0,true);
        }
        //loop types
        switch(sequence.loopData[sequence.activeLoop].type){
          //normal
          case 0:
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,down_arrow_bmp,7,7,SSD1306_WHITE);
            x1+=9;
            break;
          //rnd any
          case 1:
            x1+=4;
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,rnd_bmp,7,7,SSD1306_WHITE);
            x1+=9;
            break;
          //rnd of same length
          case 2:
            x1+=4;
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,rnd_equal_bmp,11,7,SSD1306_WHITE);
            x1+=13;
            break;
          //return
          case 3:
            x1++;
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,return_bmp,7,7,SSD1306_WHITE);
            x1+=9;
            break;
          //if it's an infinite loop
          case 4:
            //infinity sign
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop+1,inf_bmp,9,5,SSD1306_WHITE);
            x1+=11;
            break;
        }
        //length
        x1+=graphics.printFraction_small(x1,yStart+(loopHeight+3)*loop+1,stepsToMeasures(sequence.loopData[sequence.activeLoop].end-sequence.loopData[sequence.activeLoop].start))+2;
        if(playing){
          //play/iterations
          printSmall(x1,yStart+(loopHeight+3)*loop+1, "("+stringify(sequence.loopCount)+"/"+stringify(sequence.loopData[sequence.activeLoop].reps+1)+")", SSD1306_WHITE);
          x1+=4*(stringify(sequence.loopCount).length()+stringify(sequence.loopData[sequence.activeLoop].reps+1).length()+3);
        }
        else{
          // play * iterations
          graphics.drawArrow(x1+2,yStart+(loopHeight+3)*loop+3,2,0,true);
          x1+=4;
          printSmall(x1,yStart+(loopHeight+3)*loop+1, "|"+stringify(sequence.loopData[sequence.activeLoop].reps+1), SSD1306_WHITE);
          x1+=4*(stringify(sequence.loopData[sequence.activeLoop].reps+1).length()+1)+1;
        }
        //note icon
        printSmall(x1,yStart+(loopHeight+3)*loop+2+((millis()/200)%2), "$", SSD1306_WHITE);
        x1+=4;
      }
      //highlighted loop
      else if(loop+firstLoop == highlight){
        //highlight arrow
        graphics.drawArrow(xStart-9+((millis()/200)%2)+2, yStart+(loopHeight+3)*loop+4,2,0,true);
        //loop types
        switch(sequence.loopData[highlight].type){
          //normal
          case 0:
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,down_arrow_bmp,7,7,SSD1306_WHITE);
            x1+=9;
            break;
          //rnd any
          case 1:
            x1+=4;
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,rnd_bmp,7,7,SSD1306_WHITE);
            x1+=9;
            break;
          //rnd of same length
          case 2:
            x1+=4;
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,rnd_equal_bmp,11,7,SSD1306_WHITE);
            x1+=13;
            break;
          //return
          case 3:
            x1++;
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop,return_bmp,7,7,SSD1306_WHITE);
            x1+=9;
            break;
          //if it's an infinite loop
          case 4:
            //infinity sign
            display.drawBitmap(x1,yStart+(loopHeight+3)*loop+1,inf_bmp,9,5,SSD1306_WHITE);
            x1+=11;
            break;
        }
        //length
        x1+=graphics.printFraction_small(x1,yStart+(loopHeight+3)*loop+1,stepsToMeasures(sequence.loopData[highlight].end-sequence.loopData[highlight].start))+2;
        // play symbol
        graphics.drawArrow(x1+2,yStart+(loopHeight+3)*loop+3,2,0,true);
        x1+=4;
        // iterations
        printSmall(x1,yStart+(loopHeight+3)*loop+1, "|"+stringify(sequence.loopData[highlight].reps+1), SSD1306_WHITE);
        x1+=4*(stringify(sequence.loopData[highlight].reps+1).length()+1);
      }
      //if there is a return loop offscreen
      //draw a return line from the first loop (or top of screen) down
      for(uint8_t l = firstLoop+maxLoopsShown-1; l<sequence.loopData.size(); l++){
        //if there is a return loop offscreen
        //or if the last loop is normal, and offscreen
        if(sequence.loopData[l].type == RETURN || (l == sequence.loopData.size()-1 && sequence.loopData[l].type == NORMAL)){
          //if the first loop is off screen too
          //just draw a vertical line
          if(firstLoop>0){
            display.drawFastVLine(xStart-5,0,screenHeight,SSD1306_WHITE);
          }
          //if the first loop is on screen, draw a line to it
          else{
            // display.drawLine(xStart-5,yStart+(loopHeight+3)*loop+loopHeight/2+2,xStart-5,yStart+(loopHeight+3)*loop+loopHeight/2+2-11*(loop+firstLoop),SSD1306_WHITE);
            display.drawLine(xStart-5,yStart+(loopHeight+3)*loop+loopHeight/2+2,xStart-5,screenHeight,SSD1306_WHITE);
            display.drawLine(xStart-5,yStart+(loopHeight+3)*loop+loopHeight/2+2-11*(loop+firstLoop),xStart-2,yStart+(loopHeight+3)*loop+loopHeight/2-1-11*(loop+firstLoop),SSD1306_WHITE);
            display.drawLine(xStart-2,yStart+(loopHeight+3)*loop+loopHeight/2-1-11*(loop+firstLoop),xStart,yStart+(loopHeight+3)*loop+loopHeight/2-1-11*(loop+firstLoop),SSD1306_WHITE);
          }
        }
      }
      //drawing loop connectors
      switch(sequence.loopData[loop+firstLoop].type){
        //normal loop
        case 0:
          //if it's the last loop on screen, but there are more loops
          if(loop == 5){

          }
          //if it's the last loop in the sequence, make it a return
          else if(loop+firstLoop == sequence.loopData.size()-1){
            //if there's just one loop, draw it as an infinite loop (j so it looks nice)
            if(sequence.loopData.size() == 1)
              drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,1,loop+firstLoop);
            else
              drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,2,loop+firstLoop);
          }
          //if it's just another loop
          else
            drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,0,loop+firstLoop);
          break;
        //random loop, of same length
        case 1:
          drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,3,loop+firstLoop);
          break;
        //any random loop
        case 2:
          drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,4,loop+firstLoop);
          break;
        //return
        case 3:
          drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,2,loop+firstLoop);
          break;
        //infinite
        case 4:
          drawLoopArrow(xStart,yStart+(loopHeight+3)*loop+loopHeight/2,length,loopHeight+3,1,loop+firstLoop);
          break;
      }
    }
  }
}

void setActiveLoop(unsigned int id){
  if(id<sequence.loopData.size() && id >=0){
    sequence.activeLoop = id;
    sequence.loopCount = 0;
  }
}

void addLoop(){
  Loop newLoop;
  newLoop.start = sequence.loopData[sequence.activeLoop].start;
  newLoop.end = sequence.loopData[sequence.activeLoop].end;
  newLoop.reps = sequence.loopData[sequence.activeLoop].reps;
  newLoop.type = sequence.loopData[sequence.activeLoop].type;
  sequence.loopData.push_back(newLoop);
  setActiveLoop(sequence.loopData.size()-1);
}

void addLoop(unsigned short int start, unsigned short int end, unsigned short int iter, unsigned short int type){
  Loop newLoop;
  newLoop.start = start;
  newLoop.end = end;
  newLoop.reps = iter;
  newLoop.type = type;
  sequence.loopData.push_back(newLoop);
}

void deleteLoop(uint8_t id){
  if(sequence.loopData.size() > 1 && sequence.loopData.size()>id){//if there's more than one loop, and id is in sequence.loopData
    vector<Loop> tempVec;
    for(int i = 0; i<sequence.loopData.size(); i++){
      if(i!=id){
        tempVec.push_back(sequence.loopData[i]);
      }
    }
    sequence.loopData.swap(tempVec);
    //if sequence.activeLoop was the loop that got deld, or above it
    //decrement it's id so it reads correct (and existing) data
    if(sequence.activeLoop>=sequence.loopData.size()){  
      sequence.activeLoop = sequence.loopData.size()-1;
    }
  }
  setActiveLoop(sequence.activeLoop);
}

void toggleLoop(){
  sequence.isLooping = !sequence.isLooping;
}

void setLoopToInfinite(uint8_t targetL){
  //if it's already a 3, set it to 0
  if(sequence.loopData[targetL].type == INFINITE){
    sequence.loopData[targetL].type = NORMAL;
  }
  //if not, set this loop to 3
  else{
    sequence.loopData[targetL].type = INFINITE;
  }
  //set all other inf loops to 0
  for(uint8_t l = 0; l<sequence.loopData.size(); l++){
    if(l != targetL){
      if(sequence.loopData[l].type == INFINITE)
        sequence.loopData[l].type = NORMAL;
    }
  }
}

//moves to the next loop in loopSeq
void nextLoop(){
  sequence.loopCount = 0;
  if(sequence.loopData.size()>1){
    switch(sequence.loopData[sequence.activeLoop].type){
      case NORMAL:
        //move to next loop
        if(sequence.activeLoop < sequence.loopData.size()-1)
          sequence.activeLoop++;
        else
          sequence.activeLoop = 0;
        if(playing)
          playheadPos = sequence.loopData[sequence.activeLoop].start;
        if(recording)
          recheadPos = sequence.loopData[sequence.activeLoop].start;
        break;
      case RANDOM:{
        sequence.activeLoop = random(0,sequence.loopData.size());
        if(playing)
          playheadPos = sequence.loopData[sequence.activeLoop].start;
        if(recording)
          recheadPos = sequence.loopData[sequence.activeLoop].start;
        break;}
      case RANDOM_SAME:{
        //move to next loop
        if(sequence.activeLoop < sequence.loopData.size()-1)
          sequence.activeLoop++;
        else
          sequence.activeLoop = 0;
        //if rnd of same size mode, choose a random loop
        int currentLength = sequence.loopData[sequence.activeLoop].end - sequence.loopData[sequence.activeLoop].start;
        vector<uint8_t> similarLoops;
        for(int i = 0; i<sequence.loopData.size(); i++){
          int len = sequence.loopData[i].end-sequence.loopData[i].start;
          if(len == currentLength){
            similarLoops.push_back(i);
          }
        }
        sequence.activeLoop = similarLoops[random(0,similarLoops.size())];
        if(playing)
          playheadPos = sequence.loopData[sequence.activeLoop].start;
        if(recording)
          recheadPos = sequence.loopData[sequence.activeLoop].start;
        break;}
      case RETURN:{
        sequence.activeLoop = 0;
        if(playing)
          playheadPos = sequence.loopData[sequence.activeLoop].start;
        if(recording)
          recheadPos = sequence.loopData[sequence.activeLoop].start;
        break;}
      case INFINITE:{
        if(playing)
          playheadPos = sequence.loopData[sequence.activeLoop].start;
        if(recording)
          recheadPos = sequence.loopData[sequence.activeLoop].start;
        break;}
    }
  }
}


//cuts notes off when loop repeats, then starts new note at beginning
void cutLoop(){
  for(int i = 0; i<sequence.trackData.size(); i++){
    if(sequence.trackData[i].noteLastSent != 255){
      sequence.noteData[i][sequence.noteData[i].size()-1].endPos = sequence.loopData[sequence.activeLoop].end;
      //if it's about to loop again (if it's a one-shot recording, there's no need to make a new note)
      if(recMode != ONESHOT)
        writeNoteOn(sequence.loopData[sequence.activeLoop].start,sequence.trackData[i].pitch,sequence.noteData[i][sequence.noteData[i].size()-1].velocity,sequence.trackData[i].channel);
    }
  }
}

//this checks loops bounds, moves to next loop, and cuts loop
void checkLoop(){
  if(playing){
    if (playheadPos > sequence.loopData[sequence.activeLoop].end-1) { //if the timestep is past the end of the loop, loop it to the start
      sequence.loopCount++;
      if(sequence.loopCount > sequence.loopData[sequence.activeLoop].reps){
        nextLoop();
      }
      playheadPos = sequence.loopData[sequence.activeLoop].start;
      if(!sequence.isLooping)
        togglePlayMode();
    }
  }
  else if(recording){
    //one-shot record to current loop, without looping
    if(recMode == ONESHOT){
      if(recheadPos>=sequence.loopData[sequence.activeLoop].end){
        toggleRecordingMode(waitForNoteBeforeRec);
      }
    }
    //record to one loop over and over again
    else if(recMode == LOOP_MODE){
      if(recheadPos>=sequence.loopData[sequence.activeLoop].end){
        recheadPos = sequence.loopData[sequence.activeLoop].start;
      }
    }
    //record to loops as they play in sequence
    else if(recMode == LOOPSEQUENCE){
      if(recheadPos>=sequence.loopData[sequence.activeLoop].end){
        cutLoop();
        sequence.loopCount++;
        if(sequence.loopData[sequence.activeLoop].reps>=sequence.loopCount){
          nextLoop();
        }
        recheadPos = sequence.loopData[sequence.activeLoop].start;
      }
    }
  }
}

//creates a new loop immediately to the right of the current loop and copies the loop over into it (useful for on the fly beatmaking)
//then sets the OG loop to that one
void pushToNewLoop(){
  Loop newLoop = sequence.loopData[sequence.activeLoop];
  uint16_t length = newLoop.length();
  //if you're out of sequence length
  if(newLoop.end+length >= sequence.sequenceLength){
    alert("extend seq to add more loops!",1000);
    return;
  }
  //else, make a new loop to the right
  else{
    newLoop.start+=length;
    newLoop.end+=length;
    sequence.addLoop(newLoop);//add the loop to the chain

    //duplicate the loop contents and paste them next to the current loop
    clipboard.copyLoop(sequence.activeLoop);
    clipboard.pasteAt(0,newLoop.start);

    //move the view to the start of the current loop
    setCursor(newLoop.start);
    setViewStart(newLoop.start);
  }
}