#include "Arduino.h"
#include <vector>
#include <algorithm>
#include "StepchildArpeggiator.h"
#include "Stepchild.h"
#include "guiUtilities.h"

using namespace std;

;

ArpSource operator++(ArpSource &c,int) {
  c = static_cast<ArpSource>(static_cast<uint8_t>(c) + 1);
  return c;
}
ArpSource operator--(ArpSource &c,int) {
  c = static_cast<ArpSource>(static_cast<uint8_t>(c) - 1);
  return c;
}
ArpSource& operator%=(ArpSource& lhs, int rhs) {
  // Convert enum to its underlying integer type for the operation
  using underlying = std::underlying_type_t<ArpSource>;
  underlying& lhs_value = reinterpret_cast<underlying&>(lhs);
  // Perform the modulus operation and update lhs
  lhs_value %= rhs;
  // Return the updated lhs
  return lhs;
}

//arps should have velocity settings per step (parallel to lengths) and also variation factors for
//velocity, length, and chance
//variation factors could have a cool upper bound/lower bound setting screen with
//three vertical "braces" that sit side by side
//length variation should always be a multiple of a subdiv, like "how many 1/4 notes should
//this play for" and not just a scalar

StepchildArpeggiatior::StepchildArpeggiatior(Stepchild* ptr){
    stepchild = ptr;
    range = 0;
    notes = {};
    lengths = { 12, 12, 12, 12, 12, 12, 12, 12 };  //just for testing purposes
    order = {};                                    //default is 'up/down'
    playheadPos = 0;
    activeNote = 0;
    playStyle = 0;
    playing = false;
    uniformLength = true;
    holding = false;
    source = NOTES_FROM_MIDI_INPUT;
    channel = 1;

    maxVelMod = 0;
    minVelMod = 0;
    chanceMod = 100;
    repMod = 0;
    maxPitchMod = 0;
    minPitchMod = 0;
}

bool StepchildArpeggiatior::hasItBeenEnoughTime() {
  if (this->stepchild->clock.isSwinging) {
    if ((micros() - StepchildArpeggiatior::timeLastStepPlayed) >= (this->stepchild->clock.uSecPerStep + this->stepchild->clock.swingOffset(stepCount))) {
      if (!(stepCount % (uint16_t)this->stepchild->clock.swingCurve.period)) {
        StepchildArpeggiatior::offBy = (micros() - StepchildArpeggiatior::startTime) % this->stepchild->clock.uSecPerStep;
      }
      if (StepchildArpeggiatior::offBy == 0)
        StepchildArpeggiatior::startTime = micros();
      timeLastStepPlayed = micros();
      return true;
    } else
      return false;
  }
  else {
    if (micros() - StepchildArpeggiatior::timeLastStepPlayed + StepchildArpeggiatior::offBy >= this->stepchild->clock.uSecPerStep) {
      StepchildArpeggiatior::offBy = (micros() - StepchildArpeggiatior::startTime) % this->stepchild->clock.uSecPerStep;
      timeLastStepPlayed = micros();
      if (StepchildArpeggiatior::offBy == 0)
        StepchildArpeggiatior::startTime = micros();
      return true;
    } else
      return false;
  }
}

//returns which note is playing based on the active note
uint8_t StepchildArpeggiatior::getOrder(){
  if(!order.size())
    return 0;
  else
    return order[activeNote%order.size()-1];
}

void StepchildArpeggiatior::start() {
  grabNotesFromPlaylist();
  StepchildArpeggiatior::startTime = micros();
    if(order.size()>activeNote){
        if(StepchildArpeggiatior::notes.size() > getOrder()){
            stepchild->midi.noteOn(notes[getOrder()], 100, StepchildArpeggiatior::channel);
            lastPitchSent = notes[getOrder()];
            StepchildArpeggiatior::timeLastStepPlayed = micros();
        }
    }
  StepchildArpeggiatior::playing = true;
}

void StepchildArpeggiatior::stop() {
  for (uint8_t i = 0; i < notes.size(); i++)
    stepchild->midi.noteOff(notes[i], 0, channel);
  stepchild->midi.noteOff(lastPitchSent, 0, channel);
  //brute force way of doing it
  //the downside is that this will cut off ALL notes that are playing
  StepchildArpeggiatior::playheadPos = 0;
  StepchildArpeggiatior::stepCount = 0;
  StepchildArpeggiatior::activeNote = 0;
  StepchildArpeggiatior::playing = false;
}

void StepchildArpeggiatior::selectExtended() {
  //draw an arial keyboard and let the user select keys to build the scale
  vector<uint8_t> selection = selectKeys(0);
  extendedNotes = selection;
}

void StepchildArpeggiatior::grabNotesFromPlaylist() {
  //clearing out notes
  vector<uint8_t> temp;
  notes.swap(temp);
  //adds in notes from the playlist, one copy for each octave, in the order played
  for (int oct = 0; oct < range + 1; oct++) {
    switch(source){
      //if it's just external notes
      case NOTES_FROM_MIDI_INPUT:
        for (int i = 0; i < stepchild->receivedNotes.notes.size(); i++) {
          notes.push_back(stepchild->receivedNotes.notes[i].pitch + 12 * oct);
        }
      break;
      //if it's just internal notes
      case NOTES_FROM_SEQUENCE:
        for (int i = 0; i < stepchild->sentNotes.notes.size(); i++) {
          notes.push_back(stepchild->sentNotes.notes[i].pitch + 12 * oct);
        }
        break;
      //grabbing from both
      case NOTES_FROM_SEQUENCE_AND_MIDI_INPUT:
      //you don't need to do this every time! wasteful
        notes = stepchild->getAllActivePitches();
        for(uint8_t i = 0; i<notes.size(); i++){
          notes[i] = notes[i]+12*oct;
        }
        break;
    }
  }
  setOrder();
}

