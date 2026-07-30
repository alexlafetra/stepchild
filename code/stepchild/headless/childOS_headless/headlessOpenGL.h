/*
 + -------------------
 | OpenGL code for creating, updating, and drawing the headless window
 + -------------------
 */

#pragma once

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

//for writing bitmaps
#include "bitmap/bitmap_image.hpp"

void takeScreenshot();

//int encoderA, encoderB;
static void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods);

void error_callback(int error, const char* description);

void window_size_callback(GLFWwindow* window, int width, int height);

GLFWwindow* initGlfw();

//custom circle filling
void glFillCircle(int x1, int y1, int r, int numberOfVertices);
//custom circle drawing
void glDrawCircle(int x1, int y1, int r, int numberOfVertices);
//drawing radians
void glDrawRadian(int x, int y, int r, float angle);
//filling an arc, for round rects
void glFillCircleSegment(float cx, float cy, float r, int startAngle, int endAngle);
//filling round rects
void glFillRoundRect(float x, float y, float width, float height, float radius);

void drawButtons(int w, int h);

void drawPixel(int x1, int y1, int w, int h);

//checks for a window update, then draws pixels to the openGL window using the 'screenPixels' buffer
void displayWindow(void);

void loadImageTexture(const char* path);
void drawImage(GLuint& imageTexture, float x, float y, float w, float h, float r);
void launchWindow();
