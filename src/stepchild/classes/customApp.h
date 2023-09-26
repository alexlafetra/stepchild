//the default app function
void defaultAppFunction(){
    while(true){
        display.clearDisplay();
        printSmall_centered("hey! There's no app here. Create your own!",64,32,1);
        display.display();
        readButtons();
        if(itsbeen(200)){
            if(menu_Press){
                lastTime = millis():
                break;
            }
        }
    }
}

//Default ChildOS applications

#define INSTRUMENT_APP1_ICON xy_bmp
#define INSTRUMENT_APP2_ICON rain_bmp
#define INSTRUMENT_APP3_ICON knobs_bmp
#define INSTRUMENT_APP4_ICON keys_bmp
#define INSTRUMENT_APP5_ICON drump_bmp
#define INSTRUMENT_APP6_ICON cv_icon_bmp

#define INSTRUMENT_APP1_FUNCTION xyGrid
#define INSTRUMENT_APP2_FUNCTION rain
#define INSTRUMENT_APP3_FUNCTION knobs
#define INSTRUMENT_APP4_FUNCTION keyboard
#define INSTRUMENT_APP5_FUNCTION drumPad
#define INSTRUMENT_APP6_FUNCTION MIDItoCV

/*
    Check to see if the user has defined these in their code!
    If not, set them to a default function or bitmap
*/

//Icons

#ifndef INSTRUMENT_APP7_ICON
    #define INSTRUMENT_APP7_ICON epd_bitmap_empty
#endif
#ifndef INSTRUMENT_APP8_ICON
    #define INSTRUMENT_APP8_ICON epd_bitmap_empty
#endif
#ifndef INSTRUMENT_APP9_ICON
    #define INSTRUMENT_APP9_ICON epd_bitmap_empty
#endif
#ifndef INSTRUMENT_APP10_ICON
    #define INSTRUMENT_APP10_ICON epd_bitmap_empty
#endif
#ifndef INSTRUMENT_APP11_ICON
    #define INSTRUMENT_APP12_ICON epd_bitmap_empty
#endif
#ifndef INSTRUMENT_APP13_ICON
    #define INSTRUMENT_APP7_ICON epd_bitmap_empty
#endif
#ifndef INSTRUMENT_APP14_ICON
    #define INSTRUMENT_APP7_ICON epd_bitmap_empty
#endif
#ifndef INSTRUMENT_APP15_ICON
    #define INSTRUMENT_APP7_ICON epd_bitmap_empty
#endif
#ifndef INSTRUMENT_APP16_ICON
    #define INSTRUMENT_APP7_ICON epd_bitmap_empty
#endif

//Functions

#ifndef INSTRUMENT_APP7_FUNCTION
    #define INSTRUMENT_APP7_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP8_FUNCTION
    #define INSTRUMENT_APP8_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP9_FUNCTION
    #define INSTRUMENT_APP9_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP10_FUNCTION
    #define INSTRUMENT_APP10_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP11_FUNCTION
    #define INSTRUMENT_APP12_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP13_FUNCTION
    #define INSTRUMENT_APP7_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP14_FUNCTION
    #define INSTRUMENT_APP7_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP15_FUNCTION
    #define INSTRUMENT_APP7_FUNCTION defaultAppFunction
#endif
#ifndef INSTRUMENT_APP16_FUNCTION
    #define INSTRUMENT_APP7_FUNCTION defaultAppFunction
#endif

const unsigned char* instrumentApplicationIcons[16] = {
    INSTRUMENT_APP1_ICON,
    INSTRUMENT_APP2_ICON,
    INSTRUMENT_APP3_ICON,
    INSTRUMENT_APP4_ICON,
    INSTRUMENT_APP5_ICON,
    INSTRUMENT_APP6_ICON,
    INSTRUMENT_APP7_ICON,
    INSTRUMENT_APP8_ICON,
    INSTRUMENT_APP9_ICON,
    INSTRUMENT_APP10_ICON,
    INSTRUMENT_APP11_ICON,
    INSTRUMENT_APP12_ICON,
    INSTRUMENT_APP13_ICON,
    INSTRUMENT_APP14_ICON,
    INSTRUMENT_APP15_ICON,
    INSTRUMENT_APP16_ICON
};

//loading all the functions
const * void (*) instrumentApplicationFunctions[16] = {
    INSTRUMENT_APP1_FUNCTION,
    INSTRUMENT_APP2_FUNCTION,
    INSTRUMENT_APP3_FUNCTION,
    INSTRUMENT_APP4_FUNCTION,
    INSTRUMENT_APP5_FUNCTION,
    INSTRUMENT_APP6_FUNCTION,
    INSTRUMENT_APP7_FUNCTION,
    INSTRUMENT_APP8_FUNCTION,
    INSTRUMENT_APP9_FUNCTION,
    INSTRUMENT_APP10_FUNCTION,
    INSTRUMENT_APP11_FUNCTION,
    INSTRUMENT_APP12_FUNCTION,
    INSTRUMENT_APP13_FUNCTION,
    INSTRUMENT_APP14_FUNCTION,
    INSTRUMENT_APP15_FUNCTION,
    INSTRUMENT_APP16_FUNCTION
}



