#include "tetris.h"

constexpr Tetromino::_Data Tetromino::_data[];
constexpr uint8_t Tetromino::_srsOffsets[];

uint8_t* Tetromino::colorOf(Type type) {
	return (uint8_t*) _data[type].color;
}

Tetromino::Tetromino():
	type(Type::I), rotation(Rotation::rot0), x(0), y(0) {}

uint8_t* Tetromino::getColor() {
	return colorOf(type);
}

uint8_t Tetromino::getWidth() {
	return uint4_left(_data[type].dimensions);
}

uint8_t Tetromino::getHeight() {
	return uint4_right(_data[type].dimensions);
}

bool Tetromino::isMino(uint8_t idx) {
	uint8_t i = idx / 8;

	if (i > 2) {
		return false;
	}

	return _data[type].minos[rotation][i] & 1 << (7 - idx % 8);
}

uint8_t Tetromino::getSRSOffsetCount() {
	return _srsOffsets[_data[type].srsOffsets];
}

uint8_t Tetromino::getSRSOffset(Tetromino::Rotation rotation, uint8_t idx) {
	uint8_t count = getSRSOffsetCount();
	return _srsOffsets[_data[type].srsOffsets + 1 + rotation * count + idx];
}

void Tetromino::spawn(Tetromino::Type type) {
	this->type = type;
	rotation = Tetromino::Rotation::rot0;
	x = uint4_left(_data[type].spawnCoords);
	y = -uint4_right(_data[type].spawnCoords);
}

void Tetromino::draw(tetDisplay display, uint8_t* color) {
	if (color == NULL) {
		color = getColor();
	}

	uint8_t w = getWidth();
	uint8_t h = getHeight();

	for (uint8_t i = 0, n = minoCount(w, h); i < n; ++i) {
		if (isMino(i)) {
			display(minoX(x, w, i), minoY(y, w, i), color[0], color[1], color[2]);
		}
	}
}

Pile::Pile(uint8_t _width, uint8_t _height):
	_width(_width), _height(_height) {

	_data = (uint8_t*) malloc(sizeof(uint8_t) * _memSize());

	truncate();
}

bool Pile::isOccupied(uint8_t x, uint8_t y) {
	return _get(x, y) != Tetromino::Type::_;
}

void Pile::merge(Tetromino* tetromino) {
	uint8_t w = tetromino->getWidth();
	uint8_t h = tetromino->getHeight();

	for (uint8_t i = 0, n = minoCount(w, h); i < n; ++i) {
		if (tetromino->isMino(i)) {
			_set(minoX(tetromino->x, w, i), minoY(tetromino->y, w, i), tetromino->type);
		}
	}
}

void Pile::clearCompleteRows() {
	for (int8_t y = _height - 1; y >= 0; --y) {
		bool empty = true;
		bool full = true;

		for (uint8_t x = 0; x < _width; ++x) {
			if (_get(x, y) == Tetromino::Type::_) {
				full = false;
			} else {
				empty = false;
			}
		}

		if (empty) {
			break;
		}

		if (full) {
			uint8_t _y = y;

			for (; _y > 0; --_y) {
				bool empty = true;

				for (uint8_t _x = 0; _x < _width; ++_x) {
					Tetromino::Type type = _get(_x, _y - 1);

					_set(_x, _y, type);

					if (type != Tetromino::Type::_) {
						empty = false;
					}
				}

				if (empty) {
					break;
				}
			}

			if (_y == 0) {
				for (uint8_t _x = 0; _x < _width; ++_x) {
					_set(_x, _y, Tetromino::Type::_);
				}
			}

			++y;
		}
	}
}

void Pile::draw(tetDisplay display) {
	for (uint8_t x = 0; x < _width; ++x) {
		for (uint8_t y = 0; y < _height; ++y) {
			Tetromino::Type type = _get(x, y);

			if (type != Tetromino::Type::_) {
				const uint8_t* color = Tetromino::colorOf(type);
				display(x, y, color[0], color[1], color[2]);
			}
		}
	}
}

void Pile::truncate() {
	uint8_t empty = uint4_pack(Tetromino::Type::_, Tetromino::Type::_);

	for (uint8_t i = 0, n = _memSize(); i < n; ++i) {
		_data[i] = empty;
	}
}

uint8_t Pile::_cellCount() {
	return _width * _height;
}

uint8_t Pile::_memSize() {
	uint8_t cells = _cellCount();
	return cells / 2 + (cells % 2 != 0);
}

uint8_t Pile::_cell(uint8_t x, uint8_t y) {
	return y * _width + x;
}

Tetromino::Type Pile::_get(uint8_t x, uint8_t y) {
	uint8_t cell = _cell(x, y);
	uint8_t idx = cell / 2;

	return static_cast<Tetromino::Type>(cell % 2 == 0
			? uint4_left(_data[idx])
			: uint4_right(_data[idx]));
}

void Pile::_set(uint8_t x, uint8_t y, Tetromino::Type type) {
	uint8_t cell = _cell(x, y);
	uint8_t idx = cell / 2;

	if (cell % 2 == 0) {
		_data[idx] = uint4_pack(type, uint4_right(_data[idx]));
	} else {
		_data[idx] = uint4_pack(uint4_left(_data[idx]), type);
	}
}

Bag::Bag(unsigned int seed):
	_index(0) {

	for (uint8_t i = Tetromino::Type::I; i < Tetromino::Type::_; ++i) {
		_sequence[i] = static_cast<Tetromino::Type>(i);
	}

	srand(seed);
}

Tetromino::Type Bag::peek() {
	return _sequence[_index];
}

Tetromino::Type Bag::pop() {
	Tetromino::Type type = _sequence[_index];

	if (_index == TETROMINO_COUNT - 1) {
		shuffle();
	} else {
		_index++;
	}

	return type;
}

