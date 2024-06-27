/*
--------------------------------------
          Pin Definitions
--------------------------------------
*/

//MIDI I/O
#define MIDI_OUT_1 0
#define MIDI_IN 1
#define MIDI_OUT_4 2
#define MIDI_OUT_3 3
#define MIDI_OUT_2 4

//CV Outputs
#define CV3_PIN 5
#define CV2_PIN 6
#define CV1_PIN 10

//I2C bus for the LB and screen
#define I2C_SDA 8
#define I2C_SCL 9

#define SPI0_RX 16
#define SPI0_TX 7
#define OLED_CS 17
#define OLED_DC 6
#define OLED_RESET 5
#define SPI0_SCK 18

#define BUTTONS_CLOCK_ENABLE 12
#define BUTTONS_LOAD 13
#define BUTTONS_CLOCK_IN 14
#define BUTTONS_DATA 15

//Encoders
#define B_BUTTON 11//moved from pin 17
#define B_CLOCK 19
#define B_DATA 10//moved from pin 18
#define A_BUTTON 20
#define A_CLOCK 22
#define A_DATA 21

//Joystick
#define JOYSTICK_X 27
#define JOYSTICK_Y 26

//Misc. Hardware
#define VOLTAGE_PIN 29
#define USB_PIN 24
#define ONBOARD_LED 25