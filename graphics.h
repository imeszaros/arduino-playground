#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#define charHeight(header) (header >> 4)
#define bytesPerChar(header) (header & 0b1111)
#define charIndex(chr, bytesPerChar) (1 + ((chr - 32) * bytesPerChar))
#define charWidth(charData) (charData >> 4)
#define charPixelIndex(x, y, charWidth) ((y * charWidth + x) + 4)
#define charPixelByteIndex(charIndex, charPixelIndex) (charIndex + charPixelIndex / 8)
#define charPixel(charByte, charPixelIndex) (charByte >> (7 - charPixelIndex % 8) & 0b1)

#define spriteX(i) (i >> 4)
#define spriteY(i) (i & 0b1111)

typedef void (*canvas) (int8_t x, int8_t y, uint8_t r, uint8_t g, uint8_t b);

typedef uint8_t (*fontDataReader) (uint8_t* addr);

typedef uint8_t (*spriteDataReader) (uint8_t* addr);

void transitionColor(
		uint8_t r1, uint8_t g1, uint8_t b1,
		uint8_t r2, uint8_t g2, uint8_t b2,
		float percents, uint8_t* storage);

void pulsateColor(
		uint8_t r1, uint8_t g1, uint8_t b1,
		uint8_t r2, uint8_t g2, uint8_t b2,
		float percents, uint8_t* storage);

void drawChar(canvas canvas, unsigned char c, int8_t x, int8_t y,
		fontDataReader fontDataReader, const uint8_t* font, uint8_t* color);

void drawSprite(canvas canvas, const uint8_t palette[][3], spriteDataReader spriteDataReader, uint8_t* sprite, int8_t x, int8_t y);

void hsv2rgb(double H, double S, double V, uint8_t* output);

class ScrollText {

public:

	ScrollText(uint8_t x, uint8_t y, uint8_t width, fontDataReader fontDataReader, uint8_t* font);

	void setClearBackground(bool clearBackground);
	void setText(const char* text);
	void draw(canvas canvas, uint8_t * color);
	bool scroll();

private:

	uint8_t _x;
	uint8_t _y;
	uint8_t _width;
	fontDataReader _fontDataReader;
	uint8_t* _font;
	uint8_t _charHeight;
	uint8_t _bytesPerChar;
	bool _clearBackground;
	const char* _text;
	uint16_t _textLength;
	uint16_t _position;
	int8_t _offset;
};

class Animation {

public:

	const uint8_t frameCount;

	Animation(uint8_t frameCount);

	void setFrame(uint8_t idx, unsigned long duration, uint8_t* sprite);
	unsigned long getDuration(uint8_t idx);
	uint8_t* getSprite(uint8_t idx);

private:

	unsigned long* _durations;
	uint8_t** _sprites;
};

#endif
