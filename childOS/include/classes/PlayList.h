#pragma once
#include <vector>
#include "Arduino.h"
#include "commonStructs.h"

class PlayList{
  public:
    std::vector<NoteData> notes;
    NoteData mostRecentNote;

    //constructor
    PlayList(){}

    //returns true if "notes" contains a pitch that's equal to p
    bool containsPitch(uint8_t p);
    //returns a list of pitches that are present in the playlist
    std::vector<uint8_t> getUniquePitches();
    //adds a note to the playlist
    void addNote(uint8_t p, uint8_t v, uint8_t c);
    //removes a note from the playlist
    void subNote(uint8_t note);
    //empties the playlist
    void clear();
};
