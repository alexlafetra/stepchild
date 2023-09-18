vector<uint8_t> selectMultipleTracks(String text){
  //clearing out the selected tracks
  for(int i = 0; i<trackData.size(); i++){
    if(trackData[i].isSelected){
      trackData[i].isSelected = false;
    }
  }
  //to hold the id's and return
  vector<uint8_t> selection;
  while(true){
    display.clearDisplay();
    drawSeq(true, false, false, false, true, false, viewStart, viewEnd);
    printSmall(screenWidth-text.length()*4,0,text,1);

    display.setCursor(0,7);
    display.setFont(&FreeSerifItalic9pt7b);
    display.setTextColor(SSD1306_WHITE);
    display.print("Trk");
    display.print(stringify(activeTrack+1));
    display.setFont();

    display.display();
    readButtons();
    joyRead();
    if(itsbeen(100)){
      if(y == 1){
        setActiveTrack(activeTrack+1,true);
        lastTime = millis();
      }
      if(y == -1){
        setActiveTrack(activeTrack-1,true);
        lastTime = millis();
      }
    }
    if(itsbeen(200)){
      if(sel && !shift){
        sel = false;
        lastTime = millis();
        trackData[activeTrack].isSelected = !trackData[activeTrack].isSelected;
      }
      //toggle the selection on all of them
      if(shift && sel){
        n = false;
        lastTime = millis();
        trackData[activeTrack].isSelected = !trackData[activeTrack].isSelected;
        for(int i = 0; i<trackData.size(); i++){
          trackData[i].isSelected = trackData[activeTrack].isSelected;
        }
      }
      if(n && !shift){
        n = false;
        lastTime = millis();
        //adding all the selected tracks to the list
        for(int i = 0; i<trackData.size(); i++){
          if(trackData[i].isSelected){
            selection.push_back(i);
            trackData[i].isSelected = false;
          }
        }
        break;
      }
      if(del || menu_Press){
        lastTime = millis();
        for(uint8_t track = 0; track<trackData.size(); track++){
          trackData[track].isSelected = false;
        }
        selection.clear();
        break;
      }
    }
  }
  return selection;
}

vector<uint8_t> selectMultipleTracks(){
  String empty;
  return selectMultipleTracks(empty);
}

void muteTrack(unsigned short int id){
  trackData[id].isMuted = true;
}
void unmuteTrack(uint16_t id){
  trackData[id].isMuted = false;
}

void toggleMute(uint16_t id){
  trackData[id].isMuted = !trackData[id].isMuted;
}

void muteMultipleTracks(vector<uint8_t> ids){
  for(int track = 0; track<ids.size(); track++){
    muteTrack(ids[track]);
  }
}

void soloTrack(unsigned short int id){
  for(int track = 0; track<trackData.size(); track++){
    if(track != id){
      muteTrack(track);
      trackData[track].isSolo = false;
    }
    else 
      trackData[id].isSolo = true;
  }
}

void unsoloTrack(uint16_t id){
  for(int track = 0; track<trackData.size(); track++){
    if(track != id)
      unmuteTrack(track);
    else
      trackData[id].isSolo = false;
  }
}

void toggleSolo(uint16_t id){
  if(trackData[id].isSolo)
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
  if(track1 < trackData.size() && track2 < trackData.size() && track1>=0 && track2 >= 0){
    //swapping track data
    Track old_activeTrack = trackData[track1];
    trackData[track1] = trackData[track2];
    trackData[track2] = old_activeTrack;
    //swapping lookupData
    vector<unsigned short int> old_lookupData = lookupData[track1];
    lookupData[track1] = lookupData[track2];
    lookupData[track2] = old_lookupData;
    //swapping sequence data
    vector<Note> old_seqData = seqData[track1];
    seqData[track1] = seqData[track2];
    seqData[track2] = old_seqData;
  }
}

void swapTracks(){
  slideMenuOut(1,7);
  unsigned short int track1 = activeTrack;
  sel = false;
  while(true){
    if(itsbeen(60)){
      if(y == 1){
        swapTracks(track1,activeTrack+1);
        setActiveTrack(activeTrack+1,true);
        track1 = activeTrack;
        lastTime = millis();
      }
      if(y == -1){
        swapTracks(track1,activeTrack-1);
        setActiveTrack(activeTrack-1,true);
        track1 = activeTrack;
        lastTime = millis();
      }
    }
    if(itsbeen(200)){
      if(sel || menu_Press){
        sel = false;
        menu_Press = false;
        lastTime = millis();
        break;
      }
    }
    joyRead();
    readButtons();
    display.clearDisplay();
    drawSeq();
    if(millis()%1000 >= 500){
      display.drawBitmap(6,0,arrow_1_bmp,16,16,SSD1306_WHITE);
    }
    else{
      display.drawBitmap(6,0,arrow_3_bmp,16,16,SSD1306_WHITE);
    }
    display.display();
  }
  slideMenuIn(1,7);
}

