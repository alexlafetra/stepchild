/*
 + -------------------
 |  Overriding/replacing the arduino library functions
 |  and some of the Pi Pico core functions
 + -------------------
 */
#include <sys/time.h>
#include <sys/resource.h>
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
//swapping the arduino "String" object for the C++ "string" object (kinda silly, but it works)
#define String string

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

//converts degrees to radians
float radians(int deg){
    return deg*M_PI/180.0;
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
long getMemoryUsage()
{
  struct rusage usage;
  if(0 == getrusage(RUSAGE_SELF, &usage))
    return usage.ru_maxrss; // bytes
  else
    return 0;
}


/*
----------------------------------------
            Pin I/O
----------------------------------------
*/
float analogRead(uint8_t pin){
    return 65535;
}
int digitalRead(int pin){
    return 0;
}
void digitalWrite(uint8_t pin, bool value){
    return;
}
void analogWrite(uint8_t pin, uint16_t val){
}
void shiftOut(int dataPin, int clockPin, int style, int data){
    for(uint8_t i = 0; i<8; i++){
        leds[i] = (data>>i)&0b00000001;
    }
    return;
}
unsigned char shiftIn(int dataPin, int clockPin, int style){
    return 0;
}
void reset_usb_boot(int a, int b){
    return;
}
float analogReadTemp(){
    return 0.0;
}

/*
----------------------------------------
        Random Number Generator
----------------------------------------
*/
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
/*
----------------------------------------
                Timing
----------------------------------------
*/
//emulating delay
void delay(unsigned int time){
    usleep(time*1000);
}
void sleep_ms(int num){
    delay(num);
}
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
/*
----------------------------------------
                Pi Pico
----------------------------------------
*/
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
/*
----------------------------------------
            Serial
----------------------------------------
*/
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
