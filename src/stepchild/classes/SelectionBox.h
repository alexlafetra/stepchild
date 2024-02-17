//Holds coordinates and a flag set when the SelectionBox has been started
class SelectionBox{
  public:
  uint16_t x1;
  uint16_t y1;
  uint16_t x2;
  uint16_t y2;
  bool begun;
  SelectionBox();
  void displaySelBox();
};

SelectionBox::SelectionBox(){
  x1 = 0;
  y1 = 0;
  x2 = 0;
  y2 = 0;
  begun = false;
}

void SelectionBox::displaySelBox(){
  x2 = cursorPos;
  y2 = activeTrack;

  unsigned short int startX;
  unsigned short int startY;
  unsigned short int len;
  unsigned short int height;

  unsigned short int X1;
  unsigned short int X2;
  unsigned short int Y1;
  unsigned short int Y2;

  if(x1>x2){
    X1 = x2;
    X2 = x1;
  }
  else{
    X1 = x1;
    X2 = x2;
  }
  if(y1>y2){
    Y1 = y2;
    Y2 = y1;
  }
  else{
    Y1 = y1;
    Y2 = y2;
  }

  startX = trackDisplay+(X1-viewStart)*scale;
  len = (X2-X1)*scale;

  //if box starts before view
  if(X1<viewStart){
    startX = trackDisplay;//box is drawn from beggining, to x2
    len = (X2-viewStart)*scale;
  }
  //if box ends past view
  if(X2>viewEnd){
    len = (viewEnd-X1)*scale;
  }

  //same, but for tracks
  uint8_t startHeight = (menuIsActive||maxTracksShown==5)?headerHeight:8;
  startY = (Y1-startTrack)*trackHeight+startHeight;
  height = ((Y2+1-startTrack)*trackHeight)+startHeight - startY;
  if(Y1<startTrack){
    startY = startHeight;
    height = ((Y2 - startTrack + 1)*trackHeight - startY)%(screenHeight-startHeight) + startHeight;
  }
  display.drawRect(startX, startY, len, height, SSD1306_WHITE);
  display.drawRect(startX+1, startY+1, len-2, height-2, SSD1306_WHITE);

  if(len>5 && height>=trackHeight){
    display.fillRect(startX+2,startY+2, len-4, height-4, SSD1306_BLACK);
    graphics.shadeArea(startX+2,startY+2, len-4, height-4,10);
  }
}

SelectionBox selBox;


//returns a 2D vector containing a row for each track and a copy of each note that's currently selected on each track
vector<vector<Note>> grabSelectedNotes(){
    vector<vector<Note>> list;
    for(uint8_t track = 0; track<trackData.size(); track++){
        vector<Note> selectedNotesOnTrack;
        for(uint16_t note = 1; note<seqData[track].size(); note++){
            if(seqData[track][note].isSelected){
                selectedNotesOnTrack.push_back(seqData[track][note]);
            }
        }
        list.push_back(selectedNotesOnTrack);
    }
    return list;
}
vector<vector<Note>> grabAndDeleteSelectedNotes(){
    vector<vector<Note>> list;
    for(uint8_t track = 0; track<trackData.size(); track++){
        vector<Note> selectedNotesOnTrack;
        for(uint16_t note = 1; note<seqData[track].size(); note++){
          if(seqData[track][note].isSelected){
            selectedNotesOnTrack.push_back(seqData[track][note]);
            deleteNote_byID(track,note);
          }
        }
        list.push_back(selectedNotesOnTrack);
    }
    return list;
}

