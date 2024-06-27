//Providing all the data and functions that the stepchild needs to run the sequence, but none of the graphics
class StepchildSequence{
    public:
    vector<vector<uint16_t>> lookupTable;
    vector<vector<Note>> noteData;
    vector<Track> trackData;

    uint16_t activeTrack = 0;
    uint16_t cursorPos = 0;

    //Not implemented yet! Still using global vars
    uint16_t readHead = 0; 
    uint16_t writeHead = 0;

    vector<Loop> loopData;
    uint8_t activeLoop = 0;
    uint8_t isLooping = true;
    uint8_t loopCount;

    vector<Autotrack> autotrackData;
    uint8_t activeAutotrack;

    uint16_t sequenceLength;
    uint16_t viewStart;
    uint16_t viewEnd;
    float viewScale = 0.5;

    uint8_t subDivision = 24;

    enum PlayState{
        PLAY,
        RECORD,
        LIVELOOP
    };

    PlayState playState;
    uint8_t defaultChannel = 1;
    uint8_t defaultPitch = 36;
    uint8_t defaultVel = 127;
    uint8_t defaultChance = 100;

    uint16_t selectionCount = 0;

    enum NoteProperty{
        VELOCITY,
        CHANCE,
        PITCH
    };

    StepchildSequence(){}

    /*
    ----------------------------------------------------------
                        Utilities
    ----------------------------------------------------------
    */
   
