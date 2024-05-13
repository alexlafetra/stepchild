void rattle(){
    //speed when y is min
    uint32_t minLength = 0;
    //speed when y is max
    uint32_t maxLength = 500;

    //vel when x is min
    uint16_t minY = 0;
    //vel when x is max
    uint16_t maxY = 127;

    uint32_t noteLength = 100;

    float joyX = 0.5;
    float lastX = 0.5;
    float joyY = 0.5;
    float lastY = 0.5;

    uint8_t pitch = 64;
    uint8_t vel = 127;
    uint8_t channel = 1;

    bool isPlaying = false;

    //controls what the second param (y axis) is linked to
    //0 is vel, 1 is pitch, 2 is both
    uint8_t linkedTo = 1;

    uint32_t timeSinceLastNote  = millis();
    while(true){
        //storing last read vals so we can average and smoothe out the motion
        lastX = joyX;
        lastY = joyY;
        joyX = ((1.0 - analogRead(JOYSTICK_X)/1024.0)+joyX)/2;
        joyY = ((1.0 - analogRead(JOYSTICK_Y)/1024.0)+joyY)/2;

        if(linkedTo == 0){
            vel = (maxY-minY)*(joyY)+minY;
            if(vel>127)
                vel = 127;
            pitch = 64;
        }
        else if(linkedTo == 1){
            pitch = (maxY-minY)*(joyY)+minY;
            if(pitch>127)
                pitch = 127;
            vel = 127;
        }
        else if(linkedTo == 2){
            pitch = (maxY-minY)*(joyY)+minY;
            if(pitch>127)
                pitch = 127;
            vel = pitch;
        }

        noteLength = (maxLength-minLength)*(joyX)+minLength;

        //if the difference between the current time and the time the last note was sent is more than the noteLength
        if(isPlaying && ((millis()-timeSinceLastNote)>=noteLength)){
            MIDI.noteOn(pitch,vel,channel);
            MIDI.noteOff(pitch,0,channel);
            timeSinceLastNote = millis();
        }

        readButtons();
        if(utils.itsbeen(200)){
            if(play){
                isPlaying = !isPlaying;
                lastTime = millis();
            }
            if(menu_Press){
                lastTime = millis();
                break;
            }
        }
        display.clearDisplay();
        if(isPlaying)
            drawPlayIcon(1+((millis()/400)%2),0);
        printSmall(0,10,stringify(noteLength),1);
        printSmall(0,20,stringify(vel),1);

        const uint8_t x1 = 20;
        const uint8_t y1 = 16;
        //note length
        display.drawBitmap(61,y1-6,small_clock,5,5,1);
        display.drawFastVLine(x1,y1,8,1);
        display.drawFastVLine(screenWidth-x1,y1,8,1);
        display.drawFastVLine(x1+(screenWidth-2*x1)*(joyX),y1,8,1);
        //velocity
        display.drawFastVLine(x1,screenHeight-y1-8,8,1);
        display.drawFastVLine(screenWidth-x1,screenHeight-y1-8,8,1);
        display.drawFastVLine(x1+(screenWidth-2*x1)*(joyY),screenHeight-y1-8,8,1);
        String s;
        switch(linkedTo){
            //vel
            case 0:
                s = "velocity";
                break;
            //pitch
            case 1:
                s = "$";
                break;
            //both
            case 2:
                s = "vel+$";
                break;
        }
        printSmall_centered(64,screenHeight-y1+1,s,1);
        display.display();
    }
}