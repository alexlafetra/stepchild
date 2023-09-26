/*
    +====================================+
    |                                    |
    |       Custom applications!         |
    |                                    |
    +====================================+
    
    Put ya custom apps in here!

    Some rules:
        - The main function for a custom App must take no arguments and return a void object
          (eg. void customFunction())

        - You can write new object classes, functions, whatever and call that from within your function just like normal

        - The bitmap icon for your custom function, if you make one, should be 12x12px. If it's bigger,
          it'll be truncated and might look weird. If you don't define one, it'll use the default X texture

    To define app 'N' as an "instrument" app, do:
    #define INSTRUMENT_APPN_ICON bitmap_name
    #define INSTRUMENT_APPN_FUNCTION functionName
    #define INSTRUMENT_APPN_TEXT "hey"
*/

//the default app function. You can use this as a template, but copy it first!
void defaultAppFunction(){

    //set up your setup code here!

    //put your continuous code down here
    while(true){

        //reading in controls
        readButtons();
        readJoystick();

        //if it's been 200ms
        if(itsbeen(200)){
            //and the menu button was pressed...
            if(menu_Press){
                //reset "itsbeen" timer
                lastTime = millis();
                //and break out of the while loop
                break;
            }
        }
        display.clearDisplay();
        printSmall_centered(64,32,"hey! There's no app here.",1);
        printSmall_centered(64,38,"create your own!",1);
        display.display();
    }
    //put your exit code here!
}