    //creates a sequence object with default values
    void init(uint8_t numberOfTracks,uint16_t length){
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
    void init(){
        this->init(16,768);
    }
    bool isQuarterGrid(){
        return !(this->subDivision%3);
    }
    //swaps all the data vars in the sequence for new, blank data
    void erase(){
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
    Note noteAt(uint8_t track, uint16_t step){
        if(this->lookupTable[track][step]<this->noteData[track].size())
            return this->noteData[track][this->lookupTable[track][step]];
        else return this->noteData[track][0];
    }
    Note noteAtCursor(){
        return this->noteAt(this->activeTrack,this->cursorPos);
    }
    uint16_t IDAt(uint8_t track, uint16_t step){
        if(step<this->sequenceLength)
            return this->lookupTable[track][step];
        else return 0;
    }
    uint16_t IDAtCursor(){
        return this->IDAt(this->activeTrack,this->cursorPos);
    }
    /*
    ----------------------------------------------------------
                        LOADING NOTES
    ----------------------------------------------------------
    */
    //adds a note w/o checking for overlaps
    //Only use this when loading notes from a file into a blank sequence
    void loadNote(Note newNote, uint8_t track){
        this->noteData[track].push_back(newNote);
        if(newNote.isSelected())
            this->selectionCount++;
        //adding to lookupData
        for (uint16_t i =  newNote.startPos; i < newNote.endPos; i++) { //sets id
            this->lookupTable[track][i] = this->noteData[track].size()-1;
        }
    }
    void loadNote(uint16_t id, uint8_t track, uint16_t start, uint8_t velocity, bool isMuted, uint8_t chance, uint16_t end, bool selected){
        Note newNoteOn(start, end, velocity, chance, isMuted, false);
        this->loadNote(newNoteOn, track);
    }
    void loadNote(uint8_t whichTrack, uint16_t start, uint16_t end, uint8_t velocity, uint8_t chance, uint8_t flags){
        Note newNote(start, end, velocity, chance, flags);
        this->loadNote(newNote,whichTrack);
    }
    /*
    ----------------------------------------------------------
                        DELETING NOTES
    ----------------------------------------------------------
    */
    //Deletes a note on a given track with a given ID
    void deleteNote_byID(uint8_t track, uint16_t targetNoteID){
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
    void deleteNote(uint8_t track, uint16_t time){
        this->deleteNote_byID(track,this->IDAt(track,time));
    }
    //deletes a note at the current track/cursor position
    void deleteNote(){
        this->deleteNote_byID(this->activeTrack,this->IDAtCursor());
    }
    void deleteSelected(){
        if(this->selectionCount>0){
            if(binarySelectionBox(64,32,"nah","yea","del "+stringify(selectionCount)+((selectionCount == 1)?stringify(" note?"):stringify(" notes?")),drawSeq)){
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
    void makeNote(Note newNoteOn, uint8_t track, bool loudly){
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
    void makeNote(int track, int time, int length, int velocity, int chance, bool mute, bool select, bool loudly){
        Note newNoteOn(time, (time + length-1), velocity, chance, mute, select);
        this->makeNote(newNoteOn,track,loudly);
    }
    void makeNote(Note newNoteOn, uint8_t track){
        this->makeNote(newNoteOn,track,false);
    }
    void makeNote(uint8_t track, uint16_t time, uint16_t length, uint8_t velocity, uint8_t chance, bool loudly){
        Note newNoteOn(time, (time + length-1), velocity, chance, false, false);
        this->makeNote(newNoteOn,track,false);
    }
    //this one is for quickly placing a ntoe at the cursor, on the active track
    void makeNote(uint8_t track, uint16_t time, uint16_t length, bool loudly) {
        Note newNote(time,(time+length),this->defaultVel,100,false,false);
        this->makeNote(newNote,track,loudly);
    }
    void makeNote(uint16_t length, bool loudly) {
        Note newNote(this->cursorPos,this->cursorPos+length,this->defaultVel,100,false,false);
        this->makeNote(newNote,this->activeTrack,false);
    }
    //draws notes every "count" subDivs, from viewStart to viewEnd
    //this is a super useful idea for sequencing, but currently only used by the edit menu
    void stencilNotes(uint8_t count){
        for(uint16_t i = this->viewStart; i<this->viewEnd; i+=(this->subDivision*count)){
            //if there's no note there or if it's not the beginning of a note
            if(this->lookupTable[this->activeTrack][i] == 0 || (this->cursorPos != this->noteAtCursor().startPos))
                this->makeNote(this->activeTrack, i, this->subDivision*count+1, this->defaultVel, 100, false);
        }
    }

    //checks if there's a note first, and if there is it deletes it/if not it places one
    void toggleNote(uint8_t track, uint16_t step, uint16_t length){
        //if there's no note/no start there, make a note
        if(this->lookupTable[track][step] == 0 || (this->lookupTable[track][step] != 0 && this->noteAt(track,step).startPos != step)){
            if(!playing && !recording)
                this->makeNote(track, step, length, true);
            else
                this->makeNote(track, step, length, false);
        }
        //if there IS a note there, delete it
        else if(step == this->noteAt(track,step).startPos)
            this->deleteNote(track, step);
    }
    /*
    ----------------------------------------------------------
                        EDITING NOTES
    ----------------------------------------------------------
    */

    //edits a single note
    void editNoteProperty_byID(uint16_t id, uint8_t track, int8_t amount, NoteProperty which){
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
            }
        }
    }
    //edits all selected notes
    void editNotePropertyOfSelectedNotes(int8_t amount, NoteProperty which){
        for(uint8_t track = 0; track<this->trackData.size(); track++){
            for(uint16_t note = this->noteData[track].size()-1; note>0; note--){
                if(this->noteData[track][note].isSelected()){
                    this->editNoteProperty_byID(note,track, amount, which);
                }
            }
        }
    }

    //edits a note, and all selected notes, checking to make sure it doesn't double-edit
    void editNoteAndSelected(int8_t amount, NoteProperty which){
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
    void changeVel(int8_t amount){
        this->editNoteAndSelected(amount,VELOCITY);
    }
    void changeChance(int amount){
        this->editNoteAndSelected(amount,CHANCE);
    }

    //changes JUST a specific note
    void changeChance_byID(uint16_t id, uint8_t track, int8_t amount){
        this->editNoteProperty_byID(id, track, amount, CHANCE);
    }
    void changeVel_byID(uint16_t id, uint8_t track, int8_t amount){
        this->editNoteProperty_byID(id, track, amount, VELOCITY);
    }

    void muteNote(uint8_t track, uint16_t id, bool toggle){
        if(id != 0){
            if(toggle)
                this->noteData[track][id].setMuted(!this->noteData[track][id].isMuted());
            else
                this->noteData[track][id].setMuted(true);
        }
    }
    void unmuteNote(uint8_t track, uint16_t id, bool toggle){
        if(id != 0){
            if(toggle)
                this->noteData[track][id].setMuted(!this->noteData[track][id].isMuted());
            else
                this->noteData[track][id].setMuted(false);
        }
    }
    
    //mutes/unmutes all selected notes
    void setMuteStateOfSelectedNotes(bool state){
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
    void muteSelectedNotes(){
        this->setMuteStateOfSelectedNotes(true);
    }
    void unmuteSelectedNotes(){
        this->setMuteStateOfSelectedNotes(false);
    }

    //cuts a note short at a specific time
    void truncateNote(uint8_t track, uint16_t atTime){
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
    bool checkNoteMove(Note targetNote, int track, int newTrack, int newStart){
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
    bool checkNoteMove(uint16_t id, uint8_t track, uint8_t newTrack, uint16_t newStart){
        Note targetNote = this->noteData[track][id];
        return this->checkNoteMove(targetNote,track,newTrack,newStart);
    }
    //moves a note
    bool moveNote(uint16_t id, uint8_t track, uint8_t newTrack, uint16_t newStart){
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
    bool moveSelectedNotes(int16_t xOffset, int8_t yOffset){
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
    bool moveNotes(int16_t xAmount, int8_t yAmount){
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
    //slices a note into N equal pieces
    void chopNote(uint8_t track, uint16_t step, uint8_t pieces){
        if(!this->IDAt(track,step))
            return;
        Note targetNote = this->noteAt(track,step);
        uint16_t length = targetNote.endPos - targetNote.startPos;
        //if the length is too short to cut, don't
        if(!(length/pieces))
            return;
        
        //the new length of each sub note
        uint16_t newLength = length/pieces;

        //start on the second note, since the first one is already there (it just needs to be truncated)
        for(uint16_t N = 1; N<pieces; N++){
            //if you're about to make the last note, set newLength to be equal to the remaining length
            if(N == pieces-1)
                this->makeNote(track,targetNote.startPos+N*newLength,length - N*newLength,false);
            else
                this->makeNote(track,targetNote.startPos+N*newLength,newLength,false);
        }
    }
    /*
    ----------------------------------------------------------
                        EDITING SEQ
    ----------------------------------------------------------
    */
    void addTimeToSeq(uint16_t amount, uint16_t insertPoint){
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
    void removeTimeFromSeq(uint16_t amount, uint16_t insertPoint){
        //if you're trying to del more than (or as much as) exists! just return
        if(amount>=this->sequenceLength)
            return;
        //if you're trying to del more than exists between insertpoint --> sequence.sequenceLength,
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
    void displayMainSequenceLEDs(){
        if(controls.SHIFT()){
            controls.setLED(14,millis()/200%2);
            controls.setLED(15,(millis()/200+1)%2);
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
                            if(!playing || ((playheadPos < this->noteAt(this->activeTrack,step).startPos) || (playheadPos > this->noteAt(this->activeTrack,step).endPos))){
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
    void setLoopPoint(int32_t start, bool which){
        //set start
        if(which){
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
    void addLoop(Loop newLoop){
        this->loopData.push_back(newLoop);
    }
    void insertLoop(Loop newLoop, uint8_t index){
        this->loopData.insert(this->loopData.begin()+index,newLoop);
    }

    
    void setActiveLoop(unsigned int id){
        if(id<this->loopData.size() && id >=0){
            this->activeLoop = id;
            this->loopCount = 0;
        }
    }

    void addLoop(){
        Loop newLoop;
        newLoop.start = this->loopData[this->activeLoop].start;
        newLoop.end = this->loopData[this->activeLoop].end;
        newLoop.reps = this->loopData[this->activeLoop].reps;
        newLoop.type = this->loopData[this->activeLoop].type;
        this->loopData.push_back(newLoop);
        setActiveLoop(this->loopData.size()-1);
    }

    void addLoop(unsigned short int start, unsigned short int end, unsigned short int iter, uint8_t type){
        Loop newLoop;
        newLoop.start = start;
        newLoop.end = end;
        newLoop.reps = iter;
        newLoop.type = static_cast<LoopType>(type);
        this->loopData.push_back(newLoop);
    }

    void deleteLoop(uint8_t id){
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

    void toggleLoop(){
        this->isLooping = !this->isLooping;
    }

    //moves to the next loop in loopSeq
    void nextLoop(){
        this->loopCount = 0;
        if(this->loopData.size()>1){
            switch(this->loopData[this->activeLoop].type){
                case NORMAL:
                    //move to next loop
                    if(this->activeLoop < this->loopData.size()-1)
                        this->activeLoop++;
                    else
                        this->activeLoop = 0;
                    if(playing)
                        playheadPos = this->loopData[this->activeLoop].start;
                    if(recording)
                        recheadPos = this->loopData[this->activeLoop].start;
                    break;
                case RANDOM:{
                    this->activeLoop = random(0,this->loopData.size());
                    if(playing)
                        playheadPos = this->loopData[this->activeLoop].start;
                    if(recording)
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
                    if(playing)
                        playheadPos = this->loopData[this->activeLoop].start;
                    if(recording)
                        recheadPos = this->loopData[this->activeLoop].start;
                    break;}
                case RETURN:{
                    this->activeLoop = 0;
                    if(playing)
                        playheadPos = this->loopData[this->activeLoop].start;
                    if(recording)
                        recheadPos = this->loopData[this->activeLoop].start;
                    break;}
                case INFINITE:{
                    if(playing)
                        playheadPos = this->loopData[this->activeLoop].start;
                    if(recording)
                        recheadPos = this->loopData[this->activeLoop].start;
                    break;}
                }
        }
    }
    //cuts notes off when loop repeats, then starts new note at beginning
    void cutLoop(){
        for(int i = 0; i<this->trackData.size(); i++){
            if(this->trackData[i].noteLastSent != 255){
                this->noteData[i][this->noteData[i].size()-1].endPos = this->loopData[this->activeLoop].end;
                //if it's about to loop again (if it's a one-shot recording, there's no need to make a new note)
                if(recMode != ONESHOT)
                    writeNoteOn(this->loopData[this->activeLoop].start,this->trackData[i].pitch,this->noteData[i][this->noteData[i].size()-1].velocity,this->trackData[i].channel);
            }
        }
    }
};

StepchildSequence sequence;


//true if Stepchild is sending or receiving notes
bool isReceiving(){
  for(uint8_t i = 0; i<sequence.trackData.size(); i++){
    if(sequence.trackData[i].noteLastSent != 255)
      return true;
  }
  return receivedNotes.notes.size();
}
bool isSending(){
  if(sentNotes.notes.size())
    return true;
  for(uint8_t i = 0; i<sequence.trackData.size(); i++){
    if(sequence.trackData[i].noteLastSent != 255)
      return true;
  }
  return false;
}

bool isReceivingOrSending(){
  return (isReceiving() || isSending());
}


//moves the whole loop
void moveLoop(int16_t amount){
  uint16_t length = sequence.loopData[sequence.activeLoop].end-sequence.loopData[sequence.activeLoop].start;
  //if it's being moved back
  if(amount<0){
    //if amount is larger than start, meaning start would be moved before 0
    if(sequence.loopData[sequence.activeLoop].start<=amount)
      sequence.setLoopPoint(0,true);
    else
      sequence.setLoopPoint(sequence.loopData[sequence.activeLoop].start+amount,true);
    sequence.setLoopPoint(sequence.loopData[sequence.activeLoop].start+length,false);
  }
  //if it's being moved forward
  else{
    //if amount is larger than the gap between seqend and sequence.loopData[sequence.activeLoop].end
    if((sequence.sequenceLength-sequence.loopData[sequence.activeLoop].end)<=amount)
      sequence.setLoopPoint(sequence.sequenceLength,false);
    else
      sequence.setLoopPoint(sequence.loopData[sequence.activeLoop].end+amount,false);
    sequence.setLoopPoint(sequence.loopData[sequence.activeLoop].end - length,true);
  }
}
void toggleLoopMove(){
  switch(movingLoop){
    case 0:
      movingLoop = 1;
      moveCursor(sequence.loopData[sequence.activeLoop].start-sequence.cursorPos);
      break;
    case -1:
      movingLoop = 1;
      moveCursor(sequence.loopData[sequence.activeLoop].end-sequence.cursorPos);
      break;
    case 1:
      movingLoop = 2;
      break;
    case 2:
      movingLoop = 0;
      break;
  }
}

int16_t changeNoteLength(int val, unsigned short int track, unsigned short int id){
  if(id!=0){
    int newEnd;
    //if it's 1 step long and being increased by sequence.subDivision, make it sequence.subDivision steps long instead of sequence.subDivision+1 steps
    //(just to make editing more intuitive)
    if(sequence.noteData[track][id].endPos - sequence.noteData[track][id].startPos == 1 && val == sequence.subDivision){
      newEnd = sequence.noteData[track][id].endPos + val - 1;
    }
    else{
      newEnd = sequence.noteData[track][id].endPos + val;
    }
    //check and see if there's a different note there
    //if there is, set the new end to be 
    for(uint16_t step = 1; step<=val; step++){
      if(sequence.lookupTable[track][step+sequence.noteData[track][id].endPos] != 0 && sequence.lookupTable[track][step+sequence.noteData[track][id].endPos] != id){
        //if it's the first step, just fail to save time
        if(step == 1)
          return 0;
        //if it's not the first step, set the new end to right before the other note
        else{
          newEnd = sequence.noteData[track][id].endPos+step;
          break;
        }
      }
    }
    //if the new end is before/at the start, don't do anything
    if(newEnd<=sequence.noteData[track][id].startPos)
      return 0;
      // newEnd = sequence.noteData[track][id].startPos+1;
    //if the new end is past/at the end of the seq
    if(newEnd>sequence.sequenceLength){
      newEnd = sequence.sequenceLength;
    }
    Note note = sequence.noteData[track][id];
    int16_t amount = newEnd-note.endPos;
    note.endPos = newEnd;
    sequence.deleteNote_byID(track, id);
    sequence.makeNote(note, track, false);
    return amount;
  }
  return 0;
}


void changeNoteLengthSelected(int amount){
  for(int track = 0; track<sequence.trackData.size(); track++){
    for(int note = 1; note <= sequence.noteData[track].size()-1; note++){
      if(sequence.noteData[track][note].isSelected()){
        changeNoteLength(amount, track, note);
      }
    }
  }
}

int16_t changeNoteLength(int amount){
  if(sequence.selectionCount > 0){
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
    return changeNoteLength(amount, sequence.activeTrack, sequence.IDAtCursor());
  }
}

//this one jumps the cursor to the end or start of the note
void changeNoteLength_jumpToEnds(int16_t amount){
  if(sequence.selectionCount > 0){
    changeNoteLengthSelected(amount);
    vector<uint16_t> bounds = getSelectedNotesBoundingBox();
    //move the cursor to the end/beginning of the selection box
    if(amount>0)
      setCursor(bounds[2]);
    else
      // setCursor(bounds[0]);
      setCursor(bounds[2]-sequence.subDivision);//testing this
  }
  else{
    //if the note was changed
    if(changeNoteLength(amount, sequence.activeTrack, sequence.IDAtCursor()) != 0){
      //if you're shrinking the note
      if(amount<0){
        setCursor(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].endPos-sequence.subDivision);//testing this
        // setCursor(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].startPos);
        //if it's out of view
        // else
          // setCursor(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].endPos+amount);
      }
      //if you're growing it
      else
        // setCursor(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].endPos-sequence.subDivision);
        setCursor(sequence.noteData[sequence.activeTrack][sequence.noteData[sequence.activeTrack].size()-1].endPos-1);//testing this

        // setCursor(sequence.noteData[sequence.activeTrack][sequence.IDAtCursor()].endPos - amount);
    }
  }
}

//sets cursor to the visually nearest note
//steps to pixels = steps*scale
//for a note to be "visually" closer, it needs to have a smaller pixel
//distance from the cursor than another note
//compare trackDistance * trackHeight to stepDistance * scale
float getDistanceFromNoteToCursor(Note note,uint8_t track){
  //if the start of the note is closer than the end
  return sqrt(pow(sequence.activeTrack - track,2)+pow(((abs(note.startPos-sequence.cursorPos)<abs(note.endPos-sequence.cursorPos))?(note.startPos-sequence.cursorPos):(note.endPos-sequence.cursorPos)),2));
}

void setCursorToNearestNote(){
  const float maxPossibleDist = sequence.sequenceLength*sequence.viewScale+sequence.trackData.size()*trackHeight;
  float minDist = maxPossibleDist;
  int minTrack;
  int minNote;
  for(int track = 0; track<sequence.noteData.size(); track++){
    for(int note = 1; note<sequence.noteData[track].size(); note++){
      // //Serial.println("checking n:"+stringify(note)+" t:"+stringify(track));
      // Serial.flush();
      float distance = getDistanceFromNoteToCursor(sequence.noteData[track][note],track);
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
    setCursor((sequence.noteData[minTrack][minNote].startPos<sequence.cursorPos)?sequence.noteData[minTrack][minNote].startPos:sequence.noteData[minTrack][minNote].endPos-1);
    setActiveTrack(minTrack,false);
  }
}
