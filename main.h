#ifndef _main_H_
#define _main_H_

//#define BREADBOARD

#include "Arduino.h"

// debug mode
#define DEBUG false

// display
#define SPI_UART1_DATA  11
#define SPI_UART1_CLOCK 12
#define NUM_LEDS        200
#define LEDS_PER_ROW    10
#define LED_TYPE        SK9822
#define COLOR_ORDER     BGR
#define LED_FPS         60
#define BRIGHTNESS		30
#include <FastLED.h>

// audio
#include "pmf_player.h"
#include "pmf_data.h"

// audio data
#include "audio_data.h"

// vibration data
#include "vibra_data.h"

// vibra-motor support
#include <Adafruit_DRV2605.h>

// tray control
#include "tray.h"

// non-volatile memory
#include <EEPROM.h>

// button and sensor debouncing
#include <Bounce2.h>

// catris
#include "catris.h"

// game engine
#include "tetris.h"

// random seed
#include "entropy.h"

// pin configuration
#define LBO 10

#define LED_SDI    11
#define LED_SCK    12

#ifdef BREADBOARD
#define BTN_PAUSE        27
#define BTN_SOUND        28
#define BTN_MUSIC        29
#define BTN_RESET        30
#define BTN_ROTATE_LEFT  31
#define BTN_ROTATE_RIGHT 20
#define BTN_LEFT         23
#define BTN_RIGHT        22
#define BTN_DOWN         21
#define BTN_HIGH_SCORE   19
#else
#define BTN_PAUSE        29
#define BTN_SOUND        19
#define BTN_MUSIC        20
#define BTN_RESET        28
#define BTN_ROTATE_LEFT  30
#define BTN_ROTATE_RIGHT 22
#define BTN_LEFT         31
#define BTN_RIGHT        23
#define BTN_DOWN         21
#define BTN_HIGH_SCORE   27
#endif

#define PI_CLSD 13
#define PI_OPEN 14

#define M_ENABLE 24
#define M_CTRL_1 25
#define M_CTRL_2 26

// button repeat
#define REPEAT_DELAY 400
#define REPEAT_INTERVAL 50

// memory address mapping
#define EE_ADDR_MUSIC    0
#define EE_ADDR_SOUND    (EE_ADDR_MUSIC + sizeof(byte))
#define EE_ADDR_SURPRISE (EE_ADDR_SOUND + sizeof(bool))
#define EE_ADDR_HIGH_SCORE (EE_ADDR_SURPRISE + sizeof(bool))

// states
#define STATE_CATRIS_LOOP 0
#define STATE_CATRIS_ONCE 1
#define STATE_TETRIS      2

// score triggers
#define SCORES_SURPRISE_TEASER 2500
#define SCORES_SURPRISE_REVEAL 3500

// low battery detection
#define LOW_BAT_DETECTION_LIMIT 30
#define MILLIS_BATTERY_CHECK_INTERVAL 1000

// secret functions timing
#define MILLIS_SURPRISE_STATE   5000
#define MILLIS_SURPRISE_TOGGLE 10000
#define MILLIS_TRAY_TOGGLE     15000

// clear high score timing
#define MILLIS_CLEAR_HIGH_SCORE 5000

// macros
#define canvasWidth() LEDS_PER_ROW
#define canvasHeight() (NUM_LEDS / LEDS_PER_ROW)
#define clrscr() clearCanvas(&setCanvas, 0, 0, canvasWidth(), canvasHeight())

// functions
void playMusic();
void playBeepUpSound();
void playBeepDownSound();
void playSuccessSound();
void playVibra(const uint8_t* pattern);
void tetrisEvent(TetrisEvent event, uint8_t data);
bool buttonRepeat(bool reset);
void showPauseSign();
void resetTetris();
bool isTetris();
void showTetris();
bool isCatris();
void showCatris(bool loop);
void setCanvas(int8_t x, int8_t y, uint8_t r, uint8_t g, uint8_t b);
uint8_t progMemRead(uint8_t* addr);
const char* randomText(uint8_t count, ...);

#endif
