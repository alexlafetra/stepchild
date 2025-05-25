//settings are:
/*
 - on/off
 - un-prime tracks that have notes on them already

*/


NoteData setLiveLoopTrigger(){
    NoteData newTrigger = liveLoop.triggerNote;
    while(true){
        newTrigger.pitch = recentNote.pitch;
        newTrigger.channel = recentNote.channel;
        controls.readInputs();
        if(utils.itsbeen(200)){
            if(controls.MENU()){
                lastTime = millis();
                break;
            }
        }
        display.clearDisplay();
        printSmall_centered(64,10,"send a note to set trigger",1);
        printSmall_centered(64,20,"current:",1);
        String p = "$"+pitchToString(newTrigger.pitch,true,true);
        printSmall(48,32,p,1);
        uint8_t length = getSmallTextLength(p);
        display.drawBitmap(48+length+2,32,ch_tiny,6,3,1);
        printSmall(48+length+2+8,32,stringify(newTrigger.channel),1);
        display.display();
    }
    if(newTrigger.pitch > 127)
        newTrigger.pitch = 255;
    return newTrigger;
}

class LiveLoopMenu:public StepchildMenu{
    public:
        SequenceRenderSettings settings;
        LiveLoopMenu(){
            cursor = 0;
            coords = CoordinatePair(6,16,32,64);
            settings.trackLabels = true;
            settings.topLabels = false;
            settings.drawLoopPoints = true;
            settings.shrinkTopDisplay = false;
        }
        bool liveLoopMenuControls(){
            controls.readInputs();
            if(utils.itsbeen(200)){
                if(controls.DOWN() && cursor){
                    cursor--;
                    lastTime = millis();
                }
                if(controls.UP() && cursor < 4){
                    cursor++;
                    lastTime = millis();
                }
                if(controls.SELECT()){
                    switch(cursor){
                        //on/off
                        case 0:
                            liveLoop.active = !liveLoop.active;
                            lastTime = millis();
                            break;
                        //setLoopBounds/ignore loop
                        case 1:
                            liveLoop.setLoopBoundsAfterRec = !liveLoop.setLoopBoundsAfterRec;
                            lastTime = millis();
                            break;
                        //listen for note trigger
                        case 2:
                            lastTime = millis();
                            liveLoop.triggerNote = setLiveLoopTrigger();
                            return true;
                         //toggle note trigger
                        case 3:
                        case 4:
                            lastTime = millis();
                            liveLoop.triggerNote.pitch = (liveLoop.triggerNote.pitch == 255?60:255);
                            break;
                    }
                }
                while(controls.counterA){
                    switch(cursor){
                        //on/off
                        case 0:
                            liveLoop.active = !liveLoop.active;
                            break;
                        //setLoopBounds/ignore loop
                        case 1:
                            liveLoop.setLoopBoundsAfterRec = !liveLoop.setLoopBoundsAfterRec;
                            break;
                        //note pitch
                        case 3:
                            if(controls.counterA > 0 && liveLoop.triggerNote.pitch < 127)
                                liveLoop.triggerNote.pitch++;
                            else if(controls.counterA < 0 && liveLoop.triggerNote.pitch)
                                liveLoop.triggerNote.pitch--;
                            break;
                         //note channel
                        case 4:
                            if(controls.counterA > 0 && liveLoop.triggerNote.channel < 16)
                                liveLoop.triggerNote.channel++;
                            else if(controls.counterA < 0 && liveLoop.triggerNote.channel > 1)
                                liveLoop.triggerNote.channel--;
                            break;
                    }
                    controls.countDownA();
                }
                if(controls.MENU()){
                    lastTime = millis();
                    return false;
                }
            }
            return true;
        }
        void displayMenu(){
            display.clearDisplay();
            //draw sequence
            drawSeq(settings);
            //bounding box for the menu
            display.fillRoundRect(coords.start.x-10,coords.start.y,50,51,4,0);
            display.drawRoundRect(coords.start.x-10,coords.start.y,50,51,4,1);

            //buttons
            //on/off
            graphics.drawButton(coords.start.x+6,coords.start.y+2, liveLoop.active?"on":"off",cursor == 0);

            //loop
            printSmall(coords.start.x+3,coords.start.y+10,"loop:",1);
            graphics.drawButton(coords.start.x+6,coords.start.y+16, liveLoop.setLoopBoundsAfterRec?"new":"current",cursor == 1);
            
            //trigger
            graphics.drawButton(coords.start.x,coords.start.y+23, "trigger",cursor == 2);
            if(liveLoop.triggerNote.pitch != 255){
                graphics.drawButton(coords.start.x+6,coords.start.y+30,"$"+pitchToString(liveLoop.triggerNote.pitch,true,true),cursor == 3);
                String channel = stringify(liveLoop.triggerNote.channel);
                if(cursor == 4)
                    display.fillRoundRect(coords.start.x+6,coords.start.y+37,channel.length()*4+10,7,3,1);
                display.drawBitmap(coords.start.x+8,coords.start.y+38,ch_tiny,6,3,2);
                printSmall(coords.start.x+15,coords.start.y+38,channel,2);
            }
            else{
                graphics.drawButton(coords.start.x+6,coords.start.y+30,"none",cursor == 3);
            }

            //arrow
            uint8_t y = 0;
            int8_t x = -3;
            switch(cursor){
                case 0: y = 5;  x = 3; break;
                case 1: y = 19; x = 3; break;
                case 2: y = 26; break;
                case 3: y = 33; x = 3; break;
                case 4: if(liveLoop.triggerNote.pitch == 255){y = 33;}else{y = 40;} x = 3; break;
            }
            graphics.drawArrow(coords.start.x+x+(millis()/200)%2,coords.start.y+y,3,ARROW_RIGHT,false);
            display.display();
        }
};

bool liveLoopMenu(){
    controls.clearButtons();
    LiveLoopMenu liveMenu;
    liveMenu.slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
    while(liveMenu.liveLoopMenuControls()){
        //draw menu
        liveMenu.displayMenu();
    }
    liveMenu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
    return false;
}