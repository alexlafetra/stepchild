//unarms any tracks with notes on them
void disarmTracksWithNotes(){
  for(uint8_t i = 0; i<sequence.trackData.size(); i++){
    if(sequence.noteData[i].size()>1){
      sequence.trackData[i].setPrimed(false);
    }
  }
}

vector<uint8_t> selectMultipleTracks(String text){
  //clearing out the selected tracks
  for(int i = 0; i<sequence.trackData.size(); i++){
    if(sequence.trackData[i].isSelected()){
      sequence.trackData[i].setSelected(false);
    }
  }
  //to hold the id's and return
  vector<uint8_t> selection = {};
  while(true){
    display.clearDisplay();
    SequenceRenderSettings settings;
    settings.trackSelection = true;
    settings.drawPram = false;
    settings.shrinkTopDisplay = true;
    settings.topLabels = false;
    drawSeq(settings);
    printSmall(screenWidth-text.length()*4,0,text,1);

    printSmall(2,2,"Trk:"+stringify(sequence.activeTrack+1),1);

    display.display();
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(100)){
      if(controls.joystickY == 1){
        sequence.setActiveTrack(sequence.activeTrack+1,true);
        lastTime = millis();
      }
      if(controls.joystickY == -1){
        sequence.setActiveTrack(sequence.activeTrack-1,true);
        lastTime = millis();
      }
    }
    if(utils.itsbeen(200)){
      if(controls.SELECT()  && !controls.SHIFT()){
        controls.setSELECT(false);
        lastTime = millis();
        sequence.trackData[sequence.activeTrack].setSelected(!sequence.trackData[sequence.activeTrack].isSelected());
      }
      //toggle the selection on all of them
      if(controls.SHIFT() && controls.SELECT() ){
        controls.setNEW(false);
        lastTime = millis();
        sequence.trackData[sequence.activeTrack].setSelected(!sequence.trackData[sequence.activeTrack].isSelected());
        for(int i = 0; i<sequence.trackData.size(); i++){
          sequence.trackData[i].setSelected(sequence.trackData[sequence.activeTrack].isSelected());
        }
      }
      if(controls.NEW() && !controls.SHIFT()){
        controls.setNEW(false);
        lastTime = millis();
        //adding all the selected tracks to the list
        for(int i = 0; i<sequence.trackData.size(); i++){
          if(sequence.trackData[i].isSelected()){
            selection.push_back(i);
            sequence.trackData[i].setSelected(false);
          }
        }
        return selection;
      }
      if(controls.DELETE() || controls.MENU()){
        lastTime = millis();
        for(uint8_t track = 0; track<sequence.trackData.size(); track++){
          sequence.trackData[track].setSelected(false);
        }
        selection.clear();
        break;
      }
    }
  }
  return selection;
}

vector<uint8_t> selectMultipleTracks(){
  return selectMultipleTracks("");
}

void muteTrack(unsigned short int id){
  sequence.trackData[id].setMuted(true);
}
void unmuteTrack(uint16_t id){
  sequence.trackData[id].setMuted(false);
}

void toggleMute(uint16_t id){
  sequence.trackData[id].setMuted(!sequence.trackData[id].isMuted());
}

void muteMultipleTracks(vector<uint8_t> ids){
  for(int track = 0; track<ids.size(); track++){
    muteTrack(ids[track]);
  }
}

void soloTrack(unsigned short int id){
  for(int track = 0; track<sequence.trackData.size(); track++){
    if(track != id){
      muteTrack(track);
      sequence.trackData[track].setSolo(false);
    }
    else 
      sequence.trackData[id].setSolo(true);
  }
}

void unsoloTrack(uint16_t id){
  for(int track = 0; track<sequence.trackData.size(); track++){
    if(track != id)
      unmuteTrack(track);
    else
      sequence.trackData[id].setSolo(false);
  }
}

