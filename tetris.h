#include <FastLED.h>
#include <inttypes.h>

#define pack2x4(a, b) a << 4 | b

typedef void (*tetDisplay) (uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);

class Tetromino {

public:

	enum Type {
		I, J, L, O, S, T, Z
	};

	enum Rotation {
		rot_0, rot_R, rot_2, rot_L
	};

	Tetromino(Type type, Rotation rotation, uint8_t x, uint8_t y);

	const uint8_t* color();
	uint8_t width();
	uint8_t height();
	int8_t spawnX();
	int8_t spawnY();
	bool isMino(uint8_t idx);
	void rotateClockWise();
	void rotateCounterClockWise();
	void draw(tetDisplay display);

	struct Data {
		uint8_t color[3]; // r, g, b
		uint8_t dimensions; // first for bits are width, last four bits are height
		uint8_t spawnCoords; // first for bits are x, last four bits are -y
		uint8_t shapes[4][3]; // indexed by rotation, 3 bytes of data, left to right, top to bottom
	};

	static constexpr Data data[7] {
		{ // I
			{ 0, 255, 255 },
			pack2x4(5, 5),
			pack2x4(2, 3),
			{
				{ 0b00000000, 0b00011110, 0b00000000 },
				{ 0b00000001, 0b00001000, 0b01000010 },
				{ 0b00000000, 0b00111100, 0b00000000 },
				{ 0b00100001, 0b00001000, 0b01000000 }
			}
		},
		{ // J
			{ 0, 0, 255 },
			pack2x4(3, 3),
			pack2x4(3, 2),
			{
				{ 0b10011100, 0b00000000, 0b00000000 },
				{ 0b01101001, 0b00000000, 0b00000000 },
				{ 0b00011100, 0b10000000, 0b00000000 },
				{ 0b01001011, 0b00000000, 0b00000000 }
			}
		},
		{ // L
			{ 255, 170, 0 },
			pack2x4(3, 3),
			pack2x4(3, 2),
			{
				{ 0b00111100, 0b00000000, 0b00000000 },
				{ 0b01001001, 0b10000000, 0b00000000 },
				{ 0b00011110, 0b00000000, 0b00000000 },
				{ 0b11001001, 0b00000000, 0b00000000 }
			}
		},
		{ // O
			{ 255, 255, 0 },
			pack2x4(3, 3),
			pack2x4(3, 2),
			{
				{ 0b01101100, 0b00000000, 0b00000000 },
				{ 0b00001101, 0b10000000, 0b00000000 },
				{ 0b00011011, 0b00000000, 0b00000000 },
				{ 0b11011000, 0b00000000, 0b00000000 }
			}
		},
		{ // S
			{ 0, 255, 0 },
			pack2x4(3, 3),
			pack2x4(3, 2),
			{
				{ 0b01111000, 0b00000000, 0b00000000 },
				{ 0b01001100, 0b10000000, 0b00000000 },
				{ 0b00001111, 0b00000000, 0b00000000 },
				{ 0b10011001, 0b00000000, 0b00000000 }
			}
		},
		{ // T
			{ 153, 0, 255 },
			pack2x4(3, 3),
			pack2x4(3, 2),
			{
				{ 0b01011100, 0b00000000, 0b00000000 },
				{ 0b01001101, 0b00000000, 0b00000000 },
				{ 0b00011101, 0b00000000, 0b00000000 },
				{ 0b01011001, 0b00000000, 0b00000000 }
			}
		},
		{ // Z
			// red
			{ 255, 0, 0 },
			pack2x4(3, 3),
			pack2x4(3, 2),
			{
				{ 0b11001100, 0b00000000, 0b00000000 },
				{ 0b00101101, 0b00000000, 0b00000000 },
				{ 0b00011001, 0b10000000, 0b00000000 },
				{ 0b01011010, 0b00000000, 0b00000000 }
			}
		}
	};

private:

	Type type;
	Rotation rotation;

	int8_t x;
	int8_t y;
};

class Tetris {

public:
	Tetris(uint8_t w,  uint8_t h, tetDisplay display);

	void rotateClockWise();
	void rotateCounterClockWise();
	void update();

private:
	uint8_t w;
	uint8_t h;

	tetDisplay display;

	Tetromino* tet;
};
