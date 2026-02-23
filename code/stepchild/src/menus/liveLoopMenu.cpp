#include "Stepchild.h"
#include "StepchildGraphics.h"
#include "graphics/WireFrame.h"
#include "mainSequence.h"
extern StepchildGraphics graphics;
extern Stepchild stepchild;
using namespace std;

//settings are:
/*
 - on/off
 - un-prime tracks that have notes on them already

*/


NoteData setLiveLoopTrigger(){
    NoteData newTrigger = stepchild.liveLoop.triggerNote;
    while(true){
        newTrigger.pitch = stepchild.recentNote.pitch;
        newTrigger.channel = stepchild.recentNote.channel;
        stepchild.buttons.readInputs();
        if(stepchild.itsbeen(200)){
            if(stepchild.buttons.MENU()){
                stepchild.lastTime = millis();
                break;
            }
        }
        stepchild.display.clearDisplay();
        graphics.printSmall_centered(64,10,"send a note to set trigger",1);
        graphics.printSmall_centered(64,20,"current:",1);
        String p = "$"+stepchild.pitchToString(newTrigger.pitch,true,true);
        graphics.printSmall(48,32,p,1);
        uint8_t length = graphics.getSmallTextLength(p);
        stepchild.display.drawBitmap(48+length+2,32,ch_tiny,6,3,1);
        graphics.printSmall(48+length+2+8,32,stringify(newTrigger.channel),1);
        stepchild.display.display();
    }
    if(newTrigger.pitch > 127)
        newTrigger.pitch = 255;
    return newTrigger;
}

class LiveLoopMenu:public StepchildMenu{
    public:
        WireFrame topCassette;
        WireFrame reelA;
        LiveLoopMenu(){
            cursor = 0;
            coords = CoordinatePair(0,16,32,64);
            createTopCassettes();
        }
        void createTopCassettes(){
            const float r = 5;
            //wheels
            WireFrame a = makeDisc_centered(0,0,0,r,0,12,0);
            a.verts.push_back(Vertex(-r/2,0,0));//L
            a.verts.push_back(Vertex(r/2,0,0));//R
            a.verts.push_back(Vertex(0,r/2,0));//U
            a.verts.push_back(Vertex(0,-r/2,0));//D
            int16_t len = a.verts.size()-1; 
            a.edges.push_back({0,static_cast<unsigned short>(len-2)});
            a.edges.push_back({3,static_cast<unsigned short>(len-1)});
            a.edges.push_back({6,static_cast<unsigned short>(len-3)});
            a.edges.push_back({9,static_cast<unsigned short>(len)});

            reelA = a;
        }
  
