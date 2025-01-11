//creates a sequence object with default values
void StepchildSequence::init(uint8_t numberOfTracks,uint16_t length){
    //What should happen if length<192?
    this->sequenceLength = length;
    this->viewStart = 0;
    this->viewEnd = 192;

    //make the default loop
    Loop firstLoop = Loop(0,96,0,0);
    this->loopData.push_back(firstLoop);

    //reset to 120 bpm
    sequenceClock.setBPM(120);

    //resize data arrays to number of tracks
    this->lookupTable.resize(numberOfTracks);
    this->noteData.resize(numberOfTracks);

    //clear out trackData
    this->trackData = {};
    //pitch var so that all the tracks count up from this (weird bc the order the tracks are drawn in is high->low)
    uint8_t pitch = this->defaultPitch + numberOfTracks-1;
    for(uint8_t i = 0; i<numberOfTracks; i++){
        //add a track on the lookupdata
        this->lookupTable[i].resize(sequenceLength+1,0);
        this->noteData[i] = {Note()};
        this->trackData.push_back(Track(pitch,this->defaultChannel));
        pitch--;
    }
}
void StepchildSequence::init(){
    this->init(16,768);
}
bool StepchildSequence::isQuarterGrid(){
    return !(this->subDivision%3);
}
//swaps all the data vars in the sequence for new, blank data
void StepchildSequence::erase(){
    this->selectionCount = 0;
    vector<vector<uint16_t>> newLookupData;
    newLookupData.swap(this->lookupTable);
    vector<vector<Note>> newSeqData;
    newSeqData.swap(this->noteData);
    vector<Track> newTrackData;
    newTrackData.swap(this->trackData);
    vector<Autotrack> newAutotrackData;
    newAutotrackData.swap(this->autotrackData);
}
Note StepchildSequence::noteAt(uint8_t track, uint16_t step){
    if(this->lookupTable[track][step]<this->noteData[track].size())
        return this->noteData[track][this->lookupTable[track][step]];
    else return this->noteData[track][0];
}
Note StepchildSequence::noteAtCursor(){
    return this->noteAt(this->activeTrack,this->cursorPos);
}
uint16_t StepchildSequence::IDAt(uint8_t track, uint16_t step){
    if(step<this->sequenceLength)
        return this->lookupTable[track][step];
    else return 0;
}
uint16_t StepchildSequence::IDAtCursor(){
    return this->IDAt(this->activeTrack,this->cursorPos);
}

/*
----------------------------------------------------------
                    LOADING NOTES
----------------------------------------------------------
*/
//adds a note w/o checking for overlaps
//Only use this when loading notes from a file into a blank sequence
void StepchildSequence::loadNote(Note newNote, uint8_t track){
    this->noteData[track].push_back(newNote);
    if(newNote.isSelected())
        this->selectionCount++;
    //adding to lookupData
    for (uint16_t i =  newNote.startPos; i < newNote.endPos; i++) { //sets id
        this->lookupTable[track][i] = this->noteData[track].size()-1;
    }
}
void StepchildSequence::loadNote(uint16_t id, uint8_t track, uint16_t start, uint8_t velocity, bool isMuted, uint8_t chance, uint16_t end, bool selected){
    Note newNoteOn(start, end, velocity, chance, isMuted, false);
    this->loadNote(newNoteOn, track);
}
void StepchildSequence::loadNote(uint8_t whichTrack, uint16_t start, uint16_t end, uint8_t velocity, uint8_t chance, uint8_t flags){
    Note newNote(start, end, velocity, chance, flags);
    this->loadNote(newNote,whichTrack);
}
/*
----------------------------------------------------------
                    DELETING NOTES
----------------------------------------------------------
*/
//Deletes a note on a given track with a given ID
void StepchildSequence::deleteNote_byID(uint8_t track, uint16_t targetNoteID){
    //if there's a note/something here, and it's in data
    if (targetNoteID > 0 && targetNoteID < this->noteData[track].size()) {
        //clearing note from this->lookupTable
        for (int i = this->noteData[track][targetNoteID].startPos; i < this->noteData[track][targetNoteID].endPos; i++) {
            this->lookupTable[track][i] = 0;
        }
        //lowering selectionCount
        if(this->noteData[track][targetNoteID].isSelected() && this->selectionCount>0)
            this->selectionCount--;
        //erasing note from this->noteData
        //make a copy of the this->noteData[track] vector which excludes the note
        //hopefully, this does a better job of freeing memory
        //swapping it like this! this is so the memory is free'd up again
        vector<Note> temp = {Note()};
        for(int i = 1; i<=this->noteData[track].size()-1; i++){
        if(i != targetNoteID){//if it's not the target note, or an empty spot, copy it to the temp vector
            temp.push_back(this->noteData[track][i]);
        }
        }
        temp.swap(this->noteData[track]);
        //since we del'd it from this->noteData, we need to update all the lookup values that are now 'off' by 1. Any value that's higher than the del'd note's ID should be decremented.
        if(this->noteData[track].size()-1>0){
            for (uint16_t step = 0; step < this->lookupTable[track].size(); step++) {
                if (this->lookupTable[track][step] > targetNoteID) //if there's a higher note and if there are still notes to be changed
                    this->lookupTable[track][step] -= 1;
            }
        }
    }
}
//deletes a note at a specific time/place
void StepchildSequence::deleteNote(uint8_t track, uint16_t time){
    this->deleteNote_byID(track,this->IDAt(track,time));
}
//deletes a note at the current track/cursor position
void StepchildSequence::deleteNote(){
    this->deleteNote_byID(this->activeTrack,this->IDAtCursor());
}
void StepchildSequence::deleteSelected(){
    if(this->selectionCount>0){
        if(binarySelectionBox(64,32,"nah","yea","del "+stringify(selectionCount)+((selectionCount == 1)?stringify(" note?"):stringify(" notes?")),drawSeq) == 1){
            for(uint8_t track = 0; track<this->trackData.size(); track++){
                for(uint16_t note = 0; note<this->noteData[track].size(); note++){
                    if(this->noteData[track][note].isSelected()){
                        this->deleteNote_byID(track, note);
                        (note == 0) ? note = 0: note--;
                    }
                    //if you've already checked all the selected notes
                    if(this->selectionCount == 0)
                        break;
                }
            }
        }
    }
}
/*
----------------------------------------------------------
                    CREATING NOTES
----------------------------------------------------------
Lots of these are redundant/deprecated overloads... go thru em and get rid of them!
*/
void StepchildSequence::makeNote(Note newNoteOn, uint8_t track, bool loudly){
    //if you're placing it on the end of the seq, just return
    if(newNoteOn.startPos == this->sequenceLength)
        return;
    //if there's a 0 where the note is going to go, or if there's not a zero BUT it's also not the start of that other note
    if (this->lookupTable[track][newNoteOn.startPos] == 0 || newNoteOn.startPos != this->noteAt(this->activeTrack,newNoteOn.startPos).startPos) { //if there's no note there
        if (this->lookupTable[track][newNoteOn.startPos] != 0)
            this->truncateNote(track, newNoteOn.startPos);
        if(newNoteOn.isSelected())
            this->selectionCount++;
        uint16_t id = this->noteData[track].size();
        this->lookupTable[track][newNoteOn.startPos] = id;//set noteID in this->lookupTable to the index of the new note
        for (uint16_t i = newNoteOn.startPos+1; i < newNoteOn.endPos; i++) { //sets id
            if (this->lookupTable[track][i] == 0 && i<this->sequenceLength)
                this->lookupTable[track][i] = id;
            else { //if there's something there, then set the end to the step before it
                newNoteOn.endPos = i;
                break;
            }
        }
        this->noteData[track].push_back(newNoteOn);
        if (loudly) {
            MIDI.noteOn(this->trackData[track].pitch, newNoteOn.velocity, this->trackData[track].channel);
            MIDI.noteOff(this->trackData[track].pitch, 0, this->trackData[track].channel);
        }
    }
}
void StepchildSequence::makeNote(int track, int time, int length, int velocity, int chance, bool mute, bool select, bool loudly){
    Note newNoteOn(time, (time + length-1), velocity, chance, mute, select);
    this->makeNote(newNoteOn,track,loudly);
}
void StepchildSequence::makeNote(Note newNoteOn, uint8_t track){
    this->makeNote(newNoteOn,track,false);
}
void StepchildSequence::makeNote(uint8_t track, uint16_t time, uint16_t length, uint8_t velocity, uint8_t chance, bool loudly){
    Note newNoteOn(time, (time + length-1), velocity, chance, false, false);
    this->makeNote(newNoteOn,track,false);
}
//this one is for quickly placing a ntoe at the cursor, on the active track
void StepchildSequence::makeNote(uint8_t track, uint16_t time, uint16_t length, bool loudly) {
    Note newNote(time,(time+length),this->defaultVel,100,false,false);
    this->makeNote(newNote,track,loudly);
}
void StepchildSequence::makeNote(uint16_t length, bool loudly) {
    Note newNote(this->cursorPos,this->cursorPos+length,this->defaultVel,100,false,false);
    this->makeNote(newNote,this->activeTrack,false);
}
//draws notes every "count" subDivs, from viewStart to viewEnd
//this is a super useful idea for sequencing, but currently only used by the edit menu
void StepchildSequence::stencilNotes(uint8_t count){
    for(uint16_t i = this->viewStart; i<this->viewEnd; i+=(this->subDivision*count)){
        //if there's no note there or if it's not the beginning of a note
        if(this->lookupTable[this->activeTrack][i] == 0 || (this->cursorPos != this->noteAtCursor().startPos))
            this->makeNote(this->activeTrack, i, this->subDivision*count+1, this->defaultVel, 100, false);
    }
}

