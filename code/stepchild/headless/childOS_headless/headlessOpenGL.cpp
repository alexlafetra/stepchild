/*
 + -------------------
 | OpenGL code for creating, updating, and drawing the headless window
 + -------------------
 */

#include "headlessOpenGL.h"
//library for loading overlay image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Texture{
  GLuint tex;
  int width;
  int height;
  float x;
  float y;
  float rotation = 0;
  Texture(float xOff, float yOff){
    x = xOff;
    y = yOff;
  }
  Texture(GLuint t,int w, int h, float xOff, float yOff){
    tex = t;
    width = w;
    height = h;
    x = xOff;
    y = yOff;
  }
};

float windowWidth = 800.0;
float windowHeight = 800.0;

Texture uiOverlayTexture(0,0);
Texture stepchildTexture(0,0);
Texture encoderTextureA(0.722*windowWidth,0.578*windowHeight);
Texture encoderTextureB(0.722*windowWidth,0.425*windowHeight);
Texture joystickTexture(0.6625*windowWidth,0.3*windowHeight);

float joystickMovement = 10;

//this is the array holding what's shown at any given moment
//the displayBuffer in the display object is what gets changed and PUSHED to this
char screenPixels[128][64];
bool openGLready = false;
GLFWwindow* window;

//for taking screenshots
std::string screenshotFolder = "/Users/alex/Desktop/ongoing/Stepchild/sequencer_proj/screenshots_headless";
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
  image.save_image(screenshotFolder+"/"+std::to_string(numberOfScreenshots)+".bmp");
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

enum ButtonCode:int{
  NEW_BUTTON,
  SHIFT_BUTTON,
  SELECT_BUTTON,
  DELETE_BUTTON,
  LOOP_BUTTON,
  PLAY_BUTTON,
  COPY_BUTTON,
  MENU_BUTTON,
  A_PRESS,
  A_UP,
  A_DOWN,
  B_PRESS,
  B_UP,
  B_DOWN,
  JOYSTICK_UP,
  JOYSTICK_DOWN,
  JOYSTICK_LEFT,
  JOYSTICK_RIGHT,
  NONE = -1
};

struct ButtonDisplay{
  std::vector<ButtonCode> recents;
  int listLength = 4;
  ButtonDisplay(){
  
  }
  void push(ButtonCode b){
    //don't double add it if it's the same
    if(recents.size())
    	if(recents[recents.size()-1] == b)
      	return;
    if(recents.size() >= listLength){
      std::vector<ButtonCode> temp;
      for(int i = 1; i<recents.size(); i++){
        temp.push_back(recents[i]);
      }
      temp.push_back(b);
      recents.swap(temp);
    }
    else{
      recents.push_back(b);
    }
  }
};

ButtonDisplay recentButton;

//buttons
Texture button_new(0,0);
Texture button_shift(0,0);
Texture button_select(0,0);
Texture button_delete(0,0);
Texture button_loop(0,0);
Texture button_play(0,0);
Texture button_copy(0,0);
Texture button_menu(0,0);
Texture button_encoder_a_up(0,0);
Texture button_encoder_a_down(0,0);
Texture button_encoder_a_click(0,0);
Texture button_encoder_b_up(0,0);
Texture button_encoder_b_down(0,0);
Texture button_encoder_b_click(0,0);
Texture button_up(0,0);
Texture button_down(0,0);
Texture button_left(0,0);
Texture button_right(0,0);

Texture* button_images[] = {
  &button_new,
  &button_shift,
  &button_select,
  &button_delete,
  &button_loop,
  &button_play,
  &button_copy,
  &button_menu,
  &button_encoder_a_click,
  &button_encoder_a_up,
  &button_encoder_a_down,
  &button_encoder_b_click,
  &button_encoder_b_up,
  &button_encoder_b_down,
  &button_up,
  &button_down,
  &button_left,
  &button_right
};

bool leds[16] = {false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false};
bool headlessStepButtons[16] = {false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false};

