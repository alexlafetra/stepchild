//This file mainly replaces/supplements RP2040/Hardware functions and arduino core library functions

#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <chrono>//for emulating millis() and micros()
#include <unistd.h>
#include <thread>//for multithreading
#include <algorithm>

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//fonts
#include "Fonts/gfxfont.h"
#include "Fonts/ASCII_font.h"
#include "Fonts/FreeSerifItalic9pt7b.h"
#include "Fonts/FreeSerifItalic12pt7b.h"
#include "Fonts/FreeSerifItalic24pt7b.h"

//for writing bitmaps
#include "bitmap/bitmap_image.hpp"

//including this for continuity
//#include "../../stepchild/pins.h"

#define UPRIGHT 2
#define UPSIDEDOWN 0
#define SIDEWAYS_R 3
#define SIDEWAYS_L 1

#define SCREENWIDTH 128
#define SCREENHEIGHT 64
# define PI           3.14159265358979323846  /* pi */

#define SSD1306_WHITE 1
#define SSD1306_BLACK 0

#define PICO_DEFAULT_LED_PIN 0

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

#define charAt at
#define substring substr

//swapping the arduino "String" object for the C++ "string" object (kinda silly, but it works)
#define String string

using namespace std;

int counterA, counterB;

bool core0ready;
unsigned long lastTime = 0;
bool playing = false;
bool recording = false;

#include "HeadlessMIDI.h"
#include "HeadlessFileSystem.h"

//emulating micros and millis
auto progStartTime = chrono::high_resolution_clock::now();
unsigned long micros(){
    auto rn = chrono::high_resolution_clock::now();
    return chrono::duration_cast<std::chrono::microseconds>(rn-progStartTime).count();
}
unsigned long millis(){
    auto rn = chrono::high_resolution_clock::now();
    return chrono::duration_cast<std::chrono::milliseconds>(rn-progStartTime).count();
}
int constrain(int target, int lower, int upper){
    if(target<lower){
        return lower;
    }
    else if(target>upper){
        return upper;
    }
    else return target;
}

inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
  // expression in __AVR__ section may generate "dereferencing type-punned
  // pointer will break strict-aliasing rules" warning In fact, on other
  // platforms (such as STM32) there is no need to do this pointer magic as
  // program memory may be read in a usual way So expression may be simplified
  return gfxFont->glyph + c;
}

inline uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont) {
  return gfxFont->bitmap;
}

//scale of the display window
float windowScale = 3;
int topBorder = 60;
int sideBorder = 60;

//this is the array holding what's shown at any given moment
//the displayBuffer in the display object is what gets changed and PUSHED to this
char screenPixels[128][64];

void printPixels(){
    for(uint8_t i = 0; i<64; i++){
        for(int j = 0; j<128; j++){
            cout<<to_string(screenPixels[j][i]);
        }
        cout<<endl;
    }
}

void reset_usb_boot(int a, int b){
    return;
}

float analogReadTemp(){
    return 0.0;
}

#define MSBFIRST 0
#define LSBFIRST 0

void turnOffLEDs();
void initSeq(int,int);
void setNormalMode();
void updateLEDs();

#include <sys/time.h>
#include <sys/resource.h>

long getMemoryUsage()
{
  struct rusage usage;
  if(0 == getrusage(RUSAGE_SELF, &usage))
    return usage.ru_maxrss; // bytes
  else
    return 0;
}


//dummy class for faking the rp2040 methods
class DummyRP2040{
    public:
        DummyRP2040();
        long getUsedHeap();
        void reboot();
        long getTotalHeap();
        long getFreeHeap();
        int f_cpu();
        int getCycleCount();
};
DummyRP2040::DummyRP2040(){
}
int DummyRP2040::f_cpu(){
    return 0;
}
long  DummyRP2040::getUsedHeap(){
    return getMemoryUsage();
}
long DummyRP2040::getTotalHeap(){
    return 2000;
}
long DummyRP2040::getFreeHeap(){
    return 700;
}
void DummyRP2040::reboot(){
    return;
}
int DummyRP2040::getCycleCount(){
    return millis();
}

