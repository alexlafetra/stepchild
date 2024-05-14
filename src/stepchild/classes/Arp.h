//arpeggiator objects! these store which notes they play (scale), in what order (order), how fast (subDiv)
//what does an arpeggiator need? which notes to play, how fast to play them, and in what order
//also, randomness
//arps don't create notes in data, unless you commit. They play 'virtual notes', so no data is actually created unless you want it to be
class Arp {
public:
  Arp();
  vector<unsigned char> notes;
  vector<unsigned char> order;
  vector<unsigned char> lengths;
  vector<unsigned char> extendedNotes;

  uint16_t arpSubDiv = 24;

  //activeNote is the index of the order list, we move through order to call notes
  uint8_t activeNote;
  uint8_t range;
  uint8_t channel;
  uint8_t maxVelMod;
  uint8_t minVelMod;
  uint8_t chanceMod;
  uint8_t repMod;
  uint8_t maxPitchMod;
  uint8_t minPitchMod;
  unsigned char playheadPos;  //tw0 values, 0 is the step and 1 is what range it's in
  uint8_t playStyle;
  uint8_t lastPitchSent;
  bool playing;
  bool uniformLength;
  bool holding;
  uint8_t source; //can be 0 (external only) 1 (internal only) or 2 (both)

  void grabNotesFromTracks(bool);
  void grabNotesFromPlaylist();
  void playstep();
  void debugPrintArp();
  void selectExtended();
  void start();
  void stop();
  void setOrder();
  bool hasItBeenEnoughTime();
  bool addStepLength(uint16_t, uint8_t);
  bool addStepLength(uint16_t);
  bool addStepLength();
  uint8_t getOrder();
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

Arp::Arp() {
  range = 0;
  notes = {};
  lengths = { 12, 12, 12, 12, 12, 12, 12, 12 };  //just for testing purposes
  order = {};                                    //default is 'up/down'
  playheadPos = 0;
  activeNote = 0;
  playStyle = 0;
  playing = false;
  uniformLength = true;
  holding = false;
  source = 0;
  channel = 1;

  maxVelMod = 0;
  minVelMod = 0;
  chanceMod = 100;
  repMod = 0;
  maxPitchMod = 0;
  minPitchMod = 0;
}

Arp activeArp;


bool Arp::hasItBeenEnoughTime() {
  if (sequenceClock.isSwinging) {
    if ((micros() - Arp::timeLastStepPlayed) >= (sequenceClock.uSecPerStep + sequenceClock.swingOffset(stepCount))) {
      if (!(stepCount % sequenceClock.swingSubDiv)) {
        Arp::offBy = (micros() - Arp::startTime) % sequenceClock.uSecPerStep;
      }
      if (Arp::offBy == 0)
        Arp::startTime = micros();
      timeLastStepPlayed = micros();
      return true;
    } else
      return false;
  }
  else {
    if (micros() - Arp::timeLastStepPlayed + Arp::offBy >= sequenceClock.uSecPerStep) {
      Arp::offBy = (micros() - Arp::startTime) % sequenceClock.uSecPerStep;
      timeLastStepPlayed = micros();
      if (Arp::offBy == 0)
        Arp::startTime = micros();
      return true;
    } else
      return false;
  }
}

//returns which note is playing based on the active note
uint8_t Arp::getOrder(){
  if(!order.size())
    return 0;
  else
    return order[activeNote%order.size()-1];
}

void Arp::start() {
  grabNotesFromPlaylist();
  Arp::startTime = micros();
    if(order.size()>activeNote){
        if(Arp::notes.size() > getOrder()){
            MIDI.noteOn(notes[getOrder()], 100, Arp::channel);
            lastPitchSent = notes[getOrder()];
            Arp::timeLastStepPlayed = micros();
        }
    }
  Arp::playing = true;
}

void Arp::stop() {
  for (int i = 0; i < notes.size(); i++)
    MIDI.noteOff(notes[i], 0, channel);
  MIDI.noteOff(lastPitchSent, 0, channel);
  //brute force way of doing it
  //the downside is that this will cut off ALL notes that are playing
  // MIDI.allOff();
  // //Serial.println("turning arp off! (Arp::stop())");
  Arp::playheadPos = 0;
  Arp::stepCount = 0;
  Arp::activeNote = 0;
  Arp::playing = false;
}

void Arp::selectExtended() {
  //draw an arial keyboard and let the user select keys to build the scale
  vector<uint8_t> selection = selectKeys(0);
  extendedNotes = selection;
}

void Arp::grabNotesFromPlaylist() {
  //clearing out notes
  vector<uint8_t> temp;
  notes.swap(temp);
  //adds in notes from the playlist, one copy for each octave, in the order played
  for (int oct = 0; oct < range + 1; oct++) {
    switch(source){
      //if it's just external notes
      case 0:
        for (int i = 0; i < receivedNotes.notes.size(); i++) {
          notes.push_back(receivedNotes.notes[i].pitch + 12 * oct);
        }
      break;
      //if it's just internal notes
      case 1:
        for (int i = 0; i < sentNotes.notes.size(); i++) {
          notes.push_back(sentNotes.notes[i].pitch + 12 * oct);
        }
        break;
      //grabbing from both
      case 2:
      //you don't need to do this every time! wasteful
        notes = getUnionPitchList();
        for(uint8_t i = 0; i<notes.size(); i++){
          notes[i] = notes[i]+12*oct;
        }
        break;
    }
  }
  setOrder();
}

//playheadPos wraps around after each note, it just counts up until the next note needs to be played
void Arp::playstep() {
  //if there are any notes to be played
  if (notes.size() > 0) {
    if (!holding) {
      grabNotesFromPlaylist();
    }
    //if the playhead is past the current active note
    if ((uniformLength && playheadPos > arpSubDiv) || (!uniformLength && playheadPos > lengths[getOrder()])) {
      MIDI.noteOff(lastPitchSent, 0, channel);
      uint8_t willItRep = random(0, 100);
      //if it's not repeating, then play it again
      if (willItRep >= repMod || repMod == 0) {
        activeNote++;
      }
      playheadPos = 0;
      if (activeNote >= order.size()) {
        activeNote = 0;
        stepCount = 0;
      }
      uint8_t willItPlay = random(0, 100);
      //if it plays
      if (willItPlay < chanceMod) {
        //add one to the random() bounds if maxPitchMod == 0, so that it doesn't always generate lower pitched notes by excluding 0 as an optino
        int8_t randOctave = random(-minPitchMod / 16, maxPitchMod / 16 + ((maxPitchMod == 0) ? 1 : 0));
        //getting random modifiers
        int16_t randVel = random(64 - minVelMod / 2, 64 + maxVelMod / 2);
        if (randVel <= 0)
          randVel = 0;
        else if (randVel > 127)
          randVel = 127;

        //if there's no velocity mod   (when min == max) just set vel to 100
        uint8_t vel = (maxVelMod == minVelMod) ? 100 : randVel;
        int16_t pitch = notes[getOrder()];
        //same as vel, if min and max are diffferent then use the modifier
        if (maxPitchMod != minPitchMod)
          pitch += (12 * randOctave);
        MIDI.noteOn(pitch, vel, channel);
        lastPitchSent = pitch;
      }
    } else {
      playheadPos++;
      stepCount++;
    }
  }
  //if not, try to grab some notes
  else
    grabNotesFromPlaylist();
}

//range
void Arp::grabNotesFromTracks(bool sending) {
  notes.erase(notes.begin(), notes.end());
  for (int track = 0; track < trackData.size(); track++) {
    if (sending) {
      if (trackData[track].noteLastSent != 255) {
        notes.push_back(trackData[track].noteLastSent);
      }
    } else
      notes.push_back(trackData[track].pitch);
  }
}

//returns false if there are already 32 lengths in the buffer
bool Arp::addStepLength(uint16_t length, uint8_t where) {
  if (lengths.size() < 32) {
    lengths.insert(lengths.begin() + where, length);
    return true;
  }
  return false;
}
bool Arp::addStepLength(uint16_t length) {
  return addStepLength(length, lengths.size());
}
//adds a copy of the last step length
bool Arp::addStepLength() {
  return addStepLength(lengths[lengths.size() - 1]);
}

void Arp::debugPrintArp() {
  //pring notes, order, and the actual play order
  // //Serial.println("notes:");
  // //Serial.print("{");
  // for (int i = 0; i < notes.size(); i++) {
  //   //Serial.print(notes[i]);
  //   //Serial.print(",");
  // }
  // //Serial.println("}");

  // //Serial.println("order:");
  // //Serial.print("[");
  // for (int i = 0; i < order.size(); i++) {
  //   //Serial.print(order[i]);
  //   //Serial.print(",");
  // }
  // //Serial.println("]");

  //Serial.println("what you would hear:");
  //Serial.print("(");
  for (int i = 0; i < order.size(); i++) {
    //Serial.print(notes[order[i]]);
    //Serial.print(",");
  }
  //Serial.println(")");
}

bool compareArpNotes(uint8_t id1, uint8_t id2) {
  return activeArp.notes[id1] > activeArp.notes[id2];
}

bool randomSort(uint8_t id1, uint8_t id2) {
  return random(0, 2);
}

//sorts thru notes to create an order
void Arp::setOrder() {
  vector<uint8_t> temp;
  // order.swap(temp);
  order.erase(order.begin(), order.end());
  //order played is just the order of the playlist
  //there might be a more efficient way of doing this (like just assigning a range)
  for (int note = 0; note < notes.size(); note++) {
    order.push_back(note);
  }
  switch (playStyle) {
    //play order
    case 0:
      break;
    //descending by pitch
    case 1:
      sort(order.begin(), order.end(), compareArpNotes);
      break;
    //ascending
    case 2:
      sort(order.begin(), order.end(), compareArpNotes);
      reverse(order.begin(), order.end());
      break;
    //up down
    case 3:
      {
        //copy order into up
        vector<uint8_t> up = order;
        //clear order
        order.erase(order.begin(), order.end());
        //sort up by descending
        sort(up.begin(), up.end(), compareArpNotes);
        //copy to down, then reverse down so it's sorted by ascending
        vector<uint8_t> down = up;
        reverse(down.begin(), down.end());
        //push a value from up, then down, into order
        for (uint8_t i = 1; i < up.size(); i += 2) {
          order.push_back(up[i-1]);
          order.push_back(down[i]);
        }
      }
      break;
    //down up
    case 4:
      {
        //copy order into up
        vector<uint8_t> up = order;
        //clear order
        order.erase(order.begin(), order.end());
        //sort up by descending
        sort(up.begin(), up.end(), compareArpNotes);
        //copy to down, then reverse down so it's sorted by ascending
        vector<uint8_t> down = up;
        reverse(down.begin(), down.end());
        //push a value from down, then up, into order
        for (uint8_t i = 0; i < up.size() - 1; i += 2) {
          order.push_back(down[i]);
          order.push_back(up[i + 1]);
        }
      }
      break;
    //random
    case 5:
      sort(order.begin(), order.end(), randomSort);
      break;
  }
}

void drawArpStepLengths(uint8_t xStart, uint8_t yStart, uint8_t startNote, uint8_t xCursor, bool selected){
  if(activeArp.uniformLength){
    graphics.drawCenteredBanner(64,20,"using uniform steps of "+stepsToMeasures(activeArp.arpSubDiv));
    graphics.drawLabel(64,32,"[controls.SELECT() ] to toggle custom steps",true);
  }
  uint8_t spacing = 3;
  uint8_t thickness = (screenWidth-8)/activeArp.lengths.size()-spacing;
  if(activeArp.lengths.size()>=8){
    thickness = (screenWidth-12)/8-spacing;
  }
  uint8_t height;
  for(uint8_t i = 0; i<8; i++){
    //only draw blocks for lengths that exist (in case there are less than 16)
    //also, only draw blocks that will still be on screen
    if(i<activeArp.lengths.size()){
      height = float(activeArp.lengths[i+startNote])*float(64-23)/float(96);
      //drawing filled rect for steps that correspond to currently
      //held notes
      if(i + startNote<activeArp.notes.size())
        display.fillRect(9+(spacing+thickness)*i,screenHeight-height-7, thickness, height,SSD1306_WHITE);
      //and empty rects for steps that don't
      else
        display.drawRect(9+(spacing+thickness)*i,screenHeight-height-7, thickness, height,SSD1306_WHITE);
      //highlighting the step that's currently playing
      if(activeArp.playing && (i+startNote == activeArp.activeNote)){
        // display.fillRect(9+(spacing+thickness)*i,screenHeight-6,thickness,6,1);
        display.drawRect(7+(spacing+thickness)*i,screenHeight-height-9, thickness+4, height+4,SSD1306_WHITE);
      }
      printSmall(10+(spacing+thickness)*i+thickness/2-stringify(i+startNote+1).length()*2,screenHeight-5,stringify(i+startNote),2);
      //step the cursor is on
      if(i == xCursor-startNote){
        graphics.drawArrow(9+(spacing+thickness)*i+thickness/2,screenHeight-height-10+2*((millis()/200)%2),3,3,true);
        printSmall(8+(spacing+thickness)*i+thickness/2-stepsToMeasures(activeArp.lengths[i+startNote]).length()*2+2,screenHeight-height-21+2*((millis()/200)%2),stepsToMeasures(activeArp.lengths[i+startNote]),SSD1306_WHITE);
      }
      //if there are steps offscreen
      if(startNote>0){
        graphics.drawArrow(2+2*((millis()/200)%2),61,2,1,true);
      }
      if(activeArp.lengths.size()>startNote+8){
        graphics.drawArrow(screenWidth-2-2*((millis()/200)%2),61,2,0,true);
      }
    }
  }
  // printArp(28,0,"step lengths",SSD1306_WHITE);
  // printSmall_centered(64,0,"steps",1);
  display.setRotation(1);
  printItalic(16,0,"steps",1);
  display.setRotation(UPRIGHT);
}
