#include "Stepchild.h"
#include "guiUtilities.h"

using namespace std;

uint8_t getVelWithSlope(uint16_t start, uint16_t end, uint16_t location, float& velSlope){
    if(velSlope<0){
        //start is given as (start,127), with slop velSlope, find (end,X)
       return velSlope*(location - start)+127;
    }
    else if(velSlope>0){
        //end is given as (end,127), with slop velSlope, find (start,X)
       return (velSlope * (location - end) + 127);
    }
    else return 127;
}

vector<NoteTrackPair> getChoppedNotes(vector<NoteTrackPair> targetNotes, uint8_t pieces, float& velSlope){
    vector<NoteTrackPair> choppedNotes = {};
    for(uint8_t i = 0; i<targetNotes.size(); i++){
        uint16_t length = targetNotes[i].note.endPos - targetNotes[i].note.startPos;
        //the new length of each sub note
        uint16_t newLength = length/pieces;

        //if the length is too short to cut, don't
        if(newLength<2)
            return choppedNotes;

        for(uint16_t N = 0; N<pieces; N++){
            Note newNote = targetNotes[i].note;
            //if you're about to make the last note, set newLength to be equal to the remaining length
            if(N == pieces-1){
                newNote.startPos = targetNotes[i].note.startPos+N*newLength;
                newNote.endPos = newNote.startPos+length-N*newLength;
            }
            else{
                newNote.startPos = targetNotes[i].note.startPos+N*newLength;
                newNote.endPos = newNote.startPos+newLength;
            }
            newNote.velocity = getVelWithSlope(targetNotes[i].note.startPos, targetNotes[i].note.endPos,newNote.startPos,velSlope);
            choppedNotes.push_back(NoteTrackPair(newNote,targetNotes[i].trackID));
        }
    }
    return choppedNotes;
}

//returns a list of chopped notes
vector<Note> getChoppedNotes(Note targetNote, uint8_t pieces, float& velSlope){
    vector<Note> choppedNotes = {};
    uint16_t length = targetNote.endPos - targetNote.startPos;
    //the new length of each sub note
    uint16_t newLength = length/pieces;

    //if the length is too short to cut, don't
    if(newLength<2)
        return choppedNotes;

    for(uint16_t N = 0; N<pieces; N++){
        Note newNote = targetNote;
        //if you're about to make the last note, set newLength to be equal to the remaining length
        if(N == pieces-1){
          newNote.startPos = targetNote.startPos+N*newLength;
          newNote.endPos = newNote.startPos+length-N*newLength;
        }
        else{
          newNote.startPos = targetNote.startPos+N*newLength;
          newNote.endPos = newNote.startPos+newLength;
        }
        newNote.velocity = getVelWithSlope(targetNote.startPos,targetNote.endPos,newNote.startPos,velSlope);
        choppedNotes.push_back(newNote);
    }
    return choppedNotes;
}

void drawChopIcon(uint8_t x1, uint8_t y1, uint8_t height, bool animated){
    uint16_t timing = 1000;
    if(animated && (millis()%timing>(timing/2)))
        stepchild.display.drawBitmap(x1-1,y1-1,chop1_bmp,12,12,1);
    else
        stepchild.display.drawBitmap(x1,y1,chop2_bmp,12,12,1);
}