DummyRP2040 rp2040;

class DummySerial{
    public:
        DummySerial(void);
        void print(string);
        void print(int);
        void println(string);
        void println(int);
        void flush();
        int read();
        string readString();
        void end();
        void begin(int);
        void write(int);
        void readBytes(uint8_t* a, uint8_t b);
        uint8_t available();
};
DummySerial::DummySerial(){
    
}
void DummySerial::print(string s){
    cout<<s;
    return;
}
void DummySerial::print(int a){
    cout<<a;
    return;
}
void DummySerial::println(string s){
    cout<<s;
    cout<<endl;
    return;
}
void DummySerial::println(int a){
    cout<<a;
    cout<<endl;
    return;
}
void DummySerial::flush(){
    return;
}
void DummySerial::end(){
    return;
}
void DummySerial::begin(int baud){
    return;
}
int DummySerial::read(){
    return 0;
}
void DummySerial::write(int a){
    cout<<a;
    return;
}
string DummySerial::readString(){
    return "hey, you're in headless mode";
}
uint8_t DummySerial::available(){
    return 0;
}
void DummySerial::readBytes(uint8_t* a, uint8_t b){
    return;
}

DummySerial Serial;

void bootscreen();

#include "HeadlessDisplay.h"

//emulating delay
void delay(unsigned long time){
    usleep(time*1000);
}

float analogRead(uint8_t pin){
    return 65535;
}

int digitalRead(int pin){
    return 0;
}
#define LOW 0
#define HIGH 1

void digitalWrite(uint8_t pin, bool value){
    return;
}
void analogWrite(uint8_t pin, uint16_t val){
    
}

//random
int random(int lower, int upper){
    if(lower>upper){
        int temp = lower;
        lower = upper;
        upper = temp;
    }
    else if(upper==lower){
        return 0;
    }
    return rand()%(upper-lower)+lower;
}

int random(int upper){
    return random(0,upper);
}

//converts degrees to radians
float radians(int deg){
    return deg*M_PI/180.0;
}

void sleep_ms(int num){
    delay(num);
}

Display display;

//key states that the GLUT callback sets
//these are then read by the input read functions in the main program logic
//and IMPORTANTLY get reset to '0' every time they're read
//joystick
int xKeyVal;//arrow keys
int yKeyVal;

//encoders
int encASTATE;//0, //k
int encAPRESS;//o
int encBSTATE;//p, ,
int encBPRESS;//l

//buttons
int newKeyVal;//1
int shiftKeyVal;//q
int selectKeyVal;//a
int deleteKeyVal;//z

int loopKeyVal;//x
int playKeyVal;//c
int copyKeyVal;//v
int menuKeyVal;//b

bool leds[8] = {false,false,false,false,false,false,false,false};
bool headlessStepButtons[8] = {false,false,false,false,false,false,false,false};

