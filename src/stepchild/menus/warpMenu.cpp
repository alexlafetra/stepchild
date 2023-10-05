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

CoordinatePair selectArea_warp(){
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
      printSmall(trackDisplay,0,"select an area!",1);
    }
    else{
      printSmall(trackDisplay,0,"[n] to submit",1);
    }
    drawWarpIcon(0,0,12,true);
    display.display();
  }
}

//Warps the notes contained within the 'A' coordinates into the 'B' coordinates
void warpAintoB(CoordinatePair A, CoordinatePair B, bool onlySelected){
    //checking bounds
    if(A.x1 == A.x2 || B.x1 == B.x2){
        return;
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
}

//flag for only warping selected notes
void selectAreasAndWarp(bool onlySelected){
    CoordinatePair A = selectArea_random();
    CoordinatePair B = selectArea_random();
    warpAintoB(A,B,onlySelected);
}

void drawWarp(uint8_t x1, uint8_t y1, uint8_t cursor, bool onlySelected, CoordinatePair A, CoordinatePair B, bool A_selected, bool B_selected){
    

    if(cursor == 0){
        drawCenteredBracket(x1,y1-15,32+2*sin(millis()/100),17+2*sin(millis()/100));
        printSmall(x1-14,y1-16,"select A",1);
    }
    else{
        drawCenteredBracket(x1, y1-15, 32, 17);
        printItalic(x1-3,y1-16,"A",1);
    }
    drawCheckbox(x1-28,y1-17,A_selected,cursor == 0);
    
    if(cursor == 1){
        drawCenteredBracket(x1+21,y1+13,32+2*sin(millis()/100),17+2*sin(millis()/100));
        printSmall(x1+7,y1+12,"select B",1);
    }
    else{
        drawCenteredBracket(x1+21, y1+13, 32, 17);
        printItalic(x1+18,y1+12,"B",1);
    }
    drawCheckbox(x1-7,y1+11,B_selected,cursor == 1);

    // printArp_wiggly(0,3,"warp:",1);
    printItalic_wiggly(0,2,"warp",1);
    display.drawBitmap(x1+22,y1-15,into_bmp,11,13,1);
    drawArrow(x1+30,y1+2+sin(millis()/100),2,3,true);

    drawArrow(x1+48+sin(millis()/100),y1+14,4,0,true);
    if(cursor == 2){
        drawCenteredBracket(x1+62,y1+15,14+sin(millis()/100),12+2*sin(millis()/100));
    }
    else{
        drawCenteredBracket(x1+62,y1+15,14,14);
    }
    drawWarpIcon(x1+57, y1+10, 11, cursor==2);

    printSmall(10,40,"target",1);
    drawLabel(22,48,onlySelected?"selected":"all",true);
    printSmall(12,56,"notes",1);
}

void warp(){
    bool onlySelected = false;
    CoordinatePair A;
    CoordinatePair B;
    bool A_selected = false;
    bool B_selected = false;
    //0 is A, 1 is B, 2 is warp
    uint8_t cursor = 0;
    while(true){
        readButtons();
        readJoystick();
        while(counterA != 0){
            onlySelected = !onlySelected;
            counterA += counterA<0?1:-1;
        }
        while(counterB != 0){
            onlySelected = !onlySelected;
            counterB += counterB<0?1:-1;
        }
        if(itsbeen(200)){
            if(y == -1 && cursor>0){
                cursor--;
                lastTime = millis();
            }
            else if(y == 1 && cursor<2){
                cursor++;
                lastTime = millis();
            }
            if(x == 1 && cursor>0){
                cursor--;
                lastTime = millis();
            }
            else if(x == -1 && cursor<2){
                cursor++;
                lastTime = millis();
            }
            if(menu_Press){
                lastTime = millis();
                break;
            }
            if(sel){
                switch(cursor){
                    //select A
                    case 0:
                        A = selectArea_warp();
                        A_selected = true;
                        cursor = 1;
                        lastTime = millis();
                        break;
                    //select B
                    case 1:
                        B = selectArea_warp();
                        B_selected = true;
                        cursor = 2;
                        lastTime = millis();
                        break;
                    //commit
                    case 2:
                        //if both areas have been selected
                        if(A_selected && B_selected){
                            warpAintoB(A,B,onlySelected);
                            lastTime = millis();
                            return;
                        }
                        break;
                }
            }
        }
        display.clearDisplay();
        drawWarp(50,32,cursor,onlySelected,A,B,A_selected,B_selected);
        display.display();
    }
}
