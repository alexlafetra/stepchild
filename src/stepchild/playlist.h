enum origin {INTERNAL,EXTERNAL};

//stores note data (pitch, vel, channel) and source (INTERNAL, EXTERNAL)
struct NoteSourcePair{
  origin source;
  uint8_t pitch;
  uint8_t vel;
  uint8_t channel;

  NoteSourcePair(uint8_t, uint8_t, uint8_t, origin);
};

NoteSourcePair::NoteSourcePair(uint8_t p, uint8_t v, uint8_t c, origin src){
  pitch = p;
  vel = v;
  channel = c;
  source = src;
}


vector<vector<uint8_t>> playlist;//holds currently transmitting/active notes
//stored as pitch, vel, channel

//returns a list of pitches that are present in the playlist
vector<uint8_t> getUniquePitchesFromPlaylist(){
  //move thru each note and get its 'true' pitch.
  //then, check to see if that pitch is in the new list.
  //if it is, continue with the next note. if not, add 
  //this note and then continue;
  vector<uint8_t> uniquePitches;
  for(uint8_t note = 0; note<playlist.size(); note++){
    //getting the pitch relative to C
    uint8_t uniquePitch = playlist[note][0]%12;
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

void addNoteToPlaylist(uint8_t note, uint8_t vel, uint8_t channel){
  vector<uint8_t> temp = {note,vel,channel};
  playlist.push_back(temp);
}

void subNoteFromPlaylist(uint8_t note){
  vector<vector<uint8_t>> tempList;
  //keep all the notes, EXCEPT for the one you're kicking out
  for(int i = 0; i<playlist.size(); i++){
    if(note != playlist[i][0]){
      tempList.push_back(playlist[i]);
    }
  }
  playlist.swap(tempList);
}

void clearPlaylist(){
  vector<vector<uint8_t>> tempList;
  playlist.swap(tempList);
}

void debugPrintPlaylist(){
  Serial.print("{");
  for(int i = 0; i<playlist.size(); i++){
    Serial.print(playlist[i][0]);
    Serial.print(",");
  }
  Serial.println("}");
  Serial.flush();
}