#include "Stepchild.h"


;
using namespace std;
/*
    Code for the strum FX and strum menu
*/

// //macros defining different sort orders
// #define BYTRACK_DESC 0
// #define BYTRACK_ASC 1
// #define BYPITCH_DESC 2
// #define BYPITCH_ASC 3
// #define RANDOM_ORDER 4

void drawStrumIcon(uint8_t x1, uint8_t y1, uint8_t height, bool animated){
    const uint8_t numberOfStrings = 4; 
    const uint8_t spacing = 3;
    for(uint8_t i = 0; i<numberOfStrings; i++){
        int16_t y2 = y1+spacing*i+spacing/2*sin(millis()/100+i*4);
        stepchild.display.drawLine(x1,y1+(i*spacing),x1+height/2,y2,1);
        stepchild.display.drawLine(x1+height/2,y2,x1+height,y1+(i*spacing),1);
    }
}

struct StrumParameters{
    //ascending or descending
    uint8_t type = 1;
    //sort by pitch or trackID
    uint8_t sortValue = 0;
    int8_t randomness = 0;
    //default is 1/16th note
    uint16_t amount = 6;
};

vector<NoteTrackPair> getStrummedNotes(vector<NoteTrackPair> targetNotes,StrumParameters params){
    vector<NoteTrackPair> newNotes = {};
    uint16_t earliestLoc = stepchild.sequenceLength;
    NoteTrackPair earliestNote = targetNotes[0];
    for(NoteTrackPair n:targetNotes){
        if(n.note.startPos < earliestLoc){
            earliestLoc = n.note.startPos;
            earliestNote = n;
            //if the earliest pos is 0, then don't keep looking
            if(!earliestLoc){
                break;
            }
        }
    }
    targetNotes = sortNotes(targetNotes,params.sortValue,params.type);
    for(NoteTrackPair n:targetNotes){
        NoteTrackPair newNote = n;
        int16_t offset = params.amount;
        uint16_t length = newNote.note.getLength();
        newNote.note.startPos = earliestLoc;
        newNote.note.endPos = newNote.note.startPos + length;
        earliestLoc = earliestLoc + params.amount + random(-params.randomness,params.randomness);
        newNotes.push_back(newNote);
    }
    return newNotes;
}

bool strum(){
    uint16_t seed = millis();
    StrumParameters params;

    //set up render settings
    SequenceRenderSettings settings;
    settings.topLabels = false;
    settings.drawPram = false;
    settings.drawLoopFlags = false;
    settings.shrinkTopDisplay = false;

    //list of notes to apply fx to
    vector<NoteID> targetNoteIDs = {};
    vector<NoteTrackPair> targetNotes = {};
    vector<NoteTrackPair> previewNotes = {};

    if(stepchild.selectionCount)
        targetNoteIDs = getSelectedNoteIDs();
    else
        return false;

    //grab note objects
    for(NoteID n:targetNoteIDs){
        targetNotes.push_back(NoteTrackPair(n.getNote(),n.track));
    }

    //delete the targeted notes
    stepchild.deleteNotes_byID(targetNoteIDs);

    bool changed = true;

    while(true){
        stepchild.buttons.readInputs();

        //change subdivision
        while(stepchild.buttons.counterB != 0){
            //if shifting, toggle between 1/3 and 1/4 mode
            if(stepchild.buttons.SHIFT()){
            stepchild.toggleTriplets();
            changed = true;
            }
            else if(stepchild.buttons.counterB >= 1){
            stepchild.changeSubDivInt(true);
            changed = true;
            }
            //changing subdivint
            else if(stepchild.buttons.counterB <= -1){
            stepchild.changeSubDivInt(false);
            changed = true;
            }
            stepchild.buttons.countDownB();
        }

        if(stepchild.itsbeen(200)){
            if(stepchild.buttons.UP()){
                stepchild.lastTime = millis();
                changed = true;
                //ascending
                params.type = 0;
            }
            else if(stepchild.buttons.DOWN()){
                stepchild.lastTime = millis();
                changed = true;
                //descending
                params.type = 1;
            }
            //change sort basis
            if(stepchild.buttons.LEFT()){
                stepchild.lastTime = millis();
                changed = true;
                //pitch
                params.sortValue = 0;
            }
            else if(stepchild.buttons.RIGHT()){
                stepchild.lastTime = millis();
                changed = true;
                //trackID
                params.sortValue = 1;
            }
            //cancelling
            if(stepchild.buttons.MENU()){
                stepchild.lastTime = millis();
                for(NoteTrackPair n:targetNotes){
                  stepchild.makeNote(n.note,n.trackID,false);
                }
                return false;
            }
            //committing
            if(stepchild.buttons.NEW()){
                stepchild.lastTime = millis();
                for(NoteTrackPair n:previewNotes){
                    stepchild.makeNote(n.note,n.trackID,false);
                }
                return true;
            }
        }
        if(changed){
            randomSeed(seed);
            params.amount = stepchild.subDivision;
            previewNotes = getStrummedNotes(targetNotes,params);
        }

        stepchild.display.clearDisplay();
        graphics.drawSeq(settings);
        drawStrumIcon(7,1,14,true);
        for(NoteTrackPair n:previewNotes){
            graphics.drawNote(n.note,n.trackID,settings);
        }
        graphics.drawButton(84,0,"B",1);
        graphics.printSmall(93,1,"change $",1);
        graphics.drawButton(32,0,"X",1);
        graphics.drawButton(32,8,"Y",1);
        if(params.sortValue)
            graphics.printSmall(41,1,"by: $ pitch",1);
        else
            graphics.printSmall(41,1,"by: track #",1);
        if(params.type)
            graphics.printSmall(41,9,"in ascending order",1);
        else
            graphics.printSmall(41,9,"in descending order",1);

        stepchild.display.display();
    }
    return false;
}