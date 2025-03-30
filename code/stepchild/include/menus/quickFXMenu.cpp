class QuickFXMenu:public StepchildMenu{
    public:
    uint8_t menuStart = 0;
    uint8_t selectedFX = 255;
    SequenceRenderSettings settings;
    QuickFXMenu(){
        coords = CoordinatePair(0,14,35,64);
        settings.shrinkTopDisplay = false;
        settings.topLabels = false;
        settings.drawLoopFlags = false;
        settings.drawPram = false;
    }
    bool quickFXControls(){
        controls.readInputs();
        if(utils.itsbeen(200)){
            if(controls.DOWN() && cursor > 0){
                cursor--;
                lastTime = millis();
            }
            else if(controls.UP() && cursor < NUMBER_OF_FX-1){
                cursor++;
                lastTime = millis();
            }
            if(controls.MENU()){
                lastTime = millis();
                selectedFX = 255;
                controls.setMENU(false);
                return false;
            }
            if(controls.SELECT()){
                selectedFX = cursor;
                lastTime = millis();
                return false;
            }
        }
        return true;
    }
    void displayMenu(){
        if(coords.start.y < 16){
            settings.trackLabels = false;
        }
        else{
            settings.trackLabels = true;
        }
        display.clearDisplay();
        drawSeq(settings);
        display.fillRoundRect(coords.start.x, coords.start.y, coords.end.x, coords.end.y+3-coords.start.y,2,0);
        display.drawRoundRect(coords.start.x, coords.start.y, coords.end.x, coords.end.y+3-coords.start.y,2,1);
        for(uint8_t i = 0; i<NUMBER_OF_FX; i++){
            printSmall(coords.start.x+2,coords.start.y+i*6+2,fxApplicationTitles[i],1);
            if(i == cursor){
                graphics.drawArrow(coords.start.x+32+millis()/400%2,coords.start.y+i*6+4,3,ARROW_LEFT,false);
            }
        }
        display.drawBitmap(10,0,fxApplicationIcons[cursor],12,12,SSD1306_WHITE);
        printSmall_overflow(32,0,2,fxApplicationInfo[cursor],2);
        display.display();
    }
};

uint8_t quickFX(){
    QuickFXMenu quickFXMenu;
    quickFXMenu.slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM_FAST);
    while(quickFXMenu.quickFXControls()){
        quickFXMenu.displayMenu();
    }
    quickFXMenu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM_FAST);
    return quickFXMenu.selectedFX;
}