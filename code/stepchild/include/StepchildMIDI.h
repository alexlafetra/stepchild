/*

Stepchild MIDI class!

It's a little ugly because I can't store all the MIDI interface objects
in an array together (they're all different datatypes that are template types, super abstract but 
I bet it's possible)

If this is ever too messy, I could store each interface object (MIDI1, MIDI2...) as a pointer
But I think I would need different pointers for the Hardware/Software/USB interfaces

The headless MIDI class works totally differently.

*/

Adafruit_USBD_MIDI usb_midi;
SoftwareSerial Serial3 = SoftwareSerial(SerialPIO::NOPIN,MIDI_OUT_3);
SoftwareSerial Serial4 = SoftwareSerial(SerialPIO::NOPIN,MIDI_OUT_4);

//Custom MIDI Library settings (see: https://github.com/FortySevenEffects/arduino_midi_library/wiki/Using-custom-Settings)
struct StepchildMIDISettings:public midi::DefaultSettings{
  static const bool UseRunningStatus = false;
  static const bool HandleNullVelocityNoteOnAsNoteOff = true;
  static const bool Use1ByteParsing = true;
  static const unsigned SysExMaxSize = 16;//Stepchild doesn't receive SysEx as of now, but will this crash it?
  static const long BaudRate = 31250;//This one isn't documented, but you need it. Default val is 31250, will this work w/ USB?
};

