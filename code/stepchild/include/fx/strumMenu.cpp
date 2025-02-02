/*
    Code for the strum FX and strum menu
*/

//macros defining different sort orders
#define BYTRACK_DESC 0
#define BYTRACK_ASC 1
#define BYPITCH_DESC 2
#define BYPITCH_ASC 3
#define RANDOM_ORDER 4

void drawStrumIcon(uint8_t x1, uint8_t y1, uint8_t height, bool animated){
    const uint8_t numberOfStrings = 4; 
    const uint8_t spacing = 3;
    for(uint8_t i = 0; i<numberOfStrings; i++){
        int16_t y2 = y1+spacing*i+spacing/2*sin(millis()/100+i*4);
        display.drawLine(x1,y1+(i*spacing),x1+height/2,y2,1);
        display.drawLine(x1+height/2,y2,x1+height,y1+(i*spacing),1);
    }
}

//type == 0 is ascending, == 1 is descending
void strum(uint8_t type, uint8_t sortBy, uint16_t amount, int8_t variation){
    //select notes ==> all selected notes get strummed as one strum
    while(selectNotes("strum",drawStrumIcon)){
        vector<NoteID> selectedNoteIDs = grabSelectedNotesAsNoteIDs();

        //find the note that starts first
        uint16_t earliest = sequence.lookupTable[0].size();
        uint16_t earliestID = selectedNoteIDs.size();
        for(uint16_t note = 0; note<selectedNoteIDs.size(); note++){
            if(selectedNoteIDs[note].getNote().startPos<earliest){
                earliestID = note;
                earliest = selectedNoteIDs[note].getNote().startPos;
                if(earliest == 0){
                    break;
                }
            }
        }

        //sort the noteIDs
        selectedNoteIDs = sortNotes(selectedNoteIDs,sortBy,type);

        //move each note 
        for(uint16_t note = 0; note<selectedNoteIDs.size(); note++){

            //copy note then del it
            Note newNote = selectedNoteIDs[note].getNote();
            sequence.deleteNote_byID(selectedNoteIDs[note].track,selectedNoteIDs[note].id);

            //generate offset
            int16_t offset = (variation == 0)?amount:(float(random(-variation,variation))/100.0 * amount + amount);

            newNote.setSelected(false);
            uint16_t length = newNote.getLength()-1;
            newNote.startPos = earliest;
            newNote.endPos = newNote.startPos+length;
            sequence.makeNote(newNote,selectedNoteIDs[note].track);

            //move the starting position forward
            earliest+=offset;
        }
    }
}

void displayStrumMenu(uint8_t cursor, uint8_t sortType, uint8_t sortedValue, uint16_t amount, uint8_t variation){
    
    display.drawBitmap(47,52,strum_text,34,9,1);
    display.drawCircle(64,32,30,1);
    const uint8_t numberOfStrings = 6;
    const uint8_t spacing = 7;
    const uint8_t y1 = (screenHeight-(numberOfStrings-1)*spacing)/2;
    for(uint8_t i = 0; i<numberOfStrings; i++){
        int16_t y2 = y1+spacing*i+spacing/2*sin(millis()/100+i*amount/10);
        display.drawLine(0,y1+(i*spacing),64,y2,1);
        display.drawLine(64,y2,128,y1+(i*spacing),1);
    }
    //delay (amount)
    String amt = stepsToMeasures(amount);
    graphics.printFraction_small(0,3,amt);
    if(cursor == 0){
        graphics.drawArrow(amt.length()*4+((millis()/400)%2),5,3,1,false);
    }
    //type (ascending/descending)
    String type = sortType?stringify("ascending"):stringify("descending");
    printSmall(screenWidth-type.length()*4,1,type,1);
    if(cursor == 1){
        graphics.drawArrow(screenWidth-type.length()*4-4+((millis()/400)%2),3,3,0,false);
    }
    //randomness
    String rnd = "vary - "+stringify(variation)+"%";
    printSmall(0,58,rnd,1);
    if(cursor == 2){
        graphics.drawArrow(rnd.length()*4+((millis()/400)%2),60,3,1,false);
    }
    //target (pitch/track)
    String by = "by "+(sortedValue?stringify("trk #"):stringify("$"));
    printSmall(screenWidth-by.length()*4,58,by,1);
    if(cursor == 3){
        graphics.drawArrow(screenWidth-by.length()*4-4+((millis()/400)%2),60,3,0,false);
    }
}

bool strumMenu(){
    uint8_t sortType = 1;
    uint8_t sortValue = 0;
    int8_t randomness = 10;
    uint16_t amount = 6;

    uint8_t cursor = 0;

    bool atLeastOnce = false;

    vector<NoteID> ids;
    while(true){
        controls.readJoystick();
        controls.readButtons();
        while(controls.counterA != 0){
            switch(cursor){
                //amount (delay)
                case 0:
                    if(controls.SHIFT())
                        toggleTriplets(amount);
                    else
                        amount = changeSubDiv(controls.counterA>0,amount,false);
                //type
                case 1:
                    sortType = !sortType;
                    break;
                //variation
                case 2:
                    randomness+=(controls.counterA>0?1:-1);
                    if(randomness>100)
                        randomness = 100;
                    else if(randomness<0)
                        randomness = 0;
                    break;
                //target
                case 3:
                    sortValue = !sortValue;
                    break;
            }
            controls.counterA += controls.counterA<0?1:-1;
        }
        while(controls.counterB != 0){
            switch(cursor){
                //amount (delay)
                case 0:
                    if(controls.SHIFT())
                        toggleTriplets(amount);
                    else
                        amount = changeSubDiv(controls.counterB>0,amount,false);
                    break;
                //type
                case 1:
                    sortType = !sortType;
                    break;
                //variation
                case 2:
                    randomness+=(controls.counterB>0?1:-1);
                    if(randomness>100)
                        randomness = 100;
                    else if(randomness<0)
                        randomness = 0;
                    break;
                //target
                case 3:
                    sortValue = !sortValue;
                    break;
            }
            controls.counterB += controls.counterB<0?1:-1;
        }
        if(utils.itsbeen(200)){
            if(controls.MENU()){
                lastTime = millis();
                break;
            }
            if(controls.NEW()){
                lastTime = millis();
                atLeastOnce = true;
                strum(sortType, sortValue, amount, randomness);
            }
            if(controls.joystickX != 0){
                if(controls.joystickX == -1 && !(cursor%2)){//cursor == 0 or 2
                    cursor++;
                    lastTime = millis();
                }
                else if(controls.joystickX == 1 && (cursor%2)){//cursor == 1 or 3
                    cursor--;
                    lastTime = millis();
                }
            }
            if(controls.joystickY != 0){
                if(controls.joystickY == -1 && cursor>=2){
                    cursor-=2;
                    lastTime = millis();
                }
                else if(controls.joystickY == 1 && cursor<=1){
                    cursor+=2;
                    lastTime = millis();
                }
            }
        }
        display.clearDisplay();
        displayStrumMenu(cursor, sortType,sortValue,amount,randomness);
        display.display();
    }
    return atLeastOnce;
}
