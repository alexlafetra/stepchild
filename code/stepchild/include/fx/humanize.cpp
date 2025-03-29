
//stores the humanizer parameters.
/*
  timingAmount is the max % of the subDiv that a note can be moved
  eg: 100% means a note can be moved up to 1 subDiv away
  velocityAmount is the amount of change a note's vel data can change
  and chance is the same
*/
class Humanizer{
    public:
      Humanizer();
      Humanizer(int8_t,int8_t,int8_t);
      int8_t timingAmount = 0;
      int8_t velocityAmount = 0;
      int8_t chanceAmount = 0;
      int8_t * get(uint8_t);
};
Humanizer::Humanizer(){
}
Humanizer::Humanizer(int8_t t,int8_t v,int8_t c){
  timingAmount = t;
  velocityAmount = v;
  chanceAmount = c;
}
int8_t * Humanizer::get(uint8_t which){
  switch(which){
    case 0:
      return &timingAmount;
    case 1:
      return &velocityAmount;
    case 3:
      return &chanceAmount;
  }
  return 0;
}

Humanizer humanizerParameters = Humanizer();

struct PolarVertex2D{
  float r;
  float theta;
  float getX(float mod){
    return (r+mod)*cos(theta);
  }
  float getY(float mod){
    return (r+mod)*sin(theta);
  }
};

class HumanizeBlob{
  public:
    HumanizeBlob(float radius, uint8_t numberOfPoints);
    vector<PolarVertex2D> points;
    float increment;
    uint8_t xCoord = 16;
    uint8_t yCoord = 8;
    void jiggle(float amount, float amount2, float amount3, float r);
};

HumanizeBlob::HumanizeBlob(float radius, uint8_t numberOfPoints){
  vector<PolarVertex2D> p;
  increment = 2.0*float(PI)/float(numberOfPoints);
  for(uint8_t i = 0; i<numberOfPoints; i++){
      PolarVertex2D v;
      v.r = radius;
      v.theta = float(i)*increment;
      p.push_back(v);
  }
  points = p;
}
float getPseudoRandom(float theta, float amp,float timeAmp){
    return amp*cos(theta)*sin(millis()/100.0)*sin(theta*PI)*cos(3.0*theta/(2.0*PI)*sin(float(millis())/1000.0*timeAmp));
}

void HumanizeBlob::jiggle(float radiusAmount, float timingAmount, float rotationAmount, float radius){
  const uint16_t s = points.size()-1;
  for(uint8_t i = 0; i<s+1; i++){
    points[i].theta+=rotationAmount/100.0;
    points[i].r = radius;
  }
  display.drawLine(points[0].getX(getPseudoRandom(points[0].theta,radiusAmount,timingAmount))+xCoord,points[0].getY(getPseudoRandom(points[0].theta,radiusAmount,timingAmount))+yCoord,points[s].getX(getPseudoRandom(points[s].theta,radiusAmount,timingAmount))+xCoord,points[s].getY(getPseudoRandom(points[s].theta,radiusAmount,timingAmount))+yCoord,1);
  for(uint8_t i = 1; i<s+1; i++){
    display.drawLine(points[i-1].getX(getPseudoRandom(points[i-1].theta,radiusAmount,timingAmount))+xCoord,points[i-1].getY(getPseudoRandom(points[i-1].theta,radiusAmount,timingAmount))+yCoord,points[i].getX(getPseudoRandom(points[i].theta,radiusAmount,timingAmount))+xCoord,points[i].getY(getPseudoRandom(points[i].theta,radiusAmount,timingAmount))+yCoord,1);
  }
}

Note getHumanizedNote(Note& n){
  //position
  int16_t positionOffset = float(sequence.subDivision)*float(humanizerParameters.timingAmount)/100.0;
  positionOffset = float(positionOffset) * float(random(-100,100))/100.0;
  if(positionOffset + n.startPos < 0)
    positionOffset = -n.startPos;
   if(positionOffset + n.endPos>sequence.sequenceLength){
    positionOffset = sequence.sequenceLength - n.endPos;
  }
  //velocity
  int8_t velOffset = 127.0 * float(humanizerParameters.velocityAmount)/100.0;
  velOffset = float(velOffset) * float(random(-100,100))/100.0;
  if(velOffset + n.velocity < 0)
    velOffset = -n.velocity;
  else if(velOffset + n.velocity > 127)
    velOffset = 127 - n.velocity;
  
  //chance
  int8_t chanceOffset = float(humanizerParameters.chanceAmount)/100.0 * float(random(-100,100));
  if(chanceOffset + n.chance < 0)
    chanceOffset = -n.chance;
  else if(chanceOffset + n.chance > 100)
    chanceOffset = 100 - n.chance;

  Note newNote = n;
  newNote.velocity = velOffset+n.velocity;
  newNote.chance = chanceOffset+n.chance;
  newNote.startPos = positionOffset+n.startPos;
  newNote.endPos = positionOffset+n.endPos;
  newNote.setSelected(false);
  return newNote;
}


