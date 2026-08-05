#include "Arduino.h"

std::string stringify(int a){
    return std::to_string(a);
}
std::string stringify(std::string s){
    return s;
}
int toInt(std::string s){
    if(s == "")
        return 0;
    else
        return stoi(s);
}

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
        return upper;
    }
    return rand()%(upper-lower)+lower;
}
int max(int a , int b){
  return a>b?a:b;
}
float random_f(float lower, float upper){
    if(lower>upper){
        float temp = lower;
        lower = upper;
        upper = temp;
    }
    else if(upper==lower){
        return upper;
    }
    return lower + (rand() / ( RAND_MAX / (upper-lower) ) ) ;
}
void randomSeed(float rS){
  srand(rS);
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
auto progStartTime = std::chrono::high_resolution_clock::now();
unsigned long micros(){
    auto rn = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(rn-progStartTime).count();
}
unsigned long millis(){
    auto rn = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(rn-progStartTime).count();
}

/*
----------------------------------------
            Serial
----------------------------------------
*/

DummySerial::DummySerial(){
    
}
void DummySerial::print(std::string s){
    std::cout<<s;
    return;
}
void DummySerial::print(int a){
    std::cout<<a;
    return;
}
void DummySerial::println(std::string s){
    std::cout<<s;
    std::cout<<std::endl;
    return;
}
void DummySerial::println(int a){
    std::cout<<a;
    std::cout<<std::endl;
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
    std::cout<<a;
    return;
}
std::string DummySerial::readString(){
    return "hey, you're in headless mode";
}
uint8_t DummySerial::available(){
    return 0;
}
void DummySerial::readBytes(uint8_t* a, uint8_t b){
    return;
}

DummySerial Serial;
