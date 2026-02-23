#include "Stepchild.h"
#include "guiUtilities.h"
#include "StepchildGraphics.h"
extern StepchildGraphics graphics;
extern Stepchild stepchild;
using namespace std;

//lines floating to the left
void drawRevIcon(uint8_t x1, uint8_t y1, uint8_t height, bool animated){
    for(uint8_t i = 0; i<height; i++){
        int8_t start = uint8_t(height/2*sin(float(millis())/500.0+float(i*2))+height/2)%(height);
        uint8_t length = (height-start)>4?4:(height-start);
        if(start>0 && start<height-1)
            stepchild.display.drawFastHLine(x1+start,y1+i,length,1);
    }
}

//Reverses all selected notes within a start/stop bound
//Notes must be totally within bound to be affected
bool reverse(){
    Loop tempLoop = stepchild.loopData[stepchild.activeLoop];
    bool atLeastOnce = false;
    while(true){
        stepchild.clearSelection();
        CoordinatePair bounds = selectNotesAndArea("Sel notes + bounds to rev",drawRevIcon);
        //if no notes were selected (because the user backed out, or goofed)
        if(stepchild.selectionCount == 0){
            break;
        }
        //if no area was selected (because the user goofed)
        if(bounds.end.x-bounds.start.x<1){
            break;
        }

        //center of the "rotation"
        uint16_t center = (bounds.end.x-bounds.start.x)/2+bounds.start.x;

        //for each note, the distance its closest point is from center should become the -distance its farthest is from the center
        //EX: if a note starts 10 steps AFTER the centerpoint, then that note should now END 10 steps BEFORE the centerpoint
        
        //get only the notes that are selected, and that lie within the bounds (inclusive)
        vector<NoteTrackPair> targetNotes = {};
        vector<NoteID> targetIDs = {};
        vector<NoteID> potentialIDs = getSelectedNoteIDs();
        for(NoteID n:potentialIDs){
            if(n.getNote().isSelected() && (n.getNote().startPos>=bounds.start.x && n.getNote().endPos<=bounds.end.x)){
                targetNotes.push_back(NoteTrackPair(n.getNote(),n.track));
                targetIDs.push_back(n);
            }
        }
        //delete those notes from the sequence
        stepchild.deleteNotes_byID(targetIDs);

        //make the reversed notes
        vector<NoteTrackPair> reversedNotes = {};
        for(NoteTrackPair n:targetNotes){
            //get its distances from the center point
            int32_t A = n.note.endPos - center;
            int32_t B = n.note.startPos - center;

            NoteTrackPair newNote = n;

            //flip the endpoints
            newNote.note.startPos = center-A;
            newNote.note.endPos = center-B;

            //deselect the note (because it always comes selected)
            newNote.note.setSelected(false);

            reversedNotes.push_back(newNote);
        }

        //set up render settings
        SequenceRenderSettings settings;
        settings.topLabels = false;
        settings.drawPram = false;
        settings.drawLoopFlags = false;
        settings.shrinkTopDisplay = false;

        while(true){
            stepchild.buttons.readInputs();
            if(stepchild.itsbeen(200)){
                //cancelling
                if(stepchild.buttons.MENU()){
                  stepchild.lastTime = millis();
                  for(NoteTrackPair n:targetNotes){
                    stepchild.makeNote(n.note,n.trackID,false);
                  }
                  break;
                }
                //committing
                if(stepchild.buttons.NEW()){
                  stepchild.lastTime = millis();
                  for(NoteTrackPair n:reversedNotes){
                    stepchild.makeNote(n.note,n.trackID,false);
                  }
                  atLeastOnce = true;
                  break;
                }
            }
            stepchild.display.clearDisplay();
            graphics.drawSeq(settings);
            drawRevIcon(7,1,14,true);
            graphics.drawButton(32,8,"M",1);
            graphics.printSmall(41,9,"to quit",1);
            graphics.drawButton(32,0,"n",1);
            graphics.printSmall(41,1,"to apply",1);
            if(millis()/300%2){
                for(NoteTrackPair n:reversedNotes){
                    graphics.drawNote(n.note,n.trackID,settings);
                }
            }
            stepchild.display.display();
        }
    }
    //reset the loop points to the loop we stored in the beginning
    stepchild.loopData[stepchild.activeLoop] = tempLoop;
    return atLeastOnce;
}