//playheadPos wraps around after each note, it just counts up until the next note needs to be played
void StepchildArpeggiatior::playstep() {
  //if there are any notes to be played
  if (notes.size() > 0) {
    if (!holding) {
      grabNotesFromPlaylist();
    }
    //if the playhead is past the current active note
    if ((uniformLength && playheadPos > arpSubDiv) || (!uniformLength && playheadPos > lengths[getOrder()])) {
      stepchild->midi.noteOff(lastPitchSent, 0, channel);
      uint8_t willItRep = random(0, 100);
      //if it's not repeating, then play it again
      if (willItRep >= repMod || repMod == 0) {
        activeNote++;
      }
      playheadPos = 0;
      if (activeNote >= order.size()) {
        activeNote = 0;
        stepCount = 0;
      }
      uint8_t willItPlay = random(0, 100);
      //if it plays
      if (willItPlay < chanceMod) {
        //add one to the random() bounds if maxPitchMod == 0, so that it doesn't always generate lower pitched notes by excluding 0 as an optino
        int8_t randOctave = random(-minPitchMod / 16, maxPitchMod / 16 + ((maxPitchMod == 0) ? 1 : 0));
        //getting random modifiers
        int16_t randVel = random(64 - minVelMod / 2, 64 + maxVelMod / 2);
        if (randVel <= 0)
          randVel = 0;
        else if (randVel > 127)
          randVel = 127;

        //if there's no velocity mod   (when min == max) just set vel to 100
        uint8_t vel = (maxVelMod == minVelMod) ? 100 : randVel;
        int16_t pitch = notes[getOrder()];
        //same as vel, if min and max are diffferent then use the modifier
        if (maxPitchMod != minPitchMod)
          pitch += (12 * randOctave);
        stepchild->midi.noteOn(pitch, vel, channel);
        lastPitchSent = pitch;
      }
    } else {
      playheadPos++;
      stepCount++;
    }
  }
  //if not, try to grab some notes
  else
    grabNotesFromPlaylist();
}

//returns false if there are already 32 lengths in the buffer
bool StepchildArpeggiatior::addStepLength(uint16_t length, uint8_t where) {
  if (lengths.size() < 32) {
    lengths.insert(lengths.begin() + where, length);
    return true;
  }
  return false;
}
bool StepchildArpeggiatior::addStepLength(uint16_t length) {
  return addStepLength(length, lengths.size());
}
//adds a copy of the last step length
bool StepchildArpeggiatior::addStepLength() {
  return addStepLength(lengths[lengths.size() - 1]);
}

bool compareArpNotes(uint8_t id1, uint8_t id2) {
  return stepchild.arpeggiator.notes[id1] > stepchild.arpeggiator.notes[id2];
}

bool randomSort(uint8_t id1, uint8_t id2) {
  return random(0, 2);
}

//sorts thru notes to create an order
void StepchildArpeggiatior::setOrder() {
  vector<uint8_t> temp;
  // order.swap(temp);
  order.erase(order.begin(), order.end());
  //order played is just the order of the playlist
  //there might be a more efficient way of doing this (like just assigning a range)
  for (int note = 0; note < notes.size(); note++) {
    order.push_back(note);
  }
  switch (playStyle) {
    //play order
    case 0:
      break;
    //descending by pitch
    case 1:
      sort(order.begin(), order.end(), compareArpNotes);
      break;
    //ascending
    case 2:
      sort(order.begin(), order.end(), compareArpNotes);
      reverse(order.begin(), order.end());
      break;
    //up down
    case 3:
      {
        //copy order into up
        vector<uint8_t> up = order;
        //clear order
        order.erase(order.begin(), order.end());
        //sort up by descending
        sort(up.begin(), up.end(), compareArpNotes);
        //copy to down, then reverse down so it's sorted by ascending
        vector<uint8_t> down = up;
        reverse(down.begin(), down.end());
        //push a value from up, then down, into order
        for (uint8_t i = 1; i < up.size(); i += 2) {
          order.push_back(up[i-1]);
          order.push_back(down[i]);
        }
      }
      break;
    //down up
    case 4:
      {
        //copy order into up
        vector<uint8_t> up = order;
        //clear order
        order.erase(order.begin(), order.end());
        //sort up by descending
        sort(up.begin(), up.end(), compareArpNotes);
        //copy to down, then reverse down so it's sorted by ascending
        vector<uint8_t> down = up;
        reverse(down.begin(), down.end());
        //push a value from down, then up, into order
        for (uint8_t i = 0; i < up.size() - 1; i += 2) {
          order.push_back(down[i]);
          order.push_back(up[i + 1]);
        }
      }
      break;
    //random
    case 5:
      sort(order.begin(), order.end(), randomSort);
      break;
  }
}
