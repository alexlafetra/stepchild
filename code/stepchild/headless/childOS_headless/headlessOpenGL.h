/*
 + -------------------
 | OpenGL code for creating, updating, and drawing the headless window
 + -------------------
 */
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//for writing bitmaps
#include "bitmap/bitmap_image.hpp"
//scale of the display window
float windowScale = 3;
int topBorder = 60;
int sideBorder = 60;
//this is the array holding what's shown at any given moment
//the displayBuffer in the display object is what gets changed and PUSHED to this
char screenPixels[128][64];
bool openGLready = false;
GLFWwindow* window;

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
int headlessCounterA = 0, headlessCounterB = 0;

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

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

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

//custom circle filling
void glFillCircle(int x1, int y1, int r, int numberOfVertices){
    float theta = 2.0*M_PI/float(numberOfVertices);
    glBegin(GL_POLYGON);
    for(int i = 0; i<numberOfVertices; i++){
        glVertex2f(r*cos(i*theta)+x1,r*sin(i*theta)+y1);
    }
    glEnd();
}
//custom circle drawing
void glDrawCircle(int x1, int y1, int r, int numberOfVertices){
    glLineWidth(5);//set stroke width
    float theta = 2.0*M_PI/float(numberOfVertices);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i<numberOfVertices; i++){
        glVertex2f(r*cos(i*theta)+x1,r*sin(i*theta)+y1);
    }
    glEnd();
}
//drawing radians
void glDrawRadian(int x, int y, int r, float angle){
    float x2 = r*cos(angle)+x;
    float y2 = r*sin(angle)+y;
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,y);
    glVertex2f(x2,y2);
    glEnd();
}
//filling an arc, for round rects
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
//filling round rects
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
    if(openGLready){
        glClearColor(1.0f,1.0f,1.0f,1.0f);//White
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
            glFillCircle((ledLength)/16*i+ledStartX,(i%2)?buttonY-20:buttonY,3,6);
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
        float aAngle = float(encASTATE)/16.0 * 2.0*M_PI;
        float bAngle = float(encBSTATE)/16.0 * 2.0*M_PI;
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
    }
}
