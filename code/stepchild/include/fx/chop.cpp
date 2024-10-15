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

//stores note, deletes it, then lets the user select how many notes to make
void chop(uint8_t track, uint16_t step){
    //make sure there's a note there
    if(!sequence.IDAt(track,step))
        return;
    //store the note and remove it from the sequence
    Note targetNote = sequence.noteAt(track,step);
    sequence.deleteNote(track,step);

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
    vector<Note> notes = {targetNote};
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
                sequence.makeNote(targetNote,track);
                break;
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
                    sequence.makeNote(notes[i],track);
                }
                return;
            }
        }
        //update chopped notes if any changes were made
        if(changed){
            notes = getChoppedNotes(targetNote,numberOfPieces,velSlope);
            if(!notes.size()){
                maxedOut = true;
                numberOfPieces--;
                notes = getChoppedNotes(targetNote,numberOfPieces,velSlope);
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
            drawNote(notes[i],track,settings);
        }
        display.display();
    }
}
