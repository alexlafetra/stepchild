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

#pragma once

enum PlayState{
  STOPPED,
  PLAYING,
  RECORDING
};

#include <CoreAudio/CoreAudio.h>
#include <CoreMIDI/CoreMIDI.h>
#include <CoreFoundation/CoreFoundation.h>

//#define __MACOSX_CORE__
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

enum MIDI_MESSAGE_TYPE:uint8_t{
  NONE = 0,
  NOTE_OFF = 8,
  NOTE_ON = 9,
  CC_MESSAGE = 11,
  REAL_TIME_MESSAGE = 15
};

void MidiInputCallback( double deltatime, std::vector< unsigned char > *message, void *userData );

class StepchildMIDI{
public:
  bool dummyThruSettings[5] = {true,true,true,true,true};
  bool queueIsFull = false;
  uint8_t muteSettings = 0;
  uint16_t midiChannelFilters[5] = {65535,65535,65535,65535,65535};
  StepchildMIDI();
  void init();
  void processCore1Messages();
  void sendThruOn(uint8_t t, uint8_t a, uint8_t b);
  void sendThruOff(uint8_t t, uint8_t a);
  void sendThruCC(uint8_t t, uint8_t a, uint8_t b);
  void sendThruPB(uint8_t t, int a);
  void sendClock(uint8_t t);
  void sendCC(uint8_t cc, uint8_t v, uint8_t c);
  void allOff();
  void noteOn(uint8_t pitch, uint8_t vel, uint8_t channel);
  void noteOff(uint8_t pitch, uint8_t vel, uint8_t channel);
  //not really applicable in headless mode! the midi ports are *always* listening
  void read();
  
  void sendClock();
  void sendStart();
  void sendStop();
  void sendPC(uint8_t port, uint8_t val, uint8_t channel);
  //function for selecting the correct MIDI API
  RtMidi::Api chooseMidiApi();
  void start();
  bool isThru(uint8_t output);
  void setThru(uint8_t which, bool value);
  //checks if a port is filtering a channel
  bool isChannelActive(uint8_t whichChannel,uint8_t whichPort);
  void allChannelsOff(uint8_t whichPort);
  void allChannelsOn(uint8_t whichPort);
  void setAllChannels(bool state, uint8_t whichPort);
  bool toggleThru(uint8_t output);
  bool isMuted(uint8_t which);
  void toggleMute(uint8_t whichPort);
  bool isTotallyMuted(uint8_t which);
  void setMidiChannel(uint8_t channel, uint8_t output, bool status);
  //toggles the channel on an output, and returns its new value
  //toggles the channel on an output, and returns its new value
  bool toggleMidiChannel(uint8_t channel, uint8_t output);
  void muteMidiPort(uint8_t which);
  void unmuteMidiPort(uint8_t which);
  void toggleMidiPort(uint8_t which);
};