//Macro from Arduino MIDI that creates MIDI instances
MIDI_CREATE_CUSTOM_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI0, StepchildMIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, MIDI1, StepchildMIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial2, MIDI2, StepchildMIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(SoftwareSerial, Serial3, MIDI3, StepchildMIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(SoftwareSerial, Serial4, MIDI4, StepchildMIDISettings);

//at some point, create a template class that can store all the MIDI objects
template <typename midiInterfaceObject> class midiObject{
  public:
  midiInterfaceObject interface;
  midiObject(){
  }
};

class StepchildMIDI{
  public:
  uint16_t midiChannelFilters[5] = {0b1111111111111111,0b1111111111111111,0b1111111111111111,0b1111111111111111,0b1111111111111111};
  
  StepchildMIDI(){
  }
  void start(){
    //setting MIDI serial ports to non-default pins so they don't conflict
    //with other stepchild features
    Serial1.setRX(MIDI_IN);
    Serial1.setTX(MIDI_OUT_1);
    Serial2.setTX(MIDI_OUT_2);

    MIDI0.begin(MIDI_CHANNEL_OMNI);
    MIDI1.begin(MIDI_CHANNEL_OMNI);
    MIDI2.begin(MIDI_CHANNEL_OMNI);
    MIDI3.begin(MIDI_CHANNEL_OMNI);
    MIDI4.begin(MIDI_CHANNEL_OMNI);

    MIDI0.turnThruOff();
    MIDI1.turnThruOff();
    MIDI2.turnThruOff();
    MIDI3.turnThruOff();
    MIDI4.turnThruOff();
  }
  //Control Change
  void sendCC(uint8_t controller, uint8_t val, uint8_t channel){
    if(this->isChannelActive(channel, 0))
      MIDI0.sendControlChange(controller, val, channel);
    if(this->isChannelActive(channel, 1))
      MIDI1.sendControlChange(controller, val, channel);
    if(this->isChannelActive(channel, 2))
      MIDI2.sendControlChange(controller, val, channel);
    if(this->isChannelActive(channel, 3))
      MIDI3.sendControlChange(controller, val, channel);
    if(this->isChannelActive(channel, 4))
      MIDI4.sendControlChange(controller, val, channel);
  }
  //Program Change
  void sendPC(uint8_t port, uint8_t val, uint8_t channel){
    switch(port){
      case 0:
          MIDI0.sendProgramChange(val,channel);
          break;
      case 1:
          MIDI1.sendProgramChange(val,channel);
          break;
      case 2:
          MIDI2.sendProgramChange(val,channel);
          break;
      case 3:
          MIDI3.sendProgramChange(val,channel);
          break;
      case 4:
          MIDI4.sendProgramChange(val,channel);
          break;
    }
  }
  void noteOn(uint8_t pitch, uint8_t vel, uint8_t channel){
    if(this->isChannelActive(channel, 0))
      MIDI0.sendNoteOn(pitch,vel,channel);
    if(this->isChannelActive(channel, 1))
      MIDI1.sendNoteOn(pitch, vel, channel);
    if(this->isChannelActive(channel, 2))
      MIDI2.sendNoteOn(pitch, vel, channel);
    if(this->isChannelActive(channel, 3))
      MIDI3.sendNoteOn(pitch, vel, channel);
    if(this->isChannelActive(channel, 4))
      MIDI4.sendNoteOn(pitch, vel, channel);
  }
  void noteOff(uint8_t pitch, uint8_t vel, uint8_t channel){
    if(this->isChannelActive(channel, 0))
      MIDI0.sendNoteOff(pitch,vel,channel);
    if(this->isChannelActive(channel, 1))
      MIDI1.sendNoteOff(pitch, vel, channel);
    if(this->isChannelActive(channel, 2))
      MIDI2.sendNoteOff(pitch, vel, channel);
    if(this->isChannelActive(channel, 3))
      MIDI3.sendNoteOff(pitch, vel, channel);
    if(this->isChannelActive(channel, 4))
      MIDI4.sendNoteOff(pitch, vel, channel);
  }
  void allOff(){
    MIDI0.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
    MIDI1.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
    MIDI2.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
    MIDI3.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
    MIDI4.sendControlChange(midi::AllSoundOff,1,MIDI_CHANNEL_OMNI);
  }
  void read(){
    MIDI0.read();
    MIDI1.read();
  }
  void sendClock(){
    MIDI0.sendRealTime(midi::Clock);
    MIDI1.sendRealTime(midi::Clock);
    MIDI2.sendRealTime(midi::Clock);
    MIDI3.sendRealTime(midi::Clock);
    MIDI4.sendRealTime(midi::Clock);

  }
  void sendStop(){
    MIDI0.sendRealTime(midi::Stop);
    MIDI1.sendRealTime(midi::Stop);
    MIDI2.sendRealTime(midi::Stop);
    MIDI3.sendRealTime(midi::Stop);
    MIDI4.sendRealTime(midi::Stop);
  }
  void sendStart(){
    MIDI0.sendRealTime(midi::Start);
    MIDI1.sendRealTime(midi::Start);
    MIDI2.sendRealTime(midi::Start);
    MIDI3.sendRealTime(midi::Start);
    MIDI4.sendRealTime(midi::Start);
  }
  bool isThru(uint8_t output){
    switch(output){
      case 0:
        return MIDI0.getThruState();
      case 1:
        return MIDI1.getThruState();
      case 2:
        return MIDI2.getThruState();
      case 3:
        return MIDI3.getThruState();
      case 4:
        return MIDI4.getThruState();
    }
    //if it's an invalid midi port
    return 0;
  }
  void setThru(uint8_t output, bool state){
    if(state){
      switch(output){
        case 0:
          MIDI0.turnThruOn();
          break;
        case 1:
          MIDI1.turnThruOn();
          break;
        case 2:
          MIDI2.turnThruOn();
          break;
        case 3:
          MIDI3.turnThruOn();
          break;
        case 4:
          MIDI4.turnThruOn();
          break;
      }
    }
    else{
      switch(output){
        case 0:
          MIDI0.turnThruOff();
          break;
        case 1:
          MIDI1.turnThruOff();
          break;
        case 2:
          MIDI2.turnThruOff();
          break;
        case 3:
          MIDI3.turnThruOff();
          break;
        case 4:
          MIDI4.turnThruOff();
          break;
      }
    }
  }
  //checks if a port is filtering a channel
  bool isChannelActive(uint8_t whichChannel,uint8_t whichPort){
    if(this->midiChannelFilters[whichPort] == 65535){
      return true;
    }
    else{
      bool value = (this->midiChannelFilters[whichPort] & (1 << whichChannel-1)) != 0 ;
      return value;
    }
  }
  bool toggleThru(uint8_t output){
    bool isActive = this->isThru(output);
    this->setThru(output, !isActive);
    return !isActive;
  }

  bool isTotallyMuted(uint8_t which){
    if(!this->midiChannelFilters[which])
      return true;
    else
      return false;
  }
  void setMidiChannel(uint8_t channel, uint8_t output, bool status){
    //for activating, you use OR
    if(status){
      uint16_t byte = 1 << channel-1;
      this->midiChannelFilters[output] = this->midiChannelFilters[output] | byte;
    }
    //for deactivating, you use AND (and NOT to create the mask)
    else{
      uint16_t byte = ~(1 << channel-1);
      this->midiChannelFilters[output] = this->midiChannelFilters[output] & byte;
    }
  }
  //toggles the channel on an output, and returns its new value
  bool toggleMidiChannel(uint8_t channel, uint8_t output){
    bool isActive = this->isChannelActive(channel, output);
    this->setMidiChannel(channel, output, !isActive);
    return !isActive;//return new state of channel
  }
  void muteMidiPort(uint8_t which){
    this->midiChannelFilters[which] = 0;
  }
  void unmuteMidiPort(uint8_t which){
    this->midiChannelFilters[which] = 65535;
  }
  void toggleMidiPort(uint8_t which){
    bool isActive = this->toggleMidiChannel(0,which);
    for(int i = 0; i<16; i++){
      this->setMidiChannel(i+1,which,isActive);
    }
  }
};

//instance that the stepchild's code uses
StepchildMIDI MIDI;