//checks if there's a note first, and if there is it deletes it/if not it places one
void StepchildSequence::toggleNote(uint8_t track, uint16_t step, uint16_t length){
    //if there's no note/no start there, make a note
    if(this->lookupTable[track][step] == 0 || (this->lookupTable[track][step] != 0 && this->noteAt(track,step).startPos != step)){
        if(this->playing() && this->recording())
            this->makeNote(track, step, length, true);
        else
            this->makeNote(track, step, length, false);
    }
    //if there IS a note there, delete it
    else if(step == this->noteAt(track,step).startPos)
        this->deleteNote(track, step);
}
void StepchildSequence::makeNoteEveryNDivisions(uint8_t n){
    for(uint16_t step = viewStart; step<viewEnd; step+=(n*subDivision)){
        makeNote(activeTrack,step,subDivision,false);
    }
}
/*
----------------------------------------------------------
                    EDITING NOTES
----------------------------------------------------------
*/

//edits a single note
void StepchildSequence::editNoteProperty_byID(uint16_t id, uint8_t track, int8_t amount, NoteProperty which){
    switch(which){
        case VELOCITY:{
            uint8_t vel = this->noteData[track][id].velocity;
            vel += amount;
            if(vel>=127)
                this->noteData[track][id].velocity = 127;
            else if(vel<0)
                this->noteData[track][id].velocity = 0;
            else
                this->noteData[track][id].velocity = vel;
            break;
        }
        case PITCH:{

        }
        case CHANCE:{
            int8_t chance = this->noteData[track][id].chance;
            chance += amount;
            if(chance>=100)
                this->noteData[track][id].chance = 100;
            else if(chance<0)
                this->noteData[track][id].chance = 0;
            else
                this->noteData[track][id].chance = chance;
            break;
        }
    }
}
//edits all selected notes
void StepchildSequence::editNotePropertyOfSelectedNotes(int8_t amount, NoteProperty which){
    for(uint8_t track = 0; track<this->trackData.size(); track++){
        for(uint16_t note = this->noteData[track].size()-1; note>0; note--){
            if(this->noteData[track][note].isSelected()){
                this->editNoteProperty_byID(note,track, amount, which);
            }
        }
    }
}

//edits a note, and all selected notes, checking to make sure it doesn't double-edit
void StepchildSequence::editNoteAndSelected(int8_t amount, NoteProperty which){
    if(this->selectionCount>0){
        this->editNotePropertyOfSelectedNotes(amount,which);
        //only edit this note if it's not selected, so you don't double-edit it
        if(!this->noteAtCursor().isSelected())
            this->editNoteProperty_byID(this->IDAtCursor(), this->activeTrack, amount, which);
    }
    else
        this->editNoteProperty_byID(this->IDAtCursor(), this->activeTrack, amount, which);
}

//called by main controls, edits all selected notes
void StepchildSequence::changeVel(int8_t amount){
    this->editNoteAndSelected(amount,VELOCITY);
}
void StepchildSequence::changeChance(int8_t amount){
    this->editNoteAndSelected(amount,CHANCE);
}