class DummyLowerBoard{
  public:
    //This enum taken from: 
     enum CAP1214_Register_Address{
      CAP1214_MAIN_CTRL        = 0x00,/**< Main Control.*/
            CAP1214_BTN_STATUS_1    = 0x03,
            CAP1214_BTN_STATUS_2    = 0x04,
            CAP1214_SLIDER_POS        = 0x06,/**< Slider position, Volumetric Data.*/
            CAP1214_VOLUM_STEP        = 0x09,
            CAP1214_DELTA_COUNT        = 0x10,/**< [0x10 -> 0x1D] Sensor Input Delta Count. Stores the delta count for CSx (0x10 = CS1, 0x1D = CS14)*/
            CAP1214_QUEUE_CTRL        = 0x1E,
            CAP1214_DATA_SENSITIV    = 0x1F,/**< Sensitivity Control Controls the sensitivity of the threshold and delta counts and data scaling of the base counts*/
            CAP1214_GLOBAL_CONFIG    = 0x20,
            CAP1214_SENSOR_ENABLE    = 0x21,/**< Sensor Input Enable Controls whether the capacitive touch sensor inputs are sampled*/
            CAP1214_BTN_CONFIG        = 0x22,
            CAP1214_GROUP_CFG_1        = 0x23,
            CAP1214_GROUP_CFG_2        = 0x24,
            CAP1214_FRCE_CALIB        = 0x26, //Recalibrates 0-7
            CAP1214_IRQ_ENABLE_1    = 0x27,
            CAP1214_IRQ_ENABLE_2    = 0x28,
            CAP1214_SENSOR_THRES    = 0x30,/**< [30h -> 37h] Sensor Input Threshold. Stores the delta count threshold to determine a touch for Capacitive Touch Sensor Input x (30h = 1, 37h = 8)*/
            CAP1214_GLOBAL_CONFIG_2    = 0x40,/**Controls whether or not sliders are active*/
      CAP1214_FRCE_CALIB_2  = 0x46, // Recalibrates 8-14
      CAP1214_LED_STATUS_1 = 0x60,
      CAP1214_LED_STATUS_2 = 0x61,
      CAP1214_LED_DIRECTION = 0x70,
      CAP1214_LED_OUTPUT_1 = 0x73,
      CAP1214_LED_OUTPUT_2 = 0x74
    };
    DummyLowerBoard(){}
    void writeByteToRegister(CAP1214_Register_Address reg, unsigned char dataByte){}
    unsigned char readRegister(CAP1214_Register_Address reg){
      return 0;
    }
    /*
    Sensitivity values are between 0 (most sensitive) and 111b (7)
    0 = 128x, 1 = 64x, 2 = 32x, 3 = 16x, 4 = 8x, 5 = 4x, 6 = 2x, 7 = 1x
    The data sensitivity byte is stored as (4 bits for sensitivity) (4 bits for the base shift)
    This method only updates sensitivity! Datasheets says most applications shouldn't need to update the base shift values
    */
    void setSensitivity(unsigned char val){
    }
    //Clears the INT bit that gets set whenever a button is pressed
    void clearInterruptBit(){}
    bool checkInterruptBitAndReset(){
      return true;
    }
    //Sets the LED pins to LED driver mode
    void setLEDsAsOutputs(){
    }
    void disableSliders(){
    }
    void recalibrate(){
    }
    //The Main Status Control reg bits are [X DEACT SLEEP DSLEEP X X PWR_LED INT]
    void activeMode(){
    }
    void sleepMode(){
    }
    void deepSleepMode(){
    }
    void inactiveMode(){
    }

    //Returns a 16-bit number, the first 13 bits contain the 13 button states (MSB->LSB)
    unsigned short int getButtonStatus(){
      return 0;
    }
    unsigned char getStateOfMainButtons(){
      unsigned char byteHolder = 0;
      for(uint8_t i = 0; i<8; i++){
        byteHolder = byteHolder | (headlessStepButtons[i]<<(i-8));
      }
      return byteHolder;
    }
    void reset(){
    }
    void writeLEDs(unsigned char status){
      for(uint8_t i = 0; i<8; i++){
        leds[i] = (status>>i)&0b00000001;
      }
    }
    void initialize(){}
};

DummyLowerBoard lowerBoard;

void shiftOut(int dataPin, int clockPin, int style, int data){
    for(uint8_t i = 0; i<8; i++){
        leds[i] = (data>>i)&0b00000001;
    }

    return;
}
unsigned char shiftIn(int dataPin, int clockPin, int style){
    return 0;
}

void loop();
void loop1();
void setup();
void setup1();

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

//for taking screenshots
string screenshotFolder = "/Users/alex/Desktop/Stepchild/sequencer_proj/screenshots_headless";
int numberOfScreenshots = 0;
void takeScreenshot(){
    bitmap_image image(128,64);
    for(int i = 0; i<128; i++){
        for(int j = 0; j<64; j++){
            if(screenPixels[i][j] == 1)
                image.set_pixel(i,j,255,255,255);
            else
                image.set_pixel(i,j,0,0,0);
        }
    }
    image.save_image(screenshotFolder+"/"+to_string(numberOfScreenshots)+".bmp");
    numberOfScreenshots++;
}

