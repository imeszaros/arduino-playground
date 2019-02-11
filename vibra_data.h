#ifndef __VIBRA_DATA_H
#define __VIBRA_DATA_H

#include <Arduino.h>

const uint8_t PROGMEM buttonPressVibra[] = {

	1, 1
};

const uint8_t PROGMEM gameOverVibra[] = {

	2, 106, 70
};

const uint8_t PROGMEM singleRowClearVibra[] = {

	1, 14
};

const uint8_t PROGMEM doubleRowClearVibra[] = {

	3, 14, 51, 14
};

const uint8_t PROGMEM tripleRowClearVibra[] = {

	5, 89, 51, 89, 51, 89
};

const uint8_t PROGMEM tetrisVibra[] = {

	7, 83, 51, 83, 51, 83, 51, 83
};

const uint8_t PROGMEM levelUpVibra[] = {

	5, 83, 51, 83, 51, 83
};

const uint8_t PROGMEM surpriseVibra[] = {

	7, 83, 12, 83, 12, 83, 89, 16
};

#endif
