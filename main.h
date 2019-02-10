#ifndef _main_H_
#define _main_H_

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
#define LED_SDI    11
#define LED_SCK    12

#define BTN_PAUSE        27
#define BTN_SOUND        28
#define BTN_MUSIC        29
#define BTN_RESET        30
#define BTN_ROTATE_LEFT  31
#define BTN_ROTATE_RIGHT 20
#define BTN_LEFT         23
#define BTN_RIGHT        22
#define BTN_DOWN         21

#define PI_CLSD 13
#define PI_OPEN 14

#define M_ENABLE 24
#define M_CTRL_1 25
#define M_CTRL_2 26

// button repeat
#define REPEAT_DELAY 400
#define REPEAT_INTERVAL 50

// memory address mapping
#define EE_ADDR_MUSIC 0
#define EE_ADDR_SOUND EE_ADDR_MUSIC + sizeof(bool)
#define EE_ADDR_TRAY EE_ADDR_SOUND + sizeof(bool)

// states
#define STATE_CATRIS_LOOP 0
#define STATE_CATRIS_ONCE 1
#define STATE_TETRIS      2

// macros
#define canvasWidth() LEDS_PER_ROW
#define canvasHeight() (NUM_LEDS / LEDS_PER_ROW)
#define clrscr() clearCanvas(&setCanvas, 0, 0, canvasWidth(), canvasHeight())

// functions
void playMusic(const void *track);
void stopMusic();
void playGameMusic();
void playBeepUpSound();
void playBeepDownSound();
void playButtonPressVibra();
void playGameOverVibra();
void tetrisEvent(TetrisEvent event, uint8_t data);
bool buttonRepeat(bool reset);
void showPauseSign();
bool isTetris();
void showTetris();
bool isCatris();
void showCatris(bool loop);
void setCanvas(int8_t x, int8_t y, uint8_t r, uint8_t g, uint8_t b);
uint8_t progMemRead(uint8_t* addr);

#endif
