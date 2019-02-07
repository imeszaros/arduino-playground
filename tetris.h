#include <FastLED.h>
#include <inttypes.h>

#define TETROMINO_COUNT 7

#define uint4_pack(a, b) (a << 4 | (b & 0b1111))
#define uint4_left(i) (i >> 4)
#define uint4_right(i) (i & 0b1111)

#define int3_enc(i) (i < 0 ? 0b1000 | (-i & 0b111) : i & 0b0111)
#define int3_dec(i) ((i & 0b1000 ? -1 : 1) * (i & 0b111))

#define srsOffset(x, y) uint4_pack(int3_enc(x), int3_enc(y))
#define srsOffsetX(i) int3_dec(uint4_left(i))
#define srsOffsetY(i) int3_dec(uint4_right(i))

#define minoCount(tetrominoWidth, tetrominoHeight) (tetrominoWidth * tetrominoHeight)
#define minoX(tetrominoX, tetrominoWidth, minoIdx) (tetrominoX + minoIdx % tetrominoWidth)
#define minoY(tetrominoY, tetrominoWidth, minoIdx) (tetrominoY + minoIdx / tetrominoWidth)

typedef void (*tetDisplay) (int8_t x, int8_t y, uint8_t r, uint8_t g, uint8_t b);

class Tetromino {

public:

	enum Type {
		I, J, L, O, S, T, Z, _
	};

	enum Rotation {
		rot0, rotR, rot2, rotL
	};

	static uint8_t* colorOf(Type type);

	Type type;
	Rotation rotation;

	int8_t x;
	int8_t y;

	Tetromino();

	// TODO what can be private??
	uint8_t* getColor();
	uint8_t getWidth();
	uint8_t getHeight();
	bool isMino(uint8_t idx);
	uint8_t getSRSOffsetCount();
	uint8_t getSRSOffset(Rotation rotation, uint8_t idx);
	void spawn(Type type);
	void draw(tetDisplay display, uint8_t* color);

private:

	static constexpr uint8_t _srsOffsets[] = {
			// J, L, S, T, Z
			5, // number of offsets per rotation, this is index 0
			/* rot0 */ srsOffset(0, 0), srsOffset(0, 0), srsOffset(0, 0), srsOffset(0, 0), srsOffset(0, 0),
			/* rotR */ srsOffset(0, 0), srsOffset(1, 0), srsOffset(1, -1), srsOffset(0, 2), srsOffset(1, 2),
			/* rot2 */ srsOffset(0, 0), srsOffset(0, 0), srsOffset(0, 0), srsOffset(0, 0), srsOffset(0, 0),
			/* rotL */ srsOffset(0, 0), srsOffset(-1, 0), srsOffset(-1, -1), srsOffset(0, 2), srsOffset(-1, 2),

			// I
			5, // number of offsets per rotation, this is index 21

			/* rot0 */ srsOffset(0, 0), srsOffset(-1, 0), srsOffset(2, 0), srsOffset(-1, 0), srsOffset(2, 0),
			/* rotR */ srsOffset(-1, 0), srsOffset(0, 0), srsOffset(0, 0), srsOffset(0, 1), srsOffset(0, -2),
			/* rot2 */ srsOffset(-1, 1), srsOffset(1, 1), srsOffset(-2, 1), srsOffset(1, 0), srsOffset(-2, 0),
			/* rotL */ srsOffset(0, 1), srsOffset(0, 1), srsOffset(0, 1), srsOffset(0, -1), srsOffset(0, 2),

			// O
			1, // number of offsets per rotation, this is index 42
			/* rot0 */ srsOffset(0, 0),
			/* rotR */ srsOffset(0, -1),
			/* rot2 */ srsOffset(-1, -1),
			/* rotL */ srsOffset(-1, 0)
	};

	struct _Data {
		uint8_t color[3]; // r, g, b
		uint8_t dimensions; // first 4 bits are width, last 4 bits are height
		uint8_t spawnCoords; // first 4 bits are x, last 4 bits are -y
		uint8_t minos[4][3]; // indexed by rotation, 3 bytes of data, left to right, top to bottom
		uint8_t srsOffsets; // index within srsOffsets array
	};

