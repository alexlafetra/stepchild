void liveLoop(){
    Loop originalLoop = loopData[activeLoop];
    bool originalLoopState = isLooping;
    isLooping = false;
    menuIsActive = false;
    while(true){
        readButtons();
        readJoystick();
        mainSequencerEncoders();
        defaultJoystickControls(false);
        defaultSelectBoxControls();
        if(itsbeen(200)){
            defaultSelectControls();
            defaultCopyControls();
            defaultLoopControls();
            if(menu_Press){
                lastTime = millis();
                break;
            }
            //get ready to record, but wait for note
            if(play && shift){
                lastTime = millis();
                uint16_t temp = recheadPos;
                toggleRecordingMode(true);
                //if a recording just ended, shrink it to the active loop
                if(!recording){
                    CoordinatePair A = CoordinatePair(0,recheadPos);
                    CoordinatePair B = CoordinatePair(loopData[activeLoop].start,loopData[activeLoop].end);
                    warpAintoB(A,B,false);
                    isLooping = true;
                    togglePlayMode();
                }
            }
        }
        display.clearDisplay();
        drawSeq(true,true,true,false,false,false,viewStart,viewEnd);
        display.display();
    }
    menuIsActive = true;
    setNormalMode();
}