void deleteEmptyTracks(){
  for(int i = 0; i<trackData.size(); i++){
    if(seqData[i].size()-1 == 0 && trackData.size()>1){
      deleteTrack(i);
      i--;
    }
  }
  if(activeTrack>=trackData.size())
    activeTrack = trackData.size()-1;
}

//CHECK do you need to update trackID's??? idk if this will cause problems
//deletes the track AND notes stored within it (from seqData and lookupData)
//as long as you delete tracks from the back, i think this is okay
void deleteTrack(unsigned short int track){
  deleteTrack(track,false);
}

void deleteTrack(unsigned short int track, bool hard, bool askFirst){
  int choice = 1;
  if(askFirst && seqData[track].size()-1>0){
    vector<String> ops = {"nay","yeah"};
    choice = binarySelectionBox(64,32,"nay","yeah","delete track w/"+stringify(seqData[track].size()-1)+" note(s)?");
  }
  if(choice == 1){
    if(trackData.size() == 1 && !hard){
      eraseTrack(track);
      return;
    }
    //if the end track is within view
    if(endTrack == trackData.size()){
      endTrack--;
      if(startTrack>0)
        startTrack--;
    }
    if(activeTrack == track && activeTrack>0){
      activeTrack--;
    }

    eraseTrack(track);

    //making new data without the deleted track
    vector<Track> tempTrackData;
    vector<vector<Note>> tempSeqData;
    vector<vector<uint16_t>> tempLookupData;
    for(uint8_t t = 0; t<trackData.size(); t++){
      if(t != track){
        tempTrackData.push_back(trackData[t]);
        tempSeqData.push_back(seqData[t]);
        tempLookupData.push_back(lookupData[t]);
      }
    }
    trackData.swap(tempTrackData);
    seqData.swap(tempSeqData);
    lookupData.swap(tempLookupData);
    // trackData.erase(trackData.begin() + track);
    // seqData.erase(seqData.begin() + track);
    // lookupData.erase(lookupData.begin() + track);
  }
}

void deleteTrack(unsigned short int track, bool hard){
  deleteTrack(track, hard, true);
}
//deletes all tracks
void deleteAllTracks(){
  while(trackData.size()>0){
    deleteTrack(0,true,false);
  }
}

void shrinkTracks(){
  isShrunk = !isShrunk;
  //if it's already shrunk, toggle it
  if(!isShrunk){
    if(trackData.size()>6){
        maxTracksShown = 6;
        startTrack = 0;
        endTrack = 5;
      }
    else{
      maxTracksShown = trackData.size();
      startTrack = 0;
      endTrack = trackData.size()-1;
    }
    // debugHeight = 16;
  }
  //if it's not shrunk, shrink em
  else{
    maxTracksShown = 16;
    startTrack = 0;
    endTrack = 16;
    // debugHeight = 8;
  }
  setActiveTrack(0,false);
}

void transposeAllChannels(int increment){
  for(int i = 0; i<trackData.size(); i++){
    if(i == activeTrack)
      setTrackChannel(i,trackData[i].channel+increment,true);//only the active track makes a noise
    else
      setTrackChannel(i,trackData[i].channel+increment,false);//quiet bc it'd be crazy
  }
}

void setTrackChannel(int track, int channel, bool loud){
  if(channel>=1 && channel<=16){
    sendMIDInoteOff(trackData[track].pitch,0,trackData[track].channel);
    trackData[track].channel = channel;
    if(loud){
      sendMIDInoteOn(trackData[track].pitch,63,trackData[track].channel);
      sendMIDInoteOff(trackData[track].pitch,0,trackData[track].channel);
    }
  }
}

void transposeAllPitches(int increment){
  for(int i = 0; i<trackData.size(); i++){
    if(i == activeTrack)
      setTrackPitch(i,trackData[i].pitch+increment,true);//only the active track makes a noise
    else
      setTrackPitch(i,trackData[i].pitch+increment,false);//quiet bc it'd be crazy
  }
}

void setTrackPitch(int track, int note, bool loud) {
  if(note>=0 && note<=120){
    sendMIDInoteOff(trackData[track].pitch,0,trackData[track].channel);
    trackData[track].pitch = note;
    if(loud){
      sendMIDInoteOn(trackData[track].pitch,63,trackData[track].channel);
      sendMIDInoteOff(trackData[track].pitch,0,trackData[track].channel);
    }
  }
}

//Tracks ------------------------------------------------------------------------

