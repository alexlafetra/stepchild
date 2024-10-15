//Live Loop Mode!
/*

    This little app lets you do performance-focused looping, with reduced editing controls.
    While the seq is playing, the encoders are also used to send CC data.

*/

void exitRecAndStartPlaying(uint8_t& layerCount){
    CoordinatePair A = CoordinatePair(sequence.loopData[sequence.activeLoop].start,sequence.recheadPos);
    CoordinatePair B = CoordinatePair(sequence.loopData[sequence.activeLoop].start,sequence.loopData[sequence.activeLoop].end);
    //make sure to call this first, so that the recording is cleaned up
    togglePlayMode();
    //only warp selected notes
    warpAintoB(A,B,true);
    sequence.isLooping = true;
    lastTime = millis();
    //on the first layer
    //adjust the sequenceClock.BPM so that it feels like it's playing at the same speed
    if(layerCount == 0){
        float timeScale = float(B.end.x-B.start.x)/float(A.end.x-A.start.x);
        uint16_t newBPM = sequenceClock.BPM*timeScale;
        sequenceClock.setBPM(newBPM);
    }
    //disarm all the tracks that were just written to
    // disarmTracksWithNotes();
    //clear selection, so you don't re-warp the previously recorded notes
    clearSelection();
    layerCount++;
}

void liveLoop(){
    //save loop!
    Loop originalLoop = sequence.loopData[sequence.activeLoop];
    uint8_t originalRecMode = recMode;
    recMode = FULL;
    Knob knobA;
    Knob knobB;
    recordedNotesAreSelected = true;
    overwriteRecording = false;

    //note that can trigger play/stop
    int8_t startStopTriggerNote = -1; //this is disabled by default
    // int8_t startStopTriggerNote = 46; //this is disabled by default

    uint8_t layerCount = 0;
    clearSelection();
    while(true){
        controls.readButtons();
        controls.readJoystick();
        mainSequencerEncoders();
        defaultJoystickControls(false);
        defaultSelectBoxControls();
        if(utils.itsbeen(75)){
            //del
            if(controls.DELETE()){
                if (sequence.selectionCount > 0){
                    sequence.deleteSelected();
                    lastTime = millis();
                }
                else if(sequence.IDAtCursor() != 0){
                    sequence.deleteNote(sequence.activeTrack,sequence.cursorPos);
                    lastTime = millis();
                }
            }
        }
        if(utils.itsbeen(200)){
            defaultSelectControls();
            defaultCopyControls();
            defaultLoopControls();
            if(controls.MENU()){
                lastTime = millis();
                if(binarySelectionBox(64,32,"no","yes","exit?",&drawSeq) == 1){
                    break;
                }
            }
            //hold controls.SHIFT() to set the most recent note received to the trigger note
            if(controls.SHIFT() && !controls.PLAY()){
                if(recentNote.pitch<=127){
                    startStopTriggerNote = recentNote.pitch;
                    recentNote.pitch = 255;
                    lastTime = millis();
                }
            }
            //get ready to record, but wait for note
            if(controls.PLAY()){
                //if controls.SHIFT() is held, and you're not already recording
                if(!sequence.recording() && controls.SHIFT()){
                    lastTime = millis();
                    //clear selected notes, so you don't warp 'em
                    clearSelection();
                    //if you were already playing, just begin recording! don't wait for a note
                    //but if you haven't been playing/recording anything, wait for a note
                    if(sequence.playing()){
                        uint16_t oldPlayheadPos = sequence.playheadPos;
                        recMode = LOOP_MODE;
                        toggleRecordingMode(false);
                        sequence.recheadPos = oldPlayheadPos;
                    }
                    else{
                        recMode = FULL;
                        toggleRecordingMode(waitForNoteBeforeRec);
                    }
                }
                //if you are already recording
                //stop the recording and shrink it to the active loop, if you're coming out of a recording
                else if(sequence.recording() && !sequence.playing()){
                    exitRecAndStartPlaying(layerCount);
                }
            }
        }
        //drawing display
        display.clearDisplay();
        SequenceRenderSettings settings;
        settings.topLabels = false;
        drawSeq(settings);
        // display.fillRoundRect(-5,-5,39,22,5,0);
        display.fillRect(0,0,32,16,0);
        // display.drawRoundRect(-5,-5,39,22,5,1);
        // display.drawBitmap(0,0,live_loop_title_bmp,31,15,1,0);
        display.drawBitmap(0,0,live_loop_title_border_bmp,34,17,1,0);
        if(startStopTriggerNote != -1){
           printSmall(88,0,stringify("\a/\t:")+pitchToString(startStopTriggerNote,true,true),1);
        }
        else{
            printSmall(88,0,"no trigger",1);
        }
        printSmall(88,6,"layer #:"+stringify(layerCount),1);
        display.display();

        //checking if the seq should start/stop
        if(startStopTriggerNote != -1){
            if(recentNote.pitch == startStopTriggerNote){
                if(sequence.recording()){
                    exitRecAndStartPlaying(layerCount);
                }
                else{
                    togglePlayMode();
                }
                recentNote.pitch = 255;//set this to something weird so you don't double-trigger it
            }
        }

    }
    recMode = originalRecMode;
    sequence.loopData[sequence.activeLoop] = originalLoop;
}