//changes JUST a specific note
void StepchildSequence::changeChance_byID(uint16_t id, uint8_t track, int8_t amount){
    this->editNoteProperty_byID(id, track, amount, CHANCE);
}
void StepchildSequence::changeVel_byID(uint16_t id, uint8_t track, int8_t amount){
    this->editNoteProperty_byID(id, track, amount, VELOCITY);
}

void StepchildSequence::muteNote(uint8_t track, uint16_t id, bool toggle){
    if(id != 0){
        if(toggle)
            this->noteData[track][id].setMuted(!this->noteData[track][id].isMuted());
        else
            this->noteData[track][id].setMuted(true);
    }
}
void StepchildSequence::unmuteNote(uint8_t track, uint16_t id, bool toggle){
    if(id != 0){
        if(toggle)
            this->noteData[track][id].setMuted(!this->noteData[track][id].isMuted());
        else
            this->noteData[track][id].setMuted(false);
    }
}

//mutes/unmutes all selected notes
void StepchildSequence::setMuteStateOfSelectedNotes(bool state){
    uint16_t count = 0;
    for(uint8_t track = 0; track<this->noteData.size(); track++){
        for(uint16_t note = 1; note<this->noteData[track].size(); note++){
            if(this->noteData[track][note].isSelected()){
                this->noteData[track][note].setMuted(state);
                count++;
            }
            if(count>=this->selectionCount){
                return;
            }
        }
    }
}
void StepchildSequence::muteSelectedNotes(){
    this->setMuteStateOfSelectedNotes(true);
}
void StepchildSequence::unmuteSelectedNotes(){
    this->setMuteStateOfSelectedNotes(false);
}

