#include "Stepchild.h"
extern Stepchild stepchild;
using namespace std;

void LiveLooper::start(bool fromTrigger){
    stepchild.temporarilyUnprimeTracksWithNotes();
    PlayState previousPlayState = stepchild.playState;
    uint16_t playheadPos = stepchild.playheadPos;
    
    //don't wait for a note if you're starting from a trigger, but DO wait if it's a normal trigger
    stepchild.setRecMode(!fromTrigger);

    //if it was already playing, set the rechead position to the playhead position
    if(previousPlayState == PLAYING)
        stepchild.recheadPos = playheadPos;
    //turn off looping if it's that kind of live loop
    if(setLoopBoundsAfterRec_flag){
        stepchild.recheadPos = 0;
        stepchild.loopData[stepchild.activeLoop].start = 0;
        stepchild.isLooping = false;
    }
}
void LiveLooper::stop(){
    if(setLoopBoundsAfterRec_flag){
        setLoopBoundsAfterRec_flag = false;
        //turn on loop
        stepchild.isLooping = true;
        //set loop end to this step
        stepchild.loopData[stepchild.activeLoop].end = stepchild.recheadPos;
    }
    stepchild.reprimeTracks();
    stepchild.setPlayMode();
}

bool LiveLooper::checkNote(uint8_t channel, uint8_t note, uint8_t vel){
    if(active){
        //if the note matches, toggle rec/play
        if(channel == triggerNote.channel && note == triggerNote.pitch){
            switch(stepchild.playState){
                //if it's playing, start recording
                case PLAYING:
                    start(true);
                    break;
                //if it's recording, start playing
                case RECORDING:
                    stop();
                    break;
                //if it's stopped, start recording
                case STOPPED:
                    start(true);
                    break;
            }
            return true;
        }
    }
    return false;
}
