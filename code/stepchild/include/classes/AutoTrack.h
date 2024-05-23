enum TriggerSource:uint8_t{global,track,channel};

class Autotrack{
  public:
    //holds values in the range 0-127, 255 ==> NO value and a control number should not be sent
    vector<uint8_t> data;
    //control number
    uint8_t control;
    //channel val
    uint8_t channel;
    //type of track
    //0 is default, line-based, 1 is sine/cosine based, 2 is square, 3 is saw, 4 is was, 5 is triangle and 6 is random
    //5 is random
    uint8_t type;
    //this is just for the osc-based
    uint16_t period;
    uint16_t phase;
    int8_t amplitude;
    uint8_t yPos;

    //controls whether it's a 0 (default) or 1 (sp404mkII) or 2 (internal) parameter track
    uint8_t parameterType;

    //for recording to it
    bool isPrimed = true;

    // 0 is from external, 1 is encoder A, 2 is encoder B, 3 is X, 4 is Y
    uint8_t recordFrom = 0;


    //can be global, track, or channel
    TriggerSource triggerSource = global;
    //ID of the track, or channel, that the AT will trigger on
    uint8_t triggerTarget = 0;

    //gated means the track turns on/off with it's trigger (if it's a trigger track)
    //if not gated, the track will play continuously
    bool gated = true;
    uint16_t playheadPos = 0;

    //for muting/unmuting it
    bool isActive = true;

    vector<uint16_t> selectedPoints;
    Autotrack();
    Autotrack(uint8_t);
    void sendData(uint16_t);
    void play(uint16_t);
    void setTrigger(TriggerSource trigSource, uint8_t trigTarget);
};

Autotrack::Autotrack(){
  data.push_back(63);
  for(uint16_t i = 1; i<sequence.sequenceLength; i++){
    data.push_back(255);
  }
  control = 1;
  channel = 1;
  type = 0;
  parameterType = 0;
  phase = 0;
  amplitude = 64;
  yPos = 64;
  period = 96;
}

Autotrack::Autotrack(uint8_t t){
  data.push_back(63);
  for(uint16_t i = 1; i<sequence.sequenceLength; i++){
    data.push_back(255);
  }
  control = 1;
  channel = 1;
  type = t;
  parameterType = 0;
  phase = 0;
  amplitude = 64;
  yPos = 64;
  period = 96;
}

void Autotrack::sendData(uint16_t timestep){
  //bounds check for timestep --> timestep will "loop" around if bigger than data
  if(timestep>=data.size())
    timestep%=data.size();
  if(data[timestep] == 255)
    return;
  if(parameterType == 2)
    handleInternalCC(control,data[timestep],channel,yPos);
  else if(data[timestep] != 255)
    MIDI.sendCC(control,data[timestep],channel);
}
void Autotrack::play(uint16_t timestep){
  if(!this->isActive)
    return;
  //normal Autotracks that use the global timestep
  if(this->triggerSource == global){
    this->sendData(timestep);
  }
  //trigger autotracks that use internal playheads
  else{
    this->sendData(this->playheadPos);
    this->playheadPos++;
  }
}

//looks for autotracks to trigger and triggers them
void triggerAutotracks(uint8_t trackID, bool state){
  for(uint8_t i = 0; i<sequence.autotrackDataTest.size(); i++){
    switch(sequence.autotrackDataTest[i].triggerSource){
      case global:
        break;
      case track:
        //if it's a targeted autotrack
        if(sequence.autotrackDataTest[i].triggerTarget == trackID){
          //triggering it on
          if(state){
            sequence.autotrackDataTest[i].isActive = true;
            sequence.autotrackDataTest[i].playheadPos = 0;
          }
          //triggering it off
          else if(sequence.autotrackDataTest[i].gated){
            sequence.autotrackDataTest[i].isActive = false;
            sequence.autotrackDataTest[i].playheadPos = 0;
          }
        }
        break;
      case channel:
        //if it's a targeted autotrack
        if(sequence.autotrackDataTest[i].triggerTarget == sequence.trackData[trackID].channel){
          //triggering it on
          if(state){
            sequence.autotrackDataTest[i].isActive = true;
            sequence.autotrackDataTest[i].playheadPos = 0;
          }
          //triggering it off
          else if(sequence.autotrackDataTest[i].gated){
            sequence.autotrackDataTest[i].isActive = false;
            sequence.autotrackDataTest[i].playheadPos = 0;
          }
        }
        break;
    }
  }
}
void Autotrack::setTrigger(TriggerSource trigSource, uint8_t trigTarget){
  triggerSource = trigSource;
  playheadPos = 0;
  switch(triggerSource){
    case global:
      triggerTarget = 0;
      isActive = true;
      break;
    case track:
      isActive = false;
      if(trigTarget<sequence.trackData.size())
        triggerTarget = trigTarget;
      else
        triggerTarget = 0;
      break;
    case TriggerSource::channel:
      isActive = false;
      if(trigTarget<=16 && trigTarget>=1)
        triggerTarget = trigTarget;
      else
        triggerTarget = 1;
      break;
  }
}