//int encoderA, encoderB;
static void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods)
{
    //new
        //'1'
        if(key == 49 && action == GLFW_PRESS)
            newKeyVal = 1;
        if(key == 49 && action == GLFW_RELEASE)
            newKeyVal = 0;
    //shift
        //'q'
        if(key == 81 && action == GLFW_PRESS)
            shiftKeyVal = 1;
        if(key == 81 && action == GLFW_RELEASE)
            shiftKeyVal = 0;
    //select
        //'a'
        if(key == 65 && action == GLFW_PRESS)
            selectKeyVal = 1;
        if(key == 65 && action == GLFW_RELEASE)
            selectKeyVal = 0;
    //delete
        //'z'
        if(key == 90 && action == GLFW_PRESS)
            deleteKeyVal = 1;
        if(key == 90 && action == GLFW_RELEASE)
            deleteKeyVal = 0;
    //loop
        //'x'
        if(key == 88 && action == GLFW_PRESS)
            loopKeyVal = 1;
        if(key == 88 && action == GLFW_RELEASE)
            loopKeyVal = 0;
    //play
        //'c
        if(key == 67 && action == GLFW_PRESS)
            playKeyVal = 1;
        if(key == 67 && action == GLFW_RELEASE)
            playKeyVal = 0;
    //copy
        //'v'
        if(key == 86 && action == GLFW_PRESS)
            copyKeyVal = 1;
        if(key == 86 && action == GLFW_RELEASE)
            copyKeyVal = 0;
    //menu
        //'b'
        if(key == 66 && action == GLFW_PRESS)
            menuKeyVal = 1;
        if(key == 66 && action == GLFW_RELEASE)
            menuKeyVal = 0;
    
    //encoders
        //0
        if(key == 48 && action == GLFW_PRESS)
            counterA++;
        //k
        if(key == 75 && action == GLFW_PRESS)
            counterA--;
        //o
        if(key == 79 && action == GLFW_PRESS)
            encAPRESS = 1;
        if(key == 79 && action == GLFW_RELEASE)
            encAPRESS = 0;
        //-
        if(key == '-' && action == GLFW_PRESS)
            counterB++;
        //l
        if(key == 76 && action == GLFW_PRESS)
            counterB--;
        //p
        if(key == 80 && action == GLFW_PRESS)
            encBPRESS = 1;
        if(key == 80 && action == GLFW_RELEASE)
            encBPRESS = 0;
    
    //arrowkeys
        if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
            xKeyVal = -1;
        }
        if(key == GLFW_KEY_RIGHT && action == GLFW_RELEASE){
            xKeyVal = 0;
        }
        if(key == GLFW_KEY_LEFT && action == GLFW_PRESS){
            xKeyVal = 1;
        }
        if(key == GLFW_KEY_LEFT && action == GLFW_RELEASE){
            xKeyVal = 0;
        }
    
        if(key == GLFW_KEY_UP && action == GLFW_PRESS){
            yKeyVal = -1;
        }
        if(key == GLFW_KEY_UP && action == GLFW_RELEASE){
            yKeyVal = 0;
        }
        if(key == GLFW_KEY_DOWN && action == GLFW_PRESS){
            yKeyVal = 1;
        }
        if(key == GLFW_KEY_DOWN && action == GLFW_RELEASE){
            yKeyVal = 0;
        }
    //stepbuttons (2-8)
    if(key >= 50 && key <= 57 && action == GLFW_PRESS)
        headlessStepButtons[key - 50] = true;
    if(key >= 50 && key <= 57 && action == GLFW_RELEASE)
        headlessStepButtons[key - 50] = false;

    //screenshot
        if(key == GLFW_KEY_ENTER && action == GLFW_PRESS)
            takeScreenshot();
}

bool openGLready = false;

GLFWwindow* window;