//above index is an INCLUSIVE lower bound!
int getTrackWithPitch_above(uint8_t pitch, uint8_t aboveIndex){
  for(int i=aboveIndex ; i<trackData.size();i++){
    if(trackData[i].pitch == pitch)
      return i;
  }
  return 0;
}
//returns id of track with a specific pitch, returns -1 if track doesn't exist
int getTrackWithPitch(int pitch, int channel){
  for(int i=0;i<trackData.size();i++){
    if(trackData[i].pitch == pitch && trackData[i].channel == channel)
      return i;
  }
  return -1;
}
int getTrackWithPitch(int pitch){
  for(int i=0;i<trackData.size();i++){
    if(trackData[i].pitch == pitch)
      return i;
  }
  return -1;
}

int makeTrackWithPitch(int pitch, int channel){
  int track  = getTrackWithPitch(pitch,channel);
  if(track == -1){
    track = addTrack_return(pitch, channel, false);
  }
  return track;
}

void addTrack(Track newTrack, bool loudly){
  if(trackData.size()<255){
    trackData.push_back(newTrack);

    //widening lookupData
    lookupData.resize(trackData.size());
    //filling with 0
    lookupData[trackData.size()-1].resize(seqEnd,0);

    //widening seqData
    seqData.resize(trackData.size());
    seqData[trackData.size()-1] = defaultVec;
    activeTrack = trackData.size()-1;

    if(loudly){
      sendMIDInoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      sendMIDInoteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    }
  }
}
void addTrack_noMove(Track newTrack, bool loudly){
  if(trackData.size()<255){
    trackData.push_back(newTrack);

    //widening lookupData
    lookupData.resize(trackData.size());
    //filling with 0
    lookupData[trackData.size()-1].resize(seqEnd,0);

    //widening seqData
    seqData.resize(trackData.size());
    seqData[trackData.size()-1] = defaultVec;

    if(loudly){
      sendMIDInoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      sendMIDInoteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    }
  }
}

//this function should add a new row to the seqData, and lookupData initialized with 64 zeroes
void addTrack(uint8_t pitch, uint8_t channel, bool latch, uint8_t muteGroup, bool primed, bool loudly){
  if(trackData.size()<255){
    Track newTrack(pitch, channel);
    newTrack.isLatched = latch;
    newTrack.muteGroup = muteGroup;
    newTrack.isPrimed = primed;
    trackData.push_back(newTrack);//adding track to vector data
    seqData.resize(trackData.size()); //adding row to seqData
    lookupData.resize(trackData.size()); //adding a row to the lookupData vector, to store lookupID's for notes in this track
    lookupData[trackData.size()-1].resize(seqEnd, 0);//adding all the columns the track holds to the lookupData
    seqData[trackData.size()-1] = defaultVec;//setting the note data for the new track to the default blank data
    activeTrack = trackData.size()-1;
    if(loudly){
      sendMIDInoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      sendMIDInoteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    }
  }
}
void addTrack(unsigned char pitch, unsigned char channel, bool loudly) {
  addTrack(pitch, channel, false, 0, true, loudly);
}
void addTrack(unsigned char pitch) {
  addTrack(pitch, defaultChannel, false);
}
void addTrack(unsigned char pitch, bool loudly) {
  addTrack(pitch, defaultChannel, loudly);
}
int16_t addTrack_return(unsigned short int pitch, unsigned short int channel, bool loudly) {
  if(trackData.size()<256){
    Track newTrack(pitch, channel);
    trackData.push_back(newTrack);
    seqData.resize(trackData.size());
    lookupData.resize(trackData.size());
    lookupData[trackData.size()-1].resize(seqEnd, 0);
    seqData[trackData.size()-1] = defaultVec;
    activeTrack = trackData.size()-1;
    if(loudly){
      sendMIDInoteOn(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
      sendMIDInoteOff(trackData[activeTrack].pitch, defaultVel, trackData[activeTrack].channel);
    }
    return (activeTrack);
  }
  else{
    return -1;
  }
}

void dupeTrack(unsigned short int track){
  if(trackData.size()<256){
    Track newTrack = trackData[track];
    trackData.push_back(newTrack);
    lookupData.push_back(lookupData[track]);
    seqData.push_back(seqData[track]);
  }
}

//erases notes, but doesn't delete track
void eraseTrack() {
  seqData[activeTrack].resize(1);//truncating note data to just the first, blank note
  for (int i = 0; i < seqEnd; i++) {
    clearSelection(activeTrack, i);
    lookupData[activeTrack][i] = 0;
  }
}
void eraseTrack(int track) {
  seqData[track].resize(1);
  for (int i = 0; i < seqEnd; i++) {
    clearSelection(track, i);
    lookupData[track][i] = 0;
  }
}
