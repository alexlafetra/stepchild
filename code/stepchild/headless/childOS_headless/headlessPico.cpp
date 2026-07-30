/*
----------------------------------------
                Pi Pico
----------------------------------------
*/

#include "headlessPico.h"
#include "Arduino.h"

long getMemoryUsage()
{
  struct rusage usage;
  if(0 == getrusage(RUSAGE_SELF, &usage))
    return usage.ru_maxrss; // bytes
  else
    return 0;
}

DummyRP2040::DummyRP2040(){
}

int DummyRP2040::f_cpu(){
    return 0;
}
long DummyRP2040::getUsedHeap(){
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

void reset_usb_boot(int a, int b){
    return;
}
float analogReadTemp(){
    return 0.0;
}

DummyRP2040 rp2040;
