![Image of the stepchild](images/stepchild_assembly.jpg)

# The Stepchild

<!-- borrowing heavily from the europi page -->

<!-- intro -->
  
Welcome to the github page for the Stepchild! This is an open source, generative MIDI instrument, designed to be part step-sequencer, part notebook, and part utility.

The Stepchild has the capability to record and playback sequences of MIDI data while generating its own explorative MIDI data to sequence other instruments. It can act as a tool for writing music and for interfacing between MIDI, CV, and USB protocols.

<!-- links to social media, reddit, website -->

<!-- capabilities -->
  
# the what
The Stepchild has 4 hardware MIDI out ports and 1 hardware MIDI In port, supporting both 3.5mm TRS and the original MIDI standard, ands USB MIDI in/out support.
The Stepchild runs firmware called "ChildOS," designed to be easy to use and to integrate with custom applications for generative MIDI instruments and effects. The firmware is 
run by a Pi Pico and displayed on a monochrome OLED.

The Stepchild has 8 main buttons, two rotary encoders, an XY joystick, and 8 step-buttons emulating other hardware sequencers. The stepchild also exposes 3 extra pins for CV,
(although the MIDI-CV features are still being tested!) or for accessing the Pico's ADC to read in data from a 3.3v analog sensor.

As of the latest release, the ChildOS contains...

## Features
1. A graphical interface
2. A step sequencer supporting up to 128 tracks and 65,000 steps for creating and recording MIDI notes
3. A CC Automation ("Autotrack") sequencer, allowing up to 128 Autotracks with user-generated curves or LFO functions
4. Up to 256 loops of any length, with a loop composer to chain or radnomly link loops together
5. An onboard file system using the [LittleFS C++ Library](https://github.com/littlefs-project/littlefs)
6. A randomizer
7. A MIDI sniffer console, to view incoming MIDI data
8. A flexible software-based clock, integrated with Autotracks for flexible timing functions and arbitrary swing-time
9. An arpeggiator

As well as:
### Instruments
1. A joystick-to-CC application
2. Rain
3. Rattlesnake
4. 16 Virtual CC Knobs

And finally:

### Fx
1. A quantizer
2. A humanizer
3. A strum effect
4. An echo effect
5. A reverse effect

# the how
<!-- libraries childOS relies on -->
Special thanks to the creators and maintainers of the following projects:

 * adafruit gfx library
 * adafruit ssd1309 library
 * adafruit tinyusb
 * littleFS project
 * earle philhowers pico core
 * kicad <3

