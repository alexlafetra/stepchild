#define NORMAL 0
#define RANDOM 1
#define RANDOM_SAME 2
#define RETURN 3
#define INFINITE 4

// vector<Loop> loopData;

void printLoopTitle(uint8_t, uint8_t);
void drawLoopBlocksVertically(int,int,int);
void drawLoopPreview(uint8_t x1, uint8_t y1, uint8_t loop);
void drawLoopInfo(uint8_t x1, uint8_t y1, uint8_t whichLoop);

void moveCursorWithinLoop(int amount, uint8_t whichLoop){
  //if the cursor is going to move before the loop
  if(amount<0 && (cursorPos + amount)<loopData[whichLoop].start)
    moveCursor(cursorPos - loopData[whichLoop].start);
  //if the cursor is going to move past the loop
  else if(amount>0 && (cursorPos+amount)>=loopData[whichLoop].end)
    moveCursor(loopData[whichLoop].end-cursorPos);
  //if not, then it's moving within the loop
  else
    moveCursor(amount);
}

bool isWithinLoop(int val, int loop, int amount, bool inclusive){
  if(!inclusive){
    if(val<(loopData[loop].end-amount) && val>(loopData[loop].start+amount)){
      return true;
    }
    else{
      return false;
    }
  }
  else{
    if(val<=(loopData[loop].end-amount) && val>=(loopData[loop].start+amount)){
      return true;
    }
    else{
      return false;
    }
  }
}

void insertLoop(int afterThis, Loop newLoop){
  vector<Loop> tempData;
  for(int i = 0; i<loopData.size(); i++){
    tempData.push_back(loopData[i]);
    if(i == afterThis)
      tempData.push_back(newLoop);
  }
  loopData.swap(tempData);
}

void dupeLoop(int loop){
  vector<Loop> tempData;
  for(int i = 0; i<loopData.size(); i++){
    tempData.push_back(loopData[i]);
    if(i == loop)
      tempData.push_back(loopData[i]);
  }
  loopData.swap(tempData);
}

void setLoopPoint(int32_t start, bool which){
  //set start
  if(which){
    if(start<=loopData[activeLoop].end && start>=0)
      loopData[activeLoop].start = start;
    else if(start < 0)
      loopData[activeLoop].start = 0;
    else if(start>loopData[activeLoop].end)
      loopData[activeLoop].start = loopData[activeLoop].end;
    loopData[activeLoop].start = loopData[activeLoop].start;
    menuText = "loop start: "+stepsToPosition(loopData[activeLoop].start,true);
  }
  //set end
  else{
    if(start>=loopData[activeLoop].start && start <= seqEnd)
      loopData[activeLoop].end = start;
    else if(start>seqEnd)
      loopData[activeLoop].end = seqEnd;
    else if(start<loopData[activeLoop].start)
      loopData[activeLoop].end = loopData[activeLoop].start;
    loopData[activeLoop].end = loopData[activeLoop].end;
    menuText = "loop end: "+stepsToPosition(loopData[activeLoop].end,true);
  }
}

uint16_t getLongestLoop(){
  uint16_t longestLength = 0;
  for(uint8_t i = 0; i<loopData.size(); i++){
    uint16_t l = loopData[i].end - loopData[i].start;
    if(l>longestLength)
      longestLength = l;
  }
  return longestLength;
}

