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

#define DISPLAY_UPRIGHT 2
#define DISPLAY_UPSIDEDOWN 0
#define DISPLAY_SIDEWAYS_R 3
#define DISPLAY_SIDEWAYS_L 1

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

int headlessCounterA = 0, headlessCounterB = 0;

bool core0ready;
bool playing = false;
bool recording = false;

#include "headlessMIDI.h"
#include "headlessFileSystem.h"

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
        unsigned long getCycleCount();
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
unsigned long DummyRP2040::getCycleCount(){
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

#include "headlessDisplay.h"

//emulating delay
void delay(unsigned int time){
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

bool leds[16] = {false,false,false,false,false,false,false,false,
                 false,false,false,false,false,false,false,false};
bool headlessStepButtons[16] = {false,false,false,false,false,false,false,false,
                                false,false,false,false,false,false,false,false};

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
            headlessCounterA++;
        //k
        if(key == 75 && action == GLFW_PRESS)
            headlessCounterA--;
        //o
        if(key == 79 && action == GLFW_PRESS)
            encAPRESS = 1;
        if(key == 79 && action == GLFW_RELEASE)
            encAPRESS = 0;
        //-
        if(key == '-' && action == GLFW_PRESS)
            headlessCounterB++;
        //l
        if(key == 76 && action == GLFW_PRESS)
            headlessCounterB--;
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
void glFillCircle(int x1, int y1, int r, int numberOfVertices){
    float theta = 2.0*PI/float(numberOfVertices);
    glBegin(GL_POLYGON);
    for(int i = 0; i<numberOfVertices; i++){
        glVertex2f(r*cos(i*theta)+x1,r*sin(i*theta)+y1);
    }
    glEnd();
}
//custom circle drawing
void glDrawCircle(int x1, int y1, int r, int numberOfVertices){
    glLineWidth(5);//set stroke width
    float theta = 2.0*PI/float(numberOfVertices);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i<numberOfVertices; i++){
        glVertex2f(r*cos(i*theta)+x1,r*sin(i*theta)+y1);
    }
    glEnd();
}

void glDrawRadian(int x, int y, int r, float angle){
    float x2 = r*cos(angle)+x;
    float y2 = r*sin(angle)+y;
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,y);
    glVertex2f(x2,y2);
    glEnd();
}

