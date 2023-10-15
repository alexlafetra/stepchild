//Live Loop Mode!
/*

    This little app lets you do performance-focused looping, with reduced editing controls.
    While the seq is playing, the encoders are also used to send CC data.

*/
void liveLoop(){
    Loop originalLoop = loopData[activeLoop];
    uint8_t originalRecMode = recMode;
    menuIsActive = false;
    recMode = 2;
    Knob knobA;
    Knob knobB;
    recordedNotesAreSelected = true;
    clearSelection();
    while(true){
        readButtons();
        readJoystick();
        mainSequencerEncoders();
        defaultJoystickControls(false);
        defaultSelectBoxControls();
        if(itsbeen(75)){
            //delete
            if(del && !shift){
                if (selectionCount > 0){
                    deleteSelected();
                    lastTime = millis();
                }
                else if(getIDAtCursor() != 0){
                    deleteNote(activeTrack,cursorPos);
                    lastTime = millis();
                }
            }
        }
        if(itsbeen(200)){
            defaultSelectControls();
            defaultCopyControls();
            defaultLoopControls();
            if(menu_Press){
                lastTime = millis();
                break;
            }
            //get ready to record, but wait for note
            if(play){
                //if shift is held, and you're not already recording
                if(!recording && shift){
                    lastTime = millis();
                    uint16_t temp = recheadPos;
                    //if you were already playing, just begin recording! don't wait for a note
                    //but if you haven't been playing/recording anything, wait for a note
                    toggleRecordingMode(waitForNote);
                }
                //if you are already recording
                //stop the recording and shrink it to the active loop, if you're coming out of a recording
                else if(recording && !playing){
                    CoordinatePair A = CoordinatePair(loopData[activeLoop].start,recheadPos);
                    CoordinatePair B = CoordinatePair(loopData[activeLoop].start,loopData[activeLoop].end);
                    //make sure to call this first, so that the recording is cleaned up
                    togglePlayMode();
                    //only warp selected notes
                    warpAintoB(A,B,true);
                    isLooping = true;
                    lastTime = millis();
                    //adjust the bpm so that it feels like it's playing at the same speed
                    float timeScale = float(B.x2-B.x1)/float(A.x2-A.x1);
                    uint16_t newBPM = bpm*timeScale;
                    setBpm(newBPM);
                    //disarm all the tracks that were just written to
                    disarmTracksWithNotes();
                    //clear selection, so you don't re-warp the previously recorded notes
                    clearSelection();
                }
                //toggle play/pause if it's just playing
                else if(!shift){
                    togglePlayMode();
                    lastTime = millis();
                }
            }
        }
        display.clearDisplay();
        drawSeq(true,false,true,false,false,false,viewStart,viewEnd);
        if(playing){

        }
        else if(recording){
            if(millis()%1000>500)
                display.fillCircle(15,7,7,1);
            else
                display.drawCircle(15,7,7,1);
        }
        printCursive(trackDisplay,0,"live looper",1);
        display.display();
    }
    menuIsActive = true;
    recMode = originalRecMode;
    loopData[activeLoop] = originalLoop;
}