//cuts a note short at a specific time
void StepchildSequence::truncateNote(uint8_t track, uint16_t atTime){
    uint16_t id = this->lookupTable[track][atTime];
    if(id == 0 || id >= this->noteData[track].size())
        return;
    //if the note is only 1 step long, j del it
    if(this->noteData[track][id].endPos == this->noteData[track][id].startPos+1){
        this->deleteNote_byID(track,id);
        return;
    }

    //remove note from the lookupTable
    for(uint16_t i = atTime; i<this->noteData[track][id].endPos; i++){
        this->lookupTable[track][i] = 0;
    }
    //set the note end to the new cut point
    this->noteData[track][id].endPos = atTime;
}
bool StepchildSequence::checkNoteMove(Note targetNote, int track, int newTrack, int newStart){
    unsigned short int length = targetNote.endPos-targetNote.startPos;
    //checking bounds
    if(newStart<0 || newStart>this->sequenceLength || newTrack>=this->trackData.size() || newTrack<0)
        return false;
    //checking lookupData
    for(uint16_t start = newStart; start < newStart+length; start++){
        //for moving horizontally within one track
        if(track == newTrack && this->lookupTable[newTrack][start] != 0)
            return false;
        //for vertical kinds of movement, where you won't collide with yourself
        else if(this->lookupTable[newTrack][start] != 0)
            return false;
    }
    return true;
}
bool StepchildSequence::checkNoteMove(uint16_t id, uint8_t track, uint8_t newTrack, uint16_t newStart){
    Note targetNote = this->noteData[track][id];
    return this->checkNoteMove(targetNote,track,newTrack,newStart);
}
//moves a note
bool StepchildSequence::moveNote(uint16_t id, uint8_t track, uint8_t newTrack, uint16_t newStart){
    //if there's room
    if(checkNoteMove(id, track, newTrack, newStart)){
        Note targetNote = this->noteData[track][id];
        uint16_t length = targetNote.endPos-targetNote.startPos;
        //clear out old note
        this->deleteNote(track, targetNote.startPos);
        //make room
        this->makeNote(newTrack, newStart, length+1, targetNote.velocity, targetNote.chance, targetNote.isMuted(), targetNote.isSelected(), false);
        return true;
    }
    else return false;
}
bool StepchildSequence::moveSelectedNotes(int16_t xOffset, int8_t yOffset){
    //to temporarily store all the notes
    vector<vector<Note>> selectedNotes;
    selectedNotes.resize(this->noteData.size());

    //grab all the selected notes
    for(uint8_t track = 0; track<this->noteData.size(); track++){
        for(uint16_t note = 1; note<this->noteData[track].size(); note++){
            //if the note is selected, push it into the buffer and then del it
            if(this->noteData[track][note].isSelected()){
                selectedNotes[track].push_back(this->noteData[track][note]);
                this->deleteNote(track, this->noteData[track][note].startPos);
                note--;
            }
            if(this->selectionCount==0)
                break;
        }
        if(this->selectionCount==0)
            break;
    }
    //check each note in the buffer to see if it's a valid move
    for(uint8_t track = 0; track<selectedNotes.size(); track++){
        for(uint16_t note = 0; note<selectedNotes[track].size(); note++){
            //if it hits a single bad note, then remake all the notes and exit
            if(!this->checkNoteMove(selectedNotes[track][note],track,track+yOffset,selectedNotes[track][note].startPos+xOffset)){
                for(uint8_t track2 = 0; track2<selectedNotes.size(); track2++){
                    for(uint16_t note2  = 0; note2<selectedNotes[track2].size(); note2++){
                        //remake old note
                        this->makeNote(selectedNotes[track2][note2],track2,false);
                    }
                }
                return false;
            }
        }
    }
    //if all notes pass the check... move em!
    for(uint8_t track = 0; track<selectedNotes.size(); track++){
        for(uint16_t note = 0; note<selectedNotes[track].size(); note++){
            unsigned short int length = selectedNotes[track][note].endPos-selectedNotes[track][note].startPos;
            this->makeNote(track+yOffset, selectedNotes[track][note].startPos+xOffset, length+1, selectedNotes[track][note].velocity, selectedNotes[track][note].chance, selectedNotes[track][note].isMuted(), selectedNotes[track][note].isSelected(), false);
        }
    }
    return true;
}
//this should move the note the cursor is on (if any)
bool StepchildSequence::moveNotes(int16_t xAmount, int8_t yAmount){
    if(this->selectionCount == 0){
        if(this->IDAtCursor() != 0){
            return this->moveNote(this->IDAtCursor(),this->activeTrack,this->activeTrack+yAmount,this->noteData[this->activeTrack][this->IDAtCursor()].startPos+xAmount);
        }
        else return false;
    }
    else{
        return this->moveSelectedNotes(xAmount,yAmount);
    }
}
/*
----------------------------------------------------------
                    EDITING SEQ
----------------------------------------------------------
*/
void StepchildSequence::addTimeToSeq(uint16_t amount, uint16_t insertPoint){
    //make sure it doesn't overflow the uint16_t length
    if(this->sequenceLength+amount>65535)
        return;
    this->sequenceLength+=amount;
    //move through notes that appear AFTER the insert point and move them back
    //sweeping from old seq end (there should be no notes after that)-->insertPoint
    for(uint8_t t = 0; t<this->trackData.size(); t++){
        //extend lookupdata
        this->lookupTable[t].resize(this->sequenceLength,0);
        for(uint16_t i = this->sequenceLength-amount; i>insertPoint; i--){
        uint16_t id = this->IDAt(t,i);
        //if there's a note there
        if(id != 0){
            //if it starts after the insert point, move the whole note
            if(this->noteData[t][id].startPos>insertPoint){
                //set i to old beginning of note (so you definitely don't hit it twice)
                i = this->noteData[t][id].startPos;
                this->moveNote(id,t,t,this->noteData[t][id].startPos+amount);
            }
            //if the insert point intersects it somehow, truncate it
            else{
                this->truncateNote(t,insertPoint);
                //break bc that was the last note
                break;
            }
        }
        }
    }
}
void StepchildSequence::removeTimeFromSeq(uint16_t amount, uint16_t insertPoint){
    //if you're trying to del more than (or as much as) exists! just return
    if(amount>=this->sequenceLength)
        return;
    //if you're trying to del more than exists between insertpoint --> this->sequenceLength,
    //then set amount to everything there
    if(insertPoint+amount>this->sequenceLength){
        amount = this->sequenceLength-insertPoint;
    }
    //move through 'deld' area and clear out notes
    for(uint8_t t = 0; t<trackData.size(); t++){
        //if there are no notes, skip this track
        if(this->noteData[t].size() == 1)
            continue;
        for(uint16_t i = insertPoint+1; i<insertPoint+amount; i++){
            uint16_t id = this->lookupTable[t][i];
            //if there's a note there
            if(id != 0){
                //if it starts before/at insert point, truncate it
                if(this->noteData[t][id].startPos<=insertPoint){
                    truncateNote(t,insertPoint);
                    //break bc you know that was the last note
                    break;
                }
                //if it's in the del area, del it
                else if(this->noteData[t][id].startPos>insertPoint && this->noteData[t][id].startPos<=insertPoint+amount){
                    deleteNote_byID(t,id);
                }
            }
        }
    }
    //move notes that fall beyond the del area
    for(uint8_t t = 0; t<trackData.size(); t++){
        //if there're no notes, skip this track
        if(this->noteData[t].size() == 1)
            continue;
        for(uint16_t i = insertPoint+amount; i<this->sequenceLength; i++){
            uint16_t id = this->lookupTable[t][i];
            //if there's a note here, move it back by amount
            if(id != 0){
                this->moveNote(id,t,t,this->noteData[t][id].startPos-amount);
                i = this->noteData[t][id].endPos-1;
            }
        }
        //resize lookupData
        this->lookupTable[t].resize(this->sequenceLength-amount);
    }
    this->sequenceLength -= amount;
    //fixing loop point positions
    for(uint8_t loop = 0; loop<loopData.size(); loop++){
        //if start or end are past seqend, set to seqend
        if(loopData[loop].start>this->sequenceLength)
            loopData[loop].start = this->sequenceLength;
        if(loopData[loop].end>this->sequenceLength)
            loopData[loop].end = this->sequenceLength;
    }
    //make sure view stays within seq
    checkView();
}
/*
----------------------------------------------------------
                        Graphics??
----------------------------------------------------------
*/
//displays notes on LEDs
void StepchildSequence::displayMainSequenceLEDs(){
    if(controls.SHIFT()){
        uint16_t ledState = 0b0000000000001111;
        if(millis()/200%2)
            ledState |= 0b1000000000000000;
        if((millis()/200+1)%2)
            ledState |= 0b0100000000000000;
        
        controls.writeLEDs(ledState);
        return;
    }
    uint16_t dat = 0;//00000000
    if(LEDsOn && !screenSaverActive){
        uint16_t viewLength = this->viewEnd-this->viewStart;
        //move through the view, check every this->subDivision
        const uint16_t jump = this->isQuarterGrid()?(viewLength/16):(viewLength/12);
        // const uint16_t jump = viewLength/16;
        //if there are any notes, check
        if(this->noteData[this->activeTrack].size()>1){
            for(uint8_t i = 0; i<16; i++){
                uint16_t step = this->viewStart+i*jump;
                if(this->lookupTable[this->activeTrack][step] != 0){
                    //not sure if it should only light up if it's on the start step or nah
                    if(this->noteData[this->activeTrack][this->lookupTable[this->activeTrack][step]].startPos == step){
                        //if playing or recording, and the head isn't on that step, it should be on
                        //if it is on that step, then the step should blink
                        if(!this->playing() || ((playheadPos < this->noteAt(this->activeTrack,step).startPos) || (playheadPos > this->noteAt(this->activeTrack,step).endPos))){
                            dat |= (1<<i);
                        }
                    }
                }
            }
        }
    }
    controls.writeLEDs(dat);
}
/*
----------------------------------------------------------
                        Loops
----------------------------------------------------------
*/
void StepchildSequence::setLoopPoint(int32_t start, bool which){
    //set start
    if(which){
        if(start>=loopData[activeLoop].end)
            return;
        if(start<=this->loopData[this->activeLoop].end && start>=0)
            this->loopData[this->activeLoop].start = start;
        else if(start < 0)
            this->loopData[this->activeLoop].start = 0;
        else if(start>this->loopData[this->activeLoop].end)
            this->loopData[this->activeLoop].start = this->loopData[this->activeLoop].end;
        this->loopData[this->activeLoop].start = this->loopData[this->activeLoop].start;
        menuText = "loop start: "+stepsToPosition(this->loopData[this->activeLoop].start,true);
    }
    //set end
    else{
        if(start <= loopData[activeLoop].start)
            return;
        if(start>=this->loopData[this->activeLoop].start && start <= this->sequenceLength)
            this->loopData[this->activeLoop].end = start;
        else if(start>this->sequenceLength)
            this->loopData[this->activeLoop].end = this->sequenceLength;
        else if(start<this->loopData[this->activeLoop].start)
            this->loopData[this->activeLoop].end = this->loopData[this->activeLoop].start;
        this->loopData[this->activeLoop].end = this->loopData[this->activeLoop].end;
        menuText = "loop end: "+stepsToPosition(this->loopData[this->activeLoop].end,true);
    }
}
void StepchildSequence::addLoop(Loop newLoop){
    this->loopData.push_back(newLoop);
}
void StepchildSequence::insertLoop(Loop newLoop, uint8_t index){
    this->loopData.insert(this->loopData.begin()+index,newLoop);
}