void toggleSolo(uint16_t id){
  if(sequence.trackData[id].isSolo())
    unsoloTrack(id);
  else
    soloTrack(id);
}

void eraseMultipleTracks(vector<uint8_t> ids){
  for(int track = 0; track<ids.size(); track++){
    eraseTrack(ids[track]);
  }
}

void swapTracks(unsigned short int track1, unsigned short int track2){
  //making sure the tracks are real
  if(track1 < sequence.trackData.size() && track2 < sequence.trackData.size() && track1>=0 && track2 >= 0){
    //swapping track data
    Track old_activeTrack = sequence.trackData[track1];
    sequence.trackData[track1] = sequence.trackData[track2];
    sequence.trackData[track2] = old_activeTrack;
    //swapping lookupData
    vector<unsigned short int> old_lookupTable = sequence.lookupTable[track1];
    sequence.lookupTable[track1] = sequence.lookupTable[track2];
    sequence.lookupTable[track2] = old_lookupTable;
    //swapping sequence data
    vector<Note> old_noteData = sequence.noteData[track1];
    sequence.noteData[track1] = sequence.noteData[track2];
    sequence.noteData[track2] = old_noteData;
  }
}

bool sortTracksByPitch(Track t1, Track t2){
  return t1.pitch>t2.pitch;
}
bool sortTracksByChannel(Track t1, Track t2){
  return t1.channel>t2.channel;
}


void sortTrackData(uint8_t type,uint8_t target){
  //type is either 0 (ascending) or 1 (descending)
  //target is either pitch, channel, or the number of notes
  vector<Track> tempData = sequence.trackData;
  switch(target){
    case 0:
      sort(tempData.begin(),tempData.end(),sortTracksByPitch);
      break;
    case 1:
      sort(tempData.begin(),tempData.end(),sortTracksByChannel);
      break;
  }
  if(type)
    reverse(tempData.begin(),tempData.end());
  sequence.trackData = tempData;
}

void sortTracks(){
  //0 = ascending, 1 = descending
  uint8_t sortType = 0;
  //0 = track pitch, 1 = track channel, 2 =  number of notes (wip)
  int8_t sortTarget = 0;
  const uint8_t x1 = trackDisplay-2;
  const uint8_t y1 = 0;
  while(true){
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(200)){
      if(controls.MENU()){
        lastTime = millis();
        break;
      }
      if(controls.NEW() || controls.SELECT() ){
        sortTrackData(sortType,sortTarget);
        lastTime = millis();
      }
    }
    while(controls.counterA != 0){
      sortType = !sortType;
      controls.counterA+=controls.counterA<0?1:-1;
    }
    while(controls.counterB != 0){
      sortTarget+=controls.counterB<0?-1:1;
      if(sortTarget<0)
        sortTarget = 2;
      if(sortTarget == 3)
        sortTarget = 0;
      controls.counterB+=controls.counterB<0?1:-1;
    }
    display.clearDisplay();
    SequenceRenderSettings settings;
    settings.drawTrackChannel = true;
    drawSeq(settings);
    display.fillRoundRect(x1,y1,85,43,3,0);
    display.drawRoundRect(x1,y1,85,43,3,1);
    printSmall(x1+42,y1+3,"sort by:",1,CENTER);
    if(true){
      String target;
      switch(sortTarget){
        case 0:
          target = "pitch";
          break;
        case 1:
          target = "channel";
          break;
        case 2:
          target = "note count";
          break;
      }
      printItalic(x1+2,y1+10,target,1);
    }
    printSmall(x1+42,y1+19,"in:",1,CENTER);
    printItalic(x1+2,y1+26,sortType?"descending":"ascending",1);
    printSmall(x1+42,y1+35,"order",1,CENTER);
    display.display();
  }
}
void swapTracks(){
  // slideMenuOut(1,7);
  unsigned short int track1 = sequence.activeTrack;
  controls.setSELECT(false);
  while(true){
    if(utils.itsbeen(100)){
      if(controls.joystickY == 1){
        swapTracks(track1,sequence.activeTrack+1);
        sequence.setActiveTrack(sequence.activeTrack+1,true);
        track1 = sequence.activeTrack;
        lastTime = millis();
      }
      if(controls.joystickY == -1){
        swapTracks(track1,sequence.activeTrack-1);
        sequence.setActiveTrack(sequence.activeTrack-1,true);
        track1 = sequence.activeTrack;
        lastTime = millis();
      }
    }
    if(utils.itsbeen(200)){
      if(controls.NEW()){
        lastTime = millis();
        sortTracks();
      }
      if(controls.SELECT()  || controls.MENU()){
        controls.setSELECT(false);
        controls.setMENU(false) ;
        lastTime = millis();
        break;
      }
    }
    controls.readJoystick();
    controls.readButtons();
    display.clearDisplay();
    SequenceRenderSettings settings;
    settings.topLabels = false;
    settings.drawLoopPoints = false;
    drawSeq(settings);
    display.fillRect(70,0,16,16,0);
    if(millis()%1000 >= 500){
      display.drawBitmap(70,0,arrow_1_bmp,16,16,SSD1306_WHITE);
    }
    else{
      display.drawBitmap(70,0,arrow_3_bmp,16,16,SSD1306_WHITE);
    }
    display.fillRoundRect(86,-2,44,9,3,0);
    display.drawRoundRect(86,-2,44,9,3,1);
    printSmall(89,0,"[n] to sort",1);
    display.display();
  }
}