bool chopNotes(vector<NoteID> noteIDs){
    //store the note and remove it from the sequence
    vector<NoteTrackPair> targetNotes = {};
    for(NoteID n:noteIDs){
        targetNotes.push_back(NoteTrackPair(n.getNote(),n.track));
    }
    stepchild.deleteNotes_byID(noteIDs);

    //set up render settings
    SequenceRenderSettings settings;
    settings.topLabels = false;
    settings.drawPram = false;
    settings.drawLoopFlags = false;
    settings.shrinkTopDisplay = false;

    //number of pieces
    uint8_t numberOfPieces = 1;
    bool maxedOut = false;
    bool changed = false;

    //fading in/out velocity
    float velSlope = 1.0;//0 is constant, -1 is going from 127 --> 0, 1 is going from 0 --> 127 (basically velSlope is the slope)
    //newly created notes (start w/ copies of the currently selected note)
    vector<NoteTrackPair> notes = {};
    for(uint16_t i = 0; i<targetNotes.size(); i++){
        notes.push_back(NoteTrackPair(targetNotes[i].note,targetNotes[i].trackID));
    }
    while(true){
        stepchild.buttons.readJoystick();
        stepchild.buttons.readButtons();
        //change chop
        while(stepchild.buttons.counterA){
            if(stepchild.buttons.counterA < 0 && numberOfPieces > 1){
                numberOfPieces--;
                changed = true;
            }
            else if(stepchild.buttons.counterA > 0 && !maxedOut){
                numberOfPieces++;
                changed = true;
            }
            stepchild.buttons.countDownA();
        }
        //change vel
        while(stepchild.buttons.counterB){
            if(stepchild.buttons.counterB < 0 && velSlope > -1.0){
                velSlope -= 0.1;
                changed = true;
            }
            else if(stepchild.buttons.counterB > 0 && velSlope < 1.0){
                velSlope += 0.1;
                changed = true;
            }
            stepchild.buttons.countDownB();
        }
        if(stepchild.itsbeen(50)){
            //change vel slope
            if(stepchild.buttons.joystickX == 1 && velSlope < 1.0){
                stepchild.lastTime = millis();
                velSlope += 0.1;
                changed = true;
            }
            if(stepchild.buttons.joystickX == -1 && velSlope > -1.0){
                stepchild.lastTime = millis();
                velSlope -= 0.1;
                changed = true;
            }
        }
        if(stepchild.itsbeen(200)){
            //cancel out of it
            if(stepchild.buttons.MENU()){
                stepchild.lastTime = millis();
                for(uint16_t i = 0; i<targetNotes.size(); i++){
                    stepchild.makeNote(targetNotes[i].note,targetNotes[i].trackID,false);
                }
                return false;
            }
            //increase the number of chops
            if(stepchild.buttons.joystickY == -1 && !maxedOut){
                stepchild.lastTime = millis();
                numberOfPieces++;
                changed = true;
            }
            //decrease the number of chops
            else if(stepchild.buttons.joystickY == 1 && numberOfPieces > 1){
                stepchild.lastTime = millis();
                numberOfPieces--;
                changed = true;
            }
            //committing the notes
            if(stepchild.buttons.NEW()){
                stepchild.lastTime = millis();
                for(uint16_t i = 0; i<notes.size(); i++){
                    stepchild.makeNote(notes[i].note,notes[i].trackID);
                }
                return true;
            }
        }
        //update chopped notes if any changes were made
        if(changed){
            notes = getChoppedNotes(targetNotes,numberOfPieces,velSlope);
            if(!notes.size()){
                maxedOut = true;
                numberOfPieces--;
                notes = getChoppedNotes(targetNotes,numberOfPieces,velSlope);
            }
            else{
                maxedOut = false;
            }
            changed = false;
        }

        //update display
        stepchild.display.clearDisplay();
        graphics.drawSeq(settings);
        uint16_t timing = 2000/max(numberOfPieces,1);
        if(millis()%timing>(timing/2))
            stepchild.display.drawBitmap(4,4,chop2_bmp,12,12,1);
        else
            stepchild.display.drawBitmap(3,3,chop1_bmp,12,12,1);
        graphics.printSmall(20,1,"chopping into ",1);
        graphics.printCursive(71,0,stringify(numberOfPieces),1);
        graphics.printSmall(72+stringify(numberOfPieces).length()*6,1,"pcs",1);
        graphics.drawButton(85+stringify(numberOfPieces).length()*6,0,"Y/A",1);
        graphics.printSmall(20,8,"roll:",1);
        //drawing vel slope display
        if(velSlope<0){
            stepchild.display.drawLine(40,8,60,8 - velSlope*6,1);
        }
        else if(velSlope>0){
            stepchild.display.drawLine(40,8+velSlope*6,60,8,1);
        }
        else{
            stepchild.display.drawFastHLine(30,10,20,1);
        }
        graphics.drawButton(64,8,"X/B",1);
        //tooltip
        graphics.drawButton(111,0,"n",1);
      	graphics.drawCheckmark(118,1,7,1);
        graphics.drawButton(111,8,"m",1);
        graphics.printSmall(121,9,"x",1);
        //drawing notes
        for(uint16_t i = 0; i<notes.size(); i++){
            graphics.drawNote(notes[i].note,notes[i].trackID,settings);
        }
        stepchild.display.display();
    }
    return false;
}

bool getNotesToChop(){
    if(selectNotes("chop",drawChopIcon)){
        return chopNotes(getSelectedNoteIDs());
    }
    else
        return false;
}

bool chopSelectedNotes(){
    vector <NoteID> selected = getSelectedNoteIDs();
    return chopNotes(selected);
}

void chopNoteAt(uint8_t track, uint16_t step){
    if(!stepchild.IDAt(track,step))
        return;
    chopNotes(vector<NoteID> {NoteID(track,stepchild.IDAt(track,step))});
}

void chop(){
  if(stepchild.selectionCount){
    chopSelectedNotes();
  }
  else if(stepchild.IDAtCursor() != 0){
    chopNotes(vector<NoteID> {NoteID(stepchild.activeTrack,stepchild.IDAtCursor())});
  }
}