vector<vector<uint8_t>> selectMultipleNotes(String text1, String text2){
  vector<vector<uint8_t>> selectedNotes;
  selectedNotes.resize(trackData.size());
  menuIsActive = false;
  bool movingBetweenNotes = false;
  while(true){
    readButtons();
    readJoystick();
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
      if(selBox.x1>selBox.x2){
        unsigned short int x1_old = selBox.x1;
        selBox.x1 = selBox.x2;
        selBox.x2 = x1_old;
      }
      if(selBox.y1>selBox.y2){
        unsigned short int y1_old = selBox.y1;
        selBox.y1 = selBox.y2;
        selBox.y2 = y1_old;
      }
      for(int track = selBox.y1; track<=selBox.y2; track++){
        for(int time = selBox.x1; time<=selBox.x2; time++){
          if(lookupData[track][time] != 0){
            //if the note isn't in the vector yet, add it
            if(!isInVector(lookupData[track][time],selectedNotes[track]))
              selectedNotes[track].push_back(lookupData[track][time]);
            time = seqData[track][lookupData[track][time]].endPos;
          }
        }
      }
      selBox.begun = false;
    }
    if(itsbeen(100)){
      if(!movingBetweenNotes){
        if(y == 1){
          setActiveTrack(activeTrack+1,false);
          lastTime = millis();
        }
        if(y == -1){
          setActiveTrack(activeTrack-1,false);
          lastTime = millis();
        }
      }
    }
    if(itsbeen(100)){
      if(x != 0){
        if(!movingBetweenNotes){
          if (x == 1 && !shift) {
            if(cursorPos%subDivInt){
              moveCursor(-cursorPos%subDivInt);
              lastTime = millis();
            }
            else{
              moveCursor(-subDivInt);
              lastTime = millis();
            }
          }
          if (x == -1 && !shift) {
            if(cursorPos%subDivInt){
              moveCursor(subDivInt-cursorPos%subDivInt);
              lastTime = millis();
            }
            else{
              moveCursor(subDivInt);
              lastTime = millis();
            }
          }
        }
        else{
          if(x == 1){
            moveToNextNote(false,false);
            lastTime = millis();
          }
          else if(x == -1){
            moveToNextNote(true,false);
            lastTime = millis();
          }
        }
      }
    }
    if(itsbeen(200)){
      //select
      if(sel && lookupData[activeTrack][cursorPos] != 0 && !selBox.begun){
        unsigned short int id;
        id = lookupData[activeTrack][cursorPos];
        if(shift){
          //delete old vec
          vector<vector<uint8_t>> temp;
          temp.resize(trackData.size());
          selectedNotes = temp;
          selectedNotes[activeTrack].push_back(lookupData[activeTrack][cursorPos]);
        }
        else{
          //if the note isn't in the vector yet, add it
          if(!isInVector(lookupData[activeTrack][cursorPos],selectedNotes[activeTrack]))
            selectedNotes[activeTrack].push_back(lookupData[activeTrack][cursorPos]);
          //if it is, remove it
          else{
            vector<uint8_t> temp;
            for(int i = 0; i<selectedNotes[activeTrack].size(); i++){
              //push back all the notes that aren't the one the cursor is on
              if(selectedNotes[activeTrack][i] != lookupData[activeTrack][cursorPos]){
                temp.push_back(selectedNotes[activeTrack][i]);
              }
            }
            selectedNotes[activeTrack] = temp;
          }
        }
        lastTime = millis();
      }
      if(del){
        lastTime = millis();
        selectedNotes.clear();
        break;
      }
      if(n){
        n = false;
        lastTime = millis();
        break;
      }
      if(loop_Press){
        lastTime = millis();
        movingBetweenNotes = !movingBetweenNotes;
      }
    }
    display.clearDisplay();
    drawSeq(true,false,false,false,false);
    printSmall(0,0,text1,SSD1306_WHITE);
    printSmall(0,8,text2,SSD1306_WHITE);
    if(movingBetweenNotes){
      if(millis()%1000 >= 500){
        display.drawBitmap(6,0,arrow_1_bmp,16,16,SSD1306_WHITE);
      }
      else{
        display.drawBitmap(6,0,arrow_3_bmp,16,16,SSD1306_WHITE);
      }
    }
    //draw a note bracket on any note that's been added to the selection
    for(int track = 0; track<selectedNotes.size(); track++){
      for(int note = 0; note<selectedNotes[track].size(); note++){
        drawNoteBracket(seqData[track][selectedNotes[track][note]],track);
      }
    }
    display.display();
  }
  menuIsActive = true;
  return selectedNotes;
}