        bool liveLoopMenuControls(){
            stepchild.buttons.readInputs();
            defaultJoystickXControls();
            defaultLoopControls();
            if(stepchild.itsbeen(200)){
                if(stepchild.buttons.DOWN() && cursor){
                    cursor--;
                    stepchild.lastTime = millis();
                }
                if(stepchild.buttons.UP() && cursor < 4){
                    cursor++;
                    stepchild.lastTime = millis();
                }
                if(stepchild.buttons.SELECT()){
                    switch(cursor){
                        //on/off
                        case 0:
                            stepchild.liveLoop.active = !stepchild.liveLoop.active;
                            stepchild.lastTime = millis();
                            break;
                        //setLoopBounds/ignore loop
                        case 1:
                            stepchild.liveLoop.setLoopBoundsAfterRec = !stepchild.liveLoop.setLoopBoundsAfterRec;
                            stepchild.liveLoop.setLoopBoundsAfterRec_flag = stepchild.liveLoop.setLoopBoundsAfterRec;
                            stepchild.lastTime = millis();
                            break;
                        //listen for note trigger
                        case 2:
                            stepchild.lastTime = millis();
                            stepchild.liveLoop.triggerNote = setLiveLoopTrigger();
                            return true;
                         //toggle note trigger
                        case 3:
                        case 4:
                            stepchild.lastTime = millis();
                            stepchild.liveLoop.triggerNote.pitch = (stepchild.liveLoop.triggerNote.pitch == 255?60:255);
                            break;
                    }
                }
                while(stepchild.buttons.counterA){
                    switch(cursor){
                        //on/off
                        case 0:
                            stepchild.liveLoop.active = !stepchild.liveLoop.active;
                            break;
                        //setLoopBounds/ignore loop
                        case 1:
                            stepchild.liveLoop.setLoopBoundsAfterRec = !stepchild.liveLoop.setLoopBoundsAfterRec;
                            stepchild.liveLoop.setLoopBoundsAfterRec_flag = stepchild.liveLoop.setLoopBoundsAfterRec;
                            break;
                        //note pitch
                        case 3:
                            if(stepchild.buttons.counterA > 0 && stepchild.liveLoop.triggerNote.pitch < 127)
                                stepchild.liveLoop.triggerNote.pitch++;
                            else if(stepchild.buttons.counterA < 0 && stepchild.liveLoop.triggerNote.pitch)
                                stepchild.liveLoop.triggerNote.pitch--;
                            break;
                         //note channel
                        case 4:
                            if(stepchild.buttons.counterA > 0 && stepchild.liveLoop.triggerNote.channel < 16)
                                stepchild.liveLoop.triggerNote.channel++;
                            else if(stepchild.buttons.counterA < 0 && stepchild.liveLoop.triggerNote.channel > 1)
                                stepchild.liveLoop.triggerNote.channel--;
                            break;
                    }
                    stepchild.buttons.countDownA();
                }
                if(stepchild.buttons.MENU()){
                    stepchild.lastTime = millis();
                    return false;
                }
            }
            return true;
        }
        void displayMenu(){
            SequenceRenderSettings settings;
            settings.trackLabels = false;
            settings.topLabels = false;
            settings.drawLoopPoints = true;
            settings.drawLoopFlags = false;
            settings.drawPram = false;
            settings.shrinkTopDisplay = false;

            stepchild.display.clearDisplay();
            //draw sequence
            graphics.drawSeq(settings);

            const float r = 5.0;
            int16_t start = (stepchild.loopData[stepchild.activeLoop].start-settings.start)*stepchild.viewScale+32;
            int16_t length = (stepchild.loopData[stepchild.activeLoop].end - stepchild.loopData[stepchild.activeLoop].start)*stepchild.viewScale;
            int16_t end = (stepchild.loopData[stepchild.activeLoop].end-settings.start)*stepchild.viewScale+32;
            bool A = true;
            bool B = true;
            if(start >= 32 && end <= 128){
                stepchild.display.fillRoundRect(start-r-2,0,length+2*r+4,2*r+4,r+2,0);
                stepchild.display.drawRoundRect(start-r-2,0,length+2*r+4,2*r+4,r+2,1);
            }
            //if start is off the left side of the screen
            if(start<32){
                A = false;
                // start = 0;
                // length = end-start;
                graphics.drawArrow(36+(millis()/200)%2,7,3,ARROW_LEFT,true);
                // stepchild.display.drawRoundRect(start-r-2-35,0,length+2*r+4+45,2*r+4,r+2,1);
            }
            if(start >= 128){
                A = false;
                graphics.drawArrow(120 - (millis()/200)%2,7,3,ARROW_RIGHT,true);
            }
            if(end>=128){
                B = false;
                // end = 128;
                length = end-start;
                graphics.drawArrow(120 - (millis()/200)%2,7,3,ARROW_RIGHT,true);
                stepchild.display.drawRoundRect(start-r-2,0,length+2*r+4+10,2*r+4,r+2,1);
            }
            else if(end < 32){
                B = false;
                graphics.drawArrow(36+(millis()/200)%2,7,3,ARROW_LEFT,true);
            }
            if(A || B){
                // float rotation = millis()/100.0;
                reelA.setRotation(millis()/5.0,2);
                reelA.offset.y = 7;
                if(A){
                    reelA.offset.x = start;
                    reelA.render();
                }
                if(B){
                    reelA.offset.x = end;
                    reelA.render();
                }
                // reelA.rotate(-rotation,2);
            }
            if(start > 32 || end > 32)
                stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH,0,stepchild.SCREEN_HEIGHT,1);
            