void StepchildSequence::setActiveLoop(unsigned int id){
    if(id<this->loopData.size() && id >=0){
        this->activeLoop = id;
        this->loopCount = 0;
    }
}

void StepchildSequence::addLoop(){
    Loop newLoop;
    newLoop.start = this->loopData[this->activeLoop].start;
    newLoop.end = this->loopData[this->activeLoop].end;
    newLoop.reps = this->loopData[this->activeLoop].reps;
    newLoop.type = this->loopData[this->activeLoop].type;
    this->loopData.push_back(newLoop);
    setActiveLoop(this->loopData.size()-1);
}

void StepchildSequence::addLoop(unsigned short int start, unsigned short int end, unsigned short int iter, uint8_t type){
    Loop newLoop;
    newLoop.start = start;
    newLoop.end = end;
    newLoop.reps = iter;
    newLoop.type = static_cast<LoopType>(type);
    this->loopData.push_back(newLoop);
}

void StepchildSequence::deleteLoop(uint8_t id){
    if(this->loopData.size() > 1 && this->loopData.size()>id){//if there's more than one loop, and id is in this->loopData
        vector<Loop> tempVec;
        for(int i = 0; i<this->loopData.size(); i++){
            if(i!=id){
                tempVec.push_back(this->loopData[i]);
            }
        }
        this->loopData.swap(tempVec);
        //if this->activeLoop was the loop that got deld, or above it
        //decrement it's id so it reads correct (and existing) data
        if(this->activeLoop>=this->loopData.size()){  
            this->activeLoop = this->loopData.size()-1;
        }
    }
    setActiveLoop(this->activeLoop);
}

void StepchildSequence::toggleLoop(){
    this->isLooping = !this->isLooping;
}

//moves to the next loop in loopSeq
void StepchildSequence::nextLoop(){
    this->loopCount = 0;
    if(this->loopData.size()>1){
        switch(this->loopData[this->activeLoop].type){
            case NORMAL:
                //move to next loop
                if(this->activeLoop < this->loopData.size()-1)
                    this->activeLoop++;
                else
                    this->activeLoop = 0;
                if(this->playing())
                    playheadPos = this->loopData[this->activeLoop].start;
                if(this->recording())
                    recheadPos = this->loopData[this->activeLoop].start;
                break;
            case RANDOM:{
                this->activeLoop = random(0,this->loopData.size());
                if(this->playing())
                    playheadPos = this->loopData[this->activeLoop].start;
                if(this->recording())
                    recheadPos = this->loopData[this->activeLoop].start;
                break;}
            case RANDOM_SAME:{
                //move to next loop
                if(this->activeLoop < this->loopData.size()-1)
                    this->activeLoop++;
                else
                    this->activeLoop = 0;
                //if rnd of same size mode, choose a random loop
                int currentLength = this->loopData[this->activeLoop].end - this->loopData[this->activeLoop].start;
                vector<uint8_t> similarLoops;
                for(int i = 0; i<this->loopData.size(); i++){
                    int len = this->loopData[i].end-this->loopData[i].start;
                    if(len == currentLength){
                        similarLoops.push_back(i);
                    }
                }
                this->activeLoop = similarLoops[random(0,similarLoops.size())];
                if(this->playing())
                    playheadPos = this->loopData[this->activeLoop].start;
                if(this->recording())
                    recheadPos = this->loopData[this->activeLoop].start;
                break;}
            case RETURN:{
                this->activeLoop = 0;
                if(this->playing())
                    playheadPos = this->loopData[this->activeLoop].start;
                if(this->recording())
                    recheadPos = this->loopData[this->activeLoop].start;
                break;}
            case INFINITE:{
                if(this->playing())
                    playheadPos = this->loopData[this->activeLoop].start;
                if(this->recording())
                    recheadPos = this->loopData[this->activeLoop].start;
                break;}
            }
    }
}
//cuts notes off when loop repeats, then starts new note at beginning
void StepchildSequence::cutLoop(){
    for(int i = 0; i<this->trackData.size(); i++){
        if(this->trackData[i].noteLastSent != 255){
            this->noteData[i][this->noteData[i].size()-1].endPos = this->loopData[this->activeLoop].end;
            //if it's about to loop again (if it's a one-shot recording, there's no need to make a new note)
            if(recMode != ONESHOT)
                writeNoteOn(this->loopData[this->activeLoop].start,this->trackData[i].pitch,this->noteData[i][this->noteData[i].size()-1].velocity,this->trackData[i].channel);
        }
    }
}

//true if Stepchild is sending or receiving notes
bool StepchildSequence::isReceiving(){
  for(uint8_t i = 0; i<this->trackData.size(); i++){
    if(this->trackData[i].noteLastSent != 255)
      return true;
  }
  return receivedNotes.notes.size();
}
bool StepchildSequence::isSending(){
  if(sentNotes.notes.size())
    return true;
  for(uint8_t i = 0; i<this->trackData.size(); i++){
    if(this->trackData[i].noteLastSent != 255)
      return true;
  }
  return false;
}

bool StepchildSequence::isReceivingOrSending(){
  return (isReceiving() || isSending());
}


//moves the whole loop
void StepchildSequence::moveLoop(int16_t amount){
  uint16_t length = this->loopData[this->activeLoop].end-this->loopData[this->activeLoop].start;
  //if it's being moved back
  if(amount<0){
    //if amount is larger than start, meaning start would be moved before 0
    if(this->loopData[this->activeLoop].start<=amount)
      this->setLoopPoint(0,true);
    else
      this->setLoopPoint(this->loopData[this->activeLoop].start+amount,true);
    this->setLoopPoint(this->loopData[this->activeLoop].start+length,false);
  }
  //if it's being moved forward
  else{
    //if amount is larger than the gap between seqend and this->loopData[this->activeLoop].end
    if((this->sequenceLength-this->loopData[this->activeLoop].end)<=amount)
      this->setLoopPoint(this->sequenceLength,false);
    else
      this->setLoopPoint(this->loopData[this->activeLoop].end+amount,false);
    this->setLoopPoint(this->loopData[this->activeLoop].end - length,true);
  }
}
void StepchildSequence::toggleLoopMove(){
  switch(movingLoop){
    case MOVING_NO_LOOP_POINTS:
      movingLoop = MOVING_LOOP_START;
      moveCursor(this->loopData[this->activeLoop].start-this->cursorPos);
      break;
    case MOVING_LOOP_END:
      movingLoop = MOVING_LOOP_START;
      moveCursor(this->loopData[this->activeLoop].end-this->cursorPos);
      break;
    case MOVING_LOOP_START:
      movingLoop = MOVING_BOTH_LOOP_POINTS;
      break;
    case MOVING_BOTH_LOOP_POINTS:
      movingLoop = MOVING_NO_LOOP_POINTS;
      break;
  }
}