//int encoderA, encoderB;
static void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods)
{
  //new
  //'1'
  if(key == 49 && action == GLFW_PRESS){
    recentButton.push(NEW_BUTTON);
    newKeyVal = 1;
  }
  if(key == 49 && action == GLFW_RELEASE){
    newKeyVal = 0;
  }
  //shift
  //'q'
  if(key == 81 && action == GLFW_PRESS){
    recentButton.push(SHIFT_BUTTON);
    shiftKeyVal = 1;
  }
  if(key == 81 && action == GLFW_RELEASE)
    shiftKeyVal = 0;
  //select
  //'a'
  if(key == 65 && action == GLFW_PRESS){
    recentButton.push(SELECT_BUTTON);
    selectKeyVal = 1;
  }
  if(key == 65 && action == GLFW_RELEASE)
    selectKeyVal = 0;
  //delete
  //'z'
  if(key == 90 && action == GLFW_PRESS){
    recentButton.push(DELETE_BUTTON);
    deleteKeyVal = 1;
  }
  if(key == 90 && action == GLFW_RELEASE)
    deleteKeyVal = 0;
  //loop
  //'x'
  if(key == 88 && action == GLFW_PRESS){
    recentButton.push(LOOP_BUTTON);
    loopKeyVal = 1;
  }
  if(key == 88 && action == GLFW_RELEASE)
    loopKeyVal = 0;
  //play
  //'c
  if(key == 67 && action == GLFW_PRESS){
    recentButton.push(PLAY_BUTTON);
    playKeyVal = 1;
  }
  if(key == 67 && action == GLFW_RELEASE)
    playKeyVal = 0;
  //copy
  //'v'
  if(key == 86 && action == GLFW_PRESS){
    recentButton.push(COPY_BUTTON);
    copyKeyVal = 1;
  }
  if(key == 86 && action == GLFW_RELEASE)
    copyKeyVal = 0;
  //menu
  //'b'
  if(key == 66 && action == GLFW_PRESS){
    recentButton.push(MENU_BUTTON);
    menuKeyVal = 1;
  }
  if(key == 66 && action == GLFW_RELEASE)
    menuKeyVal = 0;
  
  //encoders
  //0
  if(key == 48 && action == GLFW_PRESS){
    recentButton.push(A_UP);
    headlessCounterA++;
    encoderTextureA.rotation += 360/16;
  }
  //k
  if(key == 75 && action == GLFW_PRESS){
    recentButton.push(A_DOWN);
    headlessCounterA--;
    encoderTextureA.rotation -= 360/16;
  }
  //o
  if(key == 79 && action == GLFW_PRESS){
    recentButton.push(A_PRESS);
    encAPRESS = 1;
  }
  if(key == 79 && action == GLFW_RELEASE)
    encAPRESS = 0;
  //-
  if(key == '-' && action == GLFW_PRESS){
    recentButton.push(B_UP);
    headlessCounterB++;
    encoderTextureB.rotation += 360/16;
  }
  //l
  if(key == 76 && action == GLFW_PRESS){
    recentButton.push(B_DOWN);
    headlessCounterB--;
    encoderTextureB.rotation -= 360/16;
  }
  //p
  if(key == 80 && action == GLFW_PRESS){
    recentButton.push(B_PRESS);
    encBPRESS = 1;
  }
  if(key == 80 && action == GLFW_RELEASE)
    encBPRESS = 0;
  
  //arrowkeys
  if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
    recentButton.push(JOYSTICK_LEFT);
    xKeyVal = -1;
  }
  if(key == GLFW_KEY_RIGHT && action == GLFW_RELEASE){
    xKeyVal = 0;
  }
  if(key == GLFW_KEY_LEFT && action == GLFW_PRESS){
    recentButton.push(JOYSTICK_RIGHT);
    xKeyVal = 1;
  }
  if(key == GLFW_KEY_LEFT && action == GLFW_RELEASE){
    xKeyVal = 0;
  }
  
  if(key == GLFW_KEY_UP && action == GLFW_PRESS){
    recentButton.push(JOYSTICK_UP);
    yKeyVal = -1;
  }
  if(key == GLFW_KEY_UP && action == GLFW_RELEASE){
    yKeyVal = 0;
  }
  if(key == GLFW_KEY_DOWN && action == GLFW_PRESS){
    recentButton.push(JOYSTICK_DOWN);
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
}

