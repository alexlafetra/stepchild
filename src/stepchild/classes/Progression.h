//chord object
class Chord{
  public:
  //intervals represented as half steps from root note
  vector<uint8_t> intervals;
  //root note
  uint8_t root;
  //stores the inversion of the chord
  //basically what note is the lowest (defaults to 0 --> the root)
  uint8_t inversion;
  uint16_t length;
  Chord();
  Chord(uint8_t, vector<uint8_t>, uint16_t);
  void play();
  void stop();
  void draw(uint8_t, uint8_t);
  void printPitchList(uint8_t, uint8_t);
  String getPitchList(uint8_t, uint8_t);
  void setRoot(uint8_t);
  void addInterval(uint8_t);
};
Chord::Chord(){
  root = 0;
  intervals = {0};
  length = 24;
}

Chord::Chord(uint8_t r, vector<uint8_t> i, uint16_t l){
  root = r;
  intervals = i;
  length = l;
}

//sends a midi on for each of the notes in the chord
void Chord::play(){
  for(uint8_t i = 0; i<intervals.size(); i++){
    sendMIDInoteOn(intervals[i]+root,127,0);
  }
}

//sends a midi off for each of the notes in the chord
void Chord::stop(){
  for(uint8_t i = 0; i<intervals.size(); i++){
    sendMIDInoteOff(intervals[i]+root,0,0);
  }
}

void Chord::printPitchList(uint8_t x1, uint8_t y1){
  String text = "(";
  for(uint8_t i = 0; i<intervals.size();i++){
    text+=pitchToString(intervals[i]+root,false,true);
    if(i != intervals.size()-1)
      text+=",";
    else
      text+=")";
  }
  printSmall(x1-(text.length()-1)*2,y1,text,SSD1306_WHITE);
}
String Chord::getPitchList(uint8_t x1, uint8_t y1){
  String text;
  for(uint8_t i = 0; i<intervals.size();i++){
    text+=pitchToString(intervals[i]+root,false,true);
    if(i != intervals.size()-1)
      text+=",";
  }
  return text;
}

const uint8_t chordHeight = 2;
//draws a chord
void Chord::draw(uint8_t x1, uint8_t y1){
  if(intervals.size() == 0)
    return;
  int8_t y2 = y1-(chordHeight+1)*intervals.size();
  for(uint8_t i = 0; i<intervals.size(); i++){
    display.drawRoundRect(x1,y2+i*(chordHeight+1),length/4,chordHeight,3,SSD1306_WHITE);
  }
}

class Progression{
  public:
  vector<Chord> chords;
  Progression();
  Progression(vector<Chord>);
  void drawProg(uint8_t, uint8_t, int8_t);
  void add(Chord);
  void duplicate(uint8_t);
  void remove(uint8_t);
  uint8_t getLargestChordSize();
  void commit();
};

Progression::Progression(){
}
Progression::Progression(vector<Chord> c){
  chords = c;
}
void Progression::add(Chord c){
  chords.push_back(c);
}

void Progression::remove(uint8_t chord){
  vector<Chord> newChords;
  for(uint8_t i = 0; i<chords.size(); i++){
    if(i != chord)
      newChords.push_back(chords[i]);
  }
  chords.swap(newChords);
}

void Progression::duplicate(uint8_t chord){
  vector<Chord> newChords;
  for(uint8_t i = 0; i<chords.size(); i++){
    newChords.push_back(chords[i]);
    if(i == chord)
      newChords.push_back(chords[i]);
  }
  chords.swap(newChords);
}

uint8_t Progression::getLargestChordSize(){
  uint8_t largestIndex = 0;
  uint8_t largest = 0;
  for(uint8_t i = 0; i<chords.size(); i++){
    if(chords[i].intervals.size()>largest){
      largest = chords[i].intervals.size();
      largestIndex = i;
    }
  }
  return largest;
}

void Progression::drawProg(uint8_t x1, uint8_t y1,int8_t activeChord){
  if(chords.size()==0)
    return;
  uint16_t x2 = x1;
  uint8_t height = getLargestChordSize()*(chordHeight+1);
  String pitches = "(";
  //go thru each chord
  for(uint8_t i = 0; i<chords.size(); i++){
    chords[i].draw(x2,y1);
    if(activeChord != -1){
      if(i == activeChord){
        drawCurlyBracket(x2+chords[i].length/4,y1+1,chords[i].length/4,0,true,true,1);
        // display.drawFastHLine(x2,y1+3,chords[i].length/4,SSD1306_WHITE);
        chords[i].printPitchList(x2,y1+6);
      }
    }
    x2+=chords[i].length/4+1;
  }
  // display.drawRoundRect(x1-2,y1-height,screenWidth-x1,height+2,3,SSD1306_WHITE);
}

//makes tracks and notes and places them in the sequence
void Progression::commit(){
  // //Serial.println("made it here 0");
  // Serial.flush();
  //get unique pitches
  vector<uint8_t> uniquePitches;
  for(uint8_t i = 0; i<chords.size(); i++){
    // //Serial.println("chord "+String(i)+"---------");
    // //Serial.println("root: "+String(chords[i].root));
    // //Serial.println("i:"+String(i));
    for(uint8_t j = 0; j<chords[i].intervals.size(); j++){
      // //Serial.println(String(chords[i].intervals[j]));
      // //Serial.println("j:"+String(j));
      //if a pitch isn't in the vector, add it
      if(!isInVector(chords[i].intervals[j]+chords[i].root,uniquePitches)){
        uniquePitches.push_back(chords[i].intervals[j]+chords[i].root);
      }
    }
  }
  for(uint8_t i = 0; i<uniquePitches.size(); i++){
    // //Serial.println(uniquePitches[i]);
  }
  // delay(10);
  //for knowing which tracks are 'new'
  //so we don't add a bunch of notes to old tracks
  uint8_t trackOffset = trackData.size();
  //make a new track for each unique pitch
  for(uint8_t i = 0; i<uniquePitches.size(); i++){
    addTrack_noMove(Track(uniquePitches[i],1), false);
  }
  // //Serial.println("made it here 2");
  // //Serial.println("created "+String(uniquePitches.size())+" new tracks");
  // Serial.flush();
  //get the corresponding track for each note and make it
  uint16_t writeHead = 0;//records the start of each chord
  for(uint8_t i = 0; i<chords.size(); i++){
    // //Serial.println("i:"+String(i));
    for(uint8_t j = 0; j<chords[i].intervals.size(); j++){
      // //Serial.println("j:"+String(j));
      uint8_t track = getTrackWithPitch_above(chords[i].intervals[j]+chords[i].root,trackOffset);
      // delay(10);
      Note newNote = Note(writeHead,writeHead+chords[i].length,127,100,false,false);
      makeNote(newNote,track,false);
      // activeTrack = track;
      if(writeHead>=seqEnd)
        return;
    }
    writeHead+=chords[i].length;
  }
}