int16_t StepchildSequence::changeNoteLength(int val, unsigned short int track, unsigned short int id){
  if(id!=0){
    int newEnd;
    //if it's 1 step long and being increased by this->subDivision, make it this->subDivision steps long instead of this->subDivision+1 steps
    //(just to make editing more intuitive)
    if(this->noteData[track][id].endPos - this->noteData[track][id].startPos == 1 && val == this->subDivision){
      newEnd = this->noteData[track][id].endPos + val - 1;
    }
    else{
      newEnd = this->noteData[track][id].endPos + val;
    }
    //check and see if there's a different note there
    //if there is, set the new end to be 
    for(uint16_t step = 1; step<=val; step++){
      if(this->lookupTable[track][step+this->noteData[track][id].endPos] != 0 && this->lookupTable[track][step+this->noteData[track][id].endPos] != id){
        //if it's the first step, just fail to save time
        if(step == 1)
          return 0;
        //if it's not the first step, set the new end to right before the other note
        else{
          newEnd = this->noteData[track][id].endPos+step;
          break;
        }
      }
    }
    //if the new end is before/at the start, don't do anything
    if(newEnd<=this->noteData[track][id].startPos)
      return 0;
      // newEnd = this->noteData[track][id].startPos+1;
    //if the new end is past/at the end of the seq
    if(newEnd>this->sequenceLength){
      newEnd = this->sequenceLength;
    }
    Note note = this->noteData[track][id];
    int16_t amount = newEnd-note.endPos;
    note.endPos = newEnd;
    this->deleteNote_byID(track, id);
    this->makeNote(note, track, false);
    return amount;
  }
  return 0;
}


void StepchildSequence::changeNoteLengthSelected(int amount){
  for(int track = 0; track<this->trackData.size(); track++){
    for(int note = 1; note <= this->noteData[track].size()-1; note++){
      if(this->noteData[track][note].isSelected()){
        changeNoteLength(amount, track, note);
      }
    }
  }
}

int16_t StepchildSequence::changeNoteLength(int amount){
  if(this->selectionCount > 0){
    changeNoteLengthSelected(amount);
    vector<uint16_t> bounds = getSelectedNotesBoundingBox();
    //move the cursor to the end/beginning of the selection box
    if(amount>0)
      setCursor(bounds[2]);
    else
      setCursor(bounds[0]);
    return 0;
  }
  else{
    return changeNoteLength(amount, this->activeTrack, this->IDAtCursor());
  }
}

//this one jumps the cursor to the end or start of the note
void StepchildSequence::changeNoteLength_jumpToEnds(int16_t amount){
  if(this->selectionCount > 0){
    changeNoteLengthSelected(amount);
    vector<uint16_t> bounds = getSelectedNotesBoundingBox();
    //move the cursor to the end/beginning of the selection box
    if(amount>0)
      setCursor(bounds[2]);
    else
      // setCursor(bounds[0]);
      setCursor(bounds[2]-this->subDivision);//testing this
  }
  else{
    //if the note was changed
    if(changeNoteLength(amount, this->activeTrack, this->IDAtCursor()) != 0){
      //if you're shrinking the note
      if(amount<0){
        setCursor(this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].endPos-this->subDivision);//testing this
        // setCursor(this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].startPos);
        //if it's out of view
        // else
          // setCursor(this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].endPos+amount);
      }
      //if you're growing it
      else
        // setCursor(this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].endPos-this->subDivision);
        setCursor(this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].endPos-1);//testing this

        // setCursor(this->noteData[this->activeTrack][this->IDAtCursor()].endPos - amount);
    }
  }
}

//sets cursor to the visually nearest note
//steps to pixels = steps*scale
//for a note to be "visually" closer, it needs to have a smaller pixel
//distance from the cursor than another note
//compare trackDistance * trackHeight to stepDistance * scale
float StepchildSequence::getDistanceFromNoteToCursor(Note note,uint8_t track){
  //if the start of the note is closer than the end
  return sqrt(pow(this->activeTrack - track,2)+pow(((abs(note.startPos-this->cursorPos)<abs(note.endPos-this->cursorPos))?(note.startPos-this->cursorPos):(note.endPos-this->cursorPos)),2));
}

void StepchildSequence::setCursorToNearestNote(){
  const float maxPossibleDist = this->sequenceLength*this->viewScale+this->trackData.size()*trackHeight;
  float minDist = maxPossibleDist;
  int minTrack;
  int minNote;
  for(int track = 0; track<this->noteData.size(); track++){
    for(int note = 1; note<this->noteData[track].size(); note++){
      // //Serial.println("checking n:"+stringify(note)+" t:"+stringify(track));
      // Serial.flush();
      float distance = getDistanceFromNoteToCursor(this->noteData[track][note],track);
      if(distance<minDist){
        minTrack = track;
        minNote = note;
        minDist = distance;
        if(minDist == 0)
          break;
      }
    }
    if(minDist == 0)
      break;
  }
  // //Serial.println("setting cursor...");
  // Serial.flush();
  if(minDist != maxPossibleDist){
    setCursor((this->noteData[minTrack][minNote].startPos<this->cursorPos)?this->noteData[minTrack][minNote].startPos:this->noteData[minTrack][minNote].endPos-1);
    setActiveTrack(minTrack,false);
  }
}


uint16_t StepchildSequence::getNoteCount(){
  uint16_t count = 0;
  for(uint8_t track = 0; track<this->noteData.size(); track++){
    count+=this->noteData[track].size()-1;
  }
  return count;
}

float StepchildSequence::getNoteDensity(uint16_t timestep){
  float density = 0;
  for(int track = 0; track<this->trackData.size(); track++){
    if(this->lookupTable[track][timestep] != 0){
      density++;
    }
  }
  return density/float(this->trackData.size());
}
float StepchildSequence::getNoteDensity(uint16_t start, uint16_t end){
  float density;
  for(int i = start; i<= end; i++){
    density+=getNoteDensity(i);
  }
  return density/float(end-start+1);
}

