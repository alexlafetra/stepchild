/*
 + -------------------
 | OpenGL code for creating, updating, and drawing the headless window
 + -------------------
 */

#include "headlessOpenGL.h"

float windowWidth = 128.0;
float windowHeight = 64.0;

//this is the array holding what's shown at any given moment
//the displayBuffer in the display object is what gets changed and PUSHED to this
char screenPixels[128][64];
bool openGLready = false;
GLFWwindow* window;

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


#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
extern "C"{

EMSCRIPTEN_KEEPALIVE
void pressButtonFromJS(int which){
  switch(which){
    case 0:
      newKeyVal = 1;
      break;
    case 1:
      shiftKeyVal = 1;
      break;
    case 2:
      selectKeyVal = 1;
      break;
    case 3:
      deleteKeyVal = 1;
      break;
    case 4:
      loopKeyVal = 1;
      break;
    case 5:
      playKeyVal = 1;
      break;
    case 6:
      copyKeyVal = 1;
      break;
    case 7:
      menuKeyVal = 1;
      break;
    case 8:
      headlessCounterA++;
      break;
    case 9:
      headlessCounterA--;
      break;
    case 10:
      encAPRESS = 1;
      break;
    case 11:
      headlessCounterB++;
      break;
    case 12:
      headlessCounterB--;
      break;
    case 13:
      encBPRESS = 1;
      break;
    case 14:
      xKeyVal = -1;
      break;
    case 15:
      xKeyVal = 1;
      break;
    case 16:
      yKeyVal = -1;
      break;
    case 17:
      yKeyVal = 1;
      break;
  }
}

EMSCRIPTEN_KEEPALIVE
void unpressButtonFromJS(int which){
  switch(which){
    case 0:
      newKeyVal = 0;
      break;
    case 1:
      shiftKeyVal = 0;
      break;
    case 2:
      selectKeyVal = 0;
      break;
    case 3:
      deleteKeyVal = 0;
      break;
    case 4:
      loopKeyVal = 0;
      break;
    case 5:
      playKeyVal = 0;
      break;
    case 6:
      copyKeyVal = 0;
      break;
    case 7:
      menuKeyVal = 0;
      break;
    case 10:
      encAPRESS = 0;
      break;
    case 13:
      encBPRESS = 0;
      break;
    case 14:
      xKeyVal = 0;
      break;
    case 15:
      xKeyVal = 0;
      break;
    case 16:
      yKeyVal = 0;
      break;
    case 17:
      yKeyVal = 0;
      break;
  }
}
}

#endif

//int encoderA, encoderB;
static void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods){
  //new
  //'1'
  if(key == 49 && action == GLFW_PRESS){
    newKeyVal = 1;
  }
  if(key == 49 && action == GLFW_RELEASE){
    newKeyVal = 0;
  }
  //shift
  //'q'
  if(key == 81 && action == GLFW_PRESS){
    shiftKeyVal = 1;
  }
  if(key == 81 && action == GLFW_RELEASE)
    shiftKeyVal = 0;
  //select
  //'a'
  if(key == 65 && action == GLFW_PRESS){
    selectKeyVal = 1;
  }
  if(key == 65 && action == GLFW_RELEASE)
    selectKeyVal = 0;
  //delete
  //'z'
  if(key == 90 && action == GLFW_PRESS){
    deleteKeyVal = 1;
  }
  if(key == 90 && action == GLFW_RELEASE)
    deleteKeyVal = 0;
  //loop
  //'x'
  if(key == 88 && action == GLFW_PRESS){
    loopKeyVal = 1;
  }
  if(key == 88 && action == GLFW_RELEASE)
    loopKeyVal = 0;
  //play
  //'c
  if(key == 67 && action == GLFW_PRESS){
    playKeyVal = 1;
  }
  if(key == 67 && action == GLFW_RELEASE)
    playKeyVal = 0;
  //copy
  //'v'
  if(key == 86 && action == GLFW_PRESS){
    copyKeyVal = 1;
  }
  if(key == 86 && action == GLFW_RELEASE)
    copyKeyVal = 0;
  //menu
  //'b'
  if(key == 66 && action == GLFW_PRESS){
    menuKeyVal = 1;
  }
  if(key == 66 && action == GLFW_RELEASE)
    menuKeyVal = 0;
  
  //encoders
  //0
  if(key == 48 && action == GLFW_PRESS){
    headlessCounterA++;
  }
  //k
  if(key == 75 && action == GLFW_PRESS){
    headlessCounterA--;
  }
  //o
  if(key == 79 && action == GLFW_PRESS){
    encAPRESS = 1;
  }
  if(key == 79 && action == GLFW_RELEASE)
    encAPRESS = 0;
  //-
  if(key == '-' && action == GLFW_PRESS){
    headlessCounterB++;
  }
  //l
  if(key == 76 && action == GLFW_PRESS){
    headlessCounterB--;
  }
  //p
  if(key == 80 && action == GLFW_PRESS){
    encBPRESS = 1;
  }
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
}

