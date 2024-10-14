//fonts
#include "Fonts/gfxfont.h"
#include "Fonts/ASCII_font.h"
#include "Fonts/FreeSerifItalic9pt7b.h"
#include "Fonts/FreeSerifItalic12pt7b.h"
#include "Fonts/FreeSerifItalic24pt7b.h"

#define DISPLAY_UPRIGHT 2
#define DISPLAY_UPSIDEDOWN 0
#define DISPLAY_SIDEWAYS_R 3
#define DISPLAY_SIDEWAYS_L 1

#define SCREENWIDTH 128
#define SCREENHEIGHT 64

#define SSD1306_WHITE 1
#define SSD1306_BLACK 0

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SSD1306_SETCONTRAST 0

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

//this holds all the bits for the display
class Display{
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
    
    Display(void);
    Display(int w, int h);//done
    
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
    void print(string text);
    void print(int);
    void print(uint16_t);
    void print(int16_t);
    void print(int8_t t);
    void println(string text);
    void setCursor(int x1, int y1);//done
    void setTextWrap(bool);//done
    int getCursorX();//done
    int getCursorY();//done
    void ssd1306_command(uint8_t);
};
Display::Display(void){
    Display(128,64);
}
Display::Display(int w, int h){
  _width = w;
  _height = h;
  rotation = 0;
  cursor_y = cursor_x = 0;
  textsize_x = textsize_y = 1;
  textcolor = textbgcolor = 0xFFFF;
  wrap = true;
  _cp437 = false;
  gfxFont = NULL;
}
void Display::fillScreen(int c){
    for(int column = 0;column<128;column++){
        for(int row = 0; row<64; row++){
            displayBuffer[column][row] = c;
        }
    }
}
void Display::clearDisplay(){
    fillScreen(0);
}
void Display::invertDisplay(bool i){
    //if the inversion state is changed
    if( i != inverted){
        inverted = i;
        for(int column = 0;column<128;column++){
            for(int row = 0; row<64; row++){
                displayBuffer[column][row] = !displayBuffer[column][row];
            }
        }
    }
}
void Display::ssd1306_command(uint8_t a){
    return;
}
void Display::setRotation(int r) {
    rotation = (r & 3);
    //just bc it's upside down by default on the stepchild
    rotation = (rotation+2)%4;
  switch (rotation) {
  case 0:
  case 2:
    _width = WIDTH;
    _height = HEIGHT;
    break;
  case 1:
  case 3:
    _width = HEIGHT;
    _height = WIDTH;
    break;
  }
}
//TEXT functions
void Display::drawChar(int x, int y, unsigned char c,int color, int bg, int size_x,int size_y) {

  if (!gfxFont) { // 'Classic' built-in font
    if ((x >= _width) ||              // Clip right
        (y >= _height) ||             // Clip bottom
        ((x + 6 * size_x - 1) < 0) || // Clip left
        ((y + 8 * size_y - 1) < 0))   // Clip top
      return;

    for (int8_t i = 0; i < 5; i++) { // Char bitmap = 5 columns
      uint8_t line = pgm_read_byte(&font[c * 5 + i]);
      for (int8_t j = 0; j < 8; j++, line >>= 1) {
        if (line & 1) {
          if (size_x == 1 && size_y == 1)
            drawPixel(x + i, y + j, color);
          else
            fillRect(x + i * size_x, y + j * size_y, size_x, size_y,
                          color);
        } else if (bg != color) {
          if (size_x == 1 && size_y == 1)
            drawPixel(x + i, y + j, bg);
          else
            fillRect(x + i * size_x, y + j * size_y, size_x, size_y, bg);
        }
      }
    }
    if (bg != color) { // If opaque, draw vertical line for last column
      if (size_x == 1 && size_y == 1)
        drawFastVLine(x + 5, y, 8, bg);
      else
        fillRect(x + 5 * size_x, y, size_x, 8 * size_y, bg);
    }
    
  } else { // Custom font

    // Character is assumed previously filtered by draw() to eliminate
    // newlines, returns, non-printable characters, etc.  Calling
    // drawChar() directly with 'bad' characters of font may cause mayhem!

    c -= (uint8_t)pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c);
    uint8_t *bitmap = pgm_read_bitmap_ptr(gfxFont);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t w = pgm_read_byte(&glyph->width), h = pgm_read_byte(&glyph->height);
    int8_t xo = pgm_read_byte(&glyph->xOffset),
           yo = pgm_read_byte(&glyph->yOffset);
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;

    if (size_x > 1 || size_y > 1) {
      xo16 = xo;
      yo16 = yo;
    }

    // Todo: Add character clipping here

    // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
    // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
    // has typically been used with the 'classic' font to overdraw old
    // screen contents with new data.  This ONLY works because the
    // characters are a uniform size; it's not a sensible thing to do with
    // proportionally-spaced fonts with glyphs of varying sizes (and that
    // may overlap).  To replace previously-drawn text when using a custom
    // font, use the getTextBounds() function to determine the smallest
    // rectangle encompassing a string, erase the area with fillRect(),
    // then draw new text.  This WILL infortunately 'blink' the text, but
    // is unavoidable.  Drawing 'background' pixels will NOT fix this,
    // only creates a new set of problems.  Have an idea to work around
    // this (a canvas object type for MCUs that can afford the RAM and
    // displays supporting setAddrWindow() and pushColors()), but haven't
    // implemented this yet.

    
    for (yy = 0; yy < h; yy++) {
      for (xx = 0; xx < w; xx++) {
        if (!(bit++ & 7)) {
          bits = pgm_read_byte(&bitmap[bo++]);
        }
        if (bits & 0x80) {
          if (size_x == 1 && size_y == 1) {
            drawPixel(x + xo + xx, y + yo + yy, color);
          } else {
            fillRect(x + (xo16 + xx) * size_x, y + (yo16 + yy) * size_y,
                          size_x, size_y, color);
          }
        }
        bits <<= 1;
      }
    }
    

  } // End classic vs custom font
}
void Display::drawChar(int x1, int y1, unsigned char c,int color, int bg, int size) {
  drawChar(x1, y1, c, color, bg, size, size);
}
void Display::println(string text){
    Display::print(text);
}
//all these just decay down to the unsigned char cas
void Display::print(char c) {
  if (!gfxFont) { // 'Classic' built-in font
    if (c == '\n') {              // Newline?
      cursor_x = 0;               // Reset x to zero,
      cursor_y += textsize_y * 8; // advance y one line
    } else if (c != '\r') {       // Ignore carriage returns
      if (wrap && ((cursor_x + textsize_x * 6) > _width)) { // Off right?
        cursor_x = 0;                                       // Reset x to zero,
        cursor_y += textsize_y * 8; // advance y one line
      }
      drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x,
               textsize_y);
      cursor_x += textsize_x * 6; // Advance x one char
    }

  } else { // Custom font

    if (c == '\n') {
      cursor_x = 0;
      cursor_y +=
          (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if (c != '\r') {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
        uint8_t w = pgm_read_byte(&glyph->width),
                h = pgm_read_byte(&glyph->height);
        if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
          if (wrap && ((cursor_x + textsize_x * (xo + w)) > _width)) {
            cursor_x = 0;
            cursor_y += (int16_t)textsize_y *
                        (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
          }
          drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x,
                   textsize_y);
        }
        cursor_x +=
            (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize_x;
      }
    }
  }
}
void Display::print(string t){
    for(int i = 0; i<t.length(); i++){
        print(static_cast<char>(t.at(i)));
    }
}
void Display::print(int t){
    print(to_string(t));
}
void Display::print(int16_t t){
    print(to_string(t));
}
void Display::print(uint16_t t){
    print(to_string(t));
}
void Display::print(int8_t t){
    print(to_string(t));
}
void Display::setTextSize(uint8_t s_x, uint8_t s_y) {
  textsize_x = (s_x > 0) ? s_x : 1;
  textsize_y = (s_y > 0) ? s_y : 1;
}
void Display::setTextSize(uint8_t s){
    setTextSize(s,s);
}
void Display::setTextColor(int c1, int c2){
    textbgcolor = c2;
    textcolor = c1;
}
void Display::setTextColor(int c1){
    textcolor = c1;
}
int Display::getCursorX(){
    return cursor_x;
}
int Display::getCursorY(){
    return cursor_y;
}
void Display::setCursor(int x1, int y1){
    cursor_x = x1;
    cursor_y = y1;
}
void Display::setFont(const GFXfont *f) {
  if (f) {          // Font struct pointer passed in?
    if (!gfxFont) { // And no current font struct?
      // Switching from classic to new font behavior.
      // Move cursor pos down 6 pixels so it's on baseline.
      cursor_y += 6;
    }
  }
  else if (gfxFont) { // NULL passed.  Current font struct defined?
    // Switching from new to classic font behavior.
    // Move cursor pos up 6 pixels so it's at top-left of char.
    cursor_y -= 6;
  }
  gfxFont = (GFXfont *)f;
}
void Display::setFont(){
    setFont(NULL);
}
//UNUSED so it prob doesn't matter
void Display::setTextWrap(bool wrap){
}


