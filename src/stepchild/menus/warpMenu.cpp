//Fit 'A' into 'B'

/*
    screen displaying 'A' and 'B' sections
*/

struct NoteTrackPair{
public:
    Note note;
    uint8_t trackID;
};

//Warps the notes contained within the 'A' coordinates into the 'B' coordinates
void warpAintoB(CoordinatePair A, CoordinatePair B){
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
            if(seqData[track][noteID].startPos>=A.x1 && seqData[track][noteID].startPos<A.x2){
                Note targetNote = seqData[track][noteID];
                uint16_t oldLength = targetNote.getLength();
                uint16_t distanceFromStartOfA = targetNote.startPos - A.x1;

                //deleting old note
                deleteNote_byID(track,noteID);
                //make sure to decrement noteID! so you don't warp the same note twice or skip a note
                noteID--;

                Note newNote = targetNote;
                newNote.startPos = distanceFromStartOfA*scaleFactor+B.x1;
                newNote.endPos = (oldLength-1)*scaleFactor;
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

void selectAreasAndWarp(){
    CoordinatePair A = selectArea();
    CoordinatePair B = selectArea();
    warpAintoB(A,B);
}