//counts notes within a range
uint16_t StepchildSequence::countNotesInRange(uint16_t start, uint16_t end){
  uint16_t count = 0;
  for(uint8_t t = 0; t<this->trackData.size(); t++){
    //if there are no notes, ignore it
    if(this->noteData[t].size() == 1)
      continue;
    else{
      //move over each note
      for(uint16_t i = 1; i<this->noteData.size(); i++){
        if(this->noteData[t][i].startPos>=start && this->noteData[t][i].startPos<end)
          count++;
      }
    }
  }
  return count;
}

//changes which track is active, changing only to valid tracks
bool StepchildSequence::setActiveTrack(uint8_t newActiveTrack, bool loudly) {
  if (newActiveTrack >= 0 && newActiveTrack < this->trackData.size()) {
    if((this->activeTrack == this->maxTracksShown-1) && (newActiveTrack == this->maxTracksShown) && (this->shrinkTopDisplay == false)){
      // this->maxTracksShown++;
      this->shrinkTopDisplay = true;
    }
    else if((this->activeTrack == 1) && (newActiveTrack == 0) && (this->shrinkTopDisplay)){
      // this->maxTracksShown--;
      this->shrinkTopDisplay = false;
    }
    this->activeTrack = newActiveTrack;
    if (loudly) {
      MIDI.noteOn(this->trackData[this->activeTrack].pitch, this->defaultVel, this->trackData[this->activeTrack].channel);
      MIDI.noteOff(this->trackData[this->activeTrack].pitch, 0, this->trackData[this->activeTrack].channel);
      if(this->trackData[this->activeTrack].isLatched()){
        MIDI.noteOn(this->trackData[this->activeTrack].pitch, this->defaultVel, this->trackData[this->activeTrack].channel);
        MIDI.noteOff(this->trackData[this->activeTrack].pitch, 0, this->trackData[this->activeTrack].channel);
      }
    }
    menuText = pitchToString(this->trackData[this->activeTrack].pitch,true,true);
    return true;
  }
  return false;
}

void StepchildSequence::changeTrackChannel(int id, int newChannel){
  if(newChannel>=0 && newChannel<=16){
    this->trackData[id].channel = newChannel;
  }
}


void StepchildSequence::changeAllTrackChannels(int newChannel){
  for(int track = 0; track<this->trackData.size(); track++){
    changeTrackChannel(track, newChannel);
  }
}

void StepchildSequence::moveToNextNote_inTrack(bool up){
  uint8_t track = this->activeTrack;
  uint16_t currentID = this->IDAtCursor();
  bool foundTrack = false;
  //moving the track up/down until it hits a track with notes
  //and checking bounds
  if(up){
    while(track<this->trackData.size()-1){
      track++;
      if(this->noteData[track].size()>1){
        foundTrack = true;
        break;
      }
    }
  }
  else{
    while(track>0){
      track--;
      if(this->noteData[track].size()>1){
        foundTrack = true;
        break;
      }
    }
  }
  //if you couldn't find a track with a note on it, just return
  if(!foundTrack){
    return;
  }
  for(uint16_t dist = 0; dist<this->sequenceLength; dist++){
    bool stillValid = false;
    //if the new position is in bounds
    if(this->cursorPos+dist<=this->sequenceLength){
      stillValid = true;
      //and if there's something there!
      if(this->lookupTable[track][this->cursorPos+dist] != 0){
        //move to it
        moveCursor(dist);
        setActiveTrack(track,false);
        return;
      }
    }
    if(this->cursorPos>=dist){
      stillValid = true;
      if(this->lookupTable[track][this->cursorPos-dist] != 0){
        moveCursor(-dist);
        setActiveTrack(track,false);
        return;
      }
    }
    //if it's reached the bounds
    if(!stillValid){
      return;
    }
  }
}

//moves thru each step, forward or backward, and moves the cursor to the first note it finds
void StepchildSequence::moveToNextNote(bool forward,bool endSnap){
  //if there's a note on this track at all
  if(this->noteData[this->activeTrack].size()>1){
    unsigned short int id = this->IDAtCursor();
    if(forward){
      for(int i = this->cursorPos; i<this->sequenceLength; i++){
        if(this->lookupTable[this->activeTrack][i] !=id && this->lookupTable[this->activeTrack][i] != 0){
          moveCursor(this->noteData[this->activeTrack][this->lookupTable[this->activeTrack][i]].startPos-this->cursorPos);
          return;
        }
      }
      if(endSnap){
        moveCursor(this->sequenceLength-this->cursorPos);
      }
      return;
    }
    else{
      for(int i = this->cursorPos; i>0; i--){
        if(this->lookupTable[this->activeTrack][i] !=id && this->lookupTable[this->activeTrack][i] != 0){
          moveCursor(this->noteData[this->activeTrack][this->lookupTable[this->activeTrack][i]].startPos-this->cursorPos);
          return;
        }
      }
      if(endSnap){
        moveCursor(-this->cursorPos);
      }
      return;
    }
  }
}

void StepchildSequence::moveToNextNote(bool forward){
  moveToNextNote(forward, false);
}

//View ------------------------------------------------------------------

void StepchildSequence::setViewStart(uint16_t step){
  uint16_t viewLength = this->viewEnd-this->viewStart;
  if(viewLength + step > this->sequenceLength){
    step = this->sequenceLength-viewLength;
  }
  this->viewStart = step;
  this->viewEnd = step+viewLength;
}

void StepchildSequence::moveView(int16_t val){
  if(val < 0 && abs(val)>this->viewStart){
    setViewStart(0);
  }
  else{
    setViewStart(this->viewStart+val);
  }
}

//moving the cursor around
int16_t StepchildSequence::moveCursor(int moveAmount){
  int16_t amt = 0;
  //if you're trying to move back at the start
  if(this->cursorPos == 0 && moveAmount < 0){
    return 0;
  }
  if(moveAmount<0 && this->cursorPos+moveAmount<0){
    amt = this->cursorPos;
    this->cursorPos = 0;
  }
  else{
    this->cursorPos += moveAmount;
    amt = moveAmount;
  }
  if(this->cursorPos > this->sequenceLength) {
    amt += (this->sequenceLength-this->cursorPos);
    this->cursorPos = this->sequenceLength;
  }
  //extend the note if one is being drawn (and if you're moving forward)
  if(drawingNote && moveAmount>0){
    if(this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].endPos<this->cursorPos)
      changeNoteLength(this->cursorPos-this->noteData[this->activeTrack][this->noteData[this->activeTrack].size()-1].endPos,this->activeTrack,this->noteData[this->activeTrack].size()-1);
  }
  //Move the view along with the cursor
  if(this->cursorPos<this->viewStart+this->subDivision && this->viewStart>0){
    moveView(this->cursorPos - (this->viewStart+this->subDivision));
  }
  else if(this->cursorPos > this->viewEnd-this->subDivision && this->viewEnd<this->sequenceLength){
    moveView(this->cursorPos - (this->viewEnd-this->subDivision));
  }
  //update the LEDs
  menuText = ((moveAmount>0)?(stepsToPosition(this->cursorPos,true)+">>"):("<<"+stepsToPosition(this->cursorPos,true)));
  return amt;
}