//DRAWING functions
void Display::drawPixel(int x1, int y1, int c){
    if ((x1 < 0) || (y1 < 0) || (x1 >= _width) || (y1 >= _height))
      return;

    int16_t t;
    switch (rotation) {
    case 1:
      t = x1;
      x1 = WIDTH - 1 - y1;
      y1 = t;
      break;
    case 2:
      x1 = WIDTH - 1 - x1;
      y1 = HEIGHT - 1 - y1;
      break;
    case 3:
      t = x1;
      x1 = y1;
      y1 = HEIGHT - 1 - t;
      break;
    }
    //writing pixel (if it's on screen)
    //black or white
    if(c != 2)
        displayBuffer[x1][y1] = c;
    //inverting
    else
        displayBuffer[x1][y1] = !displayBuffer[x1][y1];
}
void Display::drawLine(int x0, int y0, int x1, int y1, int c){
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
      _swap_int16_t(x0, y0);
      _swap_int16_t(x1, y1);
    }
    if (x0 > x1) {
      _swap_int16_t(x0, x1);
      _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
      ystep = 1;
    } else {
      ystep = -1;
    }

    for (; x0 <= x1; x0++) {
      if (steep) {
        drawPixel(y0, x0, c);
      } else {
        drawPixel(x0, y0, c);
      }
      err -= dy;
      if (err < 0) {
        y0 += ystep;
        err += dx;
      }
    }
}
void Display::drawFastVLine(int x1, int y1, int h, int c) {
    drawLine(x1, y1, x1, y1 + h - 1, c);
}
void Display::drawFastHLine(int x1, int y1, int w, int c) {
    drawLine(x1, y1, x1+w-1, y1, c);
}
void Display::fillRect(int x1, int y1, int w, int h, int c) {
    for (int i = x1; i < x1 + w; i++) {
        drawFastVLine(i, y1, h, c);
    }
}
void Display::drawCircle(int x0, int y0, int r, int c) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0, y0 + r, c);
  drawPixel(x0, y0 - r, c);
  drawPixel(x0 + r, y0, c);
  drawPixel(x0 - r, y0, c);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, c);
    drawPixel(x0 - x, y0 + y, c);
    drawPixel(x0 + x, y0 - y, c);
    drawPixel(x0 - x, y0 - y, c);
    drawPixel(x0 + y, y0 + x, c);
    drawPixel(x0 - y, y0 + x, c);
    drawPixel(x0 + y, y0 - x, c);
    drawPixel(x0 - y, y0 - x, c);
  }
}
void Display::drawCircleHelper(int x0, int y0, int r, int cornername, int c) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, c);
      drawPixel(x0 + y, y0 + x, c);
    }
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, c);
      drawPixel(x0 + y, y0 - x, c);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, c);
      drawPixel(x0 - x, y0 + y, c);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, c);
      drawPixel(x0 - x, y0 - y, c);
    }
  }
}
void Display::fillCircleHelper(int x0, int y0, int r,int corners, int delta,int c) {

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  int16_t px = x;
  int16_t py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    // These checks avoid double-drawing certain lines, important
    // for the SSD1306 library which has an INVERT drawing mode.
    if (x < (y + 1)) {
      if (corners & 1)
        drawFastVLine(x0 + x, y0 - y, 2 * y + delta, c);
      if (corners & 2)
        drawFastVLine(x0 - x, y0 - y, 2 * y + delta, c);
    }
    if (y != py) {
      if (corners & 1)
        drawFastVLine(x0 + py, y0 - px, 2 * px + delta, c);
      if (corners & 2)
        drawFastVLine(x0 - py, y0 - px, 2 * px + delta, c);
      py = y;
    }
    px = x;
  }
}
void Display::fillCircle(int x0, int y0, int r, int c) {
  drawFastVLine(x0, y0 - r, 2 * r + 1, c);
  fillCircleHelper(x0, y0, r, 3, 0, c);
}
void Display::drawRect(int x1, int y1, int w, int h,
                            int c) {
  drawFastHLine(x1, y1, w, c);
  drawFastHLine(x1, y1 + h - 1, w, c);
  drawFastVLine(x1, y1, h, c);
  drawFastVLine(x1 + w - 1, y1, h, c);
}
void Display::drawRoundRect(int x1, int y1, int w, int h,int r, int c) {
  int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  drawFastHLine(x1 + r, y1, w - 2 * r, c);         // Top
  drawFastHLine(x1 + r, y1 + h - 1, w - 2 * r, c); // Bottom
  drawFastVLine(x1, y1 + r, h - 2 * r, c);         // Left
  drawFastVLine(x1 + w - 1, y1 + r, h - 2 * r, c); // Right
  // draw four corners
  drawCircleHelper(x1 + r, y1 + r, r, 1, c);
  drawCircleHelper(x1 + w - r - 1, y1 + r, r, 2, c);
  drawCircleHelper(x1 + w - r - 1, y1 + h - r - 1, r, 4, c);
  drawCircleHelper(x1 + r, y1 + h - r - 1, r, 8, c);
}
void Display::fillRoundRect(int x1, int y1, int w, int h,int r, int c) {
  int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
    fillRect(x1 + r, y1, w - 2 * r, h, c);
  // draw four corners
    fillCircleHelper(x1 + w - r - 1, y1 + r, r, 1, h - 2 * r - 1, c);
    fillCircleHelper(x1 + r, y1 + r, r, 2, h - 2 * r - 1, c);
}
void Display::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int c) {
  drawLine(x0, y0, x1, y1, c);
  drawLine(x1, y1, x2, y2, c);
  drawLine(x2, y2, x0, y0, c);
}
void Display::fillTriangle(int x0, int y0, int x1, int y1,int x2, int y2, int color) {
  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    _swap_int16_t(y0, y1);
    _swap_int16_t(x0, x1);
  }
  if (y1 > y2) {
    _swap_int16_t(y2, y1);
    _swap_int16_t(x2, x1);
  }
  if (y0 > y1) {
    _swap_int16_t(y0, y1);
    _swap_int16_t(x0, x1);
  }

  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)
      a = x1;
    else if (x1 > b)
      b = x1;
    if (x2 < a)
      a = x2;
    else if (x2 > b)
      b = x2;
    drawFastHLine(a, y0, b - a + 1, color);
    return;
  }

  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
          dx12 = x2 - x1, dy12 = y2 - y1;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
    last = y1; // Include y1 scanline
  else
    last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);
    drawFastHLine(a, y, b - a + 1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = (int32_t)dx12 * (y - y1);
  sb = (int32_t)dx02 * (y - y0);
  for (; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);
    drawFastHLine(a, y, b - a + 1, color);
  }
}

void Display::drawBitmap(int x1, int y1, const uint8_t bitmap[], int w, int h, int c){
  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t b = 0;
  for (int16_t j = 0; j < h; j++, y1++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        b <<= 1;
      else
        b = bitmap[j * byteWidth + i / 8];
      if (b & 0x80)
        drawPixel(x1 + i, y1, c);
    }
  }
}
void Display::drawBitmap(int x1, int y1, const uint8_t bitmap[], int w, int h, int c, int bg){
  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t b = 0;
  for (int16_t j = 0; j < h; j++, y1++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        b <<= 1;
      else
        b = bitmap[j * byteWidth + i / 8];
      if (b & 0x80)
        drawPixel(x1 + i, y1, c);
    }
  }
}

//copies vals into display pixels
void Display::display(void){
    try{
        glfwPollEvents();
    }
    catch(...){
        cout<<"oh shit! something went wrong with glfwPollEvents() in the screen::display() fn :(";
    }
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
    displayWindow();
    delay(20);
}

Display display;

