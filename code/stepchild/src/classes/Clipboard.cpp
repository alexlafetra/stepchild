#include "classes/Clipboard.h"
#include "Stepchild.h"

using namespace std;

void Clipboard::copy(){
    //if no notes are selected, return
    if(!stepchild->selectionCount)
        return;

    //clear out old copy buffer
    vector<vector<Note>> temp1;
    temp1.resize(stepchild->trackData.size());
    this->buffer.swap(temp1);
    //get the new relative cursor pos
    this->relativeCursorPosition = Coordinate(stepchild->cursorPos,stepchild->activeTrack);

    uint16_t numberOfNotes = 0;

    //add all selected notes to the copy buffer
    if(stepchild->selectionCount>0){
        for(int track = 0; track<stepchild->trackData.size(); track++){
            for(int note = 1; note<=stepchild->noteData[track].size()-1; note++){// <= bc notes aren't 0 indexed
                if(stepchild->noteData[track][note].isSelected()){
                this->buffer[track].push_back(stepchild->noteData[track][note]);
                numberOfNotes++;
                }
            }
        }
        stepchild->clearSelection();
    }
    //or if there's a target note, but it's not selected
    else if(stepchild->IDAtCursor() != 0){
        this->buffer[stepchild->activeTrack].push_back(stepchild->noteData[stepchild->activeTrack][stepchild->IDAtCursor()]);
        numberOfNotes = 1;
    }
    stepchild->tooltipText = "copied "+stringify(numberOfNotes)+((stringify(numberOfNotes)=="1")?" note":" notes");
}
void Clipboard::copyLoop(uint8_t loopID){
    //clear copyBuffer
    while(this->buffer.size()>0){
        this->buffer.pop_back();
    }
    //making sure buffer has enough 'columns' to store the notes from each track
    this->buffer.resize(stepchild->trackData.size());
    //treat copying the loop like you're copying it from the start of the loop
    this->relativeCursorPosition = Coordinate(stepchild->loopData[loopID].start,0);
    //add all selected notes to the copy buffer
    if(stepchild->loopData[loopID].end-stepchild->loopData[loopID].start>0){
        for(uint8_t track = 0; track<stepchild->trackData.size(); track++){
            for(uint16_t step = stepchild->loopData[loopID].start; step<stepchild->loopData[loopID].end; step++){// <= bc notes aren't 0 indexed
                if(stepchild->lookupTable[track][step] != 0){
                this->buffer[track].push_back(stepchild->noteData[track][stepchild->lookupTable[track][step]]);
                //move to the end of the note, so it's not double-counted
                step = stepchild->noteData[track][stepchild->lookupTable[track][step]].endPos;
                }
            }
        }
    }
}
void Clipboard::copyLoop(){
    this->copyLoop(stepchild->activeLoop);
}
void Clipboard::pasteAt(uint8_t track, uint16_t step){
    if(this->buffer.size()>0){
        uint16_t pastedNotes = 0;
        //offset of all the notes (relative to where they were copied from)
        int16_t yOffset = track - this->relativeCursorPosition.y;
        int16_t xOffset = step - this->relativeCursorPosition.x;
        //moves through each track and note in copyBuffer, places a note at those positions in the seq
        for(int tracks = 0; tracks<this->buffer.size(); tracks++){//for each track in the copybuffer
        if(this->buffer[tracks].size()>0 && tracks+yOffset>=0 && tracks+yOffset<stepchild->trackData.size()){//if there's a note stored for this track, and it'd be copied according to the new stepchild->activeTrack
            for(int notes = 0; notes<this->buffer[tracks].size(); notes++){
            if(this->buffer[tracks][notes].startPos + xOffset<= stepchild->sequenceLength){
                int track = tracks+yOffset;
                Note newNote = this->buffer[tracks][notes];
                newNote.startPos += xOffset;
                newNote.endPos += xOffset;

                //if note ends past seq, truncate it
                if(newNote.endPos>stepchild->sequenceLength)
                newNote.endPos = stepchild->sequenceLength;
                //if note begins before seq, but also extends into seq, truncate it
                if(newNote.startPos<0 && newNote.endPos>0)
                newNote.startPos = 0;
                else if(newNote.startPos<0 && newNote.endPos<=0)
                continue;
                if(track<0||track>=stepchild->trackData.size())
                continue;
                stepchild->makeNote(newNote,track,false);
                pastedNotes++;
            }
            }
        }
        }
        stepchild->tooltipText = "pasted "+stringify(pastedNotes)+((stringify(pastedNotes)=="1")?" note":" notes");
    }
}
void Clipboard::paste(){
    this->pasteAt(stepchild->activeTrack,stepchild->cursorPos);
}