            if(A && !B){
                stepchild.display.fillRoundRect(start-r-2,0,stepchild.SCREEN_WIDTH - start + r +r + 2,2*r+4,r+2,0);
                stepchild.display.drawRoundRect(start-r-2,0,stepchild.SCREEN_WIDTH - start + r +r + 2,2*r+4,r+2,1);
            }
            else if(!A && B){
                stepchild.display.fillRoundRect(32-r-2,0,length+2*r+4,2*r+4,r+2,0);
                stepchild.display.drawRoundRect(32-r-2,0,length+2*r+4,2*r+4,r+2,1);
            }

            if(start <= 32 || end <= 32){
                stepchild.display.fillRect(0,0,stepchild.TRACK_LABEL_WIDTH,stepchild.HEADER_HEIGHT,0);
                stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH,0,stepchild.SCREEN_HEIGHT,1);
            }
            if(stepchild.playing()){
                stepchild.display.fillTriangle(8,4,8,11,28,7,1);
            }
            else if(stepchild.recording()){
                if((millis()/600)%2)
                    stepchild.display.fillCircle(16,8,8,1);
                else
                    stepchild.display.drawCircle(16,8,8,1);
            }
            else{
                if((millis()/1200)%2)
                    stepchild.display.fillRoundRect(5,0,16,16,2,1);
                else
                    stepchild.display.drawRoundRect(5,0,16,16,2,1);
            }



            //bounding box for the menu
            // stepchild.display.fillRoundRect(coords.start.x-10,coords.start.y,50,51,4,0);
            // stepchild.display.drawRoundRect(coords.start.x-10,coords.start.y,50,51,4,1);

            //buttons
            //on/off
            graphics.drawButton(coords.start.x+6,coords.start.y+2, stepchild.liveLoop.active?"on":"off",cursor == 0);

            //loop
            graphics.printSmall(coords.start.x+3,coords.start.y+10,"loop:",1);
            graphics.drawButton(coords.start.x+6,coords.start.y+16, stepchild.liveLoop.setLoopBoundsAfterRec?"new":"this",cursor == 1);
            
            //trigger
            graphics.drawButton(coords.start.x,coords.start.y+23, "strt:",cursor == 2);
            if(stepchild.liveLoop.triggerNote.pitch != 255){
                graphics.drawButton(coords.start.x+6,coords.start.y+30,"$"+stepchild.pitchToString(stepchild.liveLoop.triggerNote.pitch,true,true),cursor == 3);
                String channel = stringify(stepchild.liveLoop.triggerNote.channel);
                if(cursor == 4)
                    stepchild.display.fillRoundRect(coords.start.x+6,coords.start.y+37,channel.length()*4+10,7,3,1);
                stepchild.display.drawBitmap(coords.start.x+8,coords.start.y+38,ch_tiny,6,3,2);
                graphics.printSmall(coords.start.x+15,coords.start.y+38,channel,2);
            }
            else{
                graphics.drawButton(coords.start.x+6,coords.start.y+30,"rec",cursor == 3);
            }

            //arrow
            uint8_t y = 0;
            int8_t x = -3;
            switch(cursor){
                case 0: y = 5;  x = 3; break;
                case 1: y = 19; x = 3; break;
                case 2: y = 26; x = 28; break;
                case 3: y = 33; x = 3; break;
                case 4: if(stepchild.liveLoop.triggerNote.pitch == 255){y = 33;}else{y = 40;} x = 3; break;
            }
            graphics.drawArrow(coords.start.x+x+(millis()/200)%2,coords.start.y+y,3,cursor!=2?ARROW_RIGHT:ARROW_LEFT,false);
            stepchild.display.display();
        }
};

bool liveLoopMenu(){
    stepchild.buttons.clearButtons();
    LiveLoopMenu liveMenu;
    liveMenu.slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
    while(liveMenu.liveLoopMenuControls()){
        //draw menu
        liveMenu.displayMenu();
    }
    liveMenu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
    return false;
}