//just your normal editing controls, but you can't move the view past the loop
bool viewLoopControls(uint8_t which){
  //selectionBox
  //when sel is pressed and stick is moved, and there's no selection box
  if(sel && !selBox.begun && (controls.joystickX != 0 || controls.joystickY != 0)){
    selBox.begun = true;
    selBox.coords.start.x = cursorPos;
    selBox.coords.start.y = activeTrack;
  }
  //if sel is released, and there's a selection box
  if(!sel && selBox.begun){
    selBox.coords.end.x = cursorPos;
    selBox.coords.end.y = activeTrack;
    selectBox();
    selBox.begun = false;
  }
  if(!n)
    drawingNote = false;
  mainSequencerEncoders();
  if (utils.itsbeen(100)) {
    if (controls.joystickX == 1 && !shift) {
      //if cursor isn't on a measure marker, move it to the nearest one
      if(cursorPos%subDivInt){
        if(!movingLoop)
          moveCursorWithinLoop(-cursorPos%subDivInt,which);
        else
          moveCursor(-cursorPos%subDivInt);
        lastTime = millis();
        //moving entire loop
        if(movingLoop == 2)
          moveLoop(-cursorPos%subDivInt);
      }
      else{
        if(!movingLoop)
          moveCursorWithinLoop(-subDivInt,which);
        else
          moveCursor(-subDivInt);
        lastTime = millis();
        //moving entire loop
        if(movingLoop == 2)
          moveLoop(-subDivInt);
      }
      //moving loop start/end
      if(movingLoop == -1){
        setLoopPoint(cursorPos,true);
      }
      else if(movingLoop == 1){
        setLoopPoint(cursorPos,false);
      }
    }
    if (controls.joystickX == -1 && !shift) {
      if(cursorPos%subDivInt){
        if(!movingLoop)
          moveCursorWithinLoop(subDivInt-cursorPos%subDivInt,which);
        else
          moveCursor(subDivInt-cursorPos%subDivInt);
        lastTime = millis();
        if(movingLoop == 2)
          moveLoop(subDivInt-cursorPos%subDivInt);
      }
      else{
        if(!movingLoop)
          moveCursorWithinLoop(subDivInt,which);
        else
          moveCursor(subDivInt);
        lastTime = millis();
        if(movingLoop == 2)
          moveLoop(subDivInt);
      }
      //moving loop start/end
      if(movingLoop == -1){
        setLoopPoint(cursorPos,true);
      }
      else if(movingLoop == 1){
        setLoopPoint(cursorPos,false);
      }
    }
  }
  if(utils.itsbeen(100)){
    if (controls.joystickY == 1 && !shift && !loop_Press) {
      if(recording)//if you're not in normal mode, you don't want it to be loud
        setActiveTrack(activeTrack + 1, false);
      else
        setActiveTrack(activeTrack + 1, true);
      drawingNote = false;
      lastTime = millis();
    }
    if (controls.joystickY == -1 && !shift && !loop_Press) {
      if(recording)//if you're not in normal mode, you don't want it to be loud
        setActiveTrack(activeTrack - 1, false);
      else
        setActiveTrack(activeTrack - 1, true);
      drawingNote = false;
      lastTime = millis();
    }
  }
  if (utils.itsbeen(50)) {
    //moving
    if (controls.joystickX == 1 && shift) {
      if(!movingLoop)
        moveCursorWithinLoop(-1,which);
      else
        moveCursor(-1);
      lastTime = millis();
      if(movingLoop == 2)
        moveLoop(-1);
      else if(movingLoop == -1)
        setLoopPoint(cursorPos,true);
      else if(movingLoop == 1)
        setLoopPoint(cursorPos,false);
    }
    if (controls.joystickX == -1 && shift) {
      if(!movingLoop)
        moveCursorWithinLoop(1,which);
      else
        moveCursor(1);
      lastTime = millis();
      if(movingLoop == 2)
        moveLoop(1);
      else if(movingLoop == -1)
        loopData[activeLoop].start = cursorPos;
      else if(movingLoop == 1)
        loopData[activeLoop].end = cursorPos;
    }
    //changing vel
    if (controls.joystickY == 1 && shift) {
      changeVel(-10);
      lastTime = millis();
    }
    if (controls.joystickY == -1 && shift) {
      changeVel(10);
      lastTime = millis();
    }

    if(lookupData[activeTrack][cursorPos]==0){
      if(controls.joystickY == 1 && shift){
        defaultVel-=10;
        if(defaultVel<1)
          defaultVel = 1;
        lastTime = millis();
      }
      if(controls.joystickY == -1 && shift){
        defaultVel+=10;
        if(defaultVel>127)
          defaultVel = 127;
        lastTime = millis();
      }
    }
  }
  //delete happens a liitle faster (so you can draw/erase fast)
  if(utils.itsbeen(75)){
    //delete
    if(del && !shift){
      if (selectionCount > 0){
        deleteSelected();
        lastTime = millis();
      }
      else if(lookupData[activeTrack][cursorPos] != 0){
        deleteNote(activeTrack,cursorPos);
        lastTime = millis();
      }
    }
  }
  if(utils.itsbeen(200)){
    //new
    if(n && !controls.A() && !drawingNote && !sel){
      if((!shift)&&(lookupData[activeTrack][cursorPos] == 0 || cursorPos != seqData[activeTrack][lookupData[activeTrack][cursorPos]].startPos)){
        makeNote(activeTrack,cursorPos,subDivInt,true);
        drawingNote = true;
        lastTime = millis();
        moveCursorWithinLoop(subDivInt,which);
      }
      if(shift){
        addTrack(defaultPitch, defaultChannel);
        lastTime = millis();
      }
    }
    //select
    if(sel && !selBox.begun){
      uint16_t id = lookupData[activeTrack][cursorPos];
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
    if(del && shift){
      muteNote(activeTrack, lookupData[activeTrack][cursorPos], true);
      lastTime = millis();
    }

    //Modes: play, listen, and record
    if(play && !shift && !recording){
      togglePlayMode();
      lastTime = millis();
    }
    //if play+shift, or if play and it's already recording
    if((play && shift) || (play && recording)){
      toggleRecordingMode(waitForNoteBeforeRec);
      lastTime = millis();
    }
 
    //loop
    if(loop_Press){
      //if you're not moving a loop, start
      if(movingLoop == 0){
        //if you're on the start, move the start
        if(cursorPos == loopData[activeLoop].start){
          movingLoop = -1;
        }
        //if you're on the end
        else if(cursorPos == loopData[activeLoop].end){
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
    if(menu_Press){
      if(shift){
        lastTime = millis();
        return false;
      }
      else{
        lastTime = millis();
        menu_Press = false;
        return false;
      }
    }
    //copy/pasate
    if(copy_Press){
      if(shift)
        paste();
      else{
        copy();
      }
      lastTime = millis();
    }
  }
  return true;
}

void viewLoop(uint8_t which){
  setCursor(loopData[which].start);
  setActiveLoop(which);
  String tempText;
  while(true){
    controls.readJoystick();
    readButtons();
    if(!viewLoopControls(which))
      return;
    tempText = menuText;
    menuText = "loop-"+stringify(which)+" "+menuText;
    display.clearDisplay();
    // drawSeq(true,true,true,false,false,true,loopData[which].start,loopData[which].end*scale);
    drawSeq(true,true,true,false,false,true,viewStart,viewEnd);
    // drawSeq(true,true,true,false,false,true,loopData[which].start>viewStart?loopData[which].start:viewStart,loopData[which].end<viewEnd?loopData[which].end:viewEnd);
    display.display();
    menuText = tempText;
  }
}
const uint8_t maxLoopsShown = 6;

void loopMenu(){
  uint8_t xStart = 32;
  uint8_t targetL = activeLoop;
  uint8_t dispStart = activeLoop;
  int blockZoom = getLongestLoop()/30;
  bool showingPreview = false;
  Loop storedLoop;
  storedLoop.start = loopData[targetL].start;
  storedLoop.end = loopData[targetL].end;
  storedLoop.reps = loopData[targetL].reps;
  storedLoop.type = loopData[targetL].type;
  bool loopStored = false;
  WireFrame w = makeLoopArrows(0);
  w.xPos = 64;
  w.yPos = 32;
  w.scale = 4.0;
  while(true){
    controls.readJoystick();
    readButtons();
    //changing the loop iterations
    while(counterA != 0){
      if(counterA>0){
        loopData[targetL].reps++;
      }
      else{
        if(loopData[targetL].reps != 0)
          loopData[targetL].reps--;
      }
      counterA += counterA<0?1:-1;
    }
    //changing the loop type
    while(counterB != 0){
      if(counterB>0){
        if(loopData[targetL].type<4)
          loopData[targetL].type++;
        else
          loopData[targetL].type = 0;
      }
      else{
        if(loopData[targetL].type>0)
          loopData[targetL].type--;
        else
          loopData[targetL].type = 4;
      }
      counterB += counterB<0?1:-1;
    }
    if(utils.itsbeen(100)){
      if(controls.joystickY != 0){
        if(controls.joystickY == 1){
          if(targetL<loopData.size()-1){
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
      for(uint8_t i = 0; i<loopData.size(); i++){
        if(controls.stepButton(i)){
          setActiveLoop(i);
          lastTime = millis();
          break;
        }
      }
      //changing loop type and iterations
      if(controls.joystickX != 0){
        //iterations
        if(!shift){
          if(controls.joystickX == -1){
            loopData[targetL].reps++;
            lastTime = millis();
          }
          else if(controls.joystickX == 1 && loopData[targetL].reps>0){
            loopData[targetL].reps--;
            lastTime = millis();
          }
        }
        //loop type
        else{
          if(controls.joystickX == -1 && loopData[targetL].type<3){
            loopData[targetL].type++;
            lastTime = millis();
          }
          if(controls.joystickX == 1 && loopData[targetL].type>0){
            loopData[targetL].type--;
            lastTime = millis();
          }
        }
      }
      //copying a loop
      if(copy_Press && !shift){
        loopStored = true;
        storedLoop.start = loopData[targetL].start;
        storedLoop.end = loopData[targetL].end;
        storedLoop.reps = loopData[targetL].reps;
        storedLoop.type = loopData[targetL].type;

        lastTime = millis();
      }
      //pasting loop
      if(copy_Press && shift && loopStored){
        insertLoop(targetL,storedLoop);
        lastTime = millis();
      }
      //deleting a loop
      if(del && loopData.size()>1){
        deleteLoop(targetL);
        if(targetL>=loopData.size())
          targetL--;
        lastTime = millis();
      }
      //playing
      if(play){
        setActiveLoop(targetL);
        togglePlayMode();
        lastTime = millis();
      }
      //new loop
      if(n){
        dupeLoop(targetL);
        targetL++;
        lastTime = millis();
      }
      //infinite loop toggle
      if(loop_Press){
        if(!shift){
          isLooping = !isLooping;
          lastTime = millis();
        }
        else{
          lastTime = millis();
          setLoopToInfinite(targetL);
        }
      }
      //select loop
      if(sel){
        lastTime = millis();
        sel = false;
        uint8_t tempType = loopData[targetL].type;
        setLoopToInfinite(targetL);
        viewLoop(targetL);
        loopData[targetL].type = tempType;
      }
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        return;
      }
      if(shift && !copy_Press && controls.joystickX == 0){
        showingPreview = !showingPreview;
        lastTime = millis();
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
    if(isLooping){
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
    if(showingPreview){
      drawLoopPreview(95,48,targetL);
    }
    else{
      display.drawRoundRect(screenWidth-7,43,13,23,3,1);
      display.setRotation(SIDEWAYS_L);
      printSmall(0,screenWidth-5,"shift",1);
      display.setRotation(UPRIGHT);
    }

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
  switch(loopData[whichLoop].type){
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
  String reps = stringify(loopData[whichLoop].reps+1)+(loopData[whichLoop].reps==0?"rep":"reps");
  display.setRotation(SIDEWAYS_L);
  printSmall(screenHeight-8-reps.length()*4,screenWidth-5,reps,1);
  display.setRotation(UPRIGHT);
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
      display.setRotation(UPSIDEDOWN);
      display.drawBitmap(screenWidth-x1,screenHeight-y1-11,semicircle_bmp,3,6,SSD1306_WHITE);
      display.setRotation(UPRIGHT);
      break;
    //reflexive
    case 1:
      display.drawBitmap(x1+l,y1,semicircle_bmp,3,6,SSD1306_WHITE);
      display.drawLine(x1+l,y1+5,x1,y1+5,SSD1306_WHITE);
      display.setRotation(UPSIDEDOWN);
      display.drawBitmap(screenWidth-x1,screenHeight-y1-6,semicircle_bmp,3,6,SSD1306_WHITE);
      display.setRotation(UPRIGHT);
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
  if(isLooping){
    display.setRotation(SIDEWAYS_R);
    display.drawBitmap(screenHeight-y1-5,screenWidth-x1-9-((millis()/200)%2),loop_parenth,5,9,SSD1306_WHITE);
    display.setRotation(SIDEWAYS_L);
    display.drawBitmap(y1+5,x1-sin(millis()/100+1),loop_L,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+12,x1-sin(millis()/100+2),loop_O,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+19,x1-sin(millis()/100+3),loop_O,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+26,x1-sin(millis()/100+4),loop_P,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+33,x1-sin(millis()/100+5),loop_parenth,5,9,SSD1306_WHITE);
    display.setRotation(UPRIGHT);
  }
  else{
    display.setRotation(SIDEWAYS_R);
    display.drawBitmap(screenHeight-y1-5,screenWidth-x1-9,loop_parenth,5,9,SSD1306_WHITE);
    display.setRotation(SIDEWAYS_L);
    display.drawBitmap(y1+5,x1,loop_L,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+12,x1,loop_O,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+19,x1,loop_O,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+26,x1,loop_P,7,9,SSD1306_WHITE);
    display.drawBitmap(y1+33,x1,loop_parenth,5,9,SSD1306_WHITE);
    display.setRotation(UPRIGHT);
  }
}

vector<uint8_t> getTracksWithNotes(){
  vector<uint8_t> list;
  for(uint8_t track = 0; track<trackData.size(); track++){
    if(seqData[track].size()>1)
      list.push_back(track);
  }
  return list;
}

vector<uint8_t> getTracksWithNotesInLoop(uint8_t loop){
  vector<uint8_t> list1 = getTracksWithNotes();
  vector<uint8_t> list2;
  for(uint8_t track = 0; track<list1.size(); track++){
    for(uint16_t i = loopData[loop].start; i<loopData[loop].end; i++){
      if(lookupData[list1[track]][i] != 0){
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
  float s = float(32)/float(loopData[loop].end-loopData[loop].start);

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
      for(uint16_t i = loopData[loop].start; i<loopData[loop].end; i++){
        //if i finds the start positino of a note
        if(lookupData[t][i] != 0 && i == seqData[t][lookupData[t][i]].startPos){
          //get scaled length
          uint16_t length = (seqData[t][lookupData[t][i]].endPos - seqData[t][lookupData[t][i]].startPos+1);
          //draw line representing note
          display.drawFastHLine(x1+float(i-loopData[loop].start)*s,y1+tr,length*s,SSD1306_WHITE);
          i = seqData[t][lookupData[t][i]].endPos;
        }
      }
    }
    //drawing playhead, if it's in view
    if(playing && playheadPos>loopData[loop].start && playheadPos<loopData[loop].end){
      display.drawFastVLine(x1+(playheadPos-loopData[loop].start)*s,y1-1,tracksWithNotes.size()+2,SSD1306_WHITE);
    }
  }
  //if there are no notes
  else{
    printSmall(x1+3,y1,"[Empty]",SSD1306_WHITE);
  }

  graphics.printFraction_small_centered(x1+18,y1-18,stepsToMeasures(loopData[loop].end - loopData[loop].start));
  display.setRotation(SIDEWAYS_L);
  printSmall(screenHeight-y1+8,x1,stepsToPosition(loopData[loop].start,true),1);
  printSmall(screenHeight-y1+8,x1+27,stepsToPosition(loopData[loop].end,true),1);
  display.setRotation(UPRIGHT);
}

void drawLoopBlocksVertically(int firstLoop,int highlight, int z){
  const int yStart = 8;
  const int xStart = 32;
  const int loopHeight = 8;

  //drawing all the loops
  //starts from -1 so you draw one "before" loop (just for looks)
  for(int8_t loop = -1; loop<maxLoopsShown+1; loop++){
    if(loop+firstLoop<loopData.size()){
      //number
      printSmall(xStart-15,yStart+(loopHeight+3)*loop+2,stringify(loop+firstLoop+1),SSD1306_WHITE);

      //scale is 1:12, so every 1/4 note is 2px
      int length = (loopData[loop+firstLoop].end-loopData[loop+firstLoop].start)/z;
      int shade = (loopData[loop+firstLoop].start+loopData[loop+firstLoop].end+loop+firstLoop)%12+2;

      //clearing background
      display.fillRect(xStart, yStart+(loopHeight+3)*loop, length, loopHeight, 0);

      //play progress
      if(playing && loop+firstLoop == activeLoop)
        display.fillRect(xStart, yStart+(loopHeight+3)*loop, float(length)*float(playheadPos-loopData[activeLoop].start)/float(loopData[activeLoop].end-loopData[activeLoop].start), loopHeight, SSD1306_WHITE);

      //loop block
      display.drawRect(xStart, yStart+(loopHeight+3)*loop, length, loopHeight, 1);
      if(isLooping)
        graphics.shadeArea(xStart, yStart+(loopHeight+3)*loop, length, loopHeight,shade);
      
      uint8_t x1 = xStart+length+3;

      //active loop note
      if(loop+firstLoop == activeLoop){
        //highlight arrow
        if(highlight == activeLoop){
          graphics.drawArrow(xStart-9+((millis()/200)%2)+2, yStart+(loopHeight+3)*loop+4,2,0,true);
        }
        //loop types
        switch(loopData[activeLoop].type){
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
        x1+=graphics.printFraction_small(x1,yStart+(loopHeight+3)*loop+1,stepsToMeasures(loopData[activeLoop].end-loopData[activeLoop].start))+2;
        if(playing){
          //play/iterations
          printSmall(x1,yStart+(loopHeight+3)*loop+1, "("+stringify(loopCount)+"/"+stringify(loopData[activeLoop].reps+1)+")", SSD1306_WHITE);
          x1+=4*(stringify(loopCount).length()+stringify(loopData[activeLoop].reps+1).length()+3);
        }
        else{
          // play * iterations
          graphics.drawArrow(x1+2,yStart+(loopHeight+3)*loop+3,2,0,true);
          x1+=4;
          printSmall(x1,yStart+(loopHeight+3)*loop+1, "|"+stringify(loopData[activeLoop].reps+1), SSD1306_WHITE);
          x1+=4*(stringify(loopData[activeLoop].reps+1).length()+1)+1;
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
        switch(loopData[highlight].type){
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
        x1+=graphics.printFraction_small(x1,yStart+(loopHeight+3)*loop+1,stepsToMeasures(loopData[highlight].end-loopData[highlight].start))+2;
        // play symbol
        graphics.drawArrow(x1+2,yStart+(loopHeight+3)*loop+3,2,0,true);
        x1+=4;
        // iterations
        printSmall(x1,yStart+(loopHeight+3)*loop+1, "|"+stringify(loopData[highlight].reps+1), SSD1306_WHITE);
        x1+=4*(stringify(loopData[highlight].reps+1).length()+1);
      }
      //if there is a return loop offscreen
      //draw a return line from the first loop (or top of screen) down
      for(uint8_t l = firstLoop+maxLoopsShown-1; l<loopData.size(); l++){
        //if there is a return loop offscreen
        //or if the last loop is normal, and offscreen
        if(loopData[l].type == RETURN || (l == loopData.size()-1 && loopData[l].type == NORMAL)){
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
      switch(loopData[loop+firstLoop].type){
        //normal loop
        case 0:
          //if it's the last loop on screen, but there are more loops
          if(loop == 5){

          }
          //if it's the last loop in the sequence, make it a return
          else if(loop+firstLoop == loopData.size()-1){
            //if there's just one loop, draw it as an infinite loop (j so it looks nice)
            if(loopData.size() == 1)
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
  if(id<loopData.size() && id >=0){
    activeLoop = id;
    loopCount = 0;
  }
}

void addLoop(){
  Loop newLoop;
  newLoop.start = loopData[activeLoop].start;
  newLoop.end = loopData[activeLoop].end;
  newLoop.reps = loopData[activeLoop].reps;
  newLoop.type = loopData[activeLoop].type;
  loopData.push_back(newLoop);
  setActiveLoop(loopData.size()-1);
}

void addLoop(unsigned short int start, unsigned short int end, unsigned short int iter, unsigned short int type){
  Loop newLoop;
  newLoop.start = start;
  newLoop.end = end;
  newLoop.reps = iter;
  newLoop.type = type;
  loopData.push_back(newLoop);
}

void deleteLoop(uint8_t id){
  if(loopData.size() > 1 && loopData.size()>id){//if there's more than one loop, and id is in loopData
    vector<Loop> tempVec;
    for(int i = 0; i<loopData.size(); i++){
      if(i!=id){
        tempVec.push_back(loopData[i]);
      }
    }
    loopData.swap(tempVec);
    //if activeLoop was the loop that got deleted, or above it
    //decrement it's id so it reads correct (and existing) data
    if(activeLoop>=loopData.size()){  
      activeLoop = loopData.size()-1;
    }
  }
  setActiveLoop(activeLoop);
}

void toggleLoop(){
  isLooping = !isLooping;
}

void setLoopToInfinite(uint8_t targetL){
  //if it's already a 3, set it to 0
  if(loopData[targetL].type == INFINITE){
    loopData[targetL].type = NORMAL;
  }
  //if not, set this loop to 3
  else{
    loopData[targetL].type = INFINITE;
  }
  //set all other inf loops to 0
  for(uint8_t l = 0; l<loopData.size(); l++){
    if(l != targetL){
      if(loopData[l].type == INFINITE)
        loopData[l].type = NORMAL;
    }
  }
}

//moves to the next loop in loopSeq
void nextLoop(){
  loopCount = 0;
  if(loopData.size()>1){
    switch(loopData[activeLoop].type){
      case NORMAL:
        //move to next loop
        if(activeLoop < loopData.size()-1)
          activeLoop++;
        else
          activeLoop = 0;
        if(playing)
          playheadPos = loopData[activeLoop].start;
        if(recording)
          recheadPos = loopData[activeLoop].start;
        break;
      case RANDOM:{
        activeLoop = random(0,loopData.size());
        if(playing)
          playheadPos = loopData[activeLoop].start;
        if(recording)
          recheadPos = loopData[activeLoop].start;
        break;}
      case RANDOM_SAME:{
        //move to next loop
        if(activeLoop < loopData.size()-1)
          activeLoop++;
        else
          activeLoop = 0;
        //if rnd of same size mode, choose a random loop
        int currentLength = loopData[activeLoop].end - loopData[activeLoop].start;
        vector<uint8_t> similarLoops;
        for(int i = 0; i<loopData.size(); i++){
          int len = loopData[i].end-loopData[i].start;
          if(len == currentLength){
            similarLoops.push_back(i);
          }
        }
        activeLoop = similarLoops[random(0,similarLoops.size())];
        if(playing)
          playheadPos = loopData[activeLoop].start;
        if(recording)
          recheadPos = loopData[activeLoop].start;
        break;}
      case RETURN:{
        activeLoop = 0;
        if(playing)
          playheadPos = loopData[activeLoop].start;
        if(recording)
          recheadPos = loopData[activeLoop].start;
        break;}
      case INFINITE:{
        if(playing)
          playheadPos = loopData[activeLoop].start;
        if(recording)
          recheadPos = loopData[activeLoop].start;
        break;}
    }
  }
}


//cuts notes off when loop repeats, then starts new note at beginning
void cutLoop(){
  for(int i = 0; i<trackData.size(); i++){
    if(trackData[i].noteLastSent != 255){
      seqData[i][seqData[i].size()-1].endPos = loopData[activeLoop].end;
      //if it's about to loop again (if it's a one-shot recording, there's no need to make a new note)
      if(recMode != ONESHOT)
        writeNoteOn(loopData[activeLoop].start,trackData[i].pitch,seqData[i][seqData[i].size()-1].velocity,trackData[i].channel);
    }
  }
}

//this checks loops bounds, moves to next loop, and cuts loop
void checkLoop(){
  if(playing){
    if (playheadPos > loopData[activeLoop].end-1) { //if the timestep is past the end of the loop, loop it to the start
      loopCount++;
      if(loopCount > loopData[activeLoop].reps){
        nextLoop();
      }
      playheadPos = loopData[activeLoop].start;
      if(!isLooping)
        togglePlayMode();
    }
  }
  else if(recording){
    //one-shot record to current loop, without looping
    if(recMode == ONESHOT){
      if(recheadPos>=loopData[activeLoop].end){
        toggleRecordingMode(waitForNoteBeforeRec);
      }
    }
    //record to one loop over and over again
    else if(recMode == LOOP_MODE){
      if(recheadPos>=loopData[activeLoop].end){
        recheadPos = loopData[activeLoop].start;
      }
    }
    //record to loops as they play in sequence
    else if(recMode == LOOPSEQUENCE){
      if(recheadPos>=loopData[activeLoop].end){
        cutLoop();
        loopCount++;
        if(loopData[activeLoop].reps>=loopCount){
          nextLoop();
        }
        recheadPos = loopData[activeLoop].start;
      }
    }
  }
}