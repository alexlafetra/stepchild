class dataTrack{
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

    //Controls whether or not the track is triggered by another track
    //0-127 are trackID's of the trigger track
    //-1 means it's a normal AT
    int8_t triggerSource = -1;
    //gated means the track turns on/off with it's trigger (if it's a trigger track)
    //if not gated, the track will play continuously
    bool gated = true;
    uint16_t playheadPos = 0;

    //for muting/unmuting it
    bool isActive = true;
    vector<uint16_t> selectedPoints;
    dataTrack();
    dataTrack(uint8_t);
    void play(uint16_t);
    void setTrigger(int8_t trigSource);
};

dataTrack::dataTrack(){
  data.push_back(63);
  for(uint16_t i = 1; i<seqEnd; i++){
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

dataTrack::dataTrack(uint8_t t){
  data.push_back(63);
  for(uint16_t i = 1; i<seqEnd; i++){
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

void dataTrack::play(uint16_t timestep){
  //bounds check for timestep --> timestep will "loop" around if bigger than data
  if(timestep>=data.size())
    timestep%=data.size();
  if(data[timestep] == 255)
    return;
  if(parameterType == 2)
    handleInternalCC(control,data[timestep],channel,yPos);
  else if(data[timestep] != 255)
    sendMIDICC(control,data[timestep],channel);
}

//holds all the datatracks
vector<dataTrack> dataTrackData;

//looks for autotracks to trigger and triggers them
void triggerAutotracks(uint8_t trackID, bool state){
  for(uint8_t i = 0; i<dataTrackData.size(); i++){
    //if it's a targeted autotrack
    if(dataTrackData[i].triggerSource == trackID){
      //triggering it on
      if(state){
        dataTrackData[i].isActive = true;
        dataTrackData[i].playheadPos = 0;
      }
      //triggering it off
      else if(dataTrackData[i].gated){
        dataTrackData[i].isActive = false;
        dataTrackData[i].playheadPos = 0;
      }
    }
  }
}
void dataTrack::setTrigger(int8_t trigSource){
  triggerSource = trigSource;
  playheadPos = 0;
  if(triggerSource<0){
    isActive = true;
  }
  else{
    isActive = false;
  }
}
