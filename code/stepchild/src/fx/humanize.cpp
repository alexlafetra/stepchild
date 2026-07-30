#include "Stepchild.h"


;
using namespace std;


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
  stepchild.display.drawLine(points[0].getX(getPseudoRandom(points[0].theta,radiusAmount,timingAmount))+xCoord,points[0].getY(getPseudoRandom(points[0].theta,radiusAmount,timingAmount))+yCoord,points[s].getX(getPseudoRandom(points[s].theta,radiusAmount,timingAmount))+xCoord,points[s].getY(getPseudoRandom(points[s].theta,radiusAmount,timingAmount))+yCoord,1);
  for(uint8_t i = 1; i<s+1; i++){
    stepchild.display.drawLine(points[i-1].getX(getPseudoRandom(points[i-1].theta,radiusAmount,timingAmount))+xCoord,points[i-1].getY(getPseudoRandom(points[i-1].theta,radiusAmount,timingAmount))+yCoord,points[i].getX(getPseudoRandom(points[i].theta,radiusAmount,timingAmount))+xCoord,points[i].getY(getPseudoRandom(points[i].theta,radiusAmount,timingAmount))+yCoord,1);
  }
}

Note getHumanizedNote(Note& n){
  //position
  int16_t positionOffset = float(stepchild.subDivision)*float(humanizerParameters.timingAmount)/100.0;
  positionOffset = float(positionOffset) * float(random(-100,100))/100.0;
  if(positionOffset + n.startPos < 0)
    positionOffset = -n.startPos;
   if(positionOffset + n.endPos>stepchild.sequenceLength){
    positionOffset = stepchild.sequenceLength - n.endPos;
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
  if(stepchild.selectionCount){
    targetNoteIDs = getSelectedNoteIDs();
  }
  else if(stepchild.IDAtCursor()){
    targetNoteIDs.push_back(NoteID(stepchild.activeTrack,stepchild.IDAtCursor()));
  }
  //grab note objects
  for(NoteID n:targetNoteIDs){
    targetNotes.push_back(NoteTrackPair(n.getNote(),n.track));
  }

  stepchild.deleteNotes_byID(targetNoteIDs);

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

    stepchild.buttons.readInputs();

    if(stepchild.itsbeen(50)){
      if(stepchild.buttons.RIGHT() && humanizerParameters.timingAmount<=95){
        humanizerParameters.timingAmount+=5;
        stepchild.lastTime = millis();
        changed = true;
      }
      else if(stepchild.buttons.LEFT() && humanizerParameters.timingAmount>=5){
        humanizerParameters.timingAmount-=5;
        stepchild.lastTime = millis();
        changed = true;
      }
      if(stepchild.buttons.UP() && humanizerParameters.velocityAmount>=5){
        humanizerParameters.velocityAmount-=5;
        changed = true;
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.DOWN() && humanizerParameters.velocityAmount<=95){
        humanizerParameters.velocityAmount+=5;
        changed = true;
        stepchild.lastTime = millis();
      }
    }
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.MENU()){
        stepchild.lastTime = millis();
        for(NoteTrackPair n:targetNotes){
          stepchild.makeNote(n.note,n.trackID,false);
        }
        return false;
      }
      if(stepchild.buttons.NEW()){
        stepchild.lastTime = millis();
        for(NoteTrackPair n:previewNotes){
          stepchild.makeNote(n.note,n.trackID,false);
        }
        return true;
      }
    }

    //change subdivision
    while(stepchild.buttons.counterB != 0){
      //if shifting, toggle between 1/3 and 1/4 mode
      if(stepchild.buttons.SHIFT()){
        stepchild.toggleTriplets();
        changed = true;
      }
      else if(stepchild.buttons.counterB >= 1){
        stepchild.changeSubDivInt(true);
        changed = true;
      }
      //changing subdivint
      else if(stepchild.buttons.counterB <= -1){
        stepchild.changeSubDivInt(false);
        changed = true;
      }
      stepchild.buttons.countDownB();
    }

    if(changed){
      randomSeed(seed);
      previewNotes = {};
      previewNotes = getHumanizedNotes(targetNotes);
      changed = false;
    }

    stepchild.display.clearDisplay();
    graphics.printSmall(32,1,"% of $: "+stringify(humanizerParameters.timingAmount)+"%",1);
    graphics.drawButton(23,0,"X",1);
    graphics.drawButton(71,0,"B",1);
    graphics.printSmall(80,1,"to change $",1);
    graphics.drawButton(71,8,"M",1);
    graphics.printSmall(80,9,"to quit",1);
    graphics.printSmall(48,9,"V: "+stringify(humanizerParameters.velocityAmount)+"%",1);
    graphics.drawButton(39,8,"Y",1);
    // graphics.printSmall(60,8,stringify(humanizerParameters.chanceAmount),1);
    graphics.drawSeq(settings);
    for(NoteTrackPair n:previewNotes){
      graphics.drawNote(n.note,n.trackID,settings);
    }
    blob.jiggle(float(humanizerParameters.velocityAmount+1)/20.0,float(humanizerParameters.chanceAmount)/10.0,float(humanizerParameters.timingAmount)/500.0+1, 8.0*float(humanizerParameters.timingAmount)/100.0);
    stepchild.display.display();
  }
  return false;
}
