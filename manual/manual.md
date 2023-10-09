# Index
0. [Introduction](#0-introduction)
    1. [Overview](#01-overview)
    2. [Hardware](#02-hardware)
        1. [Buttons](#a-buttons)
        2. [MIDI Layout](#b-midi-layout)
    3. [Software](#03-software)
        1. [Notes](#a-notes)
        2. [Tracks](#b-tracks)
1. [Main Sequence](#1-main-sequence)
    1. [Moving the cursor](#11-moving-the-cursor)
    2. [Changing the view/Subdivision](#12-changing-the-view--subdivision)
    3. [Creating notes](#13-creating-notes)
    4. [Deleting & Muting Notes](#14-deleting--muting-notes)
    5. [Selecting notes](#15-selecting-notes)
    6. [Moving loops](#16-moving-loops)
    7. [Copy/paste](#17-copypaste)
    8. [Playback](#18-playback)
        1. Playing
        2. Recording
    9. Status Icons
2. [Track Editor](#2-trackeditor)
    1. Utilities
3. [Note Editor](#3-note-editor)
    1. Moving Notes
    2. Changing Note Length
    3. Changing Note Velocity
    4. Changing Note Probability
    5. Quantizing Notes
    6. Humanizing Notes
    7. QuickFX
4. [Menus](#4-menus)
    1. Main Menu
    2. Autotrack Menu
    3. Loop Menu
    4. Instrument Menu
    5. FX Menu
    6. Settings Menu
        1. Sequence
        2. System
    7. MIDI Menu
        1. Routing
        2. CV
        3. Thru
    8. File Menu
    9. [Clock Menu](#49-clock-menu)
        1. [BPM](#a-bpm)
        2. [Swing](#b-swing)
        3. [Clock Source](#c-clock-source)
5. [Autotracks](#5-autotracks)
    1. Editor Controls
    2. CC List
    3. Internal CC Messages
    4. Note about Ableton
6. [Loops](#6-loops)
    1. Types of loops
    2. Editing within Loop
7. [Arpeggiator](#7-arpeggiator)
    1. Step Lengths
    2. Modifiers
8. [Randomizer](#8-randomizer)
    1. Randomizer Settings
    2. Selecting an Area
9. [Instruments](#9-instruments)
    1. XY
    2. Storm
    3. Knobs
    4. Keys
    5. Drumpads
    6. MIDI to CV
    7. Rattle
    8. Live Loop
10. [FX](#10-fx)
    1. Quantize
    2. Humanize
    3. Strum
    4. Echo
    5. Reverse
    6. Warp
11. [Files](#11-files)
    1. [Quicksaving](#111-quicksaving)
    2. File Browsing
    3. Uploading to a computer
12. [Console](#12-console)

# 0. Introduction

## 0.1 Overview

Hello! Welcome to the Stepchild User Manual. This document begins in a lot of detail, but slowly gets a more loose and conceptual as it goes on. Hopefully, this makes it easy to read and less redundant, but it also means that jumping straight into a later section might be more confusing than helpful. Because of this, there are a lot of links to other sections! Ideally, if something is confusing to you, you should be able to click it and it will link you to its explanation.

## 0.2 Hardware

### a. Buttons

There are 8 buttons on the face of the Stepchild, two push encoders A and B, and one Joystick. The icons below are ~roughly~ shown in the same layout, with the literal name for each button next to the icon that this manual will use to reference it.

**New** ---- ![**New**](images/buttons/new.svg)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;![**A**](images/buttons/A.svg) -- **A**<br>
**Shift** ---- ![**Shift**](images/buttons/shift.svg)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;![**B**](images/buttons/B.svg) -- **B**<br>
**Select** -- ![**Select**](images/buttons/select.svg)<br>
**Delete** -- ![**Delete**](images/buttons/delete.svg)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;![**Joystick**](images/buttons/joystick.svg) -- **Joystick**<br>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;![**Loop**](images/buttons/loop.svg) ![**Play**](images/buttons/play.svg) ![**Copy**](images/buttons/copy.svg) ![**Menu**](images/buttons/menu.svg)<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| <br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| &nbsp;&nbsp;&nbsp;&nbsp; **Menu** <br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;**Copy** <br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| &nbsp;&nbsp;&nbsp;&nbsp; **Play** <br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;**Loop** <br>

Alongside the 8 face buttons and two encoders, there are 8 stepbuttons along the lower board of the Stepchild. The 8 stepbuttons are used to toggle steps directly, like you would be able to with a more traditional hardware step sequencer. Occaisionally, the stepbuttons and LEDs have some auxilary functions within different applications. 

### b. MIDI Layout

The Stepchild has 5 MIDI out ports and 2 MIDI in ports, 1 pair of which uses the Stepchilds MicroUSB port. The 5 non-USB hardware ports are each exposed as both a **TRS Type-A** 3.5mm headphone jack (like you'd use with an aux cable) *and* the traditional 5-Pin-DIN MIDI connector. Each pair of jacks are hardwired together, and will always transmit the same signals. It's designed this way to work

![MIDI Layout Diagram](images/MIDI%20Layout.svg)

EX: "Output 1" on the back of the Stepchild will send the same signals that "Output 1" on the front of the Stepchild will.

Because of this, the Stepchild has 4 unique hardware and 1 unique software outputs, *but* you can connect up to **9** instruments directly to the Stepchild. 

The Stepchild will expose itself as a MIDI device when it's connected to a computer, and should hopefully be easy to select from the MIDI input settings of a DAW. 

## 0.3 Software
![ChildOS Bootscreen](images/childOS.png)

The Stepchild is the hardware system that the ChildOS firmware uses to make music. The ChildOS firmware contains a bunch of MIDI editing tools, live generative applications, a sequencer, and a sequencer that uses the Stepchild's hardware to exchange MIDI messages with other instruments.

###### (ChildOS isn't really an operating system, it uses the 'OS' suffix because it looks cool)

This manual is written about using ChildOS with the Stepchild, the software that I created to use it as a MIDI instrument, but it totally does not need to exclusively run ChildOS. Going into flashing your own firmware for the Stepchild is a little outside the scope of this user manual, but see the **Stepchild Documentation** for a tutorial.

### a. Notes

ChildOS's concept of notes is a little different than the way traditional MIDI software and written music treats them. Because the Stepchild is designed with a focus on rhythm *and* harmony/melody, Notes are stored as start-stop signals at certain velocities and don't contain any information about pitch or channel. By keeping the pitch/channel data stored on the note's [Track](#b-tracks), you can flexibly play around with where the note is sent and what it sounds like without editing the notes themselves.

```
           + ---------------- +
Start -->  | / / Velocity / / | <-- End
           + ---------------- +
```

When the sequence is playing and the playhead reaches the start position, a MIDI 'Note On' message is sent at the note's velocity, with the pitch and channel of the track the note is on. If the playhead reaches the end of the note, then a MIDI 'Note Off' message is sent.

By default, notes are shaded by their velocity, with quieter notes being more opaque. Note sprites can also be [configured](#45-settings) to be shaded by their probability.

Notes can be created and deleted in the [Main Sequence](#13-creating-notes), but can be more extensively edited and messed with by pressing ![B](images/buttons/B.svg) to bring up the [Note Editor](#3-note-editor), or by using the [FX Applications](#8-fx).

### b. Tracks

Tracks hold the pitch and channel information that is usually associated with individual notes. This lets the pitch and routing of different notes change fluidly by changing the pitch of each track (if it's helpful, you can think about it like changing instruments or an FX stack on a track in a DAW without changing the MIDI notes).

Track pitch can be edited directly in the [Main Sequence](#1-main-sequence) by holding ![Shift](images/buttons/shift.svg) and turning ![B](images/buttons/B.svg). Tracks can be more deeply edited in the [Track Editor](#2-track-editor) which can be accessed by pressing ![B](images/buttons/B.svg) in the Main Sequence.

# 1. Main Sequence
The first thing you see after the Stepchild boots up will be the Main Sequence screen. This is the “homepage” of the Stepchild where you can directly edit a sequence and access applications and menus. The Main Sequence screen *always* displays a baby carriage in the top left corner that bounces to the BPM of the sequence.

![The Main Sequence homepage](images/mainsequence.gif)

The main sequence is displayed like the piano roll editor found in traditional DAWs, with time on the X axis and separate [tracks](#b-tracks) on the Y axis. Track pitches are shown on the left as either MIDI note numbers or western chromatic pitches, and the sequence grid is displayed on the right. You can view the channel of each track by holding ![**Shift**](images/buttons/shift.svg). 

The current [**loop points**](#15-moving-loops) and [**status icons**](#18-status-icons), like battery life,  are shown at the top of the Main Sequnce screen.

### 1.1 Moving the cursor

The **cursor** is positioned at a specific timestep (its horizontal location) and track (its vertical location). You can change which track the cursor is on by moving the [**Joystick**](#hardware) ![up-and-down](images/buttons/up_down.svg), which will automatically send a MIDI note at the new active track’s pitch to the Stepchild’s MIDI ports.

The cursor can be moved through time by moving the Joystick ![side-to-side](images/buttons/left_right.svg), which will move the cursor to the left or right to the next closest subdivision. Holding shift will move the cursor exactly 1 timestep in that direction.

### 1.2 Changing the View & Subdivision

The sequence display will automatically adjust depending on where the cursor is and how zoomed in the view is. Turning ![**B**](images/buttons/B.svg) will multiply or divide the view subdivision by 2, and holding shift while turning ![**B**](images/buttons/B.svg) will switch between triplet and base-2 subdivisions. Turning ![**A**](images/buttons/A.svg) will zoom the sequence in or out by 2x and also increase or decrease the subdivision, allowing for smoothly zooming in and out to place notes with more precision.

At a certain level, when the sequence is zoomed out far enough, notes won't be able to render individually due to the low resolution of the Stepchild's screen. Instead, many adjacent notes will render as a block of notes and you'll need to zoom in to see the details.

Clicking instead of turning ![**A**](images/buttons/A.svg) will open the [Note Editor](#3-note-editor), and clicking instead of turning ![**B**](images/buttons/B.svg) will open the [Track Editor](#2-track-editor).

### 1.3 Creating Notes
Pressing ![**New**](images/buttons/new.svg) will create a note 1 subdivision long at the cursor's position and active track, and move the cursor to the end of the new note. Alternatively, if ![**New**](images/buttons/new.svg) is held and the cursor is moved, a note will be created and extended by the cursor movement until ![**New**](images/buttons/new.svg) is released. If ![shift](images/buttons/shift.svg) is held while the cursor is moved, you can draw notes of any length!

![Gif of notes being placed](images/makingnotes.gif)

If a note is created on top of another note, it will truncate the blocking note and insert itself into the sequence, as long as it wasn't placed on the exact timestep the blocking note starts on; new notes can't delete or fully replace notes that are blocking them.

By default, new notes are placed with a velocity of 127, but the default velocity of notes can be quickly changed by holding ![**Shift**](images/buttons/shift.svg) and moving the **Joystick** ![up or down](images/buttons/up_down.svg) while the cursor is over an empty part of the sequence. If the cursor is over a note, or if any notes are selected, the velocity of that note and all selected notes will be changed, allowing you to tweak note velocities without going into the edit menu.

### 1.4 Deleting & Muting Notes
You can delete notes by hovering over them and pressing, you guessed it, ![Delete](images/buttons/delete.svg). To mute a note, you can hold ![Shift](images/buttons/shift.svg) while pressing ![Delete](images/buttons/delete.svg). Muted notes won't be sent as MIDI data, but can still be edited and used as the target of [**FX applications**](#10-fx) and can be unmuted by pressing ![Shift](images/buttons/shift.svg) + ![Delete](images/buttons/delete.svg).

###### Both muting and deleting will apply to *all* selected notes, so make sure to deselect notes you don't want to delete! When you attempt to delete multiple notes at once, you'll be asked to confirm as a way to safeguard against mass-deletions (it's happened... a lot).

### 1.5 Selecting Notes
Most operations that affect a single note will also apply to other notes that are **selected**. Notes can be selected and deselected by placing the cursor on them and pressing ![Select](images/buttons/select.svg), or by holding down ![Select](images/buttons/select.svg) while moving the **Joystick** to create a selection box. By default, selecting is *additive*, meaning each note you select gets added to the current selection. To reset your selection, you can press ![Select](images/buttons/select.svg) + ![Shift](images/buttons/shift.svg) over a note or a blank space.

![Gif of the selection box in action](images/selecting.gif)

### 1.6 Moving Loops

The currently active [Loop](#6-loops) is always shown as two flags or lines, showing the start and end of the loop, and a number representing which loop is currently active from the loop chain. When the cursor is on one of these two loop points, they'll 'pop up' to let you know that you can move them.

![Gif demonstrating how to move loops](images/movingloops.gif)

Pressing ![Loop](images/buttons/loop.svg) when hovering over a loop point will make the loop point get "stuck" to the cursor. While stuck, the loop point will move with the cursor until ![Loop](images/buttons/loop.svg) is pressed again, which will set it wherever the cursor is. If the cursor is within the two loop points, rather than on one of them, both points will "stick" to the cursor and the loop can be moved in it's entirety.

### 1.7 Copy/Paste

You can copy currently selected notes by pressing ![Copy](images/buttons/copy.svg). When


### 1.8 Playback

#### a. Playing

#### b. Recording

### 1.9 Status Icons

The top of the Main Sequence displays the loop points, the current **Status Icons**, and a **tooltip** for the last action the user did (hopefully making it easier to understand the control scheme).

Different status icons, each signifiying that a specific time-based event is taking place, will be displayed above the Main Sequence. Below is a list of all the status icons and what they signify:

![1-Shot Recording](images/top_icons/1shot_rec.jpg) --> [One-shot recording](#b-recording) mode is on.

![Arp](images/top_icons/arp.gif) --> The [Arpeggiator](#7-arpeggiator) is on.

![Autotrack](images/top_icons/autotrack.jpg) --> An [Autotrack](#5-autotracks) is active.



# 2. Track Editor
![Track Editor](images/trackedit.gif)

# 3. Note Editor
![Note Editor](images/noteedit.gif)


# 4. Menus

## 4.1 Main Menu

## 4.2 Autotrack Menu

## 4.3 Loop Menu

## 4.4 Instrument Menu

## 4.5 FX Menu

## 4.6 Settings

## 4.7 MIDI Menu
### a. Routing
### b. CV
### c. Thru

## 4.8 File Menu
The Stepchild stores sequences on its internal flash storage with the '.child' extension. There is also a settings file that can't be browsed directly from the File Menu but *can* be updated in the [Settings Menu](#46-settings).

## 4.9 Clock Menu
![Clock Menu](images/ClockMenu.jpg)
The Stepchild's **Clock Menu** has 3 parameters that can be experimented with to affect the timing of the sequence. 

### a. BPM 
The first, BPM, represents the Beats-per-minute (technically, 1/4 notes/minute or 24 timesteps/minute) of the sequence. Twisting ![**A**](images/buttons/A.svg) will increase or decrease the BPM by 10, while twisting ![**B**](images/buttons/B.svg) or holding shift while turning ![**A**](images/buttons/A.svg) will increase or decrease the BPM by 1.

### b. Swing
The Stepchild has a pretty capable Swing feature which generates offsets for its internal clock based on a Swing function. As of now, the swing function is a pure sine wave that can have its period and amplitude edited on the second tab of the clock menu. Pressing ![Select](images/buttons/select.svg) will toggle Swing on or off, turning ![**A**](images/buttons/A.svg) will change the period of the Swing curve, and turning ![**B**](images/buttons/B.svg) will change the amplitude of the curve.

###### Note: you can invert the swing curve! Experiment with it, I think the flexible clock is the best feature of the Stepchild.

### c. Clock Source
The Stepchild can either use its internal clock, or wait for an external clock to tell it time has passed with a **MIDI Clock** message. When you're on the third tab of the Clock Menu, pressing ![**Select**](images/buttons/select.svg) will swap between using the internal or external clock.

MIDI clock signals are different from MIDI timecode signals, and only MIDI clock signals will be processed by the Stepchild. Each clock signal denotes 1/24th of a 1/4 note, which is exactly one timestep in the Stepchild's sequence. When hooked up to an external clock source, this means the Stepchild will advance 1 timestep (when playing or recording) for each MIDI clock message it receives. Using more flexible timing features like Swing and the internal BPM CC messages won't work when you're in external clock mode!

When the Stepchild is using its internal clock, it sends out clock messages every timestep. It also sends MIDI start and stop messages when the sequence is stopped and started, and can be used to control a DAW or another sequencer like Ableton Live (or... another Stepchild) if the DAW or sequencer is configured for remote control.

# 5. Autotracks
## 5.1 Autotrack Editor
## 5.2 CC List
## 5.3 Internal CC Messages
## 5.4 Ableton Issues

# 6. Loops
## 6.1 Types of Loops
## 6.2 Editing within a Loop

# 7. Arpeggiator
## 7.1 Setting Step Lengths
## 7.2 Modifiers

# 8. Randomizer
## 8.1 Randomizer Parameters
## 8.2 Randomizing an Area

# 9. Instruments
## 9.1 XY
## 9.2 Storm
## 9.3 Knobs
## 9.4 Keys
## 9.5 Drumpads
## 9.6 MIDI to CV
## 9.7 Rattle
## 9.8 Live Loop

# 10. FX
## 10.1 Quantize
## 10.2 Humanize
## 10.3 Strum
## 10.4 Echo
## 10.5 Reverse
## 10.6 Warp

# 11. Files

## 11.1 Quicksaving
Selecting the quicksave icon in the [Main Menu](#23-main-menu) allows you to quicksave your sequence without going into the File Menu. If an older version of the current sequence has already been written to the Stepchild’s flash, quicksaving overwrites the old file with the current version. Holding ![**Shift**](images/buttons/shift.svg) while quicksaving restores the previously saved version of the sequence. The quicksave feature is designed to be used like a save state for making backups! I highly recommend quicksaving every once in a while and before doing something drastic to your sequence.

If the current sequence hasn’t been saved before, quicksaving will prompt you to enter a filename and save the sequence.

## 11.2 File Browsing

## 11.3 Uploading to a Computer

# 12. Console
