# Index
1. Introduction
    1. Capability Overview
    2. Hardware
        1. Buttons
        2. MIDI Layout
        3. Screen
        4. USB
    3. Software
        1. Notes
        2. Tracks
        3. Internal clock
2. [Main Sequence](#1.-main-sequencer)
    1. Moving the cursor
    2. Changing the view/Subdivision
    3. Creating and deleting notes
    4. Selecting notes
    5. Moving loops
    6. Copy/paste
    7. Changing Tracks
    8. Status Icons
3. Menus
    1. Track Menu
        1. Utilities
    2. Edit Menu
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
    13. Clock Menu
        1. BPM
        2. Swing
        3. Source
    14. Autotracks
        1. Editor Controls
        2. CC List
        3. Note about Ableton
    15. Loops
        1. Types of loops
        2. Editing within Loop
    16. Arpeggiator
        1. Step Lengths
        2. Modifiers
    17. Randomizer
        1. Randomizer Settings
        2. Selecting an Area
4. Instruments
    1. XY
    2. Storm
    3. Knobs
    4. Keys
    5. Drumpads
    6. MIDI to CV
    7. Rattle
    8. Live Loop
5. FX
    1. Quantize
    2. Humanize
    3. Strum
    4. Echo
    5. Reverse
    6. Warp
6. Quicksave
7. Console

# Introduction
    1. Hardware
    2. Software


# 1. Main Sequence
The first thing you see after the Stepchild boots up will be the Main Sequencer. This is the “homepage” of the Stepchild where you can directly edit a sequence and access applications and menus, and always displays a baby carriage in the top left corner that bounces to the BPM of the sequence.



Track pitches are shown on the left as either MIDI note numbers or western chromatic pitches, and the sequence is displayed on the right. If shift is held while viewing the main sequence, the track channel will also be displayed next to each track. At the top, the current loop points and status icons are shown.

### Moving the cursor

The cursor is positioned at a specific timestep (horizontal location) and track (vertical location). You can change which track the cursor is on by moving the Joystick up or down, which will automatically send a MIDI note with the new active track’s pitch to the Stepchild’s MIDI ports.
The cursor can be moved through time by moving the Joystick side-to-side, which will move the cursor to the left or right to the next closest subdivision. Holding shift will move the cursor exactly 1 timestep in that direction.

### Changing the View & Subdivision

The sequence display will automatically adjust depending on where the cursor is and how zoomed in the view is. Turning B will multiply or divide the view subdivision by 2, and holding shift while turning B will switch between triplet and base-2 subdivisions. Turning A will zoom the sequence in or out by 2x and also increase or decrease the subdivision, allowing for smoothly zooming in and out to place notes with more precision.

Clicking A will open the Edit Menu, and clicking B will open the Track Menu.

### Creating and Deleting Notes
Pressing New () will place a note in the sequence and move the cursor forward 1 subdivision. If new is held while the cursor is moved, the note placed will be extended until new is released. 
### Selecting Notes
### Moving Loops
### Copy/Paste
### Changing Tracks
### Status Icons
# 10. Quicksave
Selecting the quicksave icon  allows you to quicksave your sequence without going into the File Menu. If an older version of the current sequence has already been written to the Stepchild’s flash, quicksaving overwrites the old file with the current version. Holding shift while quicksaving restores the previously saved version of the sequence. The quicksave feature is designed to be used like a save state for making backups! I highly recommend quicksaving every once in a while and before doing something drastic to your sequence.

If the current sequence hasn’t been saved before, quicksaving will prompt you to enter a filename and save the sequence.
