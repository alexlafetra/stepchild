/*
 + -------------------
 |  Virtual MIDI I/O using ChildOS headless.
 |  We use RTMidi to read in and send out midi messages.
 |  Got the MIDI Messages byte values from:
 |  https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message
 |
 |
 |
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
    //debug printing the message bytes out
    //        cout<<"---------------------\n";
    //        for(uint8_t i = 0; i<message->size(); i++){
    //            cout<<to_string(message->at(i))<<endl;
    //        }
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
    
//    cout<<"---------- message received! ----------"<<endl;
//    cout<<"type: "<<to_string(type)<<endl;
//    cout<<"channel: "<<to_string(channel)<<endl;
//    cout<<"pitch: "<<to_string(pitch)<<endl;
//    cout<<"velocity: "<<to_string(velocity)<<endl;
    
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

RtMidi::Api chooseMidiApi();

RtMidiIn *virtualMidiIn;
RtMidiOut *virtualMidiOut;

void startMIDI(){
    
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

void sendMIDICC(uint8_t cc, uint8_t v, uint8_t c){
    // 11<<4 to turn it into 10110000, then | with the channel to add the channel
    vector<uint8_t> message = {static_cast<unsigned char>((11<<4)|(c&15)),cc,v};
    virtualMidiOut->sendMessage( &message );
}
void sendMIDIallOff(){
    vector<uint8_t> message = {11<<4,123,0};//all off on channel 0 (should you implement the other channels?)
    virtualMidiOut->sendMessage( &message );
}
void sendMIDInoteOn(uint8_t pitch, uint8_t vel, uint8_t channel){
    // 11<<4 to turn it into 10010000, then | with the channel to add the channel
    vector<uint8_t> message = {static_cast<unsigned char>((9<<4)|(channel&(uint8_t)15)),pitch,vel};
    virtualMidiOut->sendMessage( &message );
}
void sendMIDInoteOff(uint8_t pitch, uint8_t vel, uint8_t channel){
    // 11<<4 to turn it into 10000000, then | with the channel to add the channel
    vector<uint8_t> message = {static_cast<unsigned char>((8<<4)|(channel&15)),pitch,vel};
    virtualMidiOut->sendMessage( &message );
}

//not really applicable in headless mode! the midi ports are *always* listening
void readMIDI(){
    return;
}

void sendClock(){
    //only one byte -- 11111010
    vector<uint8_t> message = {248};
    virtualMidiOut->sendMessage( &message );
}
void sendMIDIStart(){
    //only one byte -- 11111000
    vector<uint8_t> message = {250};
    virtualMidiOut->sendMessage( &message );
}
void sendMIDIStop(){
    //only one byte -- 11111100
    vector<uint8_t> message = {252};
    virtualMidiOut->sendMessage( &message );
}
void sendMIDIProgramChange(uint8_t port, uint8_t val, uint8_t channel){
    vector<uint8_t> message = {static_cast<unsigned char>((12<<4)|(channel&15)),val};
    virtualMidiOut->sendMessage( &message );
}