	static constexpr _Data _data[] {
		{ // I
			{ 0, 255, 255 },
			uint4_pack(5, 5),
			uint4_pack(2, 3),
			{
				{ 0b00000000, 0b00011110, 0b00000000 },
				{ 0b00000001, 0b00001000, 0b01000010 },
				{ 0b00000000, 0b00111100, 0b00000000 },
				{ 0b00100001, 0b00001000, 0b01000000 }
			},
			21
		},
		{ // J
			{ 0, 0, 255 },
			uint4_pack(3, 3),
			uint4_pack(3, 2),
			{
				{ 0b10011100, 0b00000000, 0b00000000 },
				{ 0b01101001, 0b00000000, 0b00000000 },
				{ 0b00011100, 0b10000000, 0b00000000 },
				{ 0b01001011, 0b00000000, 0b00000000 }
			},
			0
		},
		{ // L
			{ 255, 170, 0 },
			uint4_pack(3, 3),
			uint4_pack(3, 2),
			{
				{ 0b00111100, 0b00000000, 0b00000000 },
				{ 0b01001001, 0b10000000, 0b00000000 },
				{ 0b00011110, 0b00000000, 0b00000000 },
				{ 0b11001001, 0b00000000, 0b00000000 }
			},
			0
		},
		{ // O
			{ 255, 255, 0 },
			uint4_pack(3, 3),
			uint4_pack(3, 2),
			{
				{ 0b01101100, 0b00000000, 0b00000000 },
				{ 0b00001101, 0b10000000, 0b00000000 },
				{ 0b00011011, 0b00000000, 0b00000000 },
				{ 0b11011000, 0b00000000, 0b00000000 }
			},
			42
		},
		{ // S
			{ 0, 255, 0 },
			uint4_pack(3, 3),
			uint4_pack(3, 2),
			{
				{ 0b01111000, 0b00000000, 0b00000000 },
				{ 0b01001100, 0b10000000, 0b00000000 },
				{ 0b00001111, 0b00000000, 0b00000000 },
				{ 0b10011001, 0b00000000, 0b00000000 }
			},
			0
		},
		{ // T
			{ 153, 0, 255 },
			uint4_pack(3, 3),
			uint4_pack(3, 2),
			{
				{ 0b01011100, 0b00000000, 0b00000000 },
				{ 0b01001101, 0b00000000, 0b00000000 },
				{ 0b00011101, 0b00000000, 0b00000000 },
				{ 0b01011001, 0b00000000, 0b00000000 }
			},
			0
		},
		{ // Z
			// red
			{ 255, 0, 0 },
			uint4_pack(3, 3),
			uint4_pack(3, 2),
			{
				{ 0b11001100, 0b00000000, 0b00000000 },
				{ 0b00101101, 0b00000000, 0b00000000 },
				{ 0b00011001, 0b10000000, 0b00000000 },
				{ 0b01011010, 0b00000000, 0b00000000 }
			},
			0
		}
	};
};

class Pile {

public:
	Pile(uint8_t width, uint8_t height);

	bool isOccupied(uint8_t x, uint8_t y);
	void merge(Tetromino* tetromino);
	void clearCompleteRows();
	void draw(tetDisplay display);
	void truncate();

private:
	uint8_t _width;
	uint8_t _height;
	uint8_t* _data;

	uint8_t _cellCount();
	uint8_t _memSize();
	uint8_t _cell(uint8_t x, uint8_t y);
	Tetromino::Type _get(uint8_t x, uint8_t y);
	void _set(uint8_t x, uint8_t y, Tetromino::Type);
};

class Bag {

public:

	Bag(unsigned int seed);

	Tetromino::Type peek();
	Tetromino::Type pop();
	void shuffle();

private:
	Tetromino::Type _sequence[TETROMINO_COUNT];
	uint8_t _index;
};

class Tetris {

public:
	Tetris(uint8_t width,  uint8_t height, unsigned int seed);

	void reset();
	bool isGameOver();
	Tetromino::Type preview();
	bool moveLeft();
	bool moveRight();
	bool rotateClockWise();
	bool rotateCounterClockWise();
	void setDropping(bool dropping);
	void setClearBackground(bool clearBackground);
	void setGhostEnabled(bool ghostEnabled);
	void setGhostColor(uint8_t r, uint8_t g, uint8_t b);
	void update();
	void draw(tetDisplay display);

private:
	uint8_t _width;
	uint8_t _height;
	Tetromino* _tetromino;
	Pile* _pile;
	Bag* _bag;
	unsigned long _lastDrop;
	uint16_t _speed;
	bool _gameOver;
	bool _dropping;
	bool _clearBackground;
	bool _ghostEnabled;
	uint8_t _ghostColor[3];

	bool _checkTetromino();
	bool _move(uint8_t x, uint8_t y);
	bool _rotate(Tetromino::Rotation to);
};
