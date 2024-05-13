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

    To define app 'N' as an "instrument" app, you need to add the preprocessor definitions after your function:

    #define INSTRUMENT_APPN_ICON bitmap_name
    #define INSTRUMENT_APPN_FUNCTION functionName
    #define INSTRUMENT_APPN_TEXT "hey"

    Each of these is technically optional, so you don't need to have app text or a bitmap icon. 
    Without a function, though, your app won't be linked to the FX or instrument menu.
    "bitmap_name" needs to be the variable name of a pointer to a const char array, aka a const char *. ChildOS will use this
    pointer to access the bitmap data.

    To create a custom bitmap, I recommend using a drawing program like Pikopixel or photoshop/GIMP and finding a bmp to cpp converter online!

*/

//the default app function.
//You can use this as a template, but only change this function after making a copy of it!
// It's the actual default function.
void defaultAppFunction(){

    //put your setup code here!

    while(true){
        //put your continuous code down here!

        //reading in controls
        readButtons();
        controls.readJoystick();

        //if it's been 200ms
        if(utils.itsbeen(200)){
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