void StepchildSequence::setCursor(uint16_t loc){
  moveCursor(loc-this->cursorPos);
}

void StepchildSequence::moveCursorIntoView(){
  if (this->cursorPos < 0) {
    this->cursorPos = 0;
  }
  if (this->cursorPos > this->sequenceLength-1) {
    this->cursorPos = this->sequenceLength-1;
  }
  if (this->cursorPos < this->viewStart) {
    moveView(this->cursorPos-this->viewStart);
  }
  if (this->cursorPos >= this->viewEnd) {//doin' it this way so the last column of pixels is drawn, but you don't interact with it
    moveView(this->cursorPos-this->viewEnd);
  }
}

//------------------------------------------------------------------------------------------------------------------------------
bool StepchildSequence::isInView(int target){
  if(target>=this->viewStart && target<=this->viewEnd)
    return true;
  else
    return false;
}

uint16_t StepchildSequence::changeSubDiv(bool direction, uint8_t subDiv, bool allowZero){
  //down
  if(!direction){
    if(subDiv == 1 && allowZero)
      subDiv = 0;
    else if(subDiv>3)
      subDiv /= 2;
    else if(subDiv == 3)
      subDiv = 1;
  }
  else{
    if(subDiv == 0)
      subDiv = 1;
    else if(subDiv == 1)//if it's one, set it to 3
      subDiv = 3;
    else if(subDiv !=  96 && subDiv != 32){
      //if triplet mode
      if(!(subDiv%2))
        subDiv *= 2;
      else if(!(subDiv%3))
        subDiv *=2;
    }
  }
  return subDiv;
}

void StepchildSequence::changeSubDivInt(bool down){
  changeSubDivInt(down,false);
}

void StepchildSequence::changeSubDivInt(bool down, bool limitToView){
  if(down){
    if(this->subDivision>3 && (!limitToView || (this->subDivision*this->viewScale)>2))
      this->subDivision /= 2;
    else if(this->subDivision == 3)
      this->subDivision = 1;
  }
  else{
    if(this->subDivision == 1)//if it's one, set it to 3
      this->subDivision = 3;
    else if(this->subDivision !=  96 && this->subDivision != 32){
      //if triplet mode
      if(!(this->subDivision%2))
        this->subDivision *= 2;
      else if(!(this->subDivision%3))
        this->subDivision *=2;
    }
  }
  menuText = "~"+stepsToMeasures(this->subDivision);
}

uint16_t StepchildSequence::toggleTriplets(uint16_t subDiv){
  //this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
  if(subDiv == 192){
    subDiv = 32;
  }
  else if(!(subDiv%3)){//if it's in 1/4 mode...
    subDiv = 2*subDiv/3;//set it to triplet mode
  }
  else if(!(subDiv%2)){//if it was in triplet mode...
    subDiv = 3*subDiv/2;//set it to 1/4 mode
  }
  menuText = stepsToMeasures(this->subDivision);
  return subDiv;
}

void StepchildSequence::toggleTriplets(){
  //this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
  if(this->subDivision == 192){
    this->subDivision = 32;
  }
  else if(!(this->subDivision%3)){//if it's in 1/4 mode...
    this->subDivision = 2*this->subDivision/3;//set it to triplet mode
  }
  else if(!(this->subDivision%2)){//if it was in triplet mode...
    this->subDivision = 3*this->subDivision/2;//set it to 1/4 mode
  }
  menuText = stepsToMeasures(this->subDivision);
}

//makes sure scale/viewend line up with the display
void StepchildSequence::checkView(){
  if(this->viewEnd>this->sequenceLength){
    this->viewScale = float(96)/float(this->sequenceLength);
    this->viewEnd = this->sequenceLength+1;
  }
}
//zooms in/out
void StepchildSequence::zoom(bool in){
  uint16_t viewLength = this->viewEnd-this->viewStart;
  if(!in && viewLength<this->sequenceLength){
    this->viewScale /= 2;
  }
  else if(in && viewLength/2>1){
    this->viewScale *= 2;
  }  
  this->viewStart = 0;
  this->viewEnd = 96/this->viewScale;
  checkView();
  changeSubDivInt(in);
  moveCursorIntoView();
  menuText = stepsToMeasures(this->viewStart)+"<-->"+stepsToMeasures(this->viewEnd)+"(~"+stepsToMeasures(this->subDivision)+")";
}
bool StepchildSequence::areThereAnyNotes(){
  for(uint8_t t = 0; t<this->noteData.size(); t++){
    if(this->noteData[t].size()>1){
      return true;
    }
  }
  return false;
}
//checks for notes above or below a track
bool StepchildSequence::areThereMoreNotes(bool above){
  if(this->trackData.size()>this->maxTracksShown){
    if(!above){
      for(int track = this->endTrack+1; track<this->trackData.size();track++){
        if(this->noteData[track].size()-1>0)
        return true;
      }
    }
    else if(above){
      for(int track = this->startTrack-1; track>=0; track--){
        if(this->noteData[track].size()-1>0)
        return true;
      }
    }
  }
  return false;
}
//sort function for sorting tracks by channel
bool StepchildSequence::compareChannels(Track t1, Track t2){
  return t1.channel>t2.channel;
}
bool StepchildSequence::playing(){
  return (playState == PLAYING);
}
bool StepchildSequence::recording(){
  return (playState == RECORDING);
}
bool StepchildSequence::liveLooping(){
  return (playState == LIVELOOPING);
}

void StepchildSequence::togglePlay(){
  if(playing()){
    playState = STOPPED;
  }
  else{
    playState = PLAYING;
  }
}
void StepchildSequence::toggleRecording(){
  if(recording()){
    playState = STOPPED;
  }
  else{
    playState = RECORDING;
  }
}