uint8_t countEmptyTracks(){
  uint8_t count = 0;
  for(auto track:sequence.noteData){
    if(track.size()==1){
      count++;
    }
  }
  return count;
}
void deleteEmptyTracks(){
  if(binarySelectionBox(64,32,"nah","yea","delete "+stringify(countEmptyTracks())+" tracks?",drawSeq) != 1){
    return;
  }
  for(uint8_t i = 0; i<sequence.trackData.size(); i++){
    //leave at least 1 track
    if(sequence.noteData[i].size()-1 == 0 && sequence.trackData.size()>1){
      deleteTrack(i);
      i--;
    }
  }
  if(sequence.activeTrack>=sequence.trackData.size())
    sequence.activeTrack = sequence.trackData.size()-1;
}

//CHECK do you need to update trackID's??? idk if this will cause problems
//dels the track AND notes stored within it (from sequence.noteData and sequence.lookupTable)
//as long as you del tracks from the back, i think this is okay
void deleteTrack(unsigned short int track){
  deleteTrack(track,false);
}

void deleteTrack(unsigned short int track, bool hard, bool askFirst){
  int choice = 1;
  if(askFirst && sequence.noteData[track].size()-1>0){
    vector<String> ops = {"nay","yeah"};                                                                                    //this is == 2 instead of 1 because sequence.noteData[track] always has the default note
    choice = binarySelectionBox(64,32,"nay","yeah","del track w/"+stringify(sequence.noteData[track].size()-1)+(sequence.noteData[track].size() == 2?" note?":" notes?"),drawSeq);
  }
  if(choice == 1){
    if(sequence.trackData.size() == 1 && !hard){
      eraseTrack(track);
      return;
    }
    //if the end track is within view
    if(sequence.endTrack == sequence.trackData.size()){
      sequence.endTrack--;
      if(sequence.startTrack>0)
        sequence.startTrack--;
    }
    if(sequence.activeTrack == track && sequence.activeTrack>0){
      sequence.activeTrack--;
    }

    eraseTrack(track);

    //making new data without the deld track
    vector<Track> tempTrackData;
    vector<vector<Note>> tempSeqData;
    vector<vector<uint16_t>> tempLookupData;
    for(uint8_t t = 0; t<sequence.trackData.size(); t++){
      if(t != track){
        tempTrackData.push_back(sequence.trackData[t]);
        tempSeqData.push_back(sequence.noteData[t]);
        tempLookupData.push_back(sequence.lookupTable[t]);
      }
    }
    sequence.trackData.swap(tempTrackData);
    sequence.noteData.swap(tempSeqData);
    sequence.lookupTable.swap(tempLookupData);
  }
}

