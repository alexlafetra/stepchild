# Index
1. Introduction
    1. Overview
    2. Hardware
        1. Buttons
        2. MIDI Layout
        3. Screen
        4. USB
    3. Software
        1. Notes
        2. Tracks
        3. Internal clock
2. [Main Sequence](#1-main-sequence)
    1. [Moving the cursor](#11-moving-the-cursor)
    2. [Changing the view/Subdivision](#12-changing-the-view--subdivision)
    3. [Creating notes](#13-creating-notes)
    4. [Deleting & Muting Notes](#14-deleting-muting-notes)
    5. [Selecting notes]
    6. Moving loops
    7. Copy/paste
    8. Changing Tracks
    9. Status Icons
3. Menus
    1. [Track Menu](#21-track-menu)
        1. Utilities
    2. [Edit Menu](#22-edit-menu)
        1. Moving Notes
        2. Changing Note Length
        3. Changing Note Velocity
        4. Changing Note Probability
        5. Quantizing Notes
        6. Humanizing Notes
        7. QuickFX
    3. Main Menu
    4. Autotrack Menu
    5. Loop Menu
    6. Instrument Menu
    7. Settings Menu
    8. Sequence
    9. System
    10. FX Menu
    11. MIDI Menu
        1. Routing
        2. CV
        3. Thru
    12. File Menu
    13. [Clock Menu](#213-clock-menu)
        1. [BPM](#a-bpm)
        2. [Swing](#b-swing)
        3. [Clock Source](#c-clock-source)
4. Autotracks
    1. Editor Controls
    2. CC List
    3. Internal CC Messages
    4. Note about Ableton
5. Loops
    1. Types of loops
    2. Editing within Loop
6. Arpeggiator
    1. Step Lengths
    2. Modifiers
7. Randomizer
    1. Randomizer Settings
    2. Selecting an Area
8. Instruments
    1. XY
    2. Storm
    3. Knobs
    4. Keys
    5. Drumpads
    6. MIDI to CV
    7. Rattle
    8. Live Loop
9. FX
    1. Quantize
    2. Humanize
    3. Strum
    4. Echo
    5. Reverse
    6. Warp
10. [Quicksave](#10-quicksave)
11. Console

# Introduction
### Overview
### Hardware
### Software


# 1. Main Sequence
The first thing you see after the Stepchild boots up will be the Main Sequencer. This is the “homepage” of the Stepchild where you can directly edit a sequence and access applications and menus, and always displays a baby carriage in the top left corner that bounces to the BPM of the sequence.

![The Main Sequence homepage](images/MainSequence.jpg)

The main sequence is displayed like a piano roll or traditional DAW with time on the X axis and separate tracks on the Y axis. Track pitches are shown on the left as either MIDI note numbers or western chromatic pitches, and the sequence is displayed on the right. You can view the channel of each track by holding ![**Shift**](images/buttons/shift_icon.svg). At the top, the current [**loop points**](#15-moving-loops) and [**status icons**](#18-status-icons), like battery life,  are shown.

### 1.1 Moving the cursor

The **cursor** is positioned at a specific timestep (horizontal location) and track (vertical location). You can change which track the cursor is on by moving the [**Joystick**](#hardware) up or down, which will automatically send a MIDI note with the new active track’s pitch to the Stepchild’s MIDI ports.
The cursor can be moved through time by moving the Joystick side-to-side, which will move the cursor to the left or right to the next closest subdivision. Holding shift will move the cursor exactly 1 timestep in that direction.

### 1.2 Changing the View & Subdivision

The sequence display will automatically adjust depending on where the cursor is and how zoomed in the view is. Turning ![**B**](images/buttons/B_icon.svg) will multiply or divide the view subdivision by 2, and holding shift while turning ![**B**](images/buttons/B_icon.svg) will switch between triplet and base-2 subdivisions. Turning ![**A**](images/buttons/A_icon.svg) will zoom the sequence in or out by 2x and also increase or decrease the subdivision, allowing for smoothly zooming in and out to place notes with more precision.

Clicking ![**A**](images/buttons/A_icon.svg) will open the [Edit Menu](#22-edit-menu), and clicking ![**B**](images/buttons/B_icon.svg) will open the [Track Menu](#21-track-menu).

### 1.3 Creating Notes
Pressing ![**New**](images/buttons/new_icon.svg) will create a note 1 subdivision long and move the cursor to the end of the new note. If ![**New**](images/buttons/new_icon.svg) is held while the cursor is moved, the note placed will be extended until ![**New**](images/buttons/new_icon.svg) is released. If a note is created on top of another note, it will truncate the blocking note and insert itself into the sequence as long as it wasn't attempted to be placed on the exact timestep the blocking note starts on; new notes can't delete notes that are blocking them.

The default velocity of placed notes can be quickly changed by holding **Shift** and moving the **Joystick** up or down. If the cursor is over a note, or if any notes are selected, the velocity of that note and all selected notes will be changed instead of the default velocity, allowing you to tweak note velocities without going into the edit menu.

### 1.4 Deleting & Muting Notes
You can delete notes by hovering over them and pressing, you guessed it, ![Delete](images/buttons/delete_icon.svg). Instead of deleting notes, ![Delete](images/buttons/delete_icon.svg) + ![Shift](images/buttons/shift_icon.svg) will **mute** a note. Muted notes won't be sent as MIDI data, but can still be edited and used as the target of [**FX applications**](). 

Both muting and deleting will apply to *all* selected notes, so make sure to deselect notes you don't want to delete!

### 1.5 Selecting Notes
Most operations that affect a single note will also apply to other notes that are **selected**. Notes can be selected and deselected by placing the cursor on them and pressing ![Select](images/buttons/select_icon.svg), or by holding down ![Select](images/buttons/select_icon.svg) while moving the **Joystick** to create a selection box. By default, selecting is *additive*, meaning each note you select gets added to the current selection.To reset all selected notes you can press ![Select](images/buttons/select_icon.svg) + ![Shift](images/buttons/shift_icon.svg) over a note or a blank space.

### 1.6 Moving Loops
### 1.7 Copy/Paste
### 1.8 Changing Tracks
### 1.9 Status Icons

# 2. Menus
## 2.1 Track Menu
![Track Menu](images/TrackMenu.jpg)
## 2.2 Edit Menu
![Edit Menu](images/EditMenu.jpg)
## 2.13 Clock Menu
![Clock Menu](images/ClockMenu.jpg)
The Stepchild's **Clock Menu** has 3 parameters that can be experimented with to affect the timing of the sequence. 
### a. BPM 
The first, BPM, represents the Beats-per-minute (technically, 1/4 notes/minute or 24 timesteps/minute) of the sequence. Twisting ![**A**](images/buttons/A_icon.svg)will increase or decrease the BPM by 10, while twisting ![**B**](images/buttons/B_icon.svg) or holding shift while turning ![**A**](images/buttons/A_icon.svg)will increase or decrease the BPM by 1.
### b. Swing
### c. Clock Source

The Stepchild can either use its internal clock, or wait for an external clock to tell it time has passed with a **MIDI Clock** message. Pressing ![**Select**](images/buttons/select_icon.svg) will swap between using an internal or external clock.



# 10. Quicksave
Selecting the quicksave icon  allows you to quicksave your sequence without going into the File Menu. If an older version of the current sequence has already been written to the Stepchild’s flash, quicksaving overwrites the old file with the current version. Holding ![**Shift**](images/buttons/shift_icon.svg) while quicksaving restores the previously saved version of the sequence. The quicksave feature is designed to be used like a save state for making backups! I highly recommend quicksaving every once in a while and before doing something drastic to your sequence.

If the current sequence hasn’t been saved before, quicksaving will prompt you to enter a filename and save the sequence.