GLFWwindow* initGlfw(){
  if(!glfwInit()){
    std::cout<<"Window couldn't be created!\n";
    while(true){
    }
  }
  
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
  GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "childOS [Headless]", NULL, NULL);
  glfwMakeContextCurrent(window);
  
  glClearColor(0.0, 0.0, 0.0, 0.0);         // black background
  glMatrixMode(GL_PROJECTION);                // setup viewing projection
  glLoadIdentity();                           // start with identity matrix
  glOrtho(0.0,windowWidth,0.0,windowHeight,-1.0,1.0);
  
  
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

void glFillRect(float x, float y, float width, float height){
  // Top rectangle
  glBegin(GL_QUADS);
  glVertex2f(x,y);
  glVertex2f(x+width,y);
  glVertex2f(x+width,y+height);
  glVertex2f(x,y+height);
  glEnd();
}

void drawPixel(float x1, float y1, float w, float h){
  glColor4f(1.0,1.0,1.0,1.0);
  glBegin(GL_POLYGON);
  glVertex2f(x1, y1);
  glVertex2f(x1-w, y1);
  glVertex2f(x1-w, y1+h);
  glVertex2f(x1, y1+h);
  glEnd();
}

void drawButtons(){
  float buttonX = 10;
  float buttonY = 420;
  float buttonScale = 4.0;
  float buttonGap = 30;
  for(int i = 0; i<recentButton.recents.size(); i++){
    if(recentButton.recents[i] == NONE){
      return;
    }
    else{
      Texture* img = button_images[recentButton.recents[i]];
      img->x = buttonX;
      img->y = buttonY - i*buttonGap;
      drawImage(*img,buttonScale);
    }
  }
}

//checks for a window update, then draws pixels to the openGL window using the 'screenPixels' buffer
void displayWindow(void)
{
  //update the display if there's been an update
  if(!openGLready)
    return;
//  glClearColor(1.0f,1.0f,1.0f,1.0f);//White
  glClearColor(0.0f,0.0f,1.0f,1.0f);//blue
//  glClearColor(0.0f,0.0f,1.0f,0.0f);//transp blue
//  glClearColor(0.0f,0.0f,0.0f,0.0f);//transp
//  glClearColor(0.0f,0.0f,0.0f,1.0f);//black
  glClear( GL_COLOR_BUFFER_BIT);
  
  int w, h;
  glfwGetWindowSize(window, &w, &h);
  
  //scale of the display window
  float pixelWindowScale = 1.6;
  float pixelScreenOffsetX = 0.36667*windowWidth;
  float pixelScreenOffsetY = 0.4861111*windowHeight;
  
  //skirt around screen pixels, but beneath stepchild case
  int numberOfPixelsIncluded = 0;
  float gapX = 10;
  float gapY = 10;
  glColor4f(0.0f,0.0f,0.0f,1.0f);
  glFillRect(pixelScreenOffsetX-gapX,pixelScreenOffsetY-gapY,128*pixelWindowScale+2*gapX,64*pixelWindowScale+2*gapY);
  float imgScale = 0.002375*windowWidth;

  //stepchild case
  stepchildTexture.x = 0.15972*windowWidth;
  stepchildTexture.y = 0.14583*windowHeight;
  drawImage(stepchildTexture,imgScale);
  //controls
  drawImage(encoderTextureA,imgScale);
  drawImage(encoderTextureB,imgScale);
  float texOffsetX = (xKeyVal == -1)?(joystickMovement):(xKeyVal == 1?-joystickMovement:0);
  float texOffsetY = (yKeyVal == -1)?(joystickMovement):(yKeyVal == 1?-joystickMovement:0);
  joystickTexture.x = 0.6625*windowWidth + texOffsetX;
  joystickTexture.y = 0.3*windowHeight + texOffsetY;
  drawImage(joystickTexture,imgScale);
  drawImage(uiOverlayTexture,1357.0/windowWidth);
  drawButtons();
  //drawing pixels
  for(int j = 0;j<64; j++){
    for(int i = 0; i<128; i++){
      //draw white pixels
      if(screenPixels[i][j] == 1){
        numberOfPixelsIncluded++;
      }
      else{
        if(numberOfPixelsIncluded){
          float x1 = i;
          float y1 = 63 - j;
          float w = numberOfPixelsIncluded;
          drawPixel(pixelWindowScale*x1+pixelScreenOffsetX,pixelWindowScale*y1+pixelScreenOffsetY,pixelWindowScale*w, pixelWindowScale);
        }
        numberOfPixelsIncluded = 0;
      }
    }
    if(numberOfPixelsIncluded){
      drawPixel(pixelWindowScale*128+pixelScreenOffsetX,pixelWindowScale*(63-j)+pixelScreenOffsetY,pixelWindowScale*numberOfPixelsIncluded, pixelWindowScale);
    }
    numberOfPixelsIncluded = 0;
  }
  glFlush();
  glfwSwapBuffers(window);
}