void deleteTrack(unsigned short int track, bool hard){
  deleteTrack(track, hard, true);
}
//dels all tracks
void deleteAllTracks(){
  while(sequence.trackData.size()>0){
    deleteTrack(0,true,false);
  }
}

void shrinkTracks(){
  isShrunk = !isShrunk;
  //if it's already shrunk, toggle it
  if(!isShrunk){
    if(sequence.trackData.size()>6){
        sequence.maxTracksShown = 6;
        sequence.startTrack = 0;
        sequence.endTrack = 5;
      }
    else{
      sequence.maxTracksShown = sequence.trackData.size();
      sequence.startTrack = 0;
      sequence.endTrack = sequence.trackData.size()-1;
    }
    // headerHeight = 16;
  }
  //if it's not shrunk, shrink em
  else{
    sequence.maxTracksShown = 16;
    sequence.startTrack = 0;
    sequence.endTrack = 16;
    // headerHeight = 8;
  }
  sequence.setActiveTrack(0,false);
}

void transposeAllChannels(int increment){
  for(int i = 0; i<sequence.trackData.size(); i++){
    if(i == sequence.activeTrack)
      setTrackChannel(i,sequence.trackData[i].channel+increment,true);//only the active track makes a noise
    else
      setTrackChannel(i,sequence.trackData[i].channel+increment,false);//quiet bc it'd be crazy
  }
}

void setTrackChannel(int track, int channel, bool loud){
  if(channel>=1 && channel<=16){
    MIDI.noteOff(sequence.trackData[track].pitch,0,sequence.trackData[track].channel);
    sequence.trackData[track].channel = channel;
    if(loud){
      MIDI.noteOn(sequence.trackData[track].pitch,63,sequence.trackData[track].channel);
      MIDI.noteOff(sequence.trackData[track].pitch,0,sequence.trackData[track].channel);
    }
  }
}

void transposeAllPitches(int increment){
  for(int i = 0; i<sequence.trackData.size(); i++){
    if(i == sequence.activeTrack)
      setTrackPitch(i,sequence.trackData[i].pitch+increment,true);//only the active track makes a noise
    else
      setTrackPitch(i,sequence.trackData[i].pitch+increment,false);//quiet bc it'd be crazy
  }
}

void setTrackPitch(int track, int note, bool loud) {
  if(note>=0 && note<=127){
    MIDI.noteOff(sequence.trackData[track].pitch,0,sequence.trackData[track].channel);
    sequence.trackData[track].pitch = note;
    if(loud){
      MIDI.noteOn(sequence.trackData[track].pitch,63,sequence.trackData[track].channel);
      MIDI.noteOff(sequence.trackData[track].pitch,0,sequence.trackData[track].channel);
    }
  }
}

//Tracks ------------------------------------------------------------------------

//above index is an INCLUSIVE lower bound!
int getTrackWithPitch_above(uint8_t pitch, uint8_t aboveIndex){
  for(int i=aboveIndex ; i<sequence.trackData.size();i++){
    if(sequence.trackData[i].pitch == pitch)
      return i;
  }
  return 0;
}
//returns id of track with a specific pitch, returns -1 if track doesn't exist
int getTrackWithPitch(int pitch, int channel){
  for(int i=0;i<sequence.trackData.size();i++){
    if(sequence.trackData[i].pitch == pitch && sequence.trackData[i].channel == channel)
      return i;
  }
  return -1;
}
int getTrackWithPitch(int pitch){
  for(int i=0;i<sequence.trackData.size();i++){
    if(sequence.trackData[i].pitch == pitch)
      return i;
  }
  return -1;
}

