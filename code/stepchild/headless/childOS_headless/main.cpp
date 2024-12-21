
using namespace std;

#define HEADLESS

#include "../../src/stepchild.ino"

void cpu0(){
    while (!glfwWindowShouldClose(window)){
        loop();
    }
}
void cpu1(){
    while (!glfwWindowShouldClose(window)){
        loop1();
    }
}
int main(){
    delay(1000);
    //setup graphics window
    window = initGlfw();
    while(!openGLready){
    }
    //setup sequence data
    headlessSetup();
    
    //launch the cpu1 thread to run the clock
    thread core1(cpu1);
    
    //and then launch into the main thread
    cpu0();
    
    //wait for the other thread to exit before killing the window
    core1.join();
    
    //when you're ready to exit, close the window
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
