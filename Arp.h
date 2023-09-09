//arpeggiator objects! these store which notes they play (scale), in what order (order), how fast (subDiv)
//what does an arpeggiator need? which notes to play, how fast to play them, and in what order
//also, randomness
//arps don't create notes in data, unless you commit. They play 'virtual notes', so no data is actually created unless you want it to be
class Arp{
  public:
    Arp();
    vector<unsigned char> notes;
    vector<unsigned char> order;
    vector<unsigned char> lengths;
    vector<unsigned char> extendedNotes;
    unsigned short int arpSubDiv = 24;
    //activeNote is the index of the order list, we move through order to call notes
    unsigned char activeNote;
    unsigned char range;
    uint8_t channel;
    uint8_t maxVelMod;
    uint8_t minVelMod;
    uint8_t chanceMod;
    uint8_t repMod;
    uint8_t maxPitchMod;
    uint8_t minPitchMod;
    unsigned char playheadPos;//tw0 values, 0 is the step and 1 is what range it's in
    uint8_t playStyle;

    uint8_t lastPitchSent;
    bool playing;
    bool uniformLength;
    bool holding;
    bool getNotesFromExternalInput;
    void grabNotesFromTracks(bool);
    void grabNotesFromPlaylist();
    void playstep();
    void debugPrintArp();
    void selectExtended();
    void start();
    void stop();
    void setOrder();
    bool hasItBeenEnoughTime();
    bool addStepLength(uint16_t,uint8_t);
    bool addStepLength(uint16_t);
    bool addStepLength();
    //vars for the timing function
    unsigned long startTime;
    unsigned int offBy;
    unsigned long timeLastStepPlayed;
    uint16_t stepCount;
};

//arps should have velocity settings per step (parallel to lengths) and also variation factors for
//velocity, length, and chance
//variation factors could have a cool upper bound/lower bound setting screen with 
//three vertical "braces" that sit side by side
//length variation should always be a multiple of a subdiv, like "how many 1/4 notes should
//this play for" and not just a scalar

Arp::Arp(){
  range = 0;
  notes = {};
  lengths = {12,12,12,12,12,12,12,12};//just for testing purposes
  order = {};//default is 'up/down'
  playheadPos = 0;
  activeNote = 0;
  playStyle = 0;
  playing = false;
  uniformLength = true;
  holding = false;
  getNotesFromExternalInput = true;
  channel = 1;

  maxVelMod = 0;
  minVelMod = 0;
  chanceMod = 100;
  repMod = 0;
  maxPitchMod = 0;
  minPitchMod = 0;
}

bool Arp::hasItBeenEnoughTime(){
  if(swung){
    if(micros() - Arp::timeLastStepPlayed >= MicroSperTimeStep+swingOffset(stepCount)){
      if(!(stepCount%swingSubDiv)){
        Arp:: offBy = (micros()-Arp::startTime)%MicroSperTimeStep;
      }
      if(Arp::offBy == 0)
        Arp::startTime = micros();
      timeLastStepPlayed = micros();
      return true;
    }
    else
      return false;
  }
  else{
    if(micros() - Arp::timeLastStepPlayed + Arp::offBy >= MicroSperTimeStep){
      Arp:: offBy = (micros()-Arp::startTime)%MicroSperTimeStep;
      timeLastStepPlayed = micros();
      if(Arp::offBy == 0)
        Arp::startTime = micros();
      return true;
    }
    else
      return false;
  }
}

void Arp::start(){
  grabNotesFromPlaylist();
  Arp::startTime = micros();
  sendMIDInoteOn(notes[order[activeNote]],100,Arp::channel);
  lastPitchSent = notes[order[activeNote]];
  Arp::timeLastStepPlayed = micros();
  Arp::playing = true;
  // Arp::playheadPos++;
}

void Arp::stop(){
  for(int i = 0; i<notes.size(); i++)
    sendMIDInoteOff(notes[i],0,channel);
  sendMIDInoteOff(lastPitchSent,0,channel);
  //brute force way of doing it
  //the downside is that this will cut off ALL notes that are playing
  // sendMIDIallOff();
  // Serial.println("turning arp off! (Arp::stop())");
  Arp::playheadPos = 0;
  Arp::stepCount = 0;
  Arp::activeNote = 0;
  Arp::playing = false;
}

void Arp::selectExtended(){
  //draw an arial keyboard and let the user select keys to build the scale
  vector<uint8_t> selection = selectKeys();
  extendedNotes = selection;
}

void Arp::grabNotesFromPlaylist(){
  //clearing out notes
  vector<uint8_t> temp;
  notes.swap(temp);
  //adds in notes from the playlist, one copy for each octave, in the order played
  for(int oct = 0; oct<range+1; oct++){
    for(int i = 0; i<playlist.size(); i++){
      notes.push_back(playlist[i][0]+12*oct);
    }
  }
  setOrder();
}

