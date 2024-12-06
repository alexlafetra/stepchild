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

void reset_usb_boot(int a, int b){
    return;
}
float analogReadTemp(){
    return 0.0;
}