//Enter note selection screen, and display a custom icon in the corner
//Used for the FX a lot
bool selectNotes(String text, void (*iconFunction)(uint8_t,uint8_t,uint8_t,bool)){
  while(true){
    readJoystick();
    readButtons();
    defaultEncoderControls();
    if(sel && !selBox.begun && (x != 0 || y != 0)){
      selBox.begun = true;
      selBox.x1 = cursorPos;
      selBox.y1 = activeTrack;
    }
    //if sel is released, and there's a selection box
    if(!sel && selBox.begun){
      selBox.x2 = cursorPos;
      selBox.y2 = activeTrack;
      selBox.begun = false;
      selectBox();
    }
    if(itsbeen(200)){
      if(n){
        lastTime = millis();
        return true;
      }
      if(menu_Press){
        clearSelection();
        lastTime = millis();
        return false;
      }
      if(sel){
        if(shift){
          clearSelection();
          toggleSelectNote(activeTrack,getIDAtCursor(),false);
        }
        else{
          toggleSelectNote(activeTrack,getIDAtCursor(),true);
        }
        lastTime = millis();
      }
    }
    if (itsbeen(100)) {
      if (x == 1 && !shift) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(cursorPos%subDivInt){
          moveCursor(-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          moveCursor(-subDivInt);
          lastTime = millis();
        }
      }
      if (x == -1 && !shift) {
        if(cursorPos%subDivInt){
          moveCursor(subDivInt-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          moveCursor(subDivInt);
          lastTime = millis();
        }
      }
      if (y == 1) {
        if(recording)
          setActiveTrack(activeTrack + 1, false);
        else
          setActiveTrack(activeTrack + 1, false);
        lastTime = millis();
      }
      if (y == -1) {
        if(recording)
          setActiveTrack(activeTrack - 1, false);
        else
          setActiveTrack(activeTrack - 1, false);
        lastTime = millis();
      }
    }
    if (itsbeen(50)) {
      if (x == 1 && shift) {
        moveCursor(-1);
        lastTime = millis();
      }
      if (x == -1 && shift) {
        moveCursor(1);
        lastTime = millis();
      }
    }
    display.clearDisplay();
    drawSeq(true, false, true, false, false, false, viewStart, viewEnd);
    if(!selectionCount){
      printSmall(trackDisplay,0,"select notes to "+text+"!",1);
    }
    else{
      printSmall(trackDisplay,0,"[n] to "+text+" "+stringify(selectionCount)+(selectionCount == 1?" note":" notes"),1);
    }
    iconFunction(7,1,14,true);
    display.display();
  }
}

vector<uint16_t> getSelectedNotesBoundingBox(){
  //stored as xStart,yStart,xEnd,yEnd
  //(initially store it as dramatic as possible)
  vector<uint16_t> bounds = {seqEnd,(uint16_t)trackData.size(),0,0};
  uint16_t checkedNotes = 0;
  for(uint8_t track = 0; track<seqData.size(); track++){
    for(uint16_t note = 1; note<seqData[track].size(); note++){
      if(seqData[track][note].isSelected){
        //if it's the new highest track
        if(track<bounds[1])
          bounds[1] = track;
        //if it's the new lowest track
        if(track>bounds[3])
          bounds[3] = track;
        //if it's the new earliest note
        if(seqData[track][note].startPos<bounds[0])
          bounds[0] = seqData[track][note].startPos;
        //if it's the new latest note
        if(seqData[track][note].endPos>bounds[2])
          bounds[2] = seqData[track][note].endPos;

        //if you've checked all the selected notes, return
        checkedNotes++;
        if(checkedNotes == selectionCount)
          return bounds;
      }
    }
  }
  return bounds;
}

vector<uint8_t> getTracksWithSelectedNotes(){
  vector<uint8_t> list;
  uint16_t count;
  if(selectionCount>0){
    for(uint8_t track = 0; track<trackData.size(); track++){
      for(uint16_t note = 1; note<seqData[track].size(); note++){
        //once you find a selected note, jump to the next track
        if(seqData[track][note].isSelected){
          list.push_back(track);
          track++;
          note = 1;
        }
      }
    }
  }
  return list;
}

void clearSelection(int track, int time) {
  if(lookupData[track][time] != 0 && seqData[track][lookupData[track][time]].isSelected){
    selectionCount--;
    seqData[track][lookupData[track][time]].isSelected = false;
  }
}

void clearSelection(){
  if(selectionCount>0){
    for(int track = 0; track<trackData.size(); track++){
      if(selectionCount<=0)
          return;
      for(int note = seqData[track].size()-1; note>0; note--){
        if(selectionCount<=0)
          return;
        if(seqData[track][note].isSelected){
          seqData[track][note].isSelected = false;
          selectionCount--;
        }
      }
    }
  }
}

void deselectNote(uint8_t track, uint16_t id){
  if(seqData[track][id].isSelected){
    selectionCount--;
    seqData[track][id].isSelected = false;
  }
}

void selectNotesInTrack(uint8_t track){
  for(uint16_t note = 1; note<seqData.size(); note++){
    if(!seqData[track][note].isSelected){
      seqData[track][note].isSelected = true;
      selectionCount++;
    }
  }
}

//select a note
void selectNote(uint8_t track, uint16_t id){
  //if it's already selected
  if(seqData[track][id].isSelected)
    return;
  seqData[track][id].isSelected = true;
  selectionCount++;
}

//togglet a note's selection state  by it's track and ID
void toggleSelectNote(uint8_t track, uint16_t id, bool additive){
    //if id == 0, just return
    if(!id){
      return;
    }
    if(!additive&&!seqData[track][id].isSelected){
      clearSelection();
      selectNote(track,id);
    }
    else{
      if(seqData[track][id].isSelected && selectionCount>0)
        deselectNote(track,id);
      else if(!seqData[track][id].isSelected){
        selectNote(track,id);
      }
    }
}


void selectAllNotesInTrack(){
  for(uint16_t i = 1; i<seqData[activeTrack].size();i++){
    selectNote(activeTrack, i);
  }
}

//selects all notes in a sequence, or in a track (or at a timestep maybe? not sure if that'd be useful for flow)
void selectAll() {
  for(uint8_t track = 0; track<trackData.size(); track++){
    for(uint16_t id = 1; id<seqData[track].size(); id++){
      selectNote(track,id);
    }
  }
}

void selectBox(){
  if(selBox.x1>selBox.x2){
    unsigned short int x1_old = selBox.x1;
    selBox.x1 = selBox.x2;
    selBox.x2 = x1_old;
  }
  if(selBox.y1>selBox.y2){
    unsigned short int y1_old = selBox.y1;
    selBox.y1 = selBox.y2;
    selBox.y2 = y1_old;
  }
  for(int track = selBox.y1; track<=selBox.y2; track++){
    for(int time = selBox.x1; time<selBox.x2; time++){//< and not <= so it doesn't grab trailing notes
      if(lookupData[track][time] != 0){
        //this is a little inconsistent with how select usually works, but it allows whatever's in the box to DEFINITELY be selected.
        //it makes sense (a little) because it seems rare that you would ever need to deselect notes using the box
        selectNote(track, lookupData[track][time]);
        time = seqData[track][lookupData[track][time]].endPos-1;
      }
    }
  }
}


void copy(){
  if(selectionCount == 0) 
    return;
  //making sure buffer has enough 'columns' to store the notes from each track
  //clearing copybuffer
  vector<vector<Note>>temp1;
  vector<Note> temp2 = {};
  for(int i = 0; i<trackData.size(); i++){
    temp1.push_back(temp2);
  }
  copyBuffer.swap(temp1);

  copyPos[0] = activeTrack;
  copyPos[1] = cursorPos;

  uint16_t numberOfNotes = 0;

  //add all selected notes to the copy buffer
  if(selectionCount>0){
    for(int track = 0; track<trackData.size(); track++){
      for(int note = 1; note<=seqData[track].size()-1; note++){// <= bc notes aren't 0 indexed
        if(seqData[track][note].isSelected){
          copyBuffer[track].push_back(seqData[track][note]);
          numberOfNotes++;
        }
      }
    }
    clearSelection();
  }
  //or if there's a target note, but it's not selected
  else if(lookupData[activeTrack][cursorPos] != 0){
    copyBuffer[activeTrack].push_back(seqData[activeTrack][lookupData[activeTrack][cursorPos]]);
    numberOfNotes = 1;
  }
  menuText = "copied "+stringify(numberOfNotes)+((stringify(numberOfNotes)=="1")?" note":" notes");
}

void copyLoop(){
  //clear copyBuffer
  while(copyBuffer.size()>0){
    copyBuffer.pop_back();
  }
  //making sure buffer has enough 'columns' to store the notes from each track
  copyBuffer.resize(trackData.size());
  //treat copying the loop like you're copying it from the start of the loop
  copyPos[0] = 0;
  copyPos[1] = loopData[activeLoop].start;
  //add all selected notes to the copy buffer
  if(loopData[activeLoop].end-loopData[activeLoop].start>0){
    for(int track = 0; track<trackData.size(); track++){
      for(int step = loopData[activeLoop].start; step<=loopData[activeLoop].end; step++){// <= bc notes aren't 0 indexed
        if(lookupData[track][step] != 0){
          copyBuffer[track].push_back(seqData[track][lookupData[track][step]]);
          //move to the end of the note, so it's not double-counted
          step = seqData[track][lookupData[track][step]].endPos;
        }
      }
    }
  }
}

//pastes copybuffer
void paste(){
  if(copyBuffer.size()>0){
    uint16_t pastedNotes;
    //offset of all the notes (relative to where they were copied from)
    int yOffset = activeTrack - copyPos[0];
    int xOffset = cursorPos - copyPos[1];
    //moves through each track and note in copyBuffer, places a note at those positions in the seq
    for(int tracks = 0; tracks<copyBuffer.size(); tracks++){//for each track in the copybuffer
      if(copyBuffer[tracks].size()>0 && tracks+yOffset>=0 && tracks+yOffset<trackData.size()){//if there's a note stored for this track, and it'd be copied according to the new activeTrack
        for(int notes = 0; notes<copyBuffer[tracks].size(); notes++){
          if(copyBuffer[tracks][notes].startPos + xOffset<= seqEnd){
            //if the note will be copied to someWhere within the seqence, make note
            int start = xOffset+copyBuffer[tracks][notes].startPos;
            int end = xOffset+copyBuffer[tracks][notes].endPos;
            int track = tracks+yOffset;
            unsigned char vel = copyBuffer[tracks][notes].velocity;
            unsigned char chance = copyBuffer[tracks][notes].chance;
            bool mute = copyBuffer[tracks][notes].muted;
            //if note ends past seq, truncate it
            if(end>seqEnd){
              end = seqEnd;
            }
            //if note begins before seq, but also extends into seq, truncate it
            if(start<0 && end>0){
              start = 0;
            }
            else if(start<0 && end<=0){
              continue;
            }
            if(track<0||track>=trackData.size()){
              continue;
            }
            Note newNoteOn(start,end, vel, chance, mute, true);
            makeNote(newNoteOn,track,false);
            pastedNotes++;
          }
        }
      }
    }
  menuText = "pasted "+stringify(pastedNotes)+((stringify(pastedNotes)=="1")?" note":" notes");
  }
}

