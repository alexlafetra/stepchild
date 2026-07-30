/*
 + -------------------
 |  Overriding/replacing the arduino library functions
 |  and some of the Pi Pico core functions
 + -------------------
 */
#pragma once

#include <sys/time.h>
#include <sys/resource.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <chrono>

/*
----------------------------------------
            Constants
----------------------------------------
*/
#define PI           3.14159265358979323846  /* pi */
#define MSBFIRST 0
#define LSBFIRST 0
#define LOW 0
#define HIGH 1
//swapping the arduino "String" object for the C++ "std::string" object (kinda silly, but it works)
#define String std::string

/*
----------------------------------------
            Utilities
----------------------------------------
*/

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


std::string stringify(int a);
std::string stringify(std::string s);
int toInt(std::string s);

//converts degrees to radians
float radians(int deg);
int constrain(int target, int lower, int upper);

/*
----------------------------------------
            Pin I/O
----------------------------------------
*/
float analogRead(uint8_t pin);
int digitalRead(int pin);
void digitalWrite(uint8_t pin, bool value);
void analogWrite(uint8_t pin, uint16_t val);
//void shiftOut(int dataPin, int clockPin, int style, int data){
//    for(uint8_t i = 0; i<8; i++){
//        leds[i] = (data>>i)&0b00000001;
//    }
//    return;
//}
//unsigned char shiftIn(int dataPin, int clockPin, int style){
//    return 0;
//}

/*
----------------------------------------
        Random Number Generator
----------------------------------------
*/
int random(int lower, int upper);
int max(int a , int b);
float random_f(float lower, float upper);
void randomSeed(float rS);
int random(int upper);
/*
----------------------------------------
                Timing
----------------------------------------
*/
//emulating delay
void delay(unsigned int time);
void sleep_ms(int num);
//emulating micros and millis
unsigned long micros();
unsigned long millis();

/*
----------------------------------------
            Serial
----------------------------------------
*/
class DummySerial{
    public:
        DummySerial(void);
        void print(std::string);
        void print(int);
        void println(std::string);
        void println(int);
        void flush();
        int read();
        std::string readString();
        void end();
        void begin(int);
        void write(int);
        void readBytes(uint8_t* a, uint8_t b);
        uint8_t available();
};

extern DummySerial Serial;