void glFillCircleSegment(float cx, float cy, float r, int startAngle, int endAngle) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = startAngle; i <= endAngle; i++) {
        float theta = i * 3.1415926f / 180.0f;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void glFillRoundRect(float x, float y, float width, float height, float radius) {
    if (radius > width / 2.0f) radius = width / 2.0f;
    if (radius > height / 2.0f) radius = height / 2.0f;

    float right = x + width;
    float top = y + height;
    float left = x;
    float bottom = y;

    // Center rectangle
    glBegin(GL_QUADS);
    glVertex2f(left + radius, bottom);
    glVertex2f(right - radius, bottom);
    glVertex2f(right - radius, top);
    glVertex2f(left + radius, top);
    glEnd();

    // Left rectangle
    glBegin(GL_QUADS);
    glVertex2f(left, bottom + radius);
    glVertex2f(left + radius, bottom + radius);
    glVertex2f(left + radius, top - radius);
    glVertex2f(left, top - radius);
    glEnd();

    // Right rectangle
    glBegin(GL_QUADS);
    glVertex2f(right - radius, bottom + radius);
    glVertex2f(right, bottom + radius);
    glVertex2f(right, top - radius);
    glVertex2f(right - radius, top - radius);
    glEnd();

    // Bottom rectangle
    glBegin(GL_QUADS);
    glVertex2f(left + radius, bottom);
    glVertex2f(right - radius, bottom);
    glVertex2f(right - radius, bottom + radius);
    glVertex2f(left + radius, bottom + radius);
    glEnd();

    // Top rectangle
    glBegin(GL_QUADS);
    glVertex2f(left + radius, top - radius);
    glVertex2f(right - radius, top - radius);
    glVertex2f(right - radius, top);
    glVertex2f(left + radius, top);
    glEnd();

    // Bottom-left corner
    glFillCircleSegment(left + radius, bottom + radius, radius, 180, 270);

    // Bottom-right corner
    glFillCircleSegment(right - radius, bottom + radius, radius, 270, 360);

    // Top-right corner
    glFillCircleSegment(right - radius, top - radius, radius, 0, 90);

    // Top-left corner
    glFillCircleSegment(left + radius, top - radius, radius, 90, 180);
}

//checks for a window update, then draws pixels to the openGL window using the 'screenPixels' buffer
void displayWindow(void)
{
    //update the display if there's been an update
    if(display.displayUpdate && openGLready){
        
        //filling bg with white
        glClearColor(1.0f,1.0f,1.0f,1.0f);//white
//        glClearColor(0.0f,0.0f,0.0f,0.0f);//clear
        glClear( GL_COLOR_BUFFER_BIT);
        
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        
        //drawing rounded rect to emulate screen border
        glColor3f(0.0,0.0,0.0);
        glFillRoundRect(sideBorder-10,topBorder-10, w-2*sideBorder+20, h-2*topBorder+20, 20);
        
        //drawing buttons
        const int buttonX = (sideBorder-10)/2;
        const int buttonY = topBorder - 25;
        const int buttonGap = 30;
        glColor3f(1.0,0.0,0.0); //delete
        deleteKeyVal?glDrawCircle(buttonX,buttonY+buttonGap,10,20):glFillCircle(buttonX,buttonY+buttonGap,10,20);
        glColor3f(1.0,1.0,0.0); //select
        selectKeyVal?glDrawCircle(buttonX,buttonY+2*buttonGap,10,20):glFillCircle(buttonX,buttonY+2*buttonGap,10,20);
        glColor3f(0.9,0.9,0.9); //shift
        shiftKeyVal?glDrawCircle(buttonX,buttonY+3*buttonGap,10,20):glFillCircle(buttonX,buttonY+3*buttonGap,10,20);
        glColor3f(0.0,1.0,0.0); //new
        newKeyVal?glDrawCircle(buttonX,buttonY+4*buttonGap,10,20):glFillCircle(buttonX,buttonY+4*buttonGap,10,20);
        
        glColor3f(0.0,0.0,1.0); //Loop
        loopKeyVal?glDrawCircle(buttonX+buttonGap,buttonY,10,20):glFillCircle(buttonX+buttonGap,buttonY,10,20);
        glColor3f(0.9,0.9,0.9); //play
        playKeyVal?glDrawCircle(buttonX+2*buttonGap,buttonY,10,20):glFillCircle(buttonX+2*buttonGap,buttonY,10,20);
        glColor3f(0.6,0.6,0.6); //copy
        copyKeyVal?glDrawCircle(buttonX+3*buttonGap,buttonY,10,20):glFillCircle(buttonX+3*buttonGap,buttonY,10,20);
        glColor3f(0.0,0.0,0.0); //menu
        menuKeyVal?glDrawCircle(buttonX+4*buttonGap,buttonY,10,20):glFillCircle(buttonX+4*buttonGap,buttonY,10,20);
        
        //drawing leds
        const int ledStartX = (buttonX+5*buttonGap);
        const int ledLength = w/3;
        for(int i = 0; i<16; i++){
            if(leds[i]){
                glColor3f(1.0,0.0,0.0); //red
            }
            else{
                glColor3f(0.0,0.0,0.0); //dark
            }
            glFillCircle((ledLength)/16*i+ledStartX,(i%2)?buttonY-20:buttonY,10,20);
        }
        
        //joystick
        int yOffset = 0;
        if(yKeyVal>0)
            yOffset = -10;
        else if(yKeyVal<0)
            yOffset = 10;
        int xOffset = 0;
        if(xKeyVal>0)
            xOffset = -10;
        else if(xKeyVal<0)
            xOffset = 10;
        glColor3f(0.0,0.0,0.0);
        glDrawCircle(ledStartX + ledLength + 20, buttonY-10, 25, 20);
        glFillCircle(ledStartX + ledLength + 20 + xOffset, buttonY-10 + yOffset, 20, 20);
        
        //drawing encoders
        const int encoderXStart = w - sideBorder/2+10;
        const int encoderYStart = h - topBorder - 10;
        float aAngle = float(encASTATE)/16.0 * 2.0*PI;
        float bAngle = float(encBSTATE)/16.0 * 2.0*PI;
        glColor3f(0.0,0.0,0.0); //dark
        glDrawCircle(encoderXStart, encoderYStart, 15, 20);
        if(!encAPRESS)
            glDrawRadian(encoderXStart,encoderYStart,15,aAngle);
        glDrawCircle(encoderXStart, encoderYStart - 40, 15, 20);
        if(!encBPRESS)
            glDrawRadian(encoderXStart,encoderYStart-40,15,bAngle);


        
        //drawing pixels
        for(int j = 0;j<64; j++){
            for(int i = 0; i<128; i++){
                //draw white pixels
                if(screenPixels[i][j] == 1){
                    glColor4f(1.0,1.0,1.0,1.0);
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
    //pushing the display buffer into screenpixels
    for(int column = 0;column<128;column++){
        for(int row = 0; row<64; row++){
            screenPixels[column][row] = displayBuffer[column][row];
        }
    }
    displayUpdate = true;
    displayWindow();
    delay(20);
}

#include "../../stepchild/include/stepchild.ino"
