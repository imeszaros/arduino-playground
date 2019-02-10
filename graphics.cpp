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

void drawChar(canvas canvas, unsigned char c, int8_t x, int8_t y,
		fontDataReader fontDataReader, uint8_t* font, uint8_t* color) {

	uint8_t header = fontDataReader(font);
	uint16_t charIndex = charIndex(c, bytesPerChar(header));
	uint8_t charWidth = charWidth(fontDataReader(font + charIndex));

	for (uint8_t _y = 0, h = charHeight(header); _y < h; ++_y) {
		for (uint8_t _x = 0; _x < charWidth; ++_x) {
			uint8_t charPixelIndex = charPixelIndex(_x, _y, charWidth);

			if (charPixel(fontDataReader(font + charPixelByteIndex(charIndex, charPixelIndex)), charPixelIndex)) {
				canvas(x + _x, y + _y, color[0], color[1], color[2]);
			}
		}
	}
}

void drawSprite(canvas canvas, const uint8_t palette[][3], spriteDataReader spriteDataReader, uint8_t* sprite, int8_t x, int8_t y) {
	uint8_t* addr = sprite;

	do {
		const uint8_t* color = palette[spriteDataReader(addr++)];
		uint8_t pixelCount = spriteDataReader(addr++);

		if (pixelCount == 0) {
			break;
		}

		for (uint8_t p = 0; p < pixelCount; ++p) {
			uint8_t coords = spriteDataReader(addr++);
			canvas(x + spriteX(coords), y + spriteY(coords), color[0], color[1], color[2]);
		}
	} while(1);
}

void hsv2rgb(double H, double S, double V, uint8_t* output) {
	double r = 0, g = 0, b = 0;

	if (S == 0) {
		r = V;
		g = V;
		b = V;
	} else {
		int i;
		double f, p, q, t;

		if (H == 360) {
			H = 0;
		} else {
			H = H / 60;
		}

		i = (int) trunc(H);
		f = H - i;

		p = V * (1.0 - S);
		q = V * (1.0 - (S * f));
		t = V * (1.0 - (S * (1.0 - f)));

		switch (i) {
		case 0:
			r = V;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = V;
			b = p;
			break;
		case 2:
			r = p;
			g = V;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = V;
			break;
		case 4:
			r = t;
			g = p;
			b = V;
			break;
		default:
			r = V;
			g = p;
			b = q;
			break;
		}

	}

	output[0] = r * 255;
	output[1] = g * 255;
	output[2] = b * 255;
}

void clearCanvas(canvas canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
	for (uint8_t  _x = x; _x < width; ++_x) {
		for (uint8_t _y = y; _y < height; ++_y) {
			canvas(_x, _y, 0, 0, 0);
		}
	}
}

ScrollText::ScrollText(uint8_t x, uint8_t y, uint8_t width, fontDataReader fontDataReader, uint8_t* font):
		_x(x), _y(y), _width(width), _fontDataReader(fontDataReader), _font(font),
		_clearBackground(true), _text(NULL), _textLength(0), _position(0), _offset(0) {

	uint8_t header = fontDataReader(font);

	_charHeight = charHeight(header);
	_bytesPerChar = bytesPerChar(header);
}

void ScrollText::setClearBackground(bool clearBackground) {
	_clearBackground = clearBackground;
}

void ScrollText::setText(const char* text) {
	_text = text;
	_textLength = strlen(_text);

	_position = 0;
	_offset = 0;
}

void ScrollText::draw(canvas canvas, uint8_t * color) {
	if (_clearBackground) {
		clearCanvas(canvas, _x, _y, _x + _width, _y + _charHeight);
	}

	uint16_t pos = _position;

	for (int8_t x = _x + _offset, w = _x + _width; x < w;) {
		drawChar(canvas, _text[pos], x, _y, _fontDataReader, _font, color);

		x += charWidth(_fontDataReader(_font + charIndex(_text[pos], _bytesPerChar))) + 1;

		if (pos < _textLength - 1) {
			pos++;
		} else {
			pos = 0;
		}
	}
}

bool ScrollText::scroll() {
	int8_t charWidth = charWidth(_fontDataReader(_font + charIndex(_text[_position], _bytesPerChar)));

	if (_offset < -charWidth) {
		_offset = -1;

		if (_position < _textLength - 1) {
			_position++;

			return false;
		} else {
			_position = 0;

			return true;
		}
	} else {
		_offset--;

		return false;
	}
}

Animation::Animation(uint8_t frameCount): frameCount(frameCount) {
	_durations = (unsigned long *) malloc(sizeof(unsigned long) * frameCount);
	_sprites = (uint8_t**) malloc(sizeof(uint8_t*) * frameCount);
}

void Animation::setFrame(uint8_t idx, unsigned long duration, uint8_t* sprite) {
	_durations[idx] = duration;
	_sprites[idx] = sprite;
}

unsigned long Animation::getDuration(uint8_t idx) {
	return _durations[idx];
}

uint8_t* Animation::getSprite(uint8_t idx) {
	return _sprites[idx];
}
