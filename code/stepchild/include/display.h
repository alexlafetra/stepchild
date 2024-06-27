
//Adafruit GFX library
//there's a pico optimized version here, but it's unclear what's optimized in it/if it matters for the SSD1306: https://github.com/Bodmer/Adafruit-GFX-Library
#include <Adafruit_GFX.h>

//grab the display libraries
#include <Adafruit_SSD1306.h>
//for a nice font
#include <Fonts/FreeSerifItalic9pt7b.h>
//you only use a few glyphs from these, you don't need to include the whole thing
#include <Fonts/FreeSerifItalic12pt7b.h>
#include <Fonts/FreeSerifItalic24pt7b.h>
#include <SPI.h>

//setting up screen
#define SCREEN_ADDR 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// #define OLED_RESET -1   // No reset pin!

#define SSD1306_NO_SPLASH

#define DISPLAY_UPRIGHT 2
#define DISPLAY_UPSIDEDOWN 0
#define DISPLAY_SIDEWAYS_R 3
#define DISPLAY_SIDEWAYS_L 1

//this is a derived class so that you can still call the same draw() functions on it!
class USBSerialCaptureCard: public Adafruit_SSD1306{
  public:
    uint32_t timeLastFrameSent = 0;//stores the time the last frame was sent
    const uint8_t minMsDelay = 10;//min amount of time to wait (ms) before sending a new frame
    //ridiculous derived constructor
    USBSerialCaptureCard(uint8_t w, uint8_t h, TwoWire *twi = &Wire, int8_t rst_pin = -1, uint32_t clkDuring = 400000UL,
                   uint32_t clkAfter = 100000UL):Adafruit_SSD1306(w,h,twi,rst_pin,clkDuring,clkAfter){}
    void writeScreenToSerial(){
      if((millis()-this->timeLastFrameSent)>this->minMsDelay){
        Serial.write(this->getBuffer(),1024);
        this->timeLastFrameSent = millis();
      }
    }
    void display(){
      this->writeScreenToSerial();
      Adafruit_SSD1306::display();//call the original display method
    }
};

//Defining capture card switches the Adafruit_SSD1306 object to the USBSerialCaptureCard object
#ifdef CAPTURECARD
USBSerialCaptureCard display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#else
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);
#endif