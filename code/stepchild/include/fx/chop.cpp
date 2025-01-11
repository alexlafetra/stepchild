// 'chop', 12x12px
const unsigned char chop1_bmp []  = {
	0x03, 0x80, 0x1c, 0x80, 0x62, 0x40, 0x20, 0x40, 0x20, 0x20, 0x10, 0xe0, 0x17, 0x70, 0x18, 0x30, 
	0x00, 0x30, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00
};
const unsigned char chop2_bmp []  = {
	0x08, 0x30, 0x88, 0x70, 0x41, 0xe0, 0x06, 0xc0, 0x18, 0x80, 0x20, 0x80, 0x28, 0x80, 0x20, 0x80, 
	0x21, 0x30, 0x26, 0x00, 0x28, 0x80, 0x30, 0x40
};



uint8_t getVelWithSlope(uint16_t start, uint16_t end, uint16_t location, float& velSlope){
    if(velSlope<0){
        //start is given as (start,127), with slop velSlope, find (end,X)
       return velSlope*(location - start)+127;
    }
    else if(velSlope>0){
        //end is given as (end,127), with slop velSlope, find (start,X)
       return (velSlope * (location - end) + 127);
    }
    else return 127;
}

vector<NoteTrackPair> getChoppedNotes(vector<NoteTrackPair> targetNotes, uint8_t pieces, float& velSlope){
    vector<NoteTrackPair> choppedNotes = {};
    for(uint8_t i = 0; i<targetNotes.size(); i++){
        uint16_t length = targetNotes[i].note.endPos - targetNotes[i].note.startPos;
        //the new length of each sub note
        uint16_t newLength = length/pieces;

        //if the length is too short to cut, don't
        if(newLength<2)
            return choppedNotes;

        for(uint16_t N = 0; N<pieces; N++){
            Note newNote = targetNotes[i].note;
            //if you're about to make the last note, set newLength to be equal to the remaining length
            if(N == pieces-1){
                newNote.startPos = targetNotes[i].note.startPos+N*newLength;
                newNote.endPos = newNote.startPos+length-N*newLength;
            }
            else{
                newNote.startPos = targetNotes[i].note.startPos+N*newLength;
                newNote.endPos = newNote.startPos+newLength;
            }
            newNote.velocity = getVelWithSlope(targetNotes[i].note.startPos, targetNotes[i].note.endPos,newNote.startPos,velSlope);
            choppedNotes.push_back(NoteTrackPair(newNote,targetNotes[i].trackID));
        }
    }
    return choppedNotes;
}

//returns a list of chopped notes
vector<Note> getChoppedNotes(Note targetNote, uint8_t pieces, float& velSlope){
    vector<Note> choppedNotes = {};
    uint16_t length = targetNote.endPos - targetNote.startPos;
    //the new length of each sub note
    uint16_t newLength = length/pieces;

    //if the length is too short to cut, don't
    if(newLength<2)
        return choppedNotes;

    for(uint16_t N = 0; N<pieces; N++){
        Note newNote = targetNote;
        //if you're about to make the last note, set newLength to be equal to the remaining length
        if(N == pieces-1){
          newNote.startPos = targetNote.startPos+N*newLength;
          newNote.endPos = newNote.startPos+length-N*newLength;
        }
        else{
          newNote.startPos = targetNote.startPos+N*newLength;
          newNote.endPos = newNote.startPos+newLength;
        }
        newNote.velocity = getVelWithSlope(targetNote.startPos,targetNote.endPos,newNote.startPos,velSlope);
        choppedNotes.push_back(newNote);
    }
    return choppedNotes;
}

void drawChopIcon(uint8_t x1, uint8_t y1, uint8_t height, bool animated){
    uint16_t timing = 1000;
    if(animated && (millis()%timing>(timing/2)))
        display.drawBitmap(x1-1,y1-1,chop1_bmp,12,12,1);
    else
        display.drawBitmap(x1,y1,chop2_bmp,12,12,1);
}

