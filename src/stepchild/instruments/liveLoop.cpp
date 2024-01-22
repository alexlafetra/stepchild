//Live Loop Mode!
/*

    This little app lets you do performance-focused looping, with reduced editing controls.
    While the seq is playing, the encoders are also used to send CC data.

*/
void liveLoop(){
    //save loop!
    Loop originalLoop = loopData[activeLoop];
    uint8_t originalRecMode = recMode;
    menuIsActive = false;
    recMode = FULL;
    Knob knobA;
    Knob knobB;
    recordedNotesAreSelected = true;
    overwriteRecording = false;

    uint8_t layerCount = 0;
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
                    //clear selected notes, so you don't warp 'em
                    clearSelection();
                    //if you were already playing, just begin recording! don't wait for a note
                    //but if you haven't been playing/recording anything, wait for a note
                    if(playing){
                        uint16_t oldPlayheadPos = playheadPos;
                        recMode = LOOP;
                        toggleRecordingMode(false);
                        recheadPos = oldPlayheadPos;
                    }
                    else{
                        recMode = FULL;
                        toggleRecordingMode(waitForNoteBeforeRec);
                    }
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
                    //on the first layer
                    //adjust the bpm so that it feels like it's playing at the same speed
                    if(layerCount == 0){
                        float timeScale = float(B.x2-B.x1)/float(A.x2-A.x1);
                        uint16_t newBPM = bpm*timeScale;
                        setBpm(newBPM);
                    }
                    //disarm all the tracks that were just written to
                    // disarmTracksWithNotes();
                    //clear selection, so you don't re-warp the previously recorded notes
                    clearSelection();
                    layerCount++;
                }
            }
        }
        display.clearDisplay();
        drawSeq(true,false,true,false,false,false,viewStart,viewEnd);
        printSmall(trackDisplay,0,"#:"+stringify(layerCount),1);
        if(!recording){
            int8_t offset = 0;
            if(playing)
                offset = 2*((millis()/200)%2);
            uint8_t x0 = 6+offset;
            const uint8_t y0 = 0;
            const uint8_t len = 16;
            uint8_t len2 = len-(16-(millis()/100)%16);
            uint8_t height;
            if(maxTracksShown == 6 && !menuIsActive){
                height = 6;
            }
            else{
                height = 12;
            }
            if(playing){
                display.fillTriangle(x0,y0,x0,y0+height,x0+len,y0+height/2,1);
                display.fillRect(x0+len2,y0,len-len2,height,0);
            }
            display.drawTriangle(x0,y0,x0,y0+height,x0+len,y0+height/2,1);
        }
        else if(recording){
            uint8_t radius = 7;
            uint8_t y0 = 7;
            const uint8_t x0 = 15;
            if(maxTracksShown == 6 && !menuIsActive){
                y0 = 4;
                radius = 4;
            }
            if(millis()%1000>500)
                display.fillCircle(x0,y0,radius,1);
            else
                display.drawCircle(x0,y0,radius,1);
        }
        display.display();
    }
    menuIsActive = true;
    recMode = originalRecMode;
    loopData[activeLoop] = originalLoop;
}