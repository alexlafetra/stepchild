void LiveLooper::start(bool fromTrigger){
    sequence.temporarilyUnprimeTracksWithNotes();
    PlayState previousPlayState = sequence.playState;
    uint16_t playheadPos = sequence.playheadPos;
    
    //don't wait for a note if you're starting from a trigger, but DO wait if it's a normal trigger
    sequence.setRecMode(!fromTrigger);

    //if it was already playing, set the rechead position to the playhead position
    if(previousPlayState == PLAYING)
        sequence.recheadPos = playheadPos;

    //turn off looping if it's that kind of live loop
    if(setLoopBoundsAfterRec){
        sequence.loopData[sequence.activeLoop].start = sequence.recheadPos;
        sequence.isLooping = false;
    }
}
void LiveLooper::stop(){
    if(setLoopBoundsAfterRec){
        //turn on loop
        sequence.isLooping = true;
        //set loop end to this step
        sequence.loopData[sequence.activeLoop].end = sequence.recheadPos;
    }
    sequence.reprimeTracks();
    sequence.setPlayMode();
}

bool LiveLooper::checkNote(uint8_t channel, uint8_t note, uint8_t vel){
    if(active){
        //if the note matches, toggle rec/play
        if(channel == triggerNote.channel && note == triggerNote.pitch){
            switch(sequence.playState){
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
