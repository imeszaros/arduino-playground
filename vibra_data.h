#ifndef __VIBRA_DATA_H
#define __VIBRA_DATA_H

#include <Arduino.h>

const uint8_t PROGMEM buttonPressVibra[] = {

	1, 1
};

const uint8_t PROGMEM gameOverVibra[] = {

	1, 1
};

const uint8_t PROGMEM singleRowClearVibra[] = {

	1, 7
};

const uint8_t PROGMEM doubleRowClearVibra[] = {

	2, 7, 7
};

const uint8_t PROGMEM tripleRowClearVibra[] = {

	3, 7, 7, 7
};

const uint8_t PROGMEM tetrisVibra[] = {

	4, 52, 52, 52, 52
};

const uint8_t PROGMEM levelUpVibra[] = {

	3, 89, 89, 89
};

#endif
