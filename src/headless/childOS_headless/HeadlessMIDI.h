/*
 + -------------------
 |  Virtual MIDI I/O using ChildOS headless.
 |  We use RTMidi to read in and send out midi messages.
 |  Got the MIDI Messages byte values from:
 |  https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message
 + -------------------
 */

#include <CoreAudio/CoreAudio.h>
#include <CoreMIDI/CoreMIDI.h>
#include <CoreFoundation/CoreFoundation.h>

#define __MACOSX_CORE__
#include "rtmidi-6.0.0/RtMidi.h"

void handleCC_Normal(uint8_t channel, uint8_t cc, uint8_t value);
void handleNoteOn_Normal(uint8_t channel, uint8_t note, uint8_t velocity);
void handleNoteOff_Normal(uint8_t channel, uint8_t note, uint8_t velocity);
void handleStart_Normal();
void handleStop_Normal();

void handleStop_playing();
void handleClock_playing();
void handleStart_playing();

void handleStop_recording();
void handleClock_recording();
void handleStart_recording();
void handleNoteOn_Recording(uint8_t channel, uint8_t note, uint8_t velocity);
void handleNoteOff_Recording(uint8_t channel, uint8_t note, uint8_t velocity);
void handleCC_Recording(uint8_t channel, uint8_t cc, uint8_t value);

void MidiInputCallback( double deltatime, vector< unsigned char > *message, void *userData ){
    #ifdef MIDI_DEBUG
    //debug printing the message bytes out
    cout<<"---------------------\n";
    for(uint8_t i = 0; i<message->size(); i++){
        cout<<to_string(message->at(i))<<endl;
    }
    #endif
    //get the message
    uint8_t size = message->size();
    uint8_t type = 0;
    uint8_t channel = 0;
    uint8_t pitch = 0;
    uint8_t velocity = 0;
    if(size>0){
        type = (message->at(0))>>4;//dropping the last 4 bits to get the message type
        channel = (message->at(0))&15;//masking off the first 8 bits to get the channel number
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
    
    if(playing){
        switch(type){
            //note off -- 1000
            case 8:
                handleNoteOff_Normal(channel, pitch, velocity);
                break;
            //note on -- 1001
            case 9:
                handleNoteOn_Normal(channel, pitch, velocity);
                break;
            //CC messages -- 1011
            case 11:
                handleCC_Normal(channel,pitch,velocity);
                break;
            //Real-Time message -- type will be 1111
            case 15:
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
                
        }
    }
    else if(recording){
        switch(type){
            //note off
            case 8:
                handleNoteOff_Recording(channel, pitch, velocity);
                break;
            //note on
            case 9:
                handleNoteOn_Recording(channel, pitch, velocity);
                break;
            //CC messages
            case 11:
                handleCC_Recording(channel,pitch,velocity);
                break;
            //Real-Time message -- type will be 1111
            case 15:
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
        }
    }
    else{
        switch(type){
                //note off
            case 8:
                handleNoteOff_Normal(channel, pitch, velocity);
                break;
                //note on
            case 9:
                handleNoteOn_Normal(channel, pitch, velocity);
                break;
                //CC messages
            case 11:
                handleCC_Normal(channel, pitch, velocity);
                break;
                //Real-Time message -- type will be 1111
            case 15:
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
        }
    }
}

RtMidiIn *virtualMidiIn;
RtMidiOut *virtualMidiOut;

class HeadlessMIDI{
    public:
    uint16_t midiChannelFilters[5] = {65535,65535,65535,65535,65535};
    HeadlessMIDI(){
    }

    void sendCC(uint8_t cc, uint8_t v, uint8_t c){
        // 11<<4 to turn it into 10110000, then | with the channel to add the channel
        vector<uint8_t> message = {static_cast<unsigned char>((11<<4)|(c&15)),cc,v};
        virtualMidiOut->sendMessage( &message );
    }
    void allOff(){
        vector<uint8_t> message = {11<<4,123,0};//all off on channel 0 (should you implement the other channels?)
        virtualMidiOut->sendMessage( &message );
    }
    void noteOn(uint8_t pitch, uint8_t vel, uint8_t channel){
        // 11<<4 to turn it into 10010000, then | with the channel to add the channel
        vector<uint8_t> message = {static_cast<unsigned char>((9<<4)|(channel&(uint8_t)15)),pitch,vel};
        virtualMidiOut->sendMessage( &message );
    }
    void noteOff(uint8_t pitch, uint8_t vel, uint8_t channel){
        // 11<<4 to turn it into 10000000, then | with the channel to add the channel
        vector<uint8_t> message = {static_cast<unsigned char>((8<<4)|(channel&15)),pitch,vel};
        virtualMidiOut->sendMessage( &message );
    }
    //not really applicable in headless mode! the midi ports are *always* listening
    void read(){
        return;
    }

    void sendClock(){
        //only one byte -- 11111010
        vector<uint8_t> message = {248};
        virtualMidiOut->sendMessage( &message );
    }
    void sendStart(){
        //only one byte -- 11111000
        vector<uint8_t> message = {250};
        virtualMidiOut->sendMessage( &message );
    }
    void sendStop(){
        //only one byte -- 11111100
        vector<uint8_t> message = {252};
        virtualMidiOut->sendMessage( &message );
    }
    void sendPC(uint8_t port, uint8_t val, uint8_t channel){
        vector<uint8_t> message = {static_cast<unsigned char>((12<<4)|(channel&15)),val};
        virtualMidiOut->sendMessage( &message );
    }
    //function for selecting the correct MIDI API
    RtMidi::Api chooseMidiApi()
    {
      std::vector< RtMidi::Api > apis;
      RtMidi::getCompiledApi(apis);

      if (apis.size() <= 1){
          cout<<"going w/"+RtMidi::getApiDisplayName(apis[0])+" MIDI api\n";
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
    void start(){
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
            cout << "No MIDI ports available!\n";
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
        virtualMidiOut->openVirtualPort();
    }
    bool isThru(uint8_t output){
        return false;
    }
    void setThru(uint8_t which, bool value){
        
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

HeadlessMIDI MIDI;

