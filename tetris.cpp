#include "tetris.h"

constexpr Tetromino::Data Tetromino::data[7];

Tetromino::Tetromino(Type _type, Rotation _rotation, uint8_t _x, uint8_t _y):
	type(_type), rotation(_rotation), x(_x), y(_y) {}

const uint8_t* Tetromino::color() {
	return data[type].color;
}

uint8_t Tetromino::width() {
	return data[type].dimensions >> 4;
}

uint8_t Tetromino::height() {
	return data[type].dimensions & 0b1111;
}

int8_t Tetromino::spawnX() {
	return data[type].spawnCoords >> 4;
}

int8_t Tetromino::spawnY() {
	return -(data[type].spawnCoords & 0b1111);
}

bool Tetromino::isMino(uint8_t idx) {
	uint8_t i = idx / 8;
	if (i > 2) {
		return false;
	}
	return data[type].shapes[rotation][i] & 1 << (7 - idx % 8);
}

void Tetromino::rotateClockWise() {
	switch (rotation) {
	case rot_0:
		rotation = rot_R;
		break;
	case rot_R:
		rotation = rot_2;
		break;
	case rot_2:
		rotation = rot_L;
		break;
	case rot_L:
		rotation = rot_0;
		break;
	}
}

void Tetromino::rotateCounterClockWise() {
	switch (rotation) {
	case rot_0:
		rotation = rot_L;
		break;
	case rot_L:
		rotation = rot_2;
		break;
	case rot_2:
		rotation = rot_R;
		break;
	case rot_R:
		rotation = rot_0;
		break;
	}
}

void Tetromino::draw(tetDisplay display) {
	const uint8_t* col = color();

	uint8_t w = width();
	uint8_t h = height();

	for (uint8_t i = 0, n = w * h; i < n; ++i) {
		if (isMino(i)) {
			uint8_t r = i / w;
			uint8_t c = i % w;
			display(x + c, y + r, col[0], col[1], col[2]);
		}
	}
}

Tetris::Tetris(uint8_t _w,  uint8_t _h, tetDisplay _display):
	w(_w), h(_h), display(_display), tet(NULL) {

	tet = new Tetromino(Tetromino::Type::Z, Tetromino::Rotation::rot_0, 0, 0);
}

void Tetris::rotateClockWise() {
	if (tet != NULL) {
		tet->rotateClockWise();
	}
}

void Tetris::rotateCounterClockWise() {
	if (tet != NULL) {
		tet->rotateCounterClockWise();
	}
}

void Tetris::update() {
	// TODO remove
	for (int i = 0; i < w; ++i) {
		for (int j= 0; j < h; ++j) {
			display(i, j, 20, 20, 20);
		}
	}

	if (tet != NULL) {
		tet->draw(display);
	}
}
