//storage efficient Note class:
class Note {
  public:
    Note();
    Note(unsigned short int, unsigned short int, unsigned char, unsigned char, bool, bool);
    Note(unsigned short int, unsigned short int, unsigned char);
    Note(unsigned short int, unsigned char, unsigned char);
    uint16_t getLength();
    bool isSelected;
    bool muted;
    uint8_t velocity;
    uint8_t chance;
    uint16_t startPos;
    uint16_t endPos;
    // vector<uint8_t> altPitches;//stores a list of alternative pitches this note can trigger
};

//default constructor
Note::Note() {
  startPos = cursorPos;
  endPos = cursorPos + subDivInt;
  velocity = defaultVel;
  isSelected = false;
  chance = 100;
  muted = false;
}
Note::Note(unsigned short int xPos, unsigned short int xEnd, unsigned char vel, unsigned char odds, bool mute, bool select) {
  startPos = xPos;
  endPos = xEnd;
  velocity = vel;
  isSelected = select;
  chance = odds;
  muted = mute;
}
Note::Note(unsigned short int xPos, unsigned short int xEnd, unsigned char vel){
  startPos = xPos;
  endPos = xEnd;
  velocity = vel;
  isSelected = false;
  chance = 100;
  muted = false;
}
Note::Note(unsigned short int xPos, unsigned char vel, unsigned char odds){
  startPos = xPos;
  endPos = xPos + subDivInt;
  velocity = vel;
  isSelected = false;
  chance = odds;
  muted = false;
}
uint16_t Note::getLength(){
  return endPos-startPos+1;
}

void makeNote(Note newNoteOn, int track, bool loudly);


//moves all selected notes (or doesn't)
bool moveSelectedNotes(int xOffset,int yOffset){
  //to temporarily store all the notes
  vector<vector<Note>> selectedNotes;
  selectedNotes.resize(seqData.size());

  //grab all the selected notes
  for(uint8_t track = 0; track<seqData.size(); track++){
    for(uint16_t note = 1; note<seqData[track].size(); note++){
      //if the note is selected, push it into the buffer and then delete it
      if(seqData[track][note].isSelected){
        selectedNotes[track].push_back(seqData[track][note]);
        deleteNote(track, seqData[track][note].startPos);
        note--;
      }
      if(selectionCount==0){
        break;
      }
    }
    if(selectionCount==0){
      break;
    }
  }
  //check each note in the buffer to see if it's a valid move
  for(uint8_t track = 0; track<selectedNotes.size(); track++){
    for(uint16_t note = 0; note<selectedNotes[track].size(); note++){
      //if it hits a single bad note, then remake all the notes and exit
      if(!checkNoteMove(selectedNotes[track][note],track,track+yOffset,selectedNotes[track][note].startPos+xOffset)){
        for(uint8_t track2 = 0; track2<selectedNotes.size(); track2++){
          for(uint16_t note2  = 0; note2<selectedNotes[track2].size(); note2++){
            //remake old note
            makeNote(selectedNotes[track2][note2],track2,false);
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
      makeNote(track+yOffset, selectedNotes[track][note].startPos+xOffset, length+1, selectedNotes[track][note].velocity, selectedNotes[track][note].chance, selectedNotes[track][note].muted, selectedNotes[track][note].isSelected, false);
    }
  }
  return true;
}

//this one doesn't check for self collision! use it only if the note you're trying to move is already deleted
bool checkNoteMove(Note targetNote, int track, int newTrack, int newStart){
  unsigned short int length = targetNote.endPos-targetNote.startPos;
  //checking bounds
  if(newStart<0 || newStart>seqEnd || newTrack>=trackData.size() || newTrack<0)
    return false;
  //checking lookupData
  for(uint16_t start = newStart; start < newStart+length; start++){
    //for moving horizontally within one track
    if(track == newTrack && lookupData[newTrack][start] != 0)
      return false;
    //for vertical kinds of movement, where you won't collide with yourself
    else if(lookupData[newTrack][start] != 0)
      return false;
  }
  return true;
}

bool checkNoteMove(int id, int track, int newTrack, int newStart){
  Note targetNote = seqData[track][id];
  unsigned short int length = targetNote.endPos-targetNote.startPos;
  //checking bounds
  if(newStart<0 || newStart>seqEnd || newTrack>=trackData.size() || newTrack<0){
    return false;
  }
  //checking lookupData
  for(uint16_t start = newStart; start < newStart+length; start++){
    //for moving horizontally within one track
    if(track == newTrack){
      if(lookupData[newTrack][start] != 0 && lookupData[newTrack][start] != id){
        return false;
      }
    }
    //for vertical kinds of movement, where you won't collide with yourself
    else{
      if(lookupData[newTrack][start] != 0){
        return false;
      } 
    }
  }
  return true;
}


bool moveNote(int id,int track,int newTrack,int newStart){
  //if there's room
  if(checkNoteMove(id, track, newTrack, newStart)){
    Note targetNote = seqData[track][id];
    uint16_t length = targetNote.endPos-targetNote.startPos;
    //clear out old note
    deleteNote(track, targetNote.startPos);
    //make room
    makeNote(newTrack, newStart, length+1, targetNote.velocity, targetNote.chance, targetNote.muted, targetNote.isSelected, false);
    return true;
  }
  else{
    return false;
  }
}

void muteSelectedNotes(){
  uint16_t count = 0;
  for(uint8_t track = 0; track<seqData.size(); track++){
    for(uint16_t note = 1; note<seqData[track].size(); note++){
      if(seqData[track][note].isSelected){
        seqData[track][note].muted = true;
        count++;
      }
      if(count>=selectionCount){
        return;
      }
    }
  }
}

void unmuteSelectedNotes(){
  uint16_t count = 0;
  for(uint8_t track = 0; track<seqData.size(); track++){
    for(uint16_t note = 1; note<seqData[track].size(); note++){
      if(seqData[track][note].isSelected){
        seqData[track][note].muted = false;
        count++;
      }
      if(count>=selectionCount){
        return;
      }
    }
  }
}

void muteNote(unsigned short int track, unsigned short int id, bool toggle){
  if(id != 0){
    if(toggle){
      seqData[track][id].muted = !seqData[track][id].muted;
    }
    else{
      seqData[track][id].muted = true;
    }
  }
  else
    return;
}

void unmuteNote(unsigned short int track, unsigned short int id, bool toggle){
  if(id != 0){
    if(toggle){
      seqData[track][id].muted = !seqData[track][id].muted;
    }
    else{
      seqData[track][id].muted = false;
    }
  }
  else
    return;
}