//playheadPos wraps around after each note, it just counts up until the next note needs to be played
void Arp::playstep(){
  //if there are any notes to be played
  if(notes.size()>0){
    if(!holding){
      grabNotesFromPlaylist();
    }
    //if the playhead is past the current active note
    if((uniformLength && playheadPos>arpSubDiv)|| (!uniformLength && playheadPos > lengths[order[activeNote]])){
      sendMIDInoteOff(lastPitchSent,0,channel);
      uint8_t willItRep = random(0,100);
      //if it's not repeating, then play it again
      if(willItRep>=repMod || repMod == 0){
        activeNote++;
      }
      playheadPos = 0;
      if(activeNote>=order.size()){
        activeNote = 0;
        stepCount = 0;
      }
      uint8_t willItPlay = random(0,100);
      //if it plays
      if(willItPlay<chanceMod){
        //add one to the random() bounds if maxPitchMod == 0, so that it doesn't always generate lower pitched notes by excluding 0 as an optino
        int8_t randOctave = random(-minPitchMod/16,maxPitchMod/16+(maxPitchMod==0)?1:0);
        //getting random modifiers
        int16_t randVel = random(64-minVelMod/2,64+maxVelMod/2);
        if(randVel<=0)
          randVel = 0;
        else if(randVel>127)
          randVel = 127;

        //if there's no velocity mod   (when min == max) just set vel to 100
        uint8_t vel = (maxVelMod == minVelMod)?100:randVel;
        int16_t pitch = notes[order[activeNote]];
        //same as vel, if min and max are diffferent then use the modifier
        if(maxPitchMod != minPitchMod)
          pitch+=(12*randOctave);
        sendMIDInoteOn(pitch,vel,channel);
        // Serial.println("v:"+String(vel));
        // Serial.println("p:"+String(pitch));
        lastPitchSent = pitch;
      }
    }
    else{
      playheadPos++;
      stepCount++;
    }
  }
  //if not, try to grab some notes
  else
    grabNotesFromPlaylist();
}

//range
void Arp::grabNotesFromTracks(bool sending){
  notes.erase(notes.begin(),notes.end());
  for(int track = 0; track<trackData.size(); track++){
    if(sending){
      if(trackData[track].noteLastSent != 255){
        notes.push_back(trackData[track].noteLastSent);
      }
    }
    else
      notes.push_back(trackData[track].pitch);
  }
}
//returns false if there are already 32 lengths in the buffer
bool Arp::addStepLength(uint16_t length, uint8_t where){
  if(lengths.size()<32){
    lengths.insert(lengths.begin()+where,length);
    return true;
  }
  return false;
}
bool Arp::addStepLength(uint16_t length){
  return addStepLength(length,lengths.size());
}
//adds a copy of the last step length
bool Arp::addStepLength(){
  return addStepLength(lengths[lengths.size()-1]);
}

void Arp::debugPrintArp(){
  //pring notes, order, and the actual play order
  Serial.println("notes:");
  Serial.print("{");
  for(int i = 0; i<notes.size(); i++){
    Serial.print(notes[i]);
    Serial.print(",");
  }
  Serial.println("}");

  Serial.println("order:");
  Serial.print("[");
  for(int i = 0; i<order.size(); i++){
    Serial.print(order[i]);
    Serial.print(",");
  }
  Serial.println("]");

  Serial.println("what you would hear:");
  Serial.print("(");
  for(int i = 0; i<order.size(); i++){
    Serial.print(notes[order[i]]);
    Serial.print(",");
  }
  Serial.println(")");
  Serial.flush();
}

//sorts thru notes to create an order
void Arp::setOrder(){
  vector<uint8_t> temp;
  order.swap(temp);
  //order played is just the order of the playlist
  //there might be a more efficient way of doing this (like just assigning a range)
  if(playStyle == 0){
    for(int note = 0; note<notes.size(); note++){
      order.push_back(note);
    }
  }
  //descending
  else if(Arp::playStyle == 1){
    //get the highest note
    int highest;
    int highestIndex;

    temp = notes;//this holds the notes that have yet to be ordered, gets deleted from
  
    while(temp.size()>0){
      highestIndex = 0;
      highest = 0;
      for(int i = 0; i<temp.size(); i++){
        if(temp[i]>highest){
          highest = temp[i];
          highestIndex = i;
        }
      }
      order.push_back(highestIndex);
      temp.erase(temp.begin()+highestIndex);//knocking out the notes one by one
    }
  }
  //ascending
  else if(Arp::playStyle == 2){
    //get the lowest note
    int lowest;
    int lowestIndex;

    temp.swap(notes);//this holds the notes that have yet to be ordered, gets deleted from
  
    while(temp.size()>0){
      lowestIndex = 0;
      lowest = 127;
      for(int i = 0; i<temp.size(); i++){
        if(temp[i]<lowest){
          lowest = temp[i];
          lowestIndex = i;
        }
      }
      order.push_back(lowestIndex);
      temp.erase(temp.begin()+lowestIndex);//knocking out the notes one by one
    }
  }
  //up down
  else if(Arp::playStyle == 3){
    //first, organize order low-high
    //get the highest note
    int highest;
    int highestIndex;

    temp.swap(notes);//this holds the notes that have yet to be ordered, gets deleted from
  
    while(temp.size()>0){
      highestIndex = 0;
      highest = 0;
      for(int i = 0; i<temp.size(); i++){
        if(temp[i]>highest){
          highest = temp[i];
          highestIndex = i;
        }
      }
      order.push_back(highestIndex);
      temp.erase(temp.begin()+highestIndex);//knocking out the notes one by one
    }
    //push back the notes in descending order
    for(int i = order.size()-2; i>=0; i--){
      order.push_back(order[i]);
    }
  }
  //down up
  else if(Arp::playStyle == 4){
    //get the lowest note
    int lowest;
    int lowestIndex;

    temp.swap(notes);//this holds the notes that have yet to be ordered, gets deleted from
  
    while(temp.size()>0){
      lowestIndex = 0;
      lowest = 127;
      for(int i = 0; i<temp.size(); i++){
        if(temp[i]<lowest){
          lowest = temp[i];
          lowestIndex = i;
        }
      }
      order.push_back(lowestIndex);
      temp.erase(temp.begin()+lowestIndex);//knocking out the notes one by one
    }
    //push back the notes in ascending order
    for(int i = order.size()-2; i>=0; i--){
      order.push_back(order[i]);
    }
  }
}