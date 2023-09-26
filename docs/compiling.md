# Compiling/building ChildOS

ChildOS should be easy to compile and edit on your own. To compile a binary to run on the Stepchild, or to upload directly to the Stepchild itself, you should first download the [Arduino IDE](https://www.arduino.cc/en/software). If you're using a Mac, or a computer that runs Xcode, you can also run the headless version of ChildOS that emulates the screen and internal logic (which at the moment is only really useful for graphics testing, but MIDI functionality might come later!)

## Once you have the Arduino IDE...
 Open the file "stepchild.ino" using the Arduino IDE (File > Open). You'll need to download the following libraries, but all of them can be found in the Arduino Library Manager (Tools > Manage Libraries):

 * Adafruit TinyUSB Library
 * Adafruit SSD1309 Library
 * Adafruit GFX Library
 * Arduino MIDI Library

Once you have the above libraries downloaded, you'll also need to download a core for the Pi Pico (if you don't already have one). To do this, go to the Arduino Boards Manager (Tools > Board > Boards Manager) and look for "Raspberry Pi RP2040 Boards." Install it, and select it as the board you're uploading to/compiling for (Tools > Board > Raspberry Pi RP2040 Boards > Raspberry Pi Pico). This will let the compiler access the Pico SDK functionality that ChildOS uses.

 Finally, before compiling, you need to select the Adafruit TinyUSB USB Stack. Go to Tools > USB Stack and select "Adafruit TinyUSB."

 ## Running ChildOS on Xcode
<!-- Open the Xcode project at "src/headlesschildOS_headless.xcodeproj" in Xcode. Add -->
<!-- gotta add a tutorial about adding openGL to Xcode -->
