struct CCData{
  CCData(uint8_t, uint8_t, uint8_t);
  CCData();
  uint8_t cc;
  uint8_t val;
  uint8_t channel;
};
CCData::CCData(){
  cc = 0;
  val = 0;
  channel = 0;
}
CCData::CCData(uint8_t param, uint8_t v, uint8_t c){
  cc = param;
  val = v;
  channel = c;
}

//stores note data (pitch, vel, channel) and source (INTERNAL, EXTERNAL)
struct NoteData{
  uint8_t pitch;
  uint8_t vel;
  uint8_t channel;

  NoteData();
  NoteData(uint8_t, uint8_t, uint8_t);
};

NoteData::NoteData(){
  pitch = 255;
  vel = 0;
  channel = 0;
}

NoteData::NoteData(uint8_t p, uint8_t v, uint8_t c){
  pitch = p;
  vel = v;
  channel = c;
}

class PlayList{
  public:
    PlayList();
    vector<NoteData> notes;
    NoteData mostRecentNote;

    //returns a list of all the pitches but with no repeats
    vector<uint8_t> getUniquePitches();
    //adds a note to the PlayList
    void addNote(uint8_t p, uint8_t v, uint8_t c);
    //subtracts a note from the PlayList
    void subNote(uint8_t p);
    //empties the PlayList
    void clear();
    void debugPrint();
    //returns true if "notes" contains a pitch that's equal to p
    bool containsPitch(uint8_t p);
};

//constructor
PlayList::PlayList(){
}

//returns true if "notes" contains a pitch that's equal to p
bool PlayList::containsPitch(uint8_t p){
  for(uint8_t i = 0; i<notes.size(); i++){
    if(notes.at(i).pitch == p){
      return true;
    }
  }
  return false;
}

//returns a list of pitches that are present in the playlist
vector<uint8_t> PlayList::getUniquePitches(){
  //move thru each note and get its 'true' pitch.
  //then, check to see if that pitch is in the new list.
  //if it is, continue with the next note. if not, add 
  //this note and then continue;
  vector<uint8_t> uniquePitches;
  for(uint8_t note = 0; note<notes.size(); note++){
    //getting the pitch relative to C
    uint8_t uniquePitch = notes[note].pitch%12;
    //if it's the first note, add it automatically
    if(uniquePitches.size() == 0){
      uniquePitches.push_back(uniquePitch);
    }
    //if it's not, check to see if it's unique
    else{
      bool unique = true;
      for(uint8_t uniqueP = 0; uniqueP < uniquePitches.size(); uniqueP++){
        if(uniqueP == uniquePitch){
          unique = false;
          break;
        }
      }
      if(unique){
        uniquePitches.push_back(uniquePitch);
      }
    }
  }
  return uniquePitches;
}

void PlayList::addNote(uint8_t p, uint8_t v, uint8_t c){
  NoteData newNote = NoteData(p,v,c);
  notes.push_back(newNote);
}

void PlayList::subNote(uint8_t note){
  vector<NoteData> tempList;
  //keep all the notes, EXCEPT for the one you're kicking out
  for(int i = 0; i<notes.size(); i++){
    if(note != notes[i].pitch){
      tempList.push_back(notes[i]);
    }
  }
  notes.swap(tempList);
}

void PlayList::clear(){
  vector<NoteData> tempList;
  notes.swap(tempList);
}

void PlayList::debugPrint(){
  //Serial.print("{");
  for(int i = 0; i<notes.size(); i++){
    //Serial.print(notes[i].pitch);
    //Serial.print(",");
  }
  //Serial.println("}");
  Serial.flush();
}

//Stores a list of all notes that are either being sent or received as NoteSourcePair structs
//Used to access things like currently sent notes (for the arpeggiator or instruments)
PlayList sentNotes;
PlayList receivedNotes;

//stores the recently received note / cc datapoint
volatile NoteData recentNote;
volatile CCData recentCC;

//returns a vector containing all the pitches in BOTH playlists, with no duplicates
vector<uint8_t> getUnionPitchList(){
  vector<uint8_t> A = receivedNotes.getUniquePitches();
  vector<uint8_t> B = sentNotes.getUniquePitches();
  vector<uint8_t> C;
  for(uint8_t i = 0; i<A.size(); i++){
    if(find(C.begin(),C.end(),A[i]) == C.end()){
      C.push_back(A[i]);
    }
  }
  for(uint8_t i = 0; i<B.size(); i++){
    if(find(C.begin(),C.end(),B[i]) == C.end()){
      C.push_back(B[i]);
    }
  }
  return C;
}
