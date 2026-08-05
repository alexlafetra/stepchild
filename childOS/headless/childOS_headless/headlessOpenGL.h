/*
 + -------------------
 | OpenGL code for creating, updating, and drawing the headless window
 + -------------------
 */

#pragma once

// WASM build
#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
    #include <GL/gl.h>
#else
// macOS build
    #define GL_SILENCE_DEPRECATION
    #include <OpenGL/gl.h>
#endif
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

//for writing bitmaps
#include "bitmap/bitmap_image.hpp"

//int encoderA, encoderB;
static void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods);

void error_callback(int error, const char* description);

void window_size_callback(GLFWwindow* window, int width, int height);

GLFWwindow* initGlfw();
void drawPixel(float x1, float y1, float w, float h);
//checks for a window update, then draws pixels to the openGL window using the 'screenPixels' buffer
void displayWindow(void);