int16_t makeTrackWithPitch(int pitch, int channel){
  int16_t track = getTrackWithPitch(pitch,channel);
  if(track == -1){
    return addTrack_return(pitch, channel, false);
  }
  return track;
}

void insertTrack(Track newTrack, uint8_t index){
  //if you're trying to insert past the end!
  if(index>=sequence.trackData.size()){
    //inserting new track
    sequence.trackData.push_back(newTrack);
    //inserting new lookupTable lane
    vector<uint16_t> blankLookupData(sequence.sequenceLength,0);
    sequence.lookupTable.push_back(blankLookupData);
    //inserting new noteData lane
    sequence.noteData.push_back({Note()});
  }
  else{
    //inserting new track
    sequence.trackData.insert(sequence.trackData.begin()+index,newTrack);
    //inserting new lookupTable lane
    vector<uint16_t> blankLookupData(sequence.sequenceLength,0);
    sequence.lookupTable.insert(sequence.lookupTable.begin()+index,blankLookupData);
    //inserting new noteData lane
    sequence.noteData.insert(sequence.noteData.begin()+index,{Note()});
  }
}

void addTrack(Track newTrack, bool loudly){
  if(sequence.trackData.size()<255){
    insertTrack(newTrack,sequence.activeTrack);
    if(loudly){
      MIDI.noteOn(sequence.trackData[sequence.activeTrack].pitch, sequence.defaultVel, sequence.trackData[sequence.activeTrack].channel);
      MIDI.noteOff(sequence.trackData[sequence.activeTrack].pitch, sequence.defaultVel, sequence.trackData[sequence.activeTrack].channel);
    }
  }
}

//this function should add a new row to the sequence.noteData, and sequence.lookupTable initialized with 64 zeroes
void addTrack(uint8_t pitch, uint8_t channel, bool latch, uint8_t muteGroup, bool primed, bool loudly){
  if(sequence.trackData.size()<255){
    Track newTrack(pitch, channel);
    newTrack.setLatched(latch);
    newTrack.muteGroup = muteGroup;
    newTrack.setPrimed(primed);
    addTrack(newTrack,loudly);
  }
}
void addTrack(unsigned char pitch, unsigned char channel, bool loudly) {
  addTrack(pitch, channel, false, 0, true, loudly);
}
void addTrack(unsigned char pitch) {
  addTrack(pitch, sequence.defaultChannel, false);
}
void addTrack(unsigned char pitch, bool loudly) {
  addTrack(pitch, sequence.defaultChannel, loudly);
}

//returns the index of the new track
int16_t addTrack_return(unsigned short int pitch, unsigned short int channel, bool loudly) {
  if(sequence.trackData.size()<256){
    Track newTrack(pitch, channel);
    addTrack(newTrack,loudly);
    return (sequence.activeTrack);
  }
  else{
    return -1;
  }
}

int16_t insertTrack_return(unsigned short int pitch, unsigned short int channel, bool loudly, uint8_t loc){
  if(sequence.trackData.size()<256){
    Track newTrack(pitch, channel);
    insertTrack(newTrack,loc);
    if(loudly){
      MIDI.noteOn(sequence.trackData[sequence.activeTrack].pitch, sequence.defaultVel, sequence.trackData[sequence.activeTrack].channel);
      MIDI.noteOff(sequence.trackData[sequence.activeTrack].pitch, sequence.defaultVel, sequence.trackData[sequence.activeTrack].channel);
    }
    return (loc);
  }
  else{
    return -1;
  }
}

void dupeTrack(unsigned short int track){
  if(sequence.trackData.size()<256){
    Track newTrack = sequence.trackData[track];
    insertTrack(newTrack,track);
  }
}

void eraseTrack(int track) {
  sequence.noteData[track].resize(1);
  for (int i = 0; i < sequence.sequenceLength; i++) {
    clearSelection(track, i);
    sequence.lookupTable[track][i] = 0;
  }
}
//erases notes, but doesn't del track
void eraseTrack() {
  eraseTrack(sequence.activeTrack);
}

