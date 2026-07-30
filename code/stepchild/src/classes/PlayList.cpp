#include <vector>
#include "Arduino.h"
#include "commonStructs.h"
#include "classes/PlayList.h"

//returns true if "notes" contains a pitch that's equal to p
bool PlayList::containsPitch(uint8_t p){
  for(uint8_t i = 0; i<notes.size(); i++){
    if(notes.at(i).pitch == p){
      return true;
    }
  }
  return false;
}

//returns a list of pitches that are present in the playlist
std::vector<uint8_t> PlayList::getUniquePitches(){
  //move thru each note and get its 'true' pitch.
  //then, check to see if that pitch is in the new list.
  //if it is, continue with the next note. if not, add 
  //this note and then continue;
  std::vector<uint8_t> uniquePitches;
  for(uint8_t note = 0; note<notes.size(); note++){
    //getting the pitch relative to C
    uint8_t uniquePitch = notes[note].pitch%12;
    //if it's the first note, add it automatically
    if(uniquePitches.size() == 0){
      uniquePitches.push_back(uniquePitch);
    }
    //if it's not, check to see if it's unique
    else{
      bool unique = true;
      for(uint8_t uniqueP = 0; uniqueP < uniquePitches.size(); uniqueP++){
        if(uniqueP == uniquePitch){
          unique = false;
          break;
        }
      }
      if(unique){
        uniquePitches.push_back(uniquePitch);
      }
    }
  }
  return uniquePitches;
}
//adds a note to the playlist
void PlayList::addNote(uint8_t p, uint8_t v, uint8_t c){
  NoteData newNote = NoteData(p,v,c);
  notes.push_back(newNote);
}
//removes a note from the playlist
void PlayList::subNote(uint8_t note){
  std::vector<NoteData> tempList;
  //keep all the notes, EXCEPT for the one you're kicking out
  for(int i = 0; i<notes.size(); i++){
    if(note != notes[i].pitch){
      tempList.push_back(notes[i]);
    }
  }
  notes.swap(tempList);
}

//empties the playlist
void PlayList::clear(){
  std::vector<NoteData> tempList;
  notes.swap(tempList);
}