void loadImageTexture(Texture& t, const char* path){

  stbi_set_flip_vertically_on_load(true);
  int channels;
  unsigned char* data = stbi_load(path, &t.width, &t.height, &channels, 0);
  if (!data) {
    std::cout << "Failed to load image: " << path << std::endl;
    return;
  }
  
  GLenum format = (channels == 4) ? GL_RGBA : (channels == 3) ? GL_RGB : GL_LUMINANCE;
  
  glGenTextures(1, &t.tex);
  glBindTexture(GL_TEXTURE_2D, t.tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, format, t.width, t.height, 0,
               format, GL_UNSIGNED_BYTE, data);
  
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, t.tex);
}
void drawImage(Texture &t, float scale){
  
  glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, t.tex);
  
  //allow alpha blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glColor4f(1.0, 1.0, 1.0, 1.0);
  
//  push matrix for rotating image
  glPushMatrix();
  
  float imageCenterX = t.x + t.width / 2.0 / scale;
  float imageCenterY = t.y + t.width / 2.0 / scale;
  
  glTranslatef(imageCenterX,imageCenterY,0.0);
  glRotatef(t.rotation,0.0,0.0,1.0);
  
  float halfH = t.height/2.0/scale;
  float halfW = t.width/2.0/scale;
  
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f); glVertex2f(-halfW,-halfH);
  glTexCoord2f(1.0f, 0.0f); glVertex2f(halfW,-halfH);
  glTexCoord2f(1.0f, 1.0f); glVertex2f(halfW,halfH);
  glTexCoord2f(0.0f, 1.0f); glVertex2f(-halfW,halfH);
  glEnd();
  
  glPopMatrix();
  
  glDisable(GL_BLEND);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
}

void launchWindow(){
  window = initGlfw();
  loadImageTexture(stepchildTexture, "hardware_overlay_transparent.png");
//  loadImageTexture(stepchildTexture,"hardware_overlay.png");
  loadImageTexture(encoderTextureA,"encoder.png");
  loadImageTexture(encoderTextureB,"encoder.png");
  loadImageTexture(joystickTexture,"joystick.png");
  loadImageTexture(uiOverlayTexture,"ui_overlay.png");
  
  loadImageTexture(button_new, "buttons/new.png");
  loadImageTexture(button_shift, "buttons/shift.png");
  loadImageTexture(button_select, "buttons/select.png");
  loadImageTexture(button_delete, "buttons/delete.png");
  loadImageTexture(button_loop, "buttons/loop.png");
  loadImageTexture(button_play, "buttons/play.png");
  loadImageTexture(button_copy, "buttons/copy.png");
  loadImageTexture(button_menu, "buttons/menu.png");
  loadImageTexture(button_up, "buttons/up.png");
  loadImageTexture(button_down, "buttons/down.png");
  loadImageTexture(button_left, "buttons/left.png");
  loadImageTexture(button_right, "buttons/right.png");
  loadImageTexture(button_encoder_a_up, "buttons/encoder_A_up.png");
  loadImageTexture(button_encoder_a_down, "buttons/encoder_A_down.png");
  loadImageTexture(button_encoder_a_click, "buttons/encoder_A_press.png");
  loadImageTexture(button_encoder_b_up, "buttons/encoder_B_up.png");
  loadImageTexture(button_encoder_b_down, "buttons/encoder_B_down.png");
  loadImageTexture(button_encoder_b_click, "buttons/encoder_B_press.png");

}