vector<NoteTrackPair> getHumanizedNotes(vector<NoteTrackPair> targetNotes){
  vector<NoteTrackPair> notes = {};
  for(NoteTrackPair n:targetNotes){
    notes.push_back(NoteTrackPair(getHumanizedNote(n.note),n.trackID));
  }
  return notes;
}

bool humanize(){
  HumanizeBlob blob = HumanizeBlob(8,20);

  uint16_t seed = millis();

  //list of notes to apply fx to
  vector<NoteID> targetNoteIDs = {};
  vector<NoteTrackPair> targetNotes = {};
  vector<NoteTrackPair> previewNotes = {};
  if(sequence.selectionCount){
    targetNoteIDs = getSelectedNoteIDs();
  }
  else if(sequence.IDAtCursor()){
    targetNoteIDs.push_back(NoteID(sequence.activeTrack,sequence.IDAtCursor()));
  }
  //grab note objects
  for(NoteID n:targetNoteIDs){
    targetNotes.push_back(NoteTrackPair(n.getNote(),n.track));
  }

  sequence.deleteNotes_byID(targetNoteIDs);
  randomSeed(seed);

  //set up render settings
  SequenceRenderSettings settings;
  settings.topLabels = false;
  settings.drawPram = false;
  settings.drawLoopFlags = false;
  settings.shrinkTopDisplay = false;
  // settings.fullyRenderNotes = false;

  //true to start with
  bool changed = true;

  while(true){

    controls.readInputs();

    if(utils.itsbeen(50)){
      if(controls.RIGHT() && humanizerParameters.timingAmount<=95){
        humanizerParameters.timingAmount+=5;
        lastTime = millis();
        changed = true;
      }
      else if(controls.LEFT() && humanizerParameters.timingAmount>=5){
        humanizerParameters.timingAmount-=5;
        lastTime = millis();
        changed = true;
      }
      if(controls.UP() && humanizerParameters.velocityAmount>=5){
        humanizerParameters.velocityAmount-=5;
        changed = true;
        lastTime = millis();
      }
      else if(controls.DOWN() && humanizerParameters.velocityAmount<=95){
        humanizerParameters.velocityAmount+=5;
        changed = true;
        lastTime = millis();
      }
    }
    if(utils.itsbeen(200)){
      if(controls.MENU()){
        lastTime = millis();
        for(NoteTrackPair n:targetNotes){
          sequence.makeNote(n.note,n.trackID,false);
        }
        return false;
      }
      if(controls.NEW()){
        lastTime = millis();
        for(NoteTrackPair n:previewNotes){
          sequence.makeNote(n.note,n.trackID,false);
        }
        return true;
      }
    }

    //change subdivision
    while(controls.counterB != 0){
      //if shifting, toggle between 1/3 and 1/4 mode
      if(controls.SHIFT()){
        sequence.toggleTriplets();
        changed = true;
      }
      else if(controls.counterB >= 1){
        sequence.changeSubDivInt(true);
        changed = true;
      }
      //changing subdivint
      else if(controls.counterB <= -1){
        sequence.changeSubDivInt(false);
        changed = true;
      }
      controls.countDownB();
    }

    if(changed){
      randomSeed(seed);
      previewNotes = {};
      previewNotes = getHumanizedNotes(targetNotes);
      changed = false;
    }

    display.clearDisplay();
    printSmall(32,1,"% of $: "+stringify(humanizerParameters.timingAmount)+"%",1);
    graphics.drawButton(23,0,"X",1);
    graphics.drawButton(71,0,"B",1);
    printSmall(80,1,"to change $",1);
    graphics.drawButton(71,8,"M",1);
    printSmall(80,9,"to quit",1);
    printSmall(48,9,"V: "+stringify(humanizerParameters.velocityAmount)+"%",1);
    graphics.drawButton(39,8,"Y",1);
    // printSmall(60,8,stringify(humanizerParameters.chanceAmount),1);
    drawSeq(settings);
    for(NoteTrackPair n:previewNotes){
      drawNote(n.note,n.trackID,settings);
    }
    blob.jiggle(float(humanizerParameters.velocityAmount+1)/20.0,float(humanizerParameters.chanceAmount)/10.0,float(humanizerParameters.timingAmount)/500.0+1, 8.0*float(humanizerParameters.timingAmount)/100.0);
    display.display();
  }
  return false;
}
