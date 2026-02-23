#include <MIDI.h>
#include "StepchildMIDI.h"
#include "pins.h"

/*

multicore midi buffer using the queue object provided by the pico sdk to pass midi messages from core 1 into core 0
This is an experiment to see if it helps prevent tUSB disconnects!

you should have a way of handling each of the possible messages

good resource for the midi spec:
https://users.cs.cf.ac.uk/dave/Multimedia/node158.html#:~:text=MIDI%20message%20includes%20a%20status%20byte%20and%20up%20to%20two%20data%20bytes.&text=The%20most%20significant%20bit%20of,remaining%20bits%20identify%20the%20message.

*/

Adafruit_USBD_MIDI usb_midi;
SoftwareSerial Serial3 = SoftwareSerial(SerialPIO::NOPIN,MIDI_OUT_3);
SoftwareSerial Serial4 = SoftwareSerial(SerialPIO::NOPIN,MIDI_OUT_4);

const StepchildMIDISettings midiSettings;

//Macro from Arduino MIDI that creates MIDI instances
MIDI_CREATE_CUSTOM_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI0, StepchildMIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, MIDI1, StepchildMIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial2, MIDI2, StepchildMIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(SoftwareSerial, Serial3, MIDI3, StepchildMIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(SoftwareSerial, Serial4, MIDI4, StepchildMIDISettings);

StepchildMIDI::StepchildMIDI(){}

bool StepchildMIDI::pushMessageToQueue(multicore_midi_message_t* message){
  if(queue_try_add(&multicoreBuffer,message)){
    queueIsFull = false;
    return true;
  }
  //set the 'full' flag if the buffer is full
  else if(queue_is_full(&multicoreBuffer)){
    queueIsFull = true;
  }
  return false;
}
void StepchildMIDI::init(){
  //initialize the multicore buffer queue
  queue_init(&multicoreBuffer, sizeof(multicore_midi_message_t), MULTICORE_MIDI_BUFFER_SIZE);

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
void StepchildMIDI::sendCC(uint8_t controller, uint8_t val, uint8_t channel){
  //check if this is called from the slow core
  if(get_core_num() == 1){
    multicore_midi_message_t message;
    message.data[0] = NOTE_ON | channel;
    message.data[1] = controller;
    message.data[2] = val;
    pushMessageToQueue(&message);
    return;
  }
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
void StepchildMIDI::sendPC(uint8_t port, uint8_t val, uint8_t channel){
  //check if this is called from the slow core
  if(get_core_num() == 1){
    multicore_midi_message_t message;
    message.data[0] = NOTE_ON | channel;
    message.data[1] = port;
    message.data[2] = val;
    pushMessageToQueue(&message);
    return;
  }
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
void StepchildMIDI::noteOn(uint8_t pitch, uint8_t vel, uint8_t channel){
  //check if this is called from the slow core
  if(get_core_num() == 1){
    multicore_midi_message_t message;
    message.data[0] = NOTE_ON | channel;
    message.data[1] = pitch;
    message.data[2] = vel;
    pushMessageToQueue(&message);
    return;
  }

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
void StepchildMIDI::noteOff(uint8_t pitch, uint8_t vel, uint8_t channel){
  //check if this is called from the slow core
  if(get_core_num() == 1){
    multicore_midi_message_t message;
    message.data[0] = NOTE_OFF | channel;
    message.data[1] = pitch;
    message.data[2] = vel;
    pushMessageToQueue(&message);
    return;
  }

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
void StepchildMIDI::allOff(){
  //check if this is called from the slow core
  if(get_core_num() == 1){
    multicore_midi_message_t message;
    message.data[0] = CONTROL_CHANGE;
    message.data[1] = ALL_OFF;
    message.data[2] = 1;
    pushMessageToQueue(&message);
    return;
  }
  //send allOff on all channels (this is at most 16x5 = 80 messages sent asap... is this a good idea?)
  for(uint8_t i = 1; i<=16; i++){
    MIDI0.sendControlChange(midi::AllSoundOff,0,i);
    MIDI1.sendControlChange(midi::AllSoundOff,0,i);
    MIDI2.sendControlChange(midi::AllSoundOff,0,i);
    MIDI3.sendControlChange(midi::AllSoundOff,0,i);
    MIDI4.sendControlChange(midi::AllSoundOff,0,i);
  }
}
void StepchildMIDI::sendClock(){
  //check if this is called from the slow core
  if(get_core_num() == 1){
    multicore_midi_message_t message;
    message.data[0] = MIDI_CLOCK;
    pushMessageToQueue(&message);
    return;
  }
  MIDI0.sendRealTime(midi::Clock);
  MIDI1.sendRealTime(midi::Clock);
  MIDI2.sendRealTime(midi::Clock);
  MIDI3.sendRealTime(midi::Clock);
  MIDI4.sendRealTime(midi::Clock);
}
void StepchildMIDI::sendStop(){
  //check if this is called from the slow core
  if(get_core_num() == 1){
    multicore_midi_message_t message;
    message.data[0] = MIDI_STOP;
    pushMessageToQueue(&message);
    return;
  }
  MIDI0.sendRealTime(midi::Stop);
  MIDI1.sendRealTime(midi::Stop);
  MIDI2.sendRealTime(midi::Stop);
  MIDI3.sendRealTime(midi::Stop);
  MIDI4.sendRealTime(midi::Stop);
}
void StepchildMIDI::sendStart(){
    //check if this is called from the slow core
  if(get_core_num() == 1){
    multicore_midi_message_t message;
    message.data[0] = MIDI_START;
    pushMessageToQueue(&message);
    return;
  }
  MIDI0.sendRealTime(midi::Start);
  MIDI1.sendRealTime(midi::Start);
  MIDI2.sendRealTime(midi::Start);
  MIDI3.sendRealTime(midi::Start);
  MIDI4.sendRealTime(midi::Start);
}
//function that gets called from core0 to send out midi data generated by core1
void StepchildMIDI::processCore1Messages(){
  //if there are messages in the buffer
  while(!queue_is_empty(&multicoreBuffer)){

    multicore_midi_message_t message;

    //try to read out a val
    if(!queue_try_remove(&multicoreBuffer,&message)){
      break;
    }
    //if the first 8 bits are set, it's a system message
    MULTICORE_MIDI_MESSAGE type;
    if((message.data[0]&0xF0) == 0xF0){
      type = static_cast<MULTICORE_MIDI_MESSAGE>(message.data[0]);//grab the whole thing
    }
    else{
      //if not, then it's a channel message (like note on/off)
      type = static_cast<MULTICORE_MIDI_MESSAGE>(message.data[0]&0xF0);//just grab the first 4 bits
    }
    uint8_t channel = message.data[0] & 0x0F;//just grab the last four bits
    uint8_t pitch = message.data[1];
    uint8_t vel = message.data[2];

    switch(type){
      case NOTE_OFF:
        noteOff(pitch,vel,channel);
        break;
      case NOTE_ON:
        noteOn(pitch,vel,channel);
        break;
      case CONTROL_CHANGE:
        if(pitch == ALL_OFF){
          allOff();
        }
        else
          sendCC(pitch,vel,channel);
        break;
      case PROGRAM_CHANGE:
        //this one's a little different, and uses port,value,channel, so pitch == port
        sendPC(pitch,vel,channel);
        break;
      //not implemented yet, not sure if stepchild will support this
      case PITCH_BEND:
        break;
      case MIDI_CLOCK:
        sendClock();
        break;
      case MIDI_START:
        sendStart();
        break;
      case MIDI_STOP:
        sendStop();
        break;
    }
  }
}

void StepchildMIDI::read(){
  MIDI0.read();
  MIDI1.read();
}

bool StepchildMIDI::isThru(uint8_t output){
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

void StepchildMIDI::setThru(uint8_t output, bool state){
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
//checks if a port is filtering a channel
bool StepchildMIDI::isChannelActive(uint8_t whichChannel,uint8_t whichPort){
  //if its a global channel, then yeah the port is never filtering it
  if(!whichChannel)
    return true;
  //decrement by 1!
  whichChannel -= 1;
  return (this->midiChannelFilters[whichPort] & (1 << whichChannel)) != 0 ;
}
bool StepchildMIDI::isMuted(uint8_t whichPort){
  return (midiMuteSettings>>whichPort)&1;
}
void StepchildMIDI::toggleMute(uint8_t whichPort){
  midiMuteSettings ^= 1 << whichPort;
}
void StepchildMIDI::toggleThru(uint8_t output){
  this->setThru(output, !this->isThru(output));
}

void StepchildMIDI::setMidiChannel(uint8_t channel, uint8_t output, bool status){
  if(!channel)
    return;
  else channel -= 1;
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
bool StepchildMIDI::toggleMidiChannel(uint8_t channel, uint8_t output){
  if(!channel)
    return true;
  else channel -= 1;
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
void StepchildMIDI::sendThruOn(uint8_t channel, uint8_t note, uint8_t vel){
  //if it's a valid thru & channel
  if(isThru(0) && isChannelActive(channel, 0)){
    MIDI0.sendNoteOn(note,vel,channel);
  }
  if(isThru(1) && isChannelActive(channel, 1)){
    MIDI1.sendNoteOn(note,vel,channel);
  }
  if(isThru(2) && isChannelActive(channel, 2)){
    MIDI2.sendNoteOn(note,vel,channel);
  }
  if(isThru(3) && isChannelActive(channel, 3)){
    MIDI3.sendNoteOn(note,vel,channel);
  }
  if(isThru(4) && isChannelActive(channel, 4)){
    MIDI4.sendNoteOn(note,vel,channel);
  }
}
void StepchildMIDI::sendThruOff(uint8_t channel, uint8_t note){
  //if it's a valid thru & channel
  if(isThru(0) && isChannelActive(channel, 0)){
    MIDI0.sendNoteOff(note,0,channel);
  }
  if(isThru(1) && isChannelActive(channel, 1)){
    MIDI1.sendNoteOff(note,0,channel);
  }
  if(isThru(2) && isChannelActive(channel, 2)){
    MIDI2.sendNoteOff(note,0,channel);
  }
  if(isThru(3) && isChannelActive(channel, 3)){
    MIDI3.sendNoteOff(note,0,channel);
  }
  if(isThru(4) && isChannelActive(channel, 4)){
    MIDI4.sendNoteOff(note,0,channel);
  }
}
void StepchildMIDI::sendThruCC(uint8_t channel, uint8_t cc, uint8_t val){
  //if it's a valid thru & channel
  if(isThru(0) && isChannelActive(channel, 0)){
    MIDI0.sendControlChange(cc,val,channel);
  }
  if(isThru(1) && isChannelActive(channel, 1)){
    MIDI1.sendControlChange(cc,val,channel);
  }
  if(isThru(2) && isChannelActive(channel, 2)){
    MIDI2.sendControlChange(cc,val,channel);
  }
  if(isThru(3) && isChannelActive(channel, 3)){
    MIDI3.sendControlChange(cc,val,channel);
  }
  if(isThru(4) && isChannelActive(channel, 4)){
    MIDI4.sendControlChange(cc,val,channel);
  }
}
void StepchildMIDI::sendThruPB(uint8_t ch, int val){
  //if it's a valid thru & ch
  if(isThru(0) && isChannelActive(ch, 0)){
    MIDI0.sendPitchBend(val, ch);
  }
  if(isThru(1) && isChannelActive(ch, 1)){
    MIDI1.sendPitchBend(val, ch);
  }
  if(isThru(2) && isChannelActive(ch, 2)){
    MIDI2.sendPitchBend(val, ch);
  }
  if(isThru(3) && isChannelActive(ch, 3)){
    MIDI3.sendPitchBend(val, ch);
  }
  if(isThru(4) && isChannelActive(ch, 4)){
    MIDI4.sendPitchBend(val, ch);
  }
}


//I think this is redundant! these disconnects just set the fnptr to null, so it's not needed
//unless you need to be sure you're overwriting everything
void StepchildMIDI::disconnectMIDICallbacks(){
  //disconnecting all the midi callbacks!
#ifndef HEADLESS
  MIDI1.disconnectCallbackFromType(midi::NoteOn);
  MIDI1.disconnectCallbackFromType(midi::NoteOff);
  MIDI1.disconnectCallbackFromType(midi::Clock);
  MIDI1.disconnectCallbackFromType(midi::Start);
  MIDI1.disconnectCallbackFromType(midi::Stop);

  MIDI0.disconnectCallbackFromType(midi::NoteOn);
  MIDI0.disconnectCallbackFromType(midi::NoteOff);
  MIDI0.disconnectCallbackFromType(midi::Clock);
  MIDI0.disconnectCallbackFromType(midi::Start);
  MIDI0.disconnectCallbackFromType(midi::Stop);
#endif
}

void StepchildMIDI::setMIDICallbacks(PlayState state){
  switch(state){
    default:
    case STOPPED:
      MIDI0.setHandleNoteOn(handleNoteOn_Normal);
      MIDI0.setHandleNoteOff(handleNoteOff_Normal);
      MIDI0.setHandleStart(handleStart_Normal);
      MIDI0.setHandleStop(handleStop_Normal);
      MIDI0.setHandleControlChange(handleCC_Normal);
      MIDI0.setHandlePitchBend(handlePB);
      MIDI1.setHandleNoteOn(handleNoteOn_Normal);
      MIDI1.setHandleNoteOff(handleNoteOff_Normal);
      MIDI1.setHandleStart(handleStart_Normal);
      MIDI1.setHandleStop(handleStop_Normal);
      MIDI1.setHandleControlChange(handleCC_Normal);
      MIDI1.setHandlePitchBend(handlePB);
      break;
    case PLAYING:
      MIDI1.setHandleNoteOn(handleNoteOn_Normal);
      MIDI1.setHandleNoteOff(handleNoteOff_Normal);
      MIDI1.setHandleClock(handleClock_playing);
      MIDI1.setHandleStart(handleStart_playing);
      MIDI1.setHandleStop(handleStop_playing);
      MIDI1.setHandlePitchBend(handlePB);
      MIDI1.setHandleControlChange(handleCC_Normal);
      MIDI0.setHandleNoteOn(handleNoteOn_Normal);
      MIDI0.setHandleNoteOff(handleNoteOff_Normal);
      MIDI0.setHandleClock(handleClock_playing);
      MIDI0.setHandleStart(handleStart_playing);
      MIDI0.setHandleStop(handleStop_playing);
      MIDI0.setHandleControlChange(handleCC_Normal);
      MIDI0.setHandlePitchBend(handlePB);
      break;
    case RECORDING:
      MIDI1.setHandleNoteOn(handleNoteOn_Recording);
      MIDI1.setHandleNoteOff(handleNoteOff_Recording);
      MIDI1.setHandleClock(handleClock_recording);
      MIDI1.setHandleStart(handleStart_recording);
      MIDI1.setHandleStop(handleStop_recording);
      MIDI1.setHandleControlChange(handleCC_Recording);
      MIDI0.setHandleNoteOn(handleNoteOn_Recording);
      MIDI0.setHandleNoteOff(handleNoteOff_Recording);
      MIDI0.setHandleClock(handleClock_recording);
      MIDI0.setHandleStart(handleStart_recording);
      MIDI0.setHandleStop(handleStop_recording);
      MIDI0.setHandleControlChange(handleCC_Recording);
      break;
  }
}