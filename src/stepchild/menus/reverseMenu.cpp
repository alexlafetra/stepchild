CoordinatePair selectNotesAndArea(String text, void (*iconFunction)(uint8_t,uint8_t,uint8_t,bool)){
    CoordinatePair bounds;
    //storing a copy of the current loop, so we can edit the current loop to set our bounds
    //and then reset it back to the original loop
    while(true){
        readJoystick();
        readButtons();
        defaultSelectBoxControls();
        defaultJoystickControls(false);
        defaultEncoderControls();
        if(itsbeen(200)){
            defaultLoopControls();
            defaultSelectControls();
            if(menu_Press){
                lastTime = millis();
                clearSelection();
                break;
            }
            if(n){
                lastTime = millis();
                break;
            }
        }
        display.clearDisplay();
        drawSeq(true, false, true, false, false, true, viewStart, viewEnd);
        printSmall(trackDisplay,0,text,1);
        iconFunction(7,1,14,true);
        display.display();
    }
    bounds.x1 = loopData[activeLoop].start;
    bounds.x2 = loopData[activeLoop].end;
    return bounds;
}

//lines floating to the left
void drawRevIcon(uint8_t x1, uint8_t y1, uint8_t height, bool animated){
    for(uint8_t i = 0; i<height; i++){
        int8_t start = uint8_t(height/2*sin(float(millis())/500.0+float(i*2))+height/2)%(height);
        uint8_t length = (height-start)>4?4:(height-start);
        if(start>0 && start<height-1)
            display.drawFastHLine(x1+start,y1+i,length,1);
    }
}

//Reverses all selected notes within a start/stop bound
//Notes must be totally within bound to be affected
void reverse(){
    //this is for making it transition nicely from the fx menu
    //but it doesn't play well with the edit menu :(
    // slideMenuOut(0,20);
    Loop tempLoop = loopData[activeLoop];
    while(true){
        clearSelection();
        CoordinatePair bounds = selectNotesAndArea("Sel notes + bounds to rev",drawRevIcon);
        //if no notes were selected (because the user backed out, or goofed)
        if(selectionCount == 0){
            break;
        }
        //if no area was selected (because the user goofed)
        if(bounds.x2-bounds.x1<1){
            break;
        }

        //center of the "rotation"
        uint16_t center = (bounds.x2-bounds.x1)/2+bounds.x1;

        //for each note, the distance its closest point is from center should become the -distance its farthest is from the center
        //EX: if a note starts 10 steps AFTER the centerpoint, then that note should now END 10 steps BEFORE the centerpoint
    
        //iterate over notes
        for(uint8_t track = 0; track<seqData.size(); track++){
            for(uint16_t n = 1; n<seqData[track].size(); n++){
                Note note = seqData[track][n];
                //if the note is selected & is TOTALLY within reverse bounds
                if(note.isSelected && (note.startPos>=bounds.x1 && note.endPos<=bounds.x2)){

                    //get its distances from the center point
                    int16_t startDist = note.startPos - center;
                    int16_t endDist = note.endPos - center;

                    //flip the endpoints
                    note.startPos = center - endDist;
                    note.endPos = center - startDist;

                    //deselect the note (because it always comes selected)
                    note.isSelected = false;

                    //delete the old note
                    deleteNote_byID(track,n);

                    //place the new note
                    makeNote(note,track);
                }
            }
        }
    }
    
    //reset the loop points to the loop we stored in the beginning
    loopData[activeLoop] = tempLoop;
}
