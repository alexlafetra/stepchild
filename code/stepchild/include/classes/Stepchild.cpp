/*
so much consolidation to do! i think the whole sequence class should be turned into this class
since the main sequence is really the core operation of the stepchild

also menus need a more standard API, so that subclasses can be run from some basic menu.run()

run(){
    this.updateControls()
    this.display()
}

methods. This should help not have such crazy ass functions like "PCEditingEncoderControls," the PCEditor should
just be its own class with its own control method


wireframes should be inside graphics object
maybe all drawing/screen functions should be a part of the graphics object
and then the screen/display class is a member of it too
*/
class Stepchild{
    public:
        StepchildCV cv;
        StepchildHardware io;
        StepchildDisplay display;
        StepchildMIDI midi;
        StepchildFileSystem files;
        StepchildClock clock;
    Stepchild(){

    }
};

Stepchild stepchild;