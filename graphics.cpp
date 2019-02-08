#include "graphics.h"

void transitionColor(
		uint8_t r1, uint8_t g1, uint8_t b1,
		uint8_t r2, uint8_t g2, uint8_t b2,
		float percents, uint8_t* storage) {

	storage[0] = r1 + (r2 - r1) * percents;
	storage[1] = g1 + (g2 - g1) * percents;
	storage[2] = b1 + (b2 - b1) * percents;
}

void pulsateColor(
		uint8_t r1, uint8_t g1, uint8_t b1,
		uint8_t r2, uint8_t g2, uint8_t b2,
		float percents, uint8_t* storage) {

	percents = percents > .5f ? 2 - 2 * percents : 2 * percents;

	storage[0] = r1 + (r2 - r1) * percents;
	storage[1] = g1 + (g2 - g1) * percents;
	storage[2] = b1 + (b2 - b1) * percents;
}
