/*
----------------------------------------
                Pi Pico
----------------------------------------
*/

//#pragma once

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

long getMemoryUsage();
void reset_usb_boot(int a, int b);
float analogReadTemp();
