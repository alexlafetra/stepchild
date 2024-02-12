//MIDI PINS
const uint8_t rxPin = 1;
const uint8_t txPin_1 = 0;
const uint8_t txPin_2 = 4;
const uint8_t txPin_3 = 3;
const uint8_t txPin_4 = 2;

const uint8_t Screen_SDA = 8;
const uint8_t Screen_SCL = 9;

//ENCODERS
const unsigned char encoderA_Button = 17;
const unsigned char note_clk_Pin = 18;
const unsigned char note_data_Pin = 19;

const unsigned char encoderB_Button = 20;
const unsigned char track_clk_Pin = 21;
const unsigned char track_data_Pin = 22;

//SHIFT REGISTERS
// const unsigned char dataPin_LEDS = 9;//V0.4
const unsigned char dataPin_LEDS = 7;
const unsigned char latchPin_LEDS = 10;
const unsigned char clockPin_LEDS = 11;

const unsigned char buttons_clockEnable = 16;
const unsigned char buttons_dataIn = 13;
const unsigned char buttons_load = 14;
const unsigned char buttons_clockIn = 15;
//doubling up on load, clockin, and clockenable (since i'll read from them simultaneously)
const unsigned char stepButtons_dataIn = 12;

//JOYSTICK
const unsigned char y_Pin = 26;
const unsigned char x_Pin = 27;

//MISC. HARDWARE
const unsigned char Vpin = 29;
const unsigned char usbPin = 24;

const unsigned char ledPin = 6;
const unsigned char onboard_ledPin = 25;