void setTrackToNearestPitch(vector<uint8_t>pitches,uint8_t track,bool allowDuplicates){
  int oldPitch = sequence.trackData[track].pitch;
  int pitchDistance = 127;
  int closestPitch = 0;
  int octaveOffset = 12*getOctave(oldPitch);
  for(int i = 0; i<pitches.size(); i++){
    if(abs(pitches[i]+octaveOffset-oldPitch)<pitchDistance){
      pitchDistance = abs(pitches[i]+octaveOffset-oldPitch);
      closestPitch = i;
      //if the track is already that pitch, return
      if(pitchDistance == 0){
        return;
      }
    }
  }
  //if no duplicates are allowed, check to see if there are any other tracks
  //with this pitch
  if(!allowDuplicates){
    for(uint8_t t = 0; t<sequence.trackData.size(); t++){
      if(t!=track && sequence.trackData[t].pitch == pitches[closestPitch]+octaveOffset){
        return;
      }
    }
  }
  setTrackPitch(track,pitches[closestPitch]+octaveOffset,false);
}

//this one won't double up on a pitch
void setTrackToNearestUniquePitch(vector<uint8_t>pitches,int track){
  int oldPitch = sequence.trackData[track].pitch;
  int pitchDistance = 127;
  int closestPitch = 0;
  int octaveOffset = 12*getOctave(oldPitch);
  for(int i = 0; i<pitches.size(); i++){
    if(abs(pitches[i]+octaveOffset-oldPitch)<pitchDistance){
      pitchDistance = abs(pitches[i]+octaveOffset-oldPitch);
      closestPitch = i;
      //if the distance is 0, then the track is already in tune
      if(pitchDistance == 0){
        return;
      }
    }
  }
  //if there's no track already with this pitch, set the track to the pitch
  if(getTrackWithPitch(pitches[closestPitch]+octaveOffset) == -1){
    setTrackPitch(track, pitches[closestPitch]+octaveOffset,false);
  }
  //if there is, run it again without this pitch (as long as there're still pitches left)
  else{
    if(pitches.size()>1){
      vector<uint8_t>newPitches;
      for(uint8_t i = 0; i<pitches.size(); i++){
        if(i != closestPitch)
          newPitches.push_back(pitches[i]);
      }
      pitches.swap(newPitches);
      //recursively run the function again, just without the already-occupied pitch as an option
      setTrackToNearestUniquePitch(pitches, track);
    }
    //if there's only one pitch,set it to it
    else{
      setTrackPitch(track, pitches[closestPitch]+octaveOffset,false);
    }
  }
}

void deleteDuplicateEmptyTracks(){
  for(uint8_t t = 0; t<sequence.trackData.size(); t++){
    for(uint8_t t2 = 0; t2<sequence.trackData.size(); t2++){
      if(t2 == t)
        continue;
      //if a track has the same pitch, channel, and is empty, del it
      if(sequence.trackData[t].pitch == sequence.trackData[t2].pitch &&
         sequence.trackData[t].channel == sequence.trackData[t2].channel &&
         sequence.noteData[t2].size() == 1){
        deleteTrack(t2);
      }
    }
  }
}

void tuneTracksToScale(){
  selectKeysAnimation(true);
  vector<uint8_t> pitches = selectKeys(0);
  selectKeysAnimation(false);
  if(pitches.size() == 0){
    return;
  }
  vector<uint8_t> tracks = selectMultipleTracks("select tracks to tune");
  if(!tracks.size())
    return;
  bool allowDuplicates = binarySelectionBox(64,32,"no","ye","allow duplicate pitches?",drawSeq);
  for(uint8_t track = 0; track < tracks.size(); track++){
    setTrackToNearestPitch(pitches, tracks[track], allowDuplicates);
  }
}