void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

void window_size_callback(GLFWwindow* window, int width, int height){
}

GLuint pixelTexture;
// Full-screen quad: pos.xy, uv.xy
float quad[] = {
    -1.f, -1.f,  0.f, 1.f,
    1.f, -1.f,  1.f, 1.f,
    1.f,  1.f,  1.f, 0.f,
    -1.f,  1.f,  0.f, 0.f,
};
GLuint idx[] = { 0,1,2, 2,3,0 };

GLuint vbo, ebo, vao;

void initTexture(){
  glGenTextures(1, &pixelTexture);
  glBindTexture(GL_TEXTURE_2D, pixelTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 128, 64, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
  glEnableVertexAttribArray(1);
}

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(logLen, '\0');
        glGetShaderInfoLog(shader, logLen, nullptr, &log[0]);
        std::cerr << "Shader compile error ("
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << "):\n" << log << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createProgram(const char* vsSource, const char* fsSource) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vsSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSource);
    if (!vs || !fs) return 0;

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(logLen, '\0');
        glGetProgramInfoLog(program, logLen, nullptr, &log[0]);
        std::cerr << "Program link error:\n" << log << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    // Shaders are linked into the program now; the standalone objects aren't needed anymore
    glDetachShader(program, vs);
    glDetachShader(program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

const char* vertexSrc = R"(#version 300 es
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
out vec2 vUV;

void main() {
    vUV = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* fragmentSrc = R"(#version 300 es
precision mediump float;
in vec2 vUV;
out vec4 fragColor;

uniform sampler2D uBuffer;

void main() {
    vec2 texUV = vec2(vUV.y, vUV.x); // transpose to match memory layout
    float v = texture(uBuffer, texUV).r;
    float lit = min(v * 255.0, 1.0);
    fragColor = vec4(vec3(lit),1.0);
}
)";

GLuint shaderProgram;

void initShaders() {
    shaderProgram = createProgram(vertexSrc, fragmentSrc);
    if (!shaderProgram) {
        std::cerr << "Failed to create shader program" << std::endl;
        return;
    }

    // Bind uBuffer to texture unit 0, once, since it never changes
    glUseProgram(shaderProgram);
    GLint loc = glGetUniformLocation(shaderProgram, "uBuffer");
    glUniform1i(loc, 0);
    glUseProgram(0);
}

void renderPixels(){
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pixelTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 64, 128, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 128, GL_RED, GL_UNSIGNED_BYTE, screenPixels);
  glUseProgram(shaderProgram);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

GLFWwindow* initGlfw(){
  if(!glfwInit()){
    std::cout<<"Window couldn't be created!\n";
    while(true){
    }
  }
  
  // glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "childOS [Headless]", NULL, NULL);
  glfwMakeContextCurrent(window);

  initTexture();
  initShaders();
  
  // glClearColor(0.0, 0.0, 0.0, 0.0);         // black background
  
  glfwSetErrorCallback(error_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSwapInterval(1);
  
  // glfwSetWindowSizeCallback(window,window_size_callback);
  
  openGLready = true;
  return window;
}
//checks for a window update, then draws pixels to the openGL window using the 'screenPixels' buffer
void displayWindow(void)
{
  //update the display if there's been an update
  if(!openGLready)
    return;
//  glClearColor(1.0f,1.0f,1.0f,1.0f);//White
  // glClearColor(0.0f,0.0f,1.0f,1.0f);//blue
//  glClearColor(0.0f,0.0f,1.0f,0.0f);//transp blue
//  glClearColor(0.0f,0.0f,0.0f,0.0f);//transp
  glClearColor(0.0f,0.0f,0.0f,1.0f);//black
  glClear( GL_COLOR_BUFFER_BIT);
  renderPixels();
  glFlush();
  glfwSwapBuffers(window);
}