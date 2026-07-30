/*
 + -------------------
 |  Virtual MIDI I/O using ChildOS headless.
 |  We use RTMidi to read in and send out midi messages.
 |  Got the MIDI Messages byte values from:
 |  https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message
 |  The RTMIDI setup/boilerplate code is from:
 |  https://www.music.mcgill.ca/~gary/rtmidi/
 + -------------------
 */
#include "headlessMIDI.h"

PlayState sequenceState = STOPPED;

RtMidiIn *virtualMidiIn;
RtMidiOut *virtualMidiOut;

void MidiInputCallback( double deltatime, std::vector< unsigned char > *message, void *userData ){
#ifdef MIDI_DEBUG
  //debug printing the message bytes out
  cout<<"---------------------\n";
  for(uint8_t i = 0; i<message->size(); i++){
    cout<<to_string(message->at(i))<<endl;
  }
#endif
  //get the message
  uint8_t size = message->size();
  MIDI_MESSAGE_TYPE type = NONE;
  uint8_t channel = 0;
  uint8_t pitch = 0;
  uint8_t velocity = 0;
  if(size>0){
    type = static_cast<MIDI_MESSAGE_TYPE>((message->at(0))>>4);//dropping the last 4 bits to get the message type
    channel = ((message->at(0))&15)+1;//masking off the first 8 bits to get the channel number
    //and add 1 to make it consistent with the hardware midi API
  }
  if(size>1)
    pitch = message->at(1);
  if(size>2)
    velocity = message->at(2);
#ifdef MIDI_DEBUG
  cout<<"---------- message received! ----------"<<endl;
  cout<<"type: "<<to_string(type)<<endl;
  cout<<"channel: "<<to_string(channel)<<endl;
  cout<<"pitch: "<<to_string(pitch)<<endl;
  cout<<"velocity: "<<to_string(velocity)<<endl;
#endif
  
  if(sequenceState == PLAYING){
    switch(type){
        //note off -- 1000
      case NOTE_OFF:
        handleNoteOff_Normal(channel, pitch, velocity);
        break;
        //note on -- 1001
      case NOTE_ON:
        handleNoteOn_Normal(channel, pitch, velocity);
        break;
        //CC messages -- 1011
      case CC_MESSAGE:
        handleCC_Normal(channel,pitch,velocity);
        break;
        //Real-Time message -- type will be 1111
      case REAL_TIME_MESSAGE:
        switch(channel){
            //clock -- 1000
          case 8:
            handleClock_playing();
            break;
            //start -- 1010
          case 10:
            handleStart_playing();
            break;
            //stop -- 1100
          case 12:
            handleStop_playing();
            break;
        }
        break;
      case NONE:
      default:
        return;
    }
  }
  else if(sequenceState == RECORDING){
    switch(type){
        //note off
      case NOTE_OFF:
        handleNoteOff_Recording(channel, pitch, velocity);
        break;
        //note on
      case NOTE_ON:
        handleNoteOn_Recording(channel, pitch, velocity);
        break;
        //CC messages
      case CC_MESSAGE:
        handleCC_Recording(channel,pitch,velocity);
        break;
        //Real-Time message -- type will be 1111
      case REAL_TIME_MESSAGE:
        switch(channel){
            //clock -- 1000
          case 8:
            handleClock_recording();
            break;
            //start -- 1010
          case 10:
            handleStart_recording();
            break;
            //stop -- 1100
          case 12:
            handleStop_recording();
            break;
        }
        break;
      case NONE:
      default:
        return;
    }
  }
  else{
    switch(type){
        //note off
      case NOTE_OFF:
        handleNoteOff_Normal(channel, pitch, velocity);
        break;
        //note on
      case NOTE_ON:
        handleNoteOn_Normal(channel, pitch, velocity);
        break;
        //CC messages
      case CC_MESSAGE:
        handleCC_Normal(channel, pitch, velocity);
        break;
        //Real-Time message -- type will be 1111
      case REAL_TIME_MESSAGE:
        switch(channel){
            //clock -- 1000
          case 8:
            break;
            //start -- 1010
          case 10:
            handleStart_Normal();
            break;
            //stop -- 1100
          case 12:
            handleStop_Normal();
            break;
        }
        break;
      case NONE:
      default:
        return;
    }
  }
}
	StepchildMIDI::StepchildMIDI(){
  }
  void StepchildMIDI::init(){
    start();
  }
  void StepchildMIDI::processCore1Messages(){
    
  }
  void StepchildMIDI::sendThruOn(uint8_t t, uint8_t a, uint8_t b){
    
  }
  void StepchildMIDI::sendThruOff(uint8_t t, uint8_t a){
    
  }
  void StepchildMIDI::sendThruCC(uint8_t t, uint8_t a, uint8_t b){
    
  }
  void StepchildMIDI::sendThruPB(uint8_t t, int a){
    
  }
  void StepchildMIDI::sendClock(uint8_t t){
    
  }
  void StepchildMIDI::sendCC(uint8_t cc, uint8_t v, uint8_t c){
    // 11<<4 to turn it into 10110000, then | with the channel to add the channel
    //        std::vector<uint8_t> message = {static_cast<unsigned char>((11<<4)|(c&15)),cc,v};
    //or, you can use the binary numbersdirectly
    std::vector<uint8_t> message = {static_cast<unsigned char>(0b10110000|(0b01111111&c)),cc,v};
    virtualMidiOut->sendMessage( &message );
  }
  void StepchildMIDI::allOff(){
    //        std::vector<uint8_t> message = {11<<4,123,0};//all off on channel 0 (should you implement the other channels?)
    std::vector<uint8_t> message = {0b10110000,123,0};//all off on channel 0 (should you implement the other channels?)
    virtualMidiOut->sendMessage( &message );
  }
  void StepchildMIDI::noteOn(uint8_t pitch, uint8_t vel, uint8_t channel){
    // 11<<4 to turn it into 10010000, then | with the channel to add the channel
    std::vector<uint8_t> message = {static_cast<unsigned char>(0b10010000|(channel&(uint8_t)15)),pitch,vel};
    virtualMidiOut->sendMessage( &message );
  }
  void StepchildMIDI::noteOff(uint8_t pitch, uint8_t vel, uint8_t channel){
    // 11<<4 to turn it into 10000000, then | with the channel to add the channel
    //        std::vector<uint8_t> message = {static_cast<unsigned char>((8<<4)|(channel&15)),pitch,vel};
    std::vector<uint8_t> message = {static_cast<unsigned char>(0b10000000|(channel&15)),pitch,vel};
    virtualMidiOut->sendMessage( &message );
  }
  //not really applicable in headless mode! the midi ports are *always* listening
  void StepchildMIDI::read(){
    return;
  }
  
  void StepchildMIDI::sendClock(){
    //only one byte -- 11111010
    std::vector<uint8_t> message = {248};
    virtualMidiOut->sendMessage( &message );
  }
  void StepchildMIDI::sendStart(){
    //only one byte -- 11111000
    std::vector<uint8_t> message = {250};
    virtualMidiOut->sendMessage( &message );
  }
  void StepchildMIDI::sendStop(){
    //only one byte -- 11111100
    std::vector<uint8_t> message = {252};
    virtualMidiOut->sendMessage( &message );
  }
  void StepchildMIDI::sendPC(uint8_t port, uint8_t val, uint8_t channel){
    std::vector<uint8_t> message = {static_cast<unsigned char>((12<<4)|(channel&15)),val};
    virtualMidiOut->sendMessage( &message );
  }
  //function for selecting the correct MIDI API
  RtMidi::Api StepchildMIDI::chooseMidiApi()
  {
    std::vector< RtMidi::Api > apis;
    RtMidi::getCompiledApi(apis);
    
    if (apis.size() <= 1){
      std::cout<<"going w/"+RtMidi::getApiDisplayName(apis[0])+" MIDI api\n";
      return apis[0];
    }
    
    std::cout << "\nAPIs\n  API #0: unspecified / default\n";
    for (size_t n = 0; n < apis.size(); n++)
      std::cout << "  API #" << apis[n] << ": " << RtMidi::getApiDisplayName(apis[n]) << "\n";
    
    std::cout << "\nChoose an API number: ";
    unsigned int i;
    std::cin >> i;
    
    std::string dummy;
    std::getline(std::cin, dummy);  // used to clear out stdin
    
    return static_cast<RtMidi::Api>(i);
  }
  void StepchildMIDI::start(){
    //Setting up MIDI Input
    //-------------------------------------
    //init the virtualMidiIn
    try {
      virtualMidiIn = new RtMidiIn();
    } catch (RtMidiError &error) {
      error.printMessage();
    }
    // Check available ports.
    unsigned int nPorts = virtualMidiIn->getPortCount();
    if ( nPorts == 0 ) {
      std::cout << "No MIDI ports available!\n";
    }
    //if there's a port, open it and set up MIDI Input
    else{
      virtualMidiIn->openPort( 0 );
      // Don't ignore sysex, timing, or active sensing messages.
      virtualMidiIn->ignoreTypes( false, false, false );
      
      // Set our callback function.  This should be done immediately after
      // opening the port to avoid having incoming messages written to the
      // queue.
      virtualMidiIn->setCallback( &MidiInputCallback );
    }
    
    //Setting up MIDI Output
    //-------------------------------------
    try{
      virtualMidiOut = new RtMidiOut(chooseMidiApi());
    }
    catch ( RtMidiError &error ) {
      error.printMessage();
    }
    virtualMidiOut->openVirtualPort("Stepchild (headless)");
  }
  bool StepchildMIDI::isThru(uint8_t output){
    return dummyThruSettings[output];
  }
  void StepchildMIDI::setThru(uint8_t which, bool value){
    dummyThruSettings[which] = value;
  }
  //checks if a port is filtering a channel
  bool StepchildMIDI::isChannelActive(uint8_t whichChannel,uint8_t whichPort){
    if(this->midiChannelFilters[whichPort] == 65535){
      return true;
    }
    else{
      bool value = (this->midiChannelFilters[whichPort] & (1 << (whichChannel))) != 0 ;
      return value;
    }
  }
  void StepchildMIDI::allChannelsOff(uint8_t whichPort){
    midiChannelFilters[whichPort] = 0;
  }
  void StepchildMIDI::allChannelsOn(uint8_t whichPort){
    midiChannelFilters[whichPort] = 65535;
  }
  void StepchildMIDI::setAllChannels(bool state, uint8_t whichPort){
    if(state)
      allChannelsOn(whichPort);
    else
      allChannelsOff(whichPort);
  }
  bool StepchildMIDI::toggleThru(uint8_t output){
    dummyThruSettings[output] = !dummyThruSettings[output];
    return dummyThruSettings[output];
  }
  bool StepchildMIDI::isMuted(uint8_t which){
    return (muteSettings>>which)&1;
  }
  void StepchildMIDI::toggleMute(uint8_t whichPort){
    muteSettings ^= 1 << whichPort;
  }
  bool StepchildMIDI::isTotallyMuted(uint8_t which){
    if(!this->midiChannelFilters[which])
      return true;
    else
      return false;
  }
  void StepchildMIDI::setMidiChannel(uint8_t channel, uint8_t output, bool status){
    //for activating, you use OR
    if(status){
      uint16_t byte = 1 << (channel-1);
      this->midiChannelFilters[output] = this->midiChannelFilters[output] | byte;
    }
    //for deactivating, you use AND (and NOT to create the mask)
    else{
      uint16_t byte = ~(1 << (channel-1));
      this->midiChannelFilters[output] = this->midiChannelFilters[output] & byte;
    }
  }
  //toggles the channel on an output, and returns its new value
  //toggles the channel on an output, and returns its new value
  bool StepchildMIDI::toggleMidiChannel(uint8_t channel, uint8_t output){
    midiChannelFilters[output] ^= 1 << channel;
    return this->isChannelActive(channel, output);
  }
  void StepchildMIDI::muteMidiPort(uint8_t which){
    this->midiChannelFilters[which] = 0;
  }
  void StepchildMIDI::unmuteMidiPort(uint8_t which){
    this->midiChannelFilters[which] = 65535;
  }
  void StepchildMIDI::toggleMidiPort(uint8_t which){
    bool isActive = this->toggleMidiChannel(0,which);
    for(int i = 0; i<16; i++){
      this->setMidiChannel(i+1,which,isActive);
    }
  }
StepchildMIDI MIDI;

