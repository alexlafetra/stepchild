//Fit 'A' into 'B'

// 'into', 11x13px
const unsigned char into_bmp [] = {
	0xe0, 0x00, 0x42, 0x00, 0x47, 0x00, 0xe9, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x20, 0x01, 0xe0, 
	0x00, 0x20, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x20, 0x00, 0xc0
};


struct NoteTrackPair{
public:
    Note note;
    uint8_t trackID;
};

CoordinatePair selectArea_warp(bool AorB){
  CoordinatePair coords;
  coords.x1 = 0;
  coords.x2 = 0;
  coords.y1 = 0;
  coords.y2 = 0;
  while(true){
    readJoystick();
    readButtons();
    defaultEncoderControls();
    if(sel && !selBox.begun && (x != 0 || y != 0)){
      selBox.begun = true;
      selBox.x1 = cursorPos;
      selBox.y1 = activeTrack;
      coords.x1 = cursorPos;
      coords.y1 = activeTrack;
    }
    //if sel is released, and there's a selection box
    if(!sel && selBox.begun){
      selBox.x2 = cursorPos;
      selBox.y2 = activeTrack;
      selBox.begun = false;
      coords.x2 = cursorPos;
      coords.y2 = activeTrack;
    }
    if(itsbeen(200)){
      if(n){
        lastTime = millis();
        return coords;
      }
      if(menu_Press){
        coords.x1 = 0;
        coords.x2 = 0;
        coords.y1 = 0;
        coords.y2 = 0;
        lastTime = millis();
        return coords;
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
          setActiveTrack(activeTrack + 1, true);
        lastTime = millis();
      }
      if (y == -1) {
        if(recording)
          setActiveTrack(activeTrack - 1, false);
        else
          setActiveTrack(activeTrack - 1, true);
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
    drawCoordinateBox(coords);
    if(coords.x1 == 0 && coords.x2 == 0 && coords.y1 == 0 && coords.y2 == 0){
      printSmall(trackDisplay,0,"warp",1);
      if(AorB){
        if(millis()%500>250)
          printItalic(trackDisplay+16,0,"A",1);
        printItalic(trackDisplay+42,0,"B",1);
      }
      else{
        printItalic(trackDisplay+16,0,"A",1);
        if(millis()%500>250)
          printItalic(trackDisplay+42,0,"B",1);
      }
      printSmall(trackDisplay+25,0,"into",1);
    }
    else{
      printSmall(trackDisplay,0,"[n] to submit",1);
      if(millis()%500>250)
        printItalic(trackDisplay+50,0,AorB?"A":"B",1);
    }
    drawWarpIcon(8,2,11,true);
    display.display();
  }
}

//Warps the notes contained within the 'A' coordinates into the 'B' coordinates
bool warpAintoB(CoordinatePair A, CoordinatePair B, bool onlySelected){
  //checking bounds
  if(A.x1 == A.x2 || B.x1 == B.x2){
      return false;
  }
  if(A.x1>A.x2){
      uint16_t temp = A.x2;
      A.x2 = A.x1;
      A.x1 = temp;
  }
  if(B.x1>B.x2){
      uint16_t temp = B.x2;
      B.x2 = B.x1;
      B.x1 = temp;
  }

  //this is the scale of the two areas
  float scaleFactor = float(abs(B.x2-B.x1))/float(abs(A.x2-A.x1));

  //you need to multiply every note length by that value
  //and also:
  //set the start of the note to be relative
  //to  the start of coordinatePair B * the scaleFactor

  vector<NoteTrackPair> newNotes;
  for(uint8_t track = 0; track<trackData.size(); track++){
      for(uint8_t noteID = 1; noteID<seqData[track].size(); noteID++){
          //if the note starts SOMEWHERE within A, warp it!
          //you might want to change this/add an option
          //to ONLY warp notes that are entirely contained within A
          if(seqData[track][noteID].startPos>=A.x1 && seqData[track][noteID].startPos<A.x2 && ((onlySelected && seqData[track][noteID].isSelected) || !onlySelected)){
              Note targetNote = seqData[track][noteID];
              uint16_t oldLength = targetNote.getLength();
              uint16_t distanceFromStartOfA = targetNote.startPos - A.x1;

              //deleting old note
              deleteNote_byID(track,noteID);
              //make sure to decrement noteID! so you don't warp the same note twice or skip a note
              noteID--;

              Note newNote = targetNote;
              newNote.startPos = distanceFromStartOfA*scaleFactor+B.x1;
              newNote.endPos = newNote.startPos+(oldLength-1)*scaleFactor;
              // makeNote(newNote,track,false);
              // newNote.push_back(newNote);
              NoteTrackPair newNT;
              newNT.note = newNote;
              newNT.trackID = track;
              newNotes.push_back(newNT);
          }
      }
  }
  //iterating over the newNotes vector and making each note
  for(uint8_t note = 0; note<newNotes.size(); note++){
      makeNote(newNotes[note].note,newNotes[note].trackID,false);
  }
  return true;
}

//flag for only warping selected notes
void selectAreasAndWarp(bool onlySelected){
    CoordinatePair A = selectArea_random();
    CoordinatePair B = selectArea_random();
    warpAintoB(A,B,onlySelected);
}

void warp(){
  CoordinatePair A;
  CoordinatePair B;
  //0 is A, 1 is B, 2 is warp
  uint8_t cursor = 0;
  while(true){
    A = selectArea_warp(true);
    if(A.isEmpty())
      break;
    B = selectArea_warp(false);
    if(B.isEmpty())
      break;
    //only warps selected notes, if any are selected
    if(!warpAintoB(A,B,selectionCount != 0)){
      break;
    }
  }
}
