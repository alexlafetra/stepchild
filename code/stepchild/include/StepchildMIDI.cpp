//from the pico sdk
extern "C" {
#include "pico/util/queue.h"
}
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

const StepchildMIDISettings midiSettings;

//Macro from Arduino MIDI that creates MIDI instances
MIDI_CREATE_CUSTOM_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI0, StepchildMIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, MIDI1, StepchildMIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial2, MIDI2, StepchildMIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(SoftwareSerial, Serial3, MIDI3, StepchildMIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(SoftwareSerial, Serial4, MIDI4, StepchildMIDISettings);

//at some point, create a template class that can store all the MIDI objects

/*

multicore midi buffer using the queue object provided by the pico sdk to pass midi messages from core 1 into core 0
This is an experiment to see if it helps prevent tUSB disconnects!

you should have a way of handling each of the possible messages

good resource for the midi spec:
https://users.cs.cf.ac.uk/dave/Multimedia/node158.html#:~:text=MIDI%20message%20includes%20a%20status%20byte%20and%20up%20to%20two%20data%20bytes.&text=The%20most%20significant%20bit%20of,remaining%20bits%20identify%20the%20message.

*/

typedef struct {
    uint8_t data[3];//holds the 3 data bytes of the midi message
} multicore_midi_message_t;

#define MULTICORE_MIDI_BUFFER_SIZE 16

class StepchildMIDI{
  public:
  //channels are 0 indexed here, but in stepchild world
  //channels are 1-16, with 0 indicating global channel
  uint16_t midiChannelFilters[5] = {0b1111111111111111,0b1111111111111111,0b1111111111111111,0b1111111111111111,0b1111111111111111};
  uint8_t midiMuteSettings = 0b00000000;
  queue_t multicoreBuffer;
  enum MULTICORE_MIDI_MESSAGE:unsigned short int{
    //channel messages
    NOTE_OFF = 0x80,
    NOTE_ON = 0x90,
    CONTROL_CHANGE = 0xB0,
    PROGRAM_CHANGE = 0xC0,
    PITCH_BEND = 0xE0,
    ALL_OFF = 0x7B, //not actually a status byte, it's the 1st data byte of a CC message
    //system messages
    MIDI_CLOCK = 0xF8,
    MIDI_START = 0xFA,
    MIDI_CONTINUE = 0xFB,
    MIDI_STOP = 0xFC
  };

  StepchildMIDI(){
    //initialize the multicore buffer queue
    queue_init(&multicoreBuffer, sizeof(multicore_midi_message_t), MULTICORE_MIDI_BUFFER_SIZE);
  }
  bool pushMessageToQueue(multicore_midi_message_t* message){
    // return queue_try_add(&multicoreBuffer,message);
    return false;
  }
  void init(){
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
    //check if this is called from the slow core
    if(get_core_num() == 1){
      multicore_midi_message_t message;
      message.data[0] = NOTE_ON | channel;
      message.data[1] = pitch;
      message.data[2] = vel;
      if(!pushMessageToQueue(&message)){
      }
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
  void noteOff(uint8_t pitch, uint8_t vel, uint8_t channel){
    //check if this is called from the slow core
    if(get_core_num() == 1){
      multicore_midi_message_t message;
      message.data[0] = NOTE_OFF | channel;
      message.data[1] = pitch;
      message.data[2] = vel;
      if(!pushMessageToQueue(&message)){
      }
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
  void allOff(){
    //check if this is called from the slow core
    if(get_core_num() == 1){
      multicore_midi_message_t message;
      message.data[0] = CONTROL_CHANGE | MIDI_CHANNEL_OMNI;
      message.data[1] = ALL_OFF;
      message.data[2] = 0;
      if(!pushMessageToQueue(&message)){
      }
      return;
    }
    MIDI0.sendControlChange(midi::AllSoundOff,0,MIDI_CHANNEL_OMNI);
    MIDI1.sendControlChange(midi::AllSoundOff,0,MIDI_CHANNEL_OMNI);
    MIDI2.sendControlChange(midi::AllSoundOff,0,MIDI_CHANNEL_OMNI);
    MIDI3.sendControlChange(midi::AllSoundOff,0,MIDI_CHANNEL_OMNI);
    MIDI4.sendControlChange(midi::AllSoundOff,0,MIDI_CHANNEL_OMNI);
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
  //function that gets called from core0 to send out midi data generated by core1

  /*
  u get:
  0b10010001
  u need:
  0b10010001
  
  */
  void processCore1Messages(){
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
  void read(){
    MIDI0.read();
    MIDI1.read();
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
  void allChannelsOff(uint8_t whichPort){
    midiChannelFilters[whichPort] = 0;
  }
  void allChannelsOn(uint8_t whichPort){
    midiChannelFilters[whichPort] = 65535;
  }
  void setAllChannels(bool state, uint8_t whichPort){
    if(state)
      allChannelsOn(whichPort);
    else
      allChannelsOff(whichPort);
  }
  //checks if a port is filtering a channel
  bool isChannelActive(uint8_t whichChannel,uint8_t whichPort){
    //if its a global channel, then yeah the port is never filtering it
    if(!whichChannel)
      return true;
    //decrement by 1!
    whichChannel -= 1;
    return (this->midiChannelFilters[whichPort] & (1 << whichChannel)) != 0 ;
  }
  bool isMuted(uint8_t whichPort){
    return (midiMuteSettings>>whichPort)&1;
  }
  void toggleMute(uint8_t whichPort){
    midiMuteSettings ^= 1 << whichPort;
  }
  void toggleThru(uint8_t output){
    this->setThru(output, !this->isThru(output));
  }

  void setMidiChannel(uint8_t channel, uint8_t output, bool status){
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
  bool toggleMidiChannel(uint8_t channel, uint8_t output){
    if(!channel)
      return true;
    else channel -= 1;
    midiChannelFilters[output] ^= 1 << channel;
    return this->isChannelActive(channel, output);
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
  void sendThruOn(uint8_t channel, uint8_t note, uint8_t vel){
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
  void sendThruOff(uint8_t channel, uint8_t note){
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
  void sendThruCC(uint8_t channel, uint8_t cc, uint8_t val){
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
  void sendThruPB(uint8_t ch, int val){
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
};

//instance that the stepchild's code uses
StepchildMIDI MIDI;