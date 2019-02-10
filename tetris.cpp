#include "tetris.h"

constexpr Tetromino::_Data Tetromino::_data[];
constexpr uint8_t Tetromino::_srsOffsets[];

uint8_t* Tetromino::colorOf(Type type) {
	return (uint8_t*) _data[type].color;
}

Tetromino::Tetromino():
		type(Type::I), rotation(Rotation::Rot0), x(0), y(0) {}

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
	rotation = Tetromino::Rotation::Rot0;
	x = uint4_left(_data[type].spawnCoords);
	y = -uint4_right(_data[type].spawnCoords);
}

void Tetromino::draw(canvas canvas, uint8_t* color) {
	if (color == NULL) {
		color = getColor();
	}

	uint8_t w = getWidth();
	uint8_t h = getHeight();

	for (uint8_t i = 0, n = minoCount(w, h); i < n; ++i) {
		if (isMino(i)) {
			canvas(minoX(x, w, i), minoY(y, w, i), color[0], color[1], color[2]);
		}
	}
}

Pile::Pile(uint8_t _width, uint8_t _height):
		_width(_width), _height(_height) {

	_data = (uint8_t*) malloc(sizeof(uint8_t) * _memSize());

	truncate();
}

bool Pile::isOccupied(uint8_t x, int8_t y) {
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

uint8_t Pile::clearCompleteRows() {
	uint8_t rowsCompleted = 0;

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
			rowsCompleted++;

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

	return rowsCompleted;
}

void Pile::draw(canvas canvas) {
	for (uint8_t y = 0; y < _height; ++y) {
		for (uint8_t x = 0; x < _width; ++x) {
			Tetromino::Type type = _get(x, y);

			if (type != Tetromino::Type::_) {
				const uint8_t* color = Tetromino::colorOf(type);
				canvas(x, y, color[0], color[1], color[2]);
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
	return _width * (_height + PILE_HIDDEN_ROWS);
}

uint8_t Pile::_memSize() {
	uint8_t cells = _cellCount();
	return cells / 2 + (cells % 2 != 0);
}

uint8_t Pile::_cell(uint8_t x, int8_t y) {
	return (y + PILE_HIDDEN_ROWS) * _width + x;
}

Tetromino::Type Pile::_get(uint8_t x, int8_t y) {
	uint8_t cell = _cell(x, y);
	uint8_t idx = cell / 2;

	return static_cast<Tetromino::Type>(cell % 2 == 0
			? uint4_left(_data[idx])
			: uint4_right(_data[idx]));
}

void Pile::_set(uint8_t x, int8_t y, Tetromino::Type type) {
	uint8_t cell = _cell(x, y);
	uint8_t idx = cell / 2;

	if (cell % 2 == 0) {
		_data[idx] = uint4_pack(type, uint4_right(_data[idx]));
	} else {
		_data[idx] = uint4_pack(uint4_left(_data[idx]), type);
	}
}

Bag::Bag():
		_index(0) {

	for (uint8_t i = Tetromino::Type::I; i < Tetromino::Type::_; ++i) {
		_sequence[i] = static_cast<Tetromino::Type>(i);
	}
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

Tetris::Tetris(uint8_t _width,  uint8_t _height, tetrisListener _listener):
		_width(_width), _height(_height), _listener(_listener),
		_scores(0), _rowsCompleted(0), _level(1), _gameOver(true), _paused(false),
		_clearBackground(true), _ghostEnabled(true) {

	_tetromino = new Tetromino();
	_pile = new Pile(_width, _height);
	_bag = new Bag();

	_updateTimer = new Timer(500);
	_ghostTimer = new Timer(1000);
}

void Tetris::reset() {
	_pile->truncate();
	_bag->shuffle();
	_tetromino->spawn(_bag->pop());
	_scores = 0;
	_rowsCompleted = 0;
	_setDifficulty();
	_paused = false;
	_gameOver = false;
}

bool Tetris::isGameOver() {
	return _gameOver;
}

bool Tetris::isPaused() {
	return _paused;
}

void Tetris::setPaused(bool paused) {
	_paused = paused;
}

uint32_t Tetris::getScores() {
	return _scores;
}

uint16_t Tetris::getRowsCompleted() {
	return _rowsCompleted;
}

uint8_t Tetris::getLevel() {
	return _level;
}

Tetromino::Type Tetris::preview() {
	return _bag->peek();
}

bool Tetris::moveLeft() {
	if (isPaused()) {
		return false;
	}

	return _move(-1, 0);
}

bool Tetris::moveRight() {
	if (isPaused()) {
		return false;
	}

	return _move(1, 0);
}

bool Tetris::moveDown() {
	if (isPaused()) {
		return false;
	}

	return _move(0, 1);
}

bool Tetris::rotateClockWise() {
	if (isPaused()) {
		return false;
	}

	switch (_tetromino->rotation) {
	case Tetromino::Rotation::Rot0:
		return _rotate(Tetromino::Rotation::RotR);
	case Tetromino::Rotation::RotR:
		return _rotate(Tetromino::Rotation::Rot2);
	case Tetromino::Rotation::Rot2:
		return _rotate(Tetromino::Rotation::RotL);
	case Tetromino::Rotation::RotL:
		return _rotate(Tetromino::Rotation::Rot0);
	default:
		return false;
	}
}

bool Tetris::rotateCounterClockWise() {
	if (isPaused()) {
		return false;
	}

	switch (_tetromino->rotation) {
	case Tetromino::Rotation::Rot0:
		return _rotate(Tetromino::Rotation::RotL);
	case Tetromino::Rotation::RotL:
		return _rotate(Tetromino::Rotation::Rot2);
	case Tetromino::Rotation::Rot2:
		return _rotate(Tetromino::Rotation::RotR);
	case Tetromino::Rotation::RotR:
		return _rotate(Tetromino::Rotation::Rot0);
	default:
		return false;
	}
}

void Tetris::setClearBackground(bool clearBackground) {
	_clearBackground = clearBackground;
}

void Tetris::setGhostEnabled(bool ghostEnabled) {
	_ghostEnabled = ghostEnabled;
}

void Tetris::update() {
	if (_gameOver || _paused) {
		return;
	}

	if (_updateTimer->fire()) {
		if (!_move(0, 1)) {
			_pile->merge(_tetromino);

			uint8_t rowsCleared = _pile->clearCompleteRows();

			switch (rowsCleared) {
			case 1:
				_scores += 40;
				break;
			case 2:
				_scores += 100;
				break;
			case 3:
				_scores += 300;
				break;
			case 4:
				_scores += 1200;
				break;
			default:
				;
			}

			_rowsCompleted += rowsCleared;

			if (!_setDifficulty()) {
				if (_listener != NULL) {
					_listener(TetrisEvent::RowsCompleted, rowsCleared);
				}
			}

			_tetromino->spawn(_bag->pop());

			if (!_checkTetromino()) {
				_gameOver = true;

				if (_listener != NULL) {
					_listener(TetrisEvent::GameOver, 0);
				}
			}
		}
	}
}

void Tetris::draw(canvas canvas) {
	if (_clearBackground) {
		clearCanvas(canvas, 0, 0, _width, _height);
	}

	if (!_gameOver) {
		if (_ghostEnabled) {
			uint8_t x = _tetromino->x;
			uint8_t y = _tetromino->y;

			while (_move(0, 1));

			uint8_t ghostColor[3];
			uint8_t* minoColor = Tetromino::colorOf(_tetromino->type);

			pulsateColor(255, 255, 255,
					minoColor[0], minoColor[1], minoColor[2],
					_ghostTimer->progress(true), ghostColor);

			_tetromino->draw(canvas, ghostColor);

			_tetromino->x = x;
			_tetromino->y = y;
		}

		_tetromino->draw(canvas, NULL);
	}

	_pile->draw(canvas);
}

bool Tetris::_checkTetromino() {
	uint8_t w = _tetromino->getWidth();
	uint8_t h = _tetromino->getHeight();

	for (uint8_t i = 0, n = minoCount(w, h); i < n; ++i) {
		if (_tetromino->isMino(i)) {
			int8_t x = minoX(_tetromino->x, w, i);
			int8_t y = minoY(_tetromino->y, w, i);

			if (x < 0 || x >= _width || y >= _height || _pile->isOccupied(x, y)) {
				return false;
			}
		}
	}

	return true;
}

bool Tetris::_move(int8_t x, int8_t y) {
	int8_t oldX = _tetromino->x;
	int8_t oldY = _tetromino->y;

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

bool Tetris::_setDifficulty() {
	uint8_t oldLevel = _level;

	if (_rowsCompleted == 0) {
		_level = 1;
	} else if ((_rowsCompleted >= 1) && (_rowsCompleted <= 90)) {
		_level = 1 + ((_rowsCompleted - 1) / 10);
	} else if (_rowsCompleted >= 91) {
		_level = 10;
	}

	_updateTimer->reset((11 - _level) * 50);

	if (_level > 1 && oldLevel != _level) {
		if (_listener != NULL) {
			_listener(TetrisEvent::LevelUp, _level);
		}

		return true;
	} else {
		return false;
	}
}
