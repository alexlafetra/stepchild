//  childOS_headless
//
//  Created by Alex LaFetra Thompson on 2/8/23.
//
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
#include "../../stepchild/pins.h"

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

string stringify(int a){
    return to_string(a);
}
string stringify(string s){
    return s;
}
int toInt(string s){
    if(s == "")
        return 0;
    else
        return stoi(s);
}

int counterA, counterB;

bool core0ready;
unsigned long lastTime = 0;
bool playing = false;
bool recording = false;

#include "Midi.h"
#include "FileSystem.h"

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

bool leds[8] = {false,false,false,false,false,false,false,false};

void shiftOut(int dataPin, int clockPin, int style, int data){
    for(uint8_t i = 0; i<8; i++){
        leds[i] = (data>>i)&0b00000001;
    }

    return;
}
unsigned char shiftIn(int dataPin, int clockPin, int style){
    return 0;
}

void turnOffLEDs();
void initSeq(int,int);
void setNormalMode();
void updateLEDs();


//dummy class for faking the rp2040 methods
class DummyRP2040{
    public:
        DummyRP2040();
        int getUsedHeap();
        void reboot();
        int getTotalHeap();
        int getFreeHeap();
        int f_cpu();
        int getCycleCount();
};
DummyRP2040::DummyRP2040(){
}
int DummyRP2040::f_cpu(){
    return 0;
}
int DummyRP2040::getUsedHeap(){
    return 1000;
}
int DummyRP2040::getTotalHeap(){
    return 2000;
}
int DummyRP2040::getFreeHeap(){
    return 1000;
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

DummySerial Serial;

void bootscreen();

#include "Display.h"

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

void loop();
void loop1();
void setup();
void setup1();

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

//for taking screenshots
string screenshotFolder = "/Users/alex/Desktop/sequencer_proj/screenshots_headless";
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
//        glClearColor( 0.0f, 0.0f, 0.2f, 1.0f ); //dark transparent blue
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



