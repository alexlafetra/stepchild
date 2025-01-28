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
#define CV4_PIN 17
#define CV3_PIN 5
#define CV2_PIN 6
#define CV1_PIN 15

//I2C bus for the MCP23017's
#define I2C_SDA 8
#define I2C_SCL 9

//SPI Bus for the screen
#define SPI1_RX 12
#define SPI1_TX 11
#define OLED_CS 13
#define OLED_DC 16
#define OLED_RESET 7
#define SPI1_SCK 10

//Encoders (these are hardware interrupts on the Pico)
#define B_CLOCK 19
#define B_DATA 18
#define A_CLOCK 22
#define A_DATA 21

//Joystick
#define JOYSTICK_X 27
#define JOYSTICK_Y 26

//Misc. Hardware (These are onboard pins)
#define VOLTAGE_PIN 29
#define USB_PIN 24
#define ONBOARD_LED 25

//Extra GPIO Access
#define AUX_GPIO 20
#define AUX_ADC 28

//Deprecated pins from prototypes
// #define BUTTONS_CLOCK_ENABLE 12
// #define BUTTONS_LOAD 13
// #define BUTTONS_CLOCK_IN 14
// #define BUTTONS_DATA 15
// #define A_PRESS 20
// #define B_PRESS 17