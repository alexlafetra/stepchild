<!-- ![Gif of the Stepchild Rotating](readme/stepchild.gif) -->

<p align="center">
</p>

<p align="center">
<img src = "readme/cursive_light.png#gh-dark-mode-only">
<img src = "readme/cursive_dark.png#gh-light-mode-only">
<img src="manual/images/stepchild.gif">
</p>

<!-- intro -->
# The Stepchild Sequencer
  
**Welcome to the github page for the Stepchild MIDI sequencer project!** The Stepchild is an open source, generative MIDI instrument designed as both a sequencer and experimental songwriting tool. The Stepchild can be purchased as either a [kit]() or [complete instrument]() from the Stepchild site, or you can [build your own]() by following the build guide.

![Render of The Stepchild](readme/opaque.png)

<!-- links to social media, reddit, website -->
<!-- capabilities -->
# Overview
This page gives a brief overview of the hardware and software capabilities of the Stepchild, but please [Stepchild Manual](manual/manual.md). If you're looking to make your own, please also take a look at the [Build Guide]() which gives an overview of the materials and process for building assembling a Stepchild.

# Hardware
The Stepchild's current hardware is on ***version 0.9.2***.
The Stepchild has 4 hardware MIDI out ports and 1 hardware MIDI In port, supporting both 3.5mm TRS and the original 5-pin-DIN connector MIDI standard, as well as USB MIDI in/out using the Pi Pico's microUSB port. In total, up to 8 instruments and 1 computer can be connected to output channels from the Stepchild on 4 separate hardware ports.

![Image](readme/stepchild_disassembled.jpg)

<!-- ![Image of the stepchild](readme/stepchild_assembly.jpg) -->

# Software
As of version 1.0, ChildOS has a powerful sequencer, looper, and Automation function generator, as well as 9 instruments and FX. There are a total of 16 Instrument slots and 24 FX slots that can be swapped with user-created code written in C++ (See documentation on [creating custom instruments]() for a tutorial).



### Main Features:
1. A graphical interface
2. A step sequencer
3. A note editor
4. A track editor
5. A CC Automation ("Autotrack") sequencer
6. A customizeable looper, with up to 256 loops of any length
7. An onboard file system
8. A randomizer
9. An arpeggiator
10. A console to view incoming MIDI data
11. A customizeable clock, integrated with Autotracks for weird timing

### Instruments & FX:
1. Joystick-to-CC -- <i>Use the Stepchild's joystick as a MIDI controller</i>
2. Rain           -- <i>Generate ambient notes</i>
3. Rattlesnake    -- <i>Output notes wth varying speed</i>
4. Knobs          -- <i>Use the Stepchild's encoders as MIDI controllers</i>
5. Quantizer
6. Humanizer
7. Strum
8. Echo
9. Reverse

# Filesystem & ChildOS Interface

The Stepchild uses [LittleFS]() to set aside 1MB of the Pico's onboard flash memory to store settings and sequence files. [ChildOS Interface](https://github.com/alexlafetra/childOSInterface) is a standalone Java app that lets you download saved files from the Stepchild, or to <i>convert</i> .child files to .MIDI files that can be played back in a DAW. 

# Contributors & Thank You's
<!-- libraries childOS relies on -->
The Stepchild's hardware and software is built using the hard work of a lot of other people. A special thanks to the creators and maintainers of the following projects that ChildOS uses:

 * The [EuroPi Project](https://github.com/Allen-Synthesis/EuroPi) started by [Allen Synthesis](https://www.allensynthesis.co.uk/index.html), specifically for providing the 
 * FortySevenEffect's [MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library)
 * [TinyUSB](https://github.com/hathach/tinyusb)
 * [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) and the [Adafruit SSD1306 Library](https://github.com/adafruit/Adafruit_SSD1306)
 * [LittleFS project](https://github.com/littlefs-project/littlefs)
 * Earle Philhower's [Arduino-Pico Core](https://github.com/earlephilhower/arduino-pico)
 * [KiCad](https://www.kicad.org/) <3

 Please see the [license.txt]() doc for full credits.

 ![](readme/prototype.jpeg)
*Breadboard prototype of The Stepchild*