void window_size_callback(GLFWwindow* window, int width, int height){
    // vv these two lines avoid stretching when the window is resized
    glLoadIdentity();                           //reload projection matrix
    glOrtho(0.0,width,0.0,height,-1.0,1.0);     //reload orthographic projection dimensions
    
    if(width>(2*height)){
        topBorder = ((width/10)>60)?60:width/10;
        windowScale = (float)(height-2*topBorder)/64.0;
        sideBorder = (width-128*windowScale)/2;
    }
    else{
        sideBorder = ((height/10)>60)?60:height/10;
        windowScale = (float)(width-2*sideBorder)/128.0;
        topBorder = (height-64*windowScale)/2;
    }
}

GLFWwindow* initGlfw(){
    if(!glfwInit()){
        cout<<"Window couldn't be created!\n";
        while(true){
        }
    }
    int width, height;
    width = 128*windowScale+sideBorder*2;
    height = 64*windowScale+topBorder*2;
    
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(width, height, "childOS [Headless]", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    glClearColor(0.0, 0.0, 0.0, 0.0);         // black background
    glMatrixMode(GL_PROJECTION);                // setup viewing projection
    glLoadIdentity();                           // start with identity matrix
    glOrtho(0.0,width,0.0,height,-1.0,1.0);
    
    
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);
    
    glfwSetWindowSizeCallback(window,window_size_callback);
    
    openGLready = true;
    return window;
}

//custom circle drawing
void glCircle(int x1, int y1, int r, int numberOfVertices){
    float theta = 2.0*PI/float(numberOfVertices);
    glBegin(GL_POLYGON);
    for(int i = 0; i<numberOfVertices; i++){
        glVertex2f(r*cos(i*theta)+x1,r*sin(i*theta)+y1);
    }
    glEnd();
}

//checks for a window update, then draws pixels to the openGL window using the 'screenPixels' buffer
void displayWindow(void)
{
    //update the display if there's been an update
    if(display.displayUpdate && openGLready){
//        glClearColor( 0.0f, 0.0f, 0.2f, 0.0f ); //dark transparent blue
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear( GL_COLOR_BUFFER_BIT);
        for(int j = 0;j<64; j++){
            for(int i = 0; i<128; i++){
                //draw white pixelsg
                if(screenPixels[i][j] == 1){
//                    glColor3f(0.6, 0.6, 0.0); //dark yellow
//                    glColor3f(0.9,0.9,0.0); //bright yellow
                    glColor4f(1.0,1.0,1.0,1.0);
                }
                else{
//                    glColor4f( 0.0f, 0.0f, 0.2f, 0.8f); //dark transparent blue
                    glColor4f(0.0,0.0,0.0,1.0);
                }
                int x1 = i+1;
                int y1 = 63 - j;
                glBegin(GL_POLYGON);
                glVertex2f(sideBorder+x1*windowScale, topBorder+y1*windowScale);
                glVertex2f(sideBorder+x1*windowScale-1*windowScale, topBorder+y1*windowScale);
                glVertex2f(sideBorder+x1*windowScale-1*windowScale, topBorder+y1*windowScale+1*windowScale);
                glVertex2f(sideBorder+x1*windowScale, topBorder+y1*windowScale+1*windowScale);
                glEnd();
            }
        }
        //drawing leds
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        for(int i = 0; i<8; i++){
            if(leds[i]){
                glColor3f(1.0,0.0,0.0); //red
            }
            else{
                glColor3f(0.0,0.0,0.0); //dark
            }
            glCircle(w/8*i+w/16,10,10,10);
        }        
        glFlush();
        glfwSwapBuffers(window);
        display.displayUpdate = false;
    }
}

//copies vals into display pixels
void Display::display(void){
    //if you gotta close up
    if(glfwWindowShouldClose(window)){
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(0);
    }
    for(int column = 0;column<128;column++){
        for(int row = 0; row<64; row++){
            screenPixels[column][row] = displayBuffer[column][row];
        }
    }
    displayUpdate = true;
    displayWindow();
    delay(20);
}

#include "../../stepchild/stepchild.ino"



