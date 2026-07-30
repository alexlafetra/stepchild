#pragma once


#include "headlessOpenGL.h"

//fonts
#include "Fonts/gfxfont.h"
#include "Fonts/ASCII_font.h"
#include "Fonts/FreeSerifItalic9pt7b.h"
#include "Fonts/FreeSerifItalic12pt7b.h"
#include "Fonts/FreeSerifItalic24pt7b.h"

inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c);
inline uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont);

//this holds all the bits for the display
class StepchildDisplay{
public:
  char displayBuffer[128][64];
  bool inverted = false;
  bool sendScreenViaUSB = false;
  
  //for compatability with adafruit functions
  const int16_t WIDTH = 128;        ///< This is the 'raw' display width - never changes
  const int16_t HEIGHT = 64;    ///< This is the 'raw' display height - never changes
  int16_t _width = 128;       ///< Display width as modified by current rotation
  int16_t _height = 64;     ///< Display height as modified by current rotation
  int16_t cursor_x;     ///< x location to start print()ing text
  int16_t cursor_y;     ///< y location to start print()ing text
  uint16_t textcolor;   ///< 16-bit background color for print()
  uint16_t textbgcolor; ///< 16-bit text color for print()
  uint8_t textsize_x;   ///< Desired magnification in X-axis of text to print()
  uint8_t textsize_y;   ///< Desired magnification in Y-axis of text to print()
  uint8_t rotation;     ///< Display rotation (0 thru 3)
  bool wrap;            ///< If set, 'wrap' text at right edge of display
  bool _cp437;          ///< If set, use correct CP437 charset (default is off)
  GFXfont *gfxFont;     ///< Pointer to special font
  
  StepchildDisplay(void);
  StepchildDisplay(int w, int h);//done
  
  void init(){}
  
  void display(void);//done
  
  void drawPixel(int x1, int y1, int c);//done
  void setRotation(int r);
  void invertDisplay(bool i);//done
  void fillScreen(int c);//done
  void clearDisplay();//done
  void drawLine(int x0, int y0, int x1, int y1, int c);//done
  void drawFastVLine(int x1, int y1, int h, int c);//done
  void drawFastHLine(int x1, int y1, int w, int c);//done
  void fillRect(int x1, int y1, int w, int h, int c);//done
  void drawRect(int x1, int y1, int w, int h, int c);
  void drawCircle(int x0, int y0, int r, int c);//done
  void drawCircleHelper(int x0, int y0, int r, int cornername, int c);//done
  void fillCircleHelper(int x0, int y0, int r,int corners, int delta,int c);//done
  void fillCircle(int x1, int y1, int r, int c);//done
  void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int c);//done
  void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int c);//done
  void drawRoundRect(int x1, int y1, int w, int h, int r, int c);//done
  void fillRoundRect(int x1, int y1, int w, int h, int r, int c);//done
  void drawBitmap(int x1, int y1, const uint8_t bitmap[], int w, int h, int c);//done
  void drawBitmap(int x1, int y1, const uint8_t bitmap[], int w, int h, int c, int bg);//done
  void drawChar(int x, int y, unsigned char c,int color, int bg, int size_x,int size_y);
  void drawChar(int x, int y, unsigned char character, int c, int bg, int size);
  void setTextSize(uint8_t s_x, uint8_t s_y);//done
  void setTextSize(uint8_t s);//done
  void setTextColor(int c1); //done
  void setTextColor(int c1, int c2); //done
  void setFont(const GFXfont *f);
  void setFont();
  void print(char c);
  void print(std::string text);
  void print(int);
  void print(uint16_t);
  void print(int16_t);
  void print(int8_t t);
  void println(std::string text);
  void setCursor(int x1, int y1);//done
  void setTextWrap(bool);//done
  int getCursorX();//done
  int getCursorY();//done
  void ssd1306_command(uint8_t);
};