bool chop(vector<NoteID> noteIDs){
    //store the note and remove it from the sequence
    vector<NoteTrackPair> targetNotes = {};
    for(uint16_t i = 0; i<noteIDs.size(); i++){
        targetNotes.push_back(NoteTrackPair(noteIDs[i].getNote(),noteIDs[i].track));
        sequence.deleteNote(noteIDs[i].track,noteIDs[i].id);
    }

    //set up render settings
    SequenceRenderSettings settings;
    settings.topLabels = false;
    settings.drawPram = false;
    settings.drawLoopFlags = false;

    //number of pieces
    uint8_t numberOfPieces = 1;
    bool maxedOut = false;
    bool changed = false;

    //fading in/out velocity
    float velSlope = 1.0;//0 is constant, -1 is going from 127 --> 0, 1 is going from 0 --> 127 (basically velSlope is the slope)
    //newly created notes (start w/ copies of the currently selected note)
    vector<NoteTrackPair> notes = {};
    for(uint16_t i = 0; i<targetNotes.size(); i++){
        notes.push_back(NoteTrackPair(targetNotes[i].note,targetNotes[i].trackID));
    }
    while(true){
        controls.readJoystick();
        controls.readButtons();
        //change chop
        while(controls.counterA){
            if(controls.counterA < 0 && numberOfPieces > 1){
                numberOfPieces--;
                changed = true;
            }
            else if(controls.counterA > 0 && !maxedOut){
                numberOfPieces++;
                changed = true;
            }
            controls.countDownA();
        }
        //change vel
        while(controls.counterB){
            if(controls.counterB < 0 && velSlope > -1.0){
                velSlope -= 0.1;
                changed = true;
            }
            else if(controls.counterB > 0 && velSlope < 1.0){
                velSlope += 0.1;
                changed = true;
            }
            controls.countDownB();
        }
        if(utils.itsbeen(200)){
            //cancel out of it
            if(controls.MENU()){
                lastTime = millis();
                for(uint16_t i = 0; i<targetNotes.size(); i++){
                    sequence.makeNote(targetNotes[i].note,targetNotes[i].trackID,false);
                }
                return false;
            }
            //change vel slope
            if(controls.joystickX == 1 && velSlope < 1.0){
                lastTime = millis();
                velSlope += 0.1;
                changed = true;
            }
            if(controls.joystickX == -1 && velSlope > -1.0){
                lastTime = millis();
                velSlope -= 0.1;
                changed = true;
            }
            //increase the number of chops
            if(controls.joystickY == -1 && !maxedOut){
                lastTime = millis();
                numberOfPieces++;
                changed = true;
            }
            //decrease the number of chops
            else if(controls.joystickY == 1 && numberOfPieces > 1){
                lastTime = millis();
                numberOfPieces--;
                changed = true;
            }
            //committing the notes
            if(controls.NEW()){
                lastTime = millis();
                for(uint16_t i = 0; i<notes.size(); i++){
                    sequence.makeNote(notes[i].note,notes[i].trackID);
                }
                return true;
            }
        }
        //update chopped notes if any changes were made
        if(changed){
            notes = getChoppedNotes(targetNotes,numberOfPieces,velSlope);
            if(!notes.size()){
                maxedOut = true;
                numberOfPieces--;
                notes = getChoppedNotes(targetNotes,numberOfPieces,velSlope);
            }
            else{
                maxedOut = false;
            }
            changed = false;
        }

        //update display
        display.clearDisplay();
        drawSeq(settings);
        uint16_t timing = 2000/numberOfPieces;
        if(millis()%timing>(timing/2))
            display.drawBitmap(4,4,chop2_bmp,12,12,1);
        else
            display.drawBitmap(3,3,chop1_bmp,12,12,1);
        printSmall(20,0,"chopping into ",1);
        printCursive(71,0,stringify(numberOfPieces),1);
        printSmall(80+numberOfPieces/10*6,0,"pcs",1);
        printSmall(20,8,"roll:",1);
        //drawing vel slope display
        if(velSlope<0){
            display.drawLine(40,8,60,8 - velSlope*6,1);
        }
        else if(velSlope>0){
            display.drawLine(40,8+velSlope*6,60,8,1);
        }
        else{
            display.drawFastHLine(30,10,20,1);
        }
        //tooltip
        printSmall(105,0,"[n]:",1);
        graphics.drawCheckmark(118,0,7,1);
        printSmall(94,8,"[menu]: x",1);
        //drawing notes
        for(uint16_t i = 0; i<notes.size(); i++){
            drawNote(notes[i].note,notes[i].trackID,settings);
        }
        display.display();
    }
    return false;
}

bool getNotesToChop(){
    if(selectNotes("chop",drawChopIcon)){
        return chop(getSelectedNoteIDs());
    }
    else
        return false;
}

void chopNoteAt(uint8_t track, uint16_t step){
    if(!sequence.IDAt(track,step))
        return;
    chop(vector<NoteID> {NoteID(track,sequence.IDAt(track,step))});
}