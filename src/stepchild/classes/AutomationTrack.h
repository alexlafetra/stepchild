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
    uint8_t recordFrom = 1;

    //for muting/unmuting it
    bool isActive = true;
    vector<uint16_t> selectedPoints;
    dataTrack();
    dataTrack(uint8_t);
    void play(uint16_t);
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
  if(parameterType == 2)
    handleInternalCC(control,data[timestep],channel,yPos);
  else if(data[timestep] != 255)
    sendMIDICC(control,data[timestep],channel);
}