void Bag::shuffle() {
    for (uint8_t i = TETROMINO_COUNT - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        Tetromino::Type tmp = _sequence[j];
        _sequence[j] = _sequence[i];
        _sequence[i] = tmp;
    }

	_index = 0;
}

Tetris::Tetris(uint8_t _width,  uint8_t _height, unsigned int seed):
	_width(_width), _height(_height), _lastDrop(0), _speed(500), _gameOver(true),
	_dropping(false), _clearBackground(true), _ghostEnabled(true) {

	_tetromino = new Tetromino();
	_pile = new Pile(_width, _height);
	_bag = new Bag(seed);

	setGhostColor(255, 255, 255);
}

void Tetris::reset() {
	_pile->truncate();
	_bag->shuffle();
	_tetromino->spawn(_bag->pop());
	_gameOver = false;
}

bool Tetris::isGameOver() {
	return _gameOver;
}

Tetromino::Type Tetris::preview() {
	return _bag->peek();
}

bool Tetris::moveLeft() {
	return _move(-1, 0);
}

bool Tetris::moveRight() {
	return _move(1, 0);
}

bool Tetris::rotateClockWise() {
	switch (_tetromino->rotation) {
	case Tetromino::Rotation::rot0:
		return _rotate(Tetromino::Rotation::rotR);
	case Tetromino::Rotation::rotR:
		return _rotate(Tetromino::Rotation::rot2);
	case Tetromino::Rotation::rot2:
		return _rotate(Tetromino::Rotation::rotL);
	case Tetromino::Rotation::rotL:
		return _rotate(Tetromino::Rotation::rot0);
	default:
		return false;
	}
}

bool Tetris::rotateCounterClockWise() {
	switch (_tetromino->rotation) {
	case Tetromino::Rotation::rot0:
		return _rotate(Tetromino::Rotation::rotL);
	case Tetromino::Rotation::rotL:
		return _rotate(Tetromino::Rotation::rot2);
	case Tetromino::Rotation::rot2:
		return _rotate(Tetromino::Rotation::rotR);
	case Tetromino::Rotation::rotR:
		return _rotate(Tetromino::Rotation::rot0);
	default:
		return false;
	}
}

void Tetris::setDropping(bool dropping) {
	_speed = dropping ? 50 : 500;
}

void Tetris::setClearBackground(bool clearBackground) {
	_clearBackground = clearBackground;
}

void Tetris::setGhostEnabled(bool ghostEnabled) {
	_ghostEnabled = ghostEnabled;
}

void Tetris::setGhostColor(uint8_t r, uint8_t g, uint8_t b) {
	_ghostColor[0] = r;
	_ghostColor[1] = g;
	_ghostColor[2] = b;
}

void Tetris::update() {
	if (_gameOver) {
		return;
	}

	unsigned long now = millis();

	if (now - _lastDrop > _speed) {
		_lastDrop = now;

		if (!_move(0, 1)) {
			_pile->merge(_tetromino);
			_pile->clearCompleteRows();

			_tetromino->spawn(_bag->pop());

			if (!_checkTetromino()) {
				_gameOver = true;
			}
		}
	}
}

void Tetris::draw(tetDisplay display) {
	if (_clearBackground) {
		for (uint8_t x = 0; x < _width; ++x) {
			for (uint8_t y = 0; y < _height; ++y) {
				display(x, y, 0, 0, 0);
			}
		}
	}

	if (!_gameOver) {
		if (_ghostEnabled) {
			uint8_t x = _tetromino->x;
			uint8_t y = _tetromino->y;

			while (_move(0, 1));

			_tetromino->draw(display, _ghostColor);

			_tetromino->x = x;
			_tetromino->y = y;
		}

		_tetromino->draw(display, NULL);
	}

	_pile->draw(display);
}

bool Tetris::_checkTetromino() {
	uint8_t w = _tetromino->getWidth();
	uint8_t h = _tetromino->getHeight();

	for (uint8_t i = 0, n = minoCount(w, h); i < n; ++i) {
		if (_tetromino->isMino(i)) {
			int8_t x = minoX(_tetromino->x, w, i);
			int8_t y = minoY(_tetromino->y, w, i);

			if (x < 0 || x >= _width || y >= _height
					|| (y >= 0 && _pile->isOccupied(x, y))) {

				return false;
			}
		}
	}

	return true;
}

bool Tetris::_move(uint8_t x, uint8_t y) {
	uint8_t oldX = _tetromino->x;
	uint8_t oldY = _tetromino->y;

	_tetromino->x += x;
	_tetromino->y += y;

	if (_checkTetromino()) {
		return true;
	} else {
		_tetromino->x = oldX;
		_tetromino->y = oldY;
		return false;
	}
}

bool Tetris::_rotate(Tetromino::Rotation to) {
	uint8_t oldX = _tetromino->x;
	uint8_t oldY = _tetromino->y;

	Tetromino::Rotation from = _tetromino->rotation;
	_tetromino->rotation = to;

	uint8_t srsOffsetCount = _tetromino->getSRSOffsetCount();

	for (uint8_t i = 0; i < srsOffsetCount; ++i) {
		uint8_t offsFrom = _tetromino->getSRSOffset(from, i);
		uint8_t offsTo = _tetromino->getSRSOffset(to, i);

		int8_t offX = srsOffsetX(offsFrom) - srsOffsetX(offsTo);
		int8_t offY = -srsOffsetY(offsFrom) + srsOffsetY(offsTo);

		_tetromino->x += offX;
		_tetromino->y += offY;

		if (_checkTetromino()) {
			return true;
		}
	}

	_tetromino->x = oldX;
	_tetromino->y = oldY;

	_tetromino->rotation = from;

	